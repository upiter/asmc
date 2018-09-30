/****************************************************************************
*
*  This code is Public Domain.
*
*  ========================================================================
*
* Description:	Binary output routines.
*		Used if -bin, -mz or -pe cmdline options were set.
*
****************************************************************************/

#include <stddef.h>
#include <time.h>

#include <globals.h>
#include <memalloc.h>
#include <parser.h>
#include <fixup.h>
#include <omfspec.h>
#include <bin.h>
#include <listing.h>
#include <lqueue.h>

#define SECTORMAP 1 /* 1=print sector map in listing file */

#include <coffspec.h>
#include <input.h>
#include <mangle.h>
#include <segment.h>
#include <equate.h>
#include <expreval.h>

#define RAWSIZE_ROUND 1 /* SectionHeader.SizeOfRawData is multiple FileAlign. Required by MS COFF spec */
#define IMGSIZE_ROUND 1 /* OptionalHeader.SizeOfImage is multiple ObjectAlign. Required by MS COFF spec */

#define IMPDIRSUF  "2"	/* import directory segment suffix */
#define IMPNDIRSUF "3"	/* import data null directory entry segment suffix */
#define IMPILTSUF  "4"	/* ILT segment suffix */
#define IMPIATSUF  "5"	/* IAT segment suffix */
#define IMPSTRSUF  "6"	/* import strings segment suffix */

/* pespec.h contains MZ header declaration */
#include <pespec.h>

extern void SortSegments( int );

/* default values for OPTION MZ */
#ifndef __ASMC64__
static const struct MZDATA mzdata = {0x1E, 0x10, 0, 0xFFFF };
#endif

/* these strings are to be moved to ltext.h */
static const char szCaption[]  = { "Binary Map:" };
static const char szCaption2[] = { "Segment                  Pos(file)     RVA  Size(fil) Size(mem)" };
static const char szSep[]      = { "---------------------------------------------------------------" };
static const char szHeader[]   = { "<header>" };
static const char szSegLine[]  = { "%-24s %8" I32_SPEC "X %8" I32_SPEC "X %9" I32_SPEC "X %9" I32_SPEC "X" };
static const char szTotal[]    = { "%-42s %9" I32_SPEC "X %9" I32_SPEC "X" };

struct calc_param {
    uint_8 first;	   /* 1=first call of CalcOffset() */
    uint_8 alignment;	   /* current aligment */
    uint_32 fileoffset;	   /* current file offset */
    uint_32 sizehdr;	   /* -mz: size of MZ header, else 0 */
    uint_32 entryoffset;   /* -bin only: offset of first segment */
    struct asym *entryseg; /* -bin only: segment of first segment */
    uint_32 imagestart;	   /* -bin: start offset (of first segment), else 0 */
    uint_32 rva;	   /* -pe: current RVA */
    union {
	uint_32 imagebase;    /* -pe: image base */
	uint_64 imagebase64;  /* -pe: image base */
    };
#if RAWSIZE_ROUND
    uint_32 rawpagesize;
#endif
};

/* reorder segments for DOSSEG:
 1. code
 2. unknown
 3. initialized data
 4. uninitialized data
 5. stack
 */
static const enum seg_type dosseg_order[] = {
    SEGTYPE_CODE, SEGTYPE_UNDEF, SEGTYPE_DATA,
    SEGTYPE_BSS, SEGTYPE_STACK, SEGTYPE_ABS
};
#define SIZE_DOSSEG ( sizeof( dosseg_order ) / sizeof( dosseg_order[0] ) )

static const enum seg_type flat_order[] = {
    SEGTYPE_HDR, SEGTYPE_CODE, SEGTYPE_CDATA, SEGTYPE_DATA, SEGTYPE_BSS, SEGTYPE_RSRC, SEGTYPE_RELOC
};
#define SIZE_PEFLAT ( sizeof( flat_order ) / sizeof( flat_order[0] ) )

enum pe_flags_values {
    PEF_MZHDR = 0x01,  /* 1=create mz header */
};

#define hdrname ".hdr$"

static const char hdrattr[]   = { "read public 'HDR'" };
static const char edataname[] = { ".edata" };
static const char edataattr[] = { "FLAT read public alias('.rdata') 'DATA'" };
static const char idataname[] = { ".idata$" };
static const char idataattr[] = { "FLAT read public alias('.rdata') 'DATA'" };

static const char mzcode[] = {
    "db 'MZ'\0"		  /* e_magic */
    "dw 80h, 1, 0, 4\0"	  /* e_cblp, e_cp, e_crlc, e_cparhdr */
    "dw 0, -1, 0, 0B8h\0" /* e_minalloc, e_maxalloc, e_ss, e_sp */
    "dw 0, 0, 0, 40h\0"	  /* e_csum, e_ip, e_cs, e_sp, e_lfarlc */
    "org 40h\0"		  /* e_lfanew, will be set by program */
    "push cs\0"
    "pop ds\0"
    "mov dx,@F - 40h\0"
    "mov ah,9\0"
    "int 21h\0"
    "mov ax,4C01h\0"
    "int 21h\0"
    "@@:\0"
    "db 'This is a PE executable',0Dh,0Ah,'$'"
};

#ifndef __ASMC64__
/* default 32-bit PE header */
static struct IMAGE_PE_HEADER32 pe32def = {
    'P'+ ('E' << 8 ),
    { IMAGE_FILE_MACHINE_I386, 0, 0, 0, 0, sizeof( struct IMAGE_OPTIONAL_HEADER32 ),
    IMAGE_FILE_RELOCS_STRIPPED | IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_LOCAL_SYMS_STRIPPED | IMAGE_FILE_32BIT_MACHINE
    },
    { IMAGE_NT_OPTIONAL_HDR32_MAGIC,
    5,1,0,0,0,0,0,0, /* linkervers maj/min, sizeof code/init/uninit, entrypoint, base code/data */
    0x400000,	     /* image base */
    0x1000, 0x200,   /* SectionAlignment, FileAlignment */
    4,0,0,0,4,0,     /* OSversion maj/min, Imagevers maj/min, Subsystemvers maj/min */
    0,0,0,0,	     /* Win32vers, sizeofimage, sizeofheaders, checksum */
    IMAGE_SUBSYSTEM_WINDOWS_CUI,0,  /* subsystem, dllcharacteristics */
    0x100000,0x1000, /* sizeofstack reserve/commit */
    0x100000,0x1000, /* sizeofheap reserve/commit */
    0, IMAGE_NUMBEROF_DIRECTORY_ENTRIES, /* loaderflags, numberofRVAandSizes */
    }
};
#endif
/* default 64-bit PE header */
static struct IMAGE_PE_HEADER64 pe64def = {
    'P'+ ('E' << 8 ),
    { IMAGE_FILE_MACHINE_AMD64, 0, 0, 0, 0, sizeof( struct IMAGE_OPTIONAL_HEADER64 ),
      IMAGE_FILE_RELOCS_STRIPPED | IMAGE_FILE_EXECUTABLE_IMAGE | IMAGE_FILE_LINE_NUMS_STRIPPED |
      IMAGE_FILE_LOCAL_SYMS_STRIPPED | IMAGE_FILE_LARGE_ADDRESS_AWARE | IMAGE_FILE_32BIT_MACHINE
    },
    { IMAGE_NT_OPTIONAL_HDR64_MAGIC,
    5,1,0,0,0,0,0,   /* linkervers maj/min, sizeof code/init data/uninit data, entrypoint, base code RVA */
    0x400000,	     /* image base */
    0x1000, 0x200,   /* SectionAlignment, FileAlignment */
    4,0,0,0,4,0,     /* OSversion maj/min, Imagevers maj/min, Subsystemvers maj/min */
    0,0,0,0,	     /* Win32vers, sizeofimage, sizeofheaders, checksum */
    IMAGE_SUBSYSTEM_WINDOWS_CUI,0,  /* subsystem, dllcharacteristics */
    0x100000,0x1000, /* sizeofstack reserve/commit */
    0x100000,0x1000, /* sizeofheap reserve/commit */
    0, IMAGE_NUMBEROF_DIRECTORY_ENTRIES, /* loaderflags, numberofRVAandSizes */
    }
};

/* calculate starting offset of segments and groups */

static void CalcOffset( struct dsym *curr, struct calc_param *cp )
/****************************************************************/
{
    uint_32 align;
    uint_32 alignbytes;
    uint_32 offset;
    struct dsym *grp;

    if ( curr->e.seginfo->segtype == SEGTYPE_ABS ) {
	curr->e.seginfo->start_offset = curr->e.seginfo->abs_frame << 4;
	return;
    } else if ( curr->e.seginfo->info )
	return;

    grp = (struct dsym *)curr->e.seginfo->group;
    if ( cp->alignment > curr->e.seginfo->alignment )
	align = 1 << cp->alignment;
    else
	align = 1 << curr->e.seginfo->alignment;
    alignbytes = ((cp->fileoffset + (align - 1)) & (-align)) - cp->fileoffset;
    cp->fileoffset += alignbytes;

    if ( grp == NULL ) {
	offset = cp->fileoffset - cp->sizehdr;	// + alignbytes;
    } else {
	if ( ModuleInfo.sub_format == SFORMAT_PE || ModuleInfo.sub_format == SFORMAT_64BIT ) {
	    /* v2.24 */
	    offset = cp->rva;
	} else {
	    if ( grp->sym.total_size == 0 ) {
		grp->sym.offset = cp->fileoffset - cp->sizehdr;
		offset = 0;
	    } else {
		/* v2.12: the old way wasn't correct. if there's a segment between the
		 * segments of a group, it affects the offset as well ( if it
		 * occupies space in the file )! the value stored in grp->sym.total_size
		 * is no longer used (or, more exactly, used as a flag only).
		 *
		 * offset = ( cp->fileoffset - cp->sizehdr ) - grp->sym.offset;
		 */
		 offset = grp->sym.total_size + alignbytes;
	    }
	}
    }

    /* v2.04: added */
    /* v2.05: this addition did mess sample Win32_5.asm, because the
     * "empty" alignment sections are now added to <fileoffset>.
     * todo: VA in binary map is displayed wrong.
     */
    if ( cp->first == FALSE ) {
	/* v2.05: do the reset more carefully.
	 * Do reset start_loc only if
	 * - segment is in a group and
	 * - group isn't FLAT or segment's name contains '$'
	 */
	if ( grp && ( grp != ModuleInfo.flat_grp ||
		     strchr( curr->sym.name, '$' ) ) )
	    curr->e.seginfo->start_loc = 0;
    }

    curr->e.seginfo->fileoffset = cp->fileoffset;
    curr->e.seginfo->start_offset = offset;

    if ( ModuleInfo.sub_format == SFORMAT_NONE ) {
	cp->fileoffset += curr->sym.max_offset - curr->e.seginfo->start_loc;
	if ( cp->first )
	    cp->imagestart = curr->e.seginfo->start_loc;
	/* there's no real entry address for BIN, therefore the
	 start label must be at the very beginning of the file */
	if ( cp->entryoffset == -1 ) {
	    cp->entryoffset = offset;
	    cp->entryseg = (struct asym *)curr;
	}
    } else {
	cp->rva += curr->sym.max_offset - curr->e.seginfo->start_loc;
	if ( curr->e.seginfo->segtype == SEGTYPE_BSS )
	    ;
	else
	cp->fileoffset += curr->sym.max_offset - curr->e.seginfo->start_loc;
    }

    offset += curr->sym.max_offset;
    if ( grp ) {
	grp->sym.total_size = offset;
	/* v2.07: for 16-bit groups, ensure that it fits in 64 kB */
	if ( grp->sym.total_size > 0x10000 && grp->sym.Ofssize == USE16 ) {
	    asmerr( 8003, grp->sym.name );
	}
    }
    cp->first = FALSE;
    return;
}

/*
 * if pDst==NULL: count the number of segment related fixups
 * if pDst!=NULL: write segment related fixups
 */

static int GetSegRelocs( uint_16 *pDst )
/**************************************/
{
    struct dsym *curr;
    int count = 0;
    uint_16 valueofs;
    uint_16 valueseg;
    uint_32 loc;
    struct fixup *fixup;

    for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	if ( curr->e.seginfo->segtype == SEGTYPE_ABS )
	    continue;
	for ( fixup = curr->e.seginfo->FixupList.head; fixup; fixup = fixup->nextrlc ) {
	    switch ( fixup->type ) {
	    case FIX_PTR32:
	    case FIX_PTR16:
	    case FIX_SEG:
		/* ignore fixups for absolute segments */
		if ( fixup->sym && fixup->sym->segment && ((struct dsym *)fixup->sym->segment)->e.seginfo->segtype == SEGTYPE_ABS )
		    break;
		count++;
		if ( pDst ) {
		    /* v2.04: fixed */
		    loc = fixup->locofs + ( curr->e.seginfo->start_offset & 0xf );
		    valueseg = curr->e.seginfo->start_offset >> 4;
		    if ( curr->e.seginfo->group ) {
			loc += curr->e.seginfo->group->offset & 0xf;
			valueseg += curr->e.seginfo->group->offset >> 4;
		    }
		    if ( fixup->type == FIX_PTR16 )
			loc += 2;
		    else if ( fixup->type == FIX_PTR32 )
			loc += 4;

		    /* offset may be > 64 kB */
		    while ( loc >= 0x10000 ) {
			loc -= 16;
			valueseg++;
		    };

		    valueofs = loc;
		    *pDst++ = valueofs;
		    *pDst++ = valueseg;
		}
		break;
	    }
	}
    }
    return( count );
}

/* get image size.
 * memimage=FALSE: get size without uninitialized segments (BSS and STACK)
 * memimage=TRUE: get full size
 */

static uint_32 GetImageSize( bool memimage )
/******************************************/
{
    struct dsym *curr;
    bool first;
    uint_32 vsize = 0;
    uint_32 size = 0;

    for( curr = SymTables[TAB_SEG].head, first = TRUE; curr; curr = curr->next ) {
	uint_32 tmp;
	if ( curr->e.seginfo->segtype == SEGTYPE_ABS || curr->e.seginfo->info )
	    continue;
	if ( memimage == FALSE ) {
	    if ( curr->e.seginfo->bytes_written == 0 ) {
		struct dsym *dir;
		for ( dir = curr->next; dir; dir = dir->next )
		    if ( dir->e.seginfo->bytes_written )
			break;
		if ( !dir )
		    break; /* done, skip rest of segments! */
	    }
	}
	tmp = curr->e.seginfo->fileoffset + (curr->sym.max_offset - curr->e.seginfo->start_loc );
	if ( first == FALSE )
	    vsize += curr->e.seginfo->start_loc;
	if ( memimage )
	    tmp += vsize;
	if ( size < tmp )
	    size = tmp;
	first = FALSE;
    }
    return( size );
}

/* micro-linker. resolve internal fixups.
 */

union genptr {
    uint_8  *db;
    uint_16 *dw;
    uint_32 *dd;
    uint_64 *dq;
};

/* handle the fixups contained in a segment */

static int DoFixup( struct dsym *curr, struct calc_param *cp )
/*****************************************************************/
{
    union genptr codeptr;
    struct dsym *seg;
    uint_32 value;
    uint_64 value64;
    uint_32 offset;  /* v2.07 */
    struct fixup *fixup;
    char *tmp;

    if ( curr->e.seginfo->segtype == SEGTYPE_ABS )
	return( NOT_ERROR );

    for ( fixup = curr->e.seginfo->FixupList.head; fixup; fixup = fixup->nextrlc ) {
	codeptr.db = curr->e.seginfo->CodeBuffer +
	    ( fixup->locofs - curr->e.seginfo->start_loc );

	if ( fixup->sym && ( fixup->sym->segment || fixup->sym->variable ) ) {
	    /* assembly time variable (also $ symbol) in reloc? */
	    /* v2.07: moved inside if-block, using new local var "offset" */
	    if ( fixup->sym->variable ) {
		seg = (struct dsym *)fixup->segment_var;
		offset = 0;
	    } else {
		seg = (struct dsym *)fixup->sym->segment;
		offset = fixup->sym->offset;
	    }
	    /* the offset result consists of
	     * - the symbol's offset
	     * - the fixup's offset (usually the displacement )
	     * - the segment/group offset in the image
	     */
	    switch ( fixup->type ) {
	    case FIX_OFF32_IMGREL:
		value = ( fixup->offset + offset + seg->e.seginfo->start_offset ) - cp->imagestart;
		break;
	    case FIX_OFF32_SECREL:
		value = ( fixup->offset + offset ) - seg->e.seginfo->start_loc;
		/* check if symbol's segment name contains a '$'.
		 * If yes, search the segment without suffix.
		 */
		if ( tmp = strchr( seg->sym.name, '$' ) ) {
		    int namlen = tmp - seg->sym.name;
		    struct dsym *segfirst;
		    for( segfirst = SymTables[TAB_SEG].head; segfirst; segfirst = segfirst->next ) {
			if ( segfirst->sym.name_size == namlen &&
			    ( memcmp( segfirst->sym.name, seg->sym.name, namlen ) == 0 ) ) {
			    value = ( fixup->offset + offset + seg->e.seginfo->start_offset ) - segfirst->e.seginfo->start_offset;
			    break;
			}
		    }
		}
		break;
	    case FIX_RELOFF8:
	    case FIX_RELOFF16:
	    case FIX_RELOFF32:
		/* v1.96: special handling for "relative" fixups */
		/* v2.28: removed fixup->offset.. */
		value = seg->e.seginfo->start_offset /*+ fixup->offset*/ + offset;
		break;
	    default:
		if ( seg->e.seginfo->group && fixup->frame_type != FRAME_SEG ) {
		    value = (seg->e.seginfo->group->offset & 0xF) + seg->e.seginfo->start_offset + fixup->offset + offset;
		    if ( ModuleInfo.sub_format == SFORMAT_PE || ModuleInfo.sub_format == SFORMAT_64BIT ) {
			if ( curr->e.seginfo->Ofssize == USE64 )
			    value64 = value + cp->imagebase64;
			value += cp->imagebase;
		    }
		} else
		    value = (seg->e.seginfo->start_offset & 0xF) + fixup->offset + offset;
		break;
	    }

	} else {
	    seg = NULL;
	    value = 0;
	}

	switch ( fixup->type ) {
	case FIX_RELOFF8:
	    *codeptr.db += (value - (fixup->locofs + curr->e.seginfo->start_offset) - 1) & 0xff;
	    break;
	case FIX_RELOFF16:
	    *codeptr.dw += (value - (fixup->locofs + curr->e.seginfo->start_offset) - 2) & 0xffff;
	    break;
	case FIX_RELOFF32:
	    /* adjust the location for EIP-related offsets if USE64 */
	    if ( curr->e.seginfo->Ofssize == USE64 ) {
		fixup->locofs += fixup->addbytes - 4;
	    }
	    /* changed in v1.95 */
	    *codeptr.dd += (value - (fixup->locofs + curr->e.seginfo->start_offset) - 4);
	    break;
	case FIX_OFF8:
	    *codeptr.db = value & 0xff;
	    break;
	case FIX_OFF16:
	    *codeptr.dw = value & 0xffff;
	    break;
	case FIX_OFF32:
	    *codeptr.dd = value;
	    break;
	case FIX_OFF32_IMGREL:
	    *codeptr.dd = value;
	    break;
	case FIX_OFF32_SECREL:
	    *codeptr.dd = value;
	    break;
	case FIX_OFF64:
	    if ( ( ModuleInfo.sub_format == SFORMAT_PE && curr->e.seginfo->Ofssize == USE64 ) ||
		 ModuleInfo.sub_format == SFORMAT_64BIT )
		*codeptr.dq = value64;
	    else
		*codeptr.dq = value;
	    break;
	case FIX_HIBYTE:
	    *codeptr.db = (value >> 8) & 0xff;
	    break;
	case FIX_SEG:
	    /* absolute segments are ok */
	    if ( fixup->sym &&
		fixup->sym->state == SYM_SEG &&
		((struct dsym *)fixup->sym)->e.seginfo->segtype == SEGTYPE_ABS ) {
		*codeptr.dw = ((struct dsym *)fixup->sym)->e.seginfo->abs_frame;
		break;
	    }
	    if ( ModuleInfo.sub_format == SFORMAT_MZ ) {
		if ( fixup->sym->state == SYM_GRP ) {
		    seg = (struct dsym *)fixup->sym;
		    *codeptr.dw = seg->sym.offset >> 4;
		} else if ( fixup->sym->state == SYM_SEG ) {
		    seg = (struct dsym *)fixup->sym;
		    *codeptr.dw = ( seg->e.seginfo->start_offset + ( seg->e.seginfo->group ? seg->e.seginfo->group->offset : 0 ) ) >> 4;
		} else if ( fixup->frame_type == FRAME_GRP ) {
		    *codeptr.dw = seg->e.seginfo->group->offset >> 4;
		} else {
		    *codeptr.dw = seg->e.seginfo->start_offset >> 4;
		}
		break;
	    }
	case FIX_PTR16:
	    /* v2.10: absolute segments are ok */
	    if ( seg && seg->e.seginfo->segtype == SEGTYPE_ABS ) {
		*codeptr.dw = value & 0xffff;
		codeptr.dw++;
		*codeptr.dw = seg->e.seginfo->abs_frame;
		break;
	    }
	    if ( ModuleInfo.sub_format == SFORMAT_MZ ) {
		*codeptr.dw = value & 0xffff;
		codeptr.dw++;
		if ( fixup->frame_type == FRAME_GRP ) {
		    *codeptr.dw = seg->e.seginfo->group->offset >> 4;
		} else {
		    *codeptr.dw = ( seg->e.seginfo->start_offset + ( seg->e.seginfo->group ? seg->e.seginfo->group->offset : 0 ) ) >> 4;
		}
		break;
	    }
	case FIX_PTR32:
	    /* v2.10: absolute segments are ok */
	    if ( seg && seg->e.seginfo->segtype == SEGTYPE_ABS ) {
		*codeptr.dd = value;
		codeptr.dd++;
		*codeptr.dw = seg->e.seginfo->abs_frame;
		break;
	    }
	    if ( ModuleInfo.sub_format == SFORMAT_MZ ) {
		*codeptr.dd = value;
		codeptr.dd++;
		if ( fixup->frame_type == FRAME_GRP ) {
		    *codeptr.dw = seg->e.seginfo->group->offset >> 4;
		} else {
		    *codeptr.dw = ( seg->e.seginfo->start_offset + ( seg->e.seginfo->group ? seg->e.seginfo->group->offset : 0 ) ) >> 4;
		}
		break;
	    }
	default:
	    asmerr( 3019, ModuleInfo.fmtopt->formatname, fixup->type, curr->sym.name, fixup->locofs );
	}
    }
    return( NOT_ERROR );
}

static void pe_create_MZ_header( struct module_info *modinfo )
/************************************************************/
{
    const char *p;
    struct asym *sym;

    if ( Parse_Pass == PASS_1 && SymSearch( hdrname "1" ) == NULL )
	modinfo->g.pe_flags |= PEF_MZHDR;
    if ( modinfo->g.pe_flags & PEF_MZHDR ) {
	AddLineQueueX("%r DOTNAME", T_OPTION );
	AddLineQueueX("%s1 %r USE16 %r %s", hdrname, T_SEGMENT, T_WORD, hdrattr );
	for( p = mzcode; p < mzcode + sizeof( mzcode ); p += strlen( p ) + 1 )
	    AddLineQueue( p );
	AddLineQueueX("%s1 %r", hdrname, T_ENDS );
	RunLineQueue();
	if ( ( sym = SymSearch( hdrname "1" ) ) && sym->state == SYM_SEG )
	   (( struct dsym *)sym)->e.seginfo->segtype = SEGTYPE_HDR;
    }
}

/* get/set value of @pe_file_flags variable */

static void set_file_flags( struct asym *sym, struct expr *opnd )
/***************************************************************/
{
    struct dsym *pehdr;
    struct IMAGE_PE_HEADER32 *pe;

    pehdr = ( struct dsym *)SymSearch( hdrname "2" );
    if ( !pehdr )
	return;
    pe = (struct IMAGE_PE_HEADER32 *)pehdr->e.seginfo->CodeBuffer;

    if ( opnd ) /* set the value? */
	pe->FileHeader.Characteristics = opnd->value;

    sym->value = pe->FileHeader.Characteristics;
}

void pe_create_PE_header( void )
/******************************/
{
    struct asym *sym;
    struct dsym *pehdr;
    int size;
    void *p;

    if ( Parse_Pass == PASS_1 ) {
	if ( ModuleInfo.model != MODEL_FLAT ) {
	    asmerr( 3002 );
	}
#ifndef __ASMC64__
	if ( ModuleInfo.defOfssize == USE64 ) {
#endif
	    size = sizeof( struct IMAGE_PE_HEADER64 );
	    p = (void *)&pe64def;

	    pe64def.OptionalHeader.ImageBase = 0x400000;
	    pe64def.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
	    if ( Options.pe_subsystem == 1 )
		pe64def.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;
	    if ( ModuleInfo.sub_format == SFORMAT_64BIT )
		pe64def.OptionalHeader.ImageBase = 0x140000000;
#ifndef __ASMC64__
	} else {
	    size = sizeof( struct IMAGE_PE_HEADER32 );
	    p = (void *)&pe32def;
	    pe32def.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_CUI;
	    if ( Options.pe_subsystem == 1 )
		pe32def.OptionalHeader.Subsystem = IMAGE_SUBSYSTEM_WINDOWS_GUI;

	}
#endif
	pehdr = ( struct dsym *)SymSearch( hdrname "2" );
	if ( pehdr == NULL ) {
	    pehdr = (struct dsym *)CreateIntSegment( hdrname "2", "HDR", 2, ModuleInfo.defOfssize, TRUE );
	    pehdr->e.seginfo->group = &ModuleInfo.flat_grp->sym;
	    pehdr->e.seginfo->combine = COMB_ADDOFF;  /* PUBLIC */
	    pehdr->e.seginfo->characteristics = (IMAGE_SCN_MEM_READ >> 24);
	    pehdr->e.seginfo->readonly = 1;
	    pehdr->e.seginfo->bytes_written = size; /* ensure that ORG won't set start_loc (assemble.c, SetCurrOffset) */
	    pehdr->sym.max_offset = size;
	} else {
	    if ( pehdr->sym.max_offset < size )
		pehdr->sym.max_offset = size;
	    pehdr->e.seginfo->internal = TRUE;
	    pehdr->e.seginfo->start_loc = 0;
	}
	pehdr->e.seginfo->segtype = SEGTYPE_HDR;
	pehdr->e.seginfo->CodeBuffer = (unsigned char *)LclAlloc( size );
	memcpy( pehdr->e.seginfo->CodeBuffer, p, size );
	time((time_t *)(pehdr->e.seginfo->CodeBuffer+offsetof( struct IMAGE_PE_HEADER32, FileHeader.TimeDateStamp )));
	sym = CreateVariable( "@pe_file_flags", ((struct IMAGE_PE_HEADER32 *)p)->FileHeader.Characteristics );
	if ( sym ) {
	    sym->predefined = TRUE;
	    sym->sfunc_ptr = (internal_func)&set_file_flags;
	}
    }
}

#define CHAR_READONLY ( IMAGE_SCN_MEM_READ >> 24 )

static void pe_create_section_table( void )
/*****************************************/
{
    int i;
    struct dsym *objtab;
    struct dsym *curr;
    int bCreated = FALSE;
    int objs;

    if ( Parse_Pass == PASS_1 ) {
	objtab = ( struct dsym *)SymSearch( hdrname "3" );
	if ( !objtab ) {
	    bCreated = TRUE;
	    objtab = (struct dsym *)CreateIntSegment( hdrname "3", "HDR", 2, ModuleInfo.defOfssize, TRUE );
	    objtab->e.seginfo->group = &ModuleInfo.flat_grp->sym;
	    objtab->e.seginfo->combine = COMB_ADDOFF;  /* PUBLIC */
	}
	objtab->e.seginfo->segtype = SEGTYPE_HDR;

	if ( !bCreated )
	    return;

	/* before objects can be counted, the segment types
	 * SEGTYPE_CDATA ( for readonly segments ) &
	 * SEGTYPE_RSRC ( for resource segments )
	 * SEGTYPE_RELOC ( for relocations )
	 * must be set	- also, init lname_idx field
	 */
	for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	    curr->e.seginfo->lname_idx = SEGTYPE_ERROR; /* use the highest index possible */
	    if ( curr->e.seginfo->segtype == SEGTYPE_DATA ) {
		if ( curr->e.seginfo->readonly || curr->e.seginfo->characteristics == CHAR_READONLY )
		    curr->e.seginfo->segtype = SEGTYPE_CDATA;
		else if ( curr->e.seginfo->clsym && strcmp( curr->e.seginfo->clsym->name, "CONST" ) == 0 )
		    curr->e.seginfo->segtype = SEGTYPE_CDATA;
	    } else if ( curr->e.seginfo->segtype == SEGTYPE_UNDEF ) {
		if ( ( memcmp( curr->sym.name, ".rsrc", 5 ) == 0 ) &&
		    ( *(curr->sym.name+5) == NULLC || *(curr->sym.name+5) == '$' ) )
		    curr->e.seginfo->segtype = SEGTYPE_RSRC;
		else if ( strcmp( curr->sym.name, ".reloc" ) == 0 )
		    curr->e.seginfo->segtype = SEGTYPE_RELOC;
	    }
	}

	/* count objects ( without header types ) */
	for ( i = 1, objs = 0; i < SIZE_PEFLAT; i++ ) {
	    for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
		if ( curr->e.seginfo->segtype != flat_order[i] )
		    continue;
		if ( curr->sym.max_offset ) {
		    objs++;
		    break;
		}
	    }
	}
	if ( objs ) {
	    objtab->sym.max_offset = sizeof(struct IMAGE_SECTION_HEADER) * objs;
	    /* alloc space for 1 more section (.reloc) */
	    objtab->e.seginfo->CodeBuffer = (unsigned char *)LclAlloc( objtab->sym.max_offset + sizeof(struct IMAGE_SECTION_HEADER) );
	}
    }
}

/*
 struct IMAGE_EXPORT_DIRECTORY {
 uint_32 Characteristics;
 uint_32 TimeDateStamp;
 uint_16 MajorVersion;
 uint_16 MinorVersion;
 uint_32 Name;	// RVA name of dll
 uint_32 Base;	// ordinal base
 uint_32 NumberOfFunctions;
 uint_32 NumberOfNames;
 uint_32 AddressOfFunctions; // RVA of function table (array of RVAs)
 uint_32 AddressOfNames;     // RVA of function names (array of RVAs)
 uint_32 AddressOfNameOrdinals; // RVA (array of shorts)
 };
*/

struct expitem {
    char *name;
    unsigned idx;
};

static int compare_exp( const void *p1, const void *p2 )

/*******************************************************/
{
    return( strcmp( ((struct expitem *)p1)->name, ((struct expitem *)p2)->name ) );
}

static void pe_emit_export_data( void )
/*************************************/
{
    struct dsym *curr;
    int_32 timedate;
    int cnt;
    int i;
    char *name;
    char *fname;
    struct expitem *pitems;
    struct expitem *pexp;

    for( curr = SymTables[TAB_PROC].head, cnt = 0; curr; curr = curr->nextproc ) {
	if( curr->e.procinfo->isexport )
	    cnt++;
    }
    if ( cnt ) {
	name = ModuleInfo.name;
	AddLineQueueX( "%r DOTNAME", T_OPTION );
	/* create .edata segment */
	AddLineQueueX( "%s %r %r %s", edataname, T_SEGMENT, T_DWORD, edataattr );
	time( (time_t *)&timedate );
	/* create export directory: Characteristics, Timedate, MajMin, Name, Base, ... */
	AddLineQueueX( "DD 0, 0%xh, 0, %r @%s_name, %u, %u, %u, %r @%s_func, %r @%s_names, %r @%s_nameord",
		      timedate, T_IMAGEREL, name, 1, cnt, cnt, T_IMAGEREL, name, T_IMAGEREL, name, T_IMAGEREL, name );

	/* the name pointer table must be in ascending order!
	 * so we have to fill an array of exports and sort it.
	 */
	pitems = (struct expitem *)myalloca( cnt * sizeof( struct expitem ) );
	for( curr = SymTables[TAB_PROC].head, pexp = pitems, i = 0; curr; curr = curr->nextproc ) {
	    if( curr->e.procinfo->isexport ) {
		pexp->name = curr->sym.name;
		pexp->idx  = i++;
		pexp++;
	    }
	}
	qsort( pitems, cnt, sizeof( struct expitem ), compare_exp );

	/* emit export address table.
	 * would be possible to just use the array of sorted names,
	 * but we want to emit the EAT being sorted by address.
	 */
	AddLineQueueX( "@%s_func %r DWORD", name, T_LABEL );
	for( curr = SymTables[TAB_PROC].head; curr; curr = curr->nextproc ) {
	    if( curr->e.procinfo->isexport )
		AddLineQueueX( "DD %r %s", T_IMAGEREL, curr->sym.name );
	}

	/* emit the name pointer table */
	AddLineQueueX( "@%s_names %r DWORD", name, T_LABEL );
	for ( i = 0; i < cnt; i++ )
	    AddLineQueueX( "DD %r @%s", T_IMAGEREL, (pitems+i)->name );

	/* ordinal table. each ordinal is an index into the export address table */
	AddLineQueueX( "@%s_nameord %r WORD", name, T_LABEL );
	for( i = 0; i < cnt; i++ ) {
	    AddLineQueueX( "DW %u", (pitems+i)->idx );
	}
	/* v2.10: name+ext of dll */
	for ( fname = CurrFName[OBJ] + strlen( CurrFName[OBJ] ); fname > CurrFName[OBJ]; fname-- )
	    if ( *fname == '/' || *fname == '\\' || *fname == ':' )
		break;
	AddLineQueueX( "@%s_name DB '%s',0", name, fname );

	for( curr = SymTables[TAB_PROC].head; curr; curr = curr->nextproc ) {
	    if( curr->e.procinfo->isexport ) {
		Mangle( &curr->sym, StringBufferEnd );
		AddLineQueueX( "@%s DB '%s',0", curr->sym.name, Options.no_export_decoration ? curr->sym.name : StringBufferEnd );
	    }
	}
	/* exit .edata segment */
	AddLineQueueX( "%s %r", edataname, T_ENDS );
	RunLineQueue();
    }
}

/* write import data.
 * convention:
 * .idata$2: import directory
 * .idata$3: final import directory NULL entry
 * .idata$4: ILT entry
 * .idata$5: IAT entry
 * .idata$6: strings
 */

static void pe_emit_import_data( void )
/*************************************/
{
    struct dll_desc *p;
    int type = 0;
#ifndef __ASMC64__
    int ptrtype = ( ModuleInfo.defOfssize == USE64 ? T_QWORD : T_DWORD );
    char *align = ( ModuleInfo.defOfssize == USE64 ? "ALIGN(8)" : "ALIGN(4)" );
#else
    int ptrtype = T_QWORD;
    char *align = "ALIGN(8)";
#endif

    for ( p = ModuleInfo.g.DllQueue; p; p = p->next ) {
	if ( p->cnt ) {
	    struct dsym *curr;
	    char *pdot;
	    if ( !type ) {
		type = 1;
		AddLineQueueX( "@LPPROC %r %r %r", T_TYPEDEF, T_PTR, T_PROC );
		AddLineQueueX( "%r DOTNAME", T_OPTION );
	    }

	    /* avoid . in IDs */
	    if ( pdot = strchr( p->name, '.') )
		*pdot = '_';

	    /* import directory entry */
	    AddLineQueueX( "%s" IMPDIRSUF " %r %r %s", idataname, T_SEGMENT, T_DWORD, idataattr );
	    AddLineQueueX( "DD %r @%s_ilt, 0, 0, %r @%s_name, %r @%s_iat", T_IMAGEREL, p->name, T_IMAGEREL, p->name, T_IMAGEREL, p->name );
	    AddLineQueueX( "%s" IMPDIRSUF " %r", idataname, T_ENDS );

	    /* emit ILT */
	    AddLineQueueX( "%s" IMPILTSUF " %r %s %s", idataname, T_SEGMENT, align, idataattr );
	    AddLineQueueX( "@%s_ilt label %r", p->name, ptrtype );
	    for ( curr = SymTables[TAB_EXT].head; curr != NULL ; curr = curr->next ) {
		if ( curr->sym.iat_used && curr->sym.dll == p ) {
		    AddLineQueueX( "@LPPROC %r @%s_name", T_IMAGEREL, curr->sym.name );
		}
	    }
	    /* ILT termination entry */
	    AddLineQueueX( "@LPPROC 0" );
	    AddLineQueueX( "%s" IMPILTSUF " %r", idataname, T_ENDS );

	    /* emit IAT */
	    AddLineQueueX( "%s" IMPIATSUF " %r %s %s", idataname, T_SEGMENT, align, idataattr );
	    AddLineQueueX( "@%s_iat label %r", p->name, ptrtype );

	    for ( curr = SymTables[TAB_EXT].head; curr != NULL ; curr = curr->next ) {
		if ( curr->sym.iat_used && curr->sym.dll == p ) {
		    Mangle( &curr->sym, StringBufferEnd );
		    AddLineQueueX( "%s%s @LPPROC %r @%s_name", ModuleInfo.g.imp_prefix, StringBufferEnd, T_IMAGEREL, curr->sym.name );
		}
	    }
	    /* IAT termination entry */
	    AddLineQueueX( "@LPPROC 0" );
	    AddLineQueueX( "%s" IMPIATSUF " %r", idataname, T_ENDS );

	    /* emit name table */
	    AddLineQueueX( "%s" IMPSTRSUF " %r %r %s", idataname, T_SEGMENT, T_WORD, idataattr );
	    for ( curr = SymTables[TAB_EXT].head; curr != NULL ; curr = curr->next ) {
		if ( curr->sym.iat_used && curr->sym.dll == p ) {
		    AddLineQueueX( "@%s_name dw 0", curr->sym.name );
		    AddLineQueueX( "db '%s',0", curr->sym.name );
		    AddLineQueue( "even" );
		}
	    }
	    /* dll name table entry */
	    if ( pdot ) {
		*pdot = NULLC;
		AddLineQueueX( "@%s_%s_name db '%s.%s',0", p->name, pdot+1, p->name, pdot+1 );
		*pdot = '.';  /* restore '.' in dll name */
	    } else
		AddLineQueueX( "@%s_name db '%s',0", p->name, p->name );

	    AddLineQueue( "even" );
	    AddLineQueueX( "%s" IMPSTRSUF " %r", idataname, T_ENDS );

	}
    }
    if ( is_linequeue_populated() ) {
	/* import directory NULL entry */
	AddLineQueueX( "%s" IMPNDIRSUF " %r %r %s", idataname, T_SEGMENT, T_DWORD, idataattr );
	AddLineQueueX( "DD 0, 0, 0, 0, 0" );
	AddLineQueueX( "%s" IMPNDIRSUF " %r", idataname, T_ENDS );
	RunLineQueue();
    }
}

static int get_bit( int value )
/*****************************/
{
    int rc = -1;
    while( value ) {
	value = (value >> 1);
	rc++;
    }
    return( rc );
}

static uint_32 pe_get_characteristics( struct dsym *seg )
/*******************************************************/
{
    uint_32 result = 0;

    if ( seg->e.seginfo->segtype == SEGTYPE_CODE ) {
	result |= IMAGE_SCN_CNT_CODE | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ;
    } else if ( seg->e.seginfo->segtype == SEGTYPE_BSS ) {
	result |= IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
    } else if ( seg->e.seginfo->combine == COMB_STACK && seg->e.seginfo->bytes_written == 0 ) {
	result |= IMAGE_SCN_CNT_UNINITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;
    } else if ( seg->e.seginfo->readonly ) {
	result |= IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
    } else if ( seg->e.seginfo->clsym && strcmp( seg->e.seginfo->clsym->name, "CONST" ) == 0 ) {
	result |= IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ;
    } else
	result |= IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE;

    /* manual characteristics set? */
    if ( seg->e.seginfo->characteristics ) {
	result &= 0x1FFFFFF; /* clear the IMAGE_SCN_MEM flags */
	result |= (uint_32)(seg->e.seginfo->characteristics & 0xFE) << 24;
    }
    return( result );
}

/* set base relocations */

static void pe_set_base_relocs( struct dsym *reloc )
/**************************************************/
{
    int cnt1 = 0;
    int cnt2 = 0;
    int ftype;
    uint_32 currpage = -1;
    uint_32 currloc;
    struct dsym *curr;
    struct fixup *fixup;
    struct IMAGE_BASE_RELOCATION *baserel;
    uint_16 *prel;

    for ( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	if ( curr->e.seginfo->segtype == SEGTYPE_HDR )
	    continue;
	for ( fixup = curr->e.seginfo->FixupList.head; fixup; fixup = fixup->nextrlc ) {
	    switch ( fixup->type ) {
	    case FIX_OFF16:
	    case FIX_OFF32:
	    case FIX_OFF64:
		currloc = curr->e.seginfo->start_offset + ( fixup->locofs & 0xFFFFF000 );
		if ( currloc != currpage ) {
		    currpage = currloc;
		    cnt2++;
		    if ( cnt1 & 1 )
			cnt1++;
		}
		cnt1++;
		break;
	    default:
		break;
	    }
	}
    }
    reloc->sym.max_offset = cnt2 * sizeof( struct IMAGE_BASE_RELOCATION ) + cnt1 * sizeof( uint_16 );
    reloc->e.seginfo->CodeBuffer = (unsigned char *)LclAlloc( reloc->sym.max_offset );

    baserel = (struct IMAGE_BASE_RELOCATION *)reloc->e.seginfo->CodeBuffer;
    prel = (uint_16 *)((uint_8 *)baserel + sizeof ( struct IMAGE_BASE_RELOCATION ));

    baserel->VirtualAddress = -1;
    for ( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	if ( curr->e.seginfo->segtype == SEGTYPE_HDR )
	    continue;
	for ( fixup = curr->e.seginfo->FixupList.head; fixup; fixup = fixup->nextrlc ) {
	    switch ( fixup->type ) {
	    case FIX_OFF16: ftype = IMAGE_REL_BASED_LOW; break;
	    case FIX_OFF32: ftype = IMAGE_REL_BASED_HIGHLOW; break;
	    case FIX_OFF64: ftype = IMAGE_REL_BASED_DIR64; break;
	    default: ftype = 0;
	    }
	    if ( ftype ) {
		currloc = curr->e.seginfo->start_offset + ( fixup->locofs & 0xFFFFF000 );
		if ( currloc != baserel->VirtualAddress ) {
		    if ( baserel->VirtualAddress != -1 ) {
			/* address of relocation header must be DWORD aligned */
			if ( baserel->SizeOfBlock & 2 ) {
			    *prel++ = 0;
			    baserel->SizeOfBlock += sizeof( uint_16 );
			}
			baserel = (struct IMAGE_BASE_RELOCATION *)prel;
			prel += 4; /* 4*2 = sizeof( struct IMAGE_BASE_RELOCATION ) */
		    }
		    baserel->VirtualAddress = currloc;
		    baserel->SizeOfBlock = sizeof( struct IMAGE_BASE_RELOCATION );
		}
		*prel++ = ( fixup->locofs & 0xfff ) | ( ftype << 12 );
		baserel->SizeOfBlock += sizeof( uint_16 );
	    }
	}
    }
}

#ifndef __ASMC64__
#define GHF( x ) ( ( ModuleInfo.defOfssize == USE64 ) ? ph64->x : ph32->x )
#else
#define GHF( x ) ( ph64->x )
#endif
/*
 * set values in PE header
 * including data directories:
 * special section names:
 * .edata - IMAGE_DIRECTORY_ENTRY_EXPORT
 * .idata - IMAGE_DIRECTORY_ENTRY_IMPORT, IMAGE_DIRECTORY_ENTRY_IAT
 * .rsrc  - IMAGE_DIRECTORY_ENTRY_RESOURCE
 * .pdata - IMAGE_DIRECTORY_ENTRY_EXCEPTION (64-bit only)
 * .reloc - IMAGE_DIRECTORY_ENTRY_BASERELOC
 * .tls	  - IMAGE_DIRECTORY_ENTRY_TLS
 */

static void pe_set_values( struct calc_param *cp )
/************************************************/
{
    int i;
    int falign;
    int malign;
    uint_16 ff;
    uint_32 codebase = 0;
    uint_32 database = 0;
    uint_32 codesize = 0;
    uint_32 datasize = 0;
    uint_32 sizehdr  = 0;
    uint_32 sizeimg  = 0;
    struct dsym *curr;
    struct dsym *mzhdr;
    struct dsym *pehdr;
    struct dsym *objtab;
    struct dsym *reloc = NULL;
#ifndef __ASMC64__
    struct IMAGE_PE_HEADER32 *ph32;
#endif
    struct IMAGE_PE_HEADER64 *ph64;
    struct IMAGE_FILE_HEADER *fh;
    struct IMAGE_SECTION_HEADER *section;
    struct IMAGE_DATA_DIRECTORY *datadir;
    char *secname;
    char buffer[MAX_ID_LEN+1];

    mzhdr  = ( struct dsym *)SymSearch( hdrname "1" );
    pehdr  = ( struct dsym *)SymSearch( hdrname "2" );
    objtab = ( struct dsym *)SymSearch( hdrname "3" );

    /* make sure all header objects are in FLAT group */
    mzhdr->e.seginfo->group = &ModuleInfo.flat_grp->sym;
#ifndef __ASMC64__
    if ( ModuleInfo.defOfssize == USE64 ) {
#endif
	ph64 = ( struct IMAGE_PE_HEADER64 *)pehdr->e.seginfo->CodeBuffer;
	ff = ph64->FileHeader.Characteristics;
#ifndef __ASMC64__
    } else {
	ph32 = ( struct IMAGE_PE_HEADER32 *)pehdr->e.seginfo->CodeBuffer;
	ff = ph32->FileHeader.Characteristics;
    }
#endif
    if ( !( ff & IMAGE_FILE_RELOCS_STRIPPED ) ) {
	reloc = (struct dsym *)CreateIntSegment( ".reloc", "RELOC", 2, ModuleInfo.defOfssize, TRUE );
	if ( reloc ) {
	    reloc->e.seginfo->group = &ModuleInfo.flat_grp->sym;
	    reloc->e.seginfo->combine = COMB_ADDOFF;  /* PUBLIC */
	    reloc->e.seginfo->segtype = SEGTYPE_RELOC;
	    reloc->e.seginfo->characteristics = ((IMAGE_SCN_MEM_DISCARDABLE | IMAGE_SCN_MEM_READ) >> 24 );
	    /* make sure the section isn't empty ( true size will be calculated later ) */
	    reloc->sym.max_offset = sizeof( struct IMAGE_BASE_RELOCATION );
	    reloc->e.seginfo->bytes_written = reloc->sym.max_offset;
	    /* clear the additionally allocated entry in object table */
	    memset( objtab->e.seginfo->CodeBuffer + objtab->sym.max_offset, 0, sizeof( struct IMAGE_SECTION_HEADER ) );
	    objtab->sym.max_offset += sizeof( struct IMAGE_SECTION_HEADER );
	}
    }


    /* sort: header, executable, readable, read-write segments, resources, relocs */
    for ( i = 0; i < SIZE_PEFLAT; i++ ) {
	for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	    if ( curr->e.seginfo->segtype == flat_order[i] ) {
		curr->e.seginfo->lname_idx = i;
	    }
	}

    }
    SortSegments( 2 );
    falign = get_bit( GHF( OptionalHeader.FileAlignment ) );
    malign = GHF( OptionalHeader.SectionAlignment );

    /* assign RVAs to sections */

    for ( curr = SymTables[TAB_SEG].head, i = -1; curr; curr = curr->next ) {
	if ( curr->e.seginfo->lname_idx == SEGTYPE_ERROR || curr->e.seginfo->lname_idx != i ) {
	    i = curr->e.seginfo->lname_idx;
	    cp->alignment = falign;
	    cp->rva = (cp->rva + (malign - 1)) & (~(malign-1));
	} else {
	    uint_32 align = 1 << curr->e.seginfo->alignment;
	    cp->alignment = 0;
	    cp->rva = (cp->rva + (align - 1)) & (~(align-1));
	}
	CalcOffset( curr, cp );
    }

    if ( reloc ) {
	pe_set_base_relocs( reloc );
	cp->rva = reloc->e.seginfo->start_offset + reloc->sym.max_offset;
    }

    sizeimg = cp->rva;

    /* set e_lfanew of dosstub to start of PE header */
    if ( mzhdr->sym.max_offset >= 0x40 )
	((struct IMAGE_DOS_HEADER *)mzhdr->e.seginfo->CodeBuffer)->e_lfanew = pehdr->e.seginfo->fileoffset;

    /* set number of sections in PE file header (doesn't matter if it's 32- or 64-bit) */
    fh = &((struct IMAGE_PE_HEADER32 *)pehdr->e.seginfo->CodeBuffer)->FileHeader;
    fh->NumberOfSections = objtab->sym.max_offset / sizeof( struct IMAGE_SECTION_HEADER );

#if RAWSIZE_ROUND
#ifndef __ASMC64__
    cp->rawpagesize = ( ModuleInfo.defOfssize == USE64 ? ph64->OptionalHeader.FileAlignment : ph32->OptionalHeader.FileAlignment );
#else
    cp->rawpagesize = ph64->OptionalHeader.FileAlignment;
#endif
#endif

    /* fill object table values */
    section = (struct IMAGE_SECTION_HEADER *)objtab->e.seginfo->CodeBuffer;
    for( curr = SymTables[TAB_SEG].head, i = -1; curr; curr = curr->next ) {
	if ( curr->e.seginfo->segtype == SEGTYPE_HDR )
	    continue;
	if ( curr->sym.max_offset == 0 ) /* ignore empty sections */
	    continue;
	if ( curr->e.seginfo->lname_idx != i ) {
	    i = curr->e.seginfo->lname_idx;
	    secname = ( curr->e.seginfo->aliasname ? curr->e.seginfo->aliasname : ConvertSectionName( &curr->sym, NULL, buffer ) );
	    strncpy( section->Name, secname, sizeof ( section->Name ) );
	    if ( curr->e.seginfo->segtype != SEGTYPE_BSS )
		section->PointerToRawData = curr->e.seginfo->fileoffset;
	    section->VirtualAddress = curr->e.seginfo->start_offset;
	    /* file offset of first section in object table defines SizeOfHeader */
	    if ( sizehdr == 0 )
		sizehdr = curr->e.seginfo->fileoffset;
	}
	section->Characteristics |= pe_get_characteristics( curr );
	if ( curr->e.seginfo->segtype != SEGTYPE_BSS ) {
	    section->SizeOfRawData += curr->sym.max_offset;
	}

	section->Misc.VirtualSize = curr->sym.max_offset + ( curr->e.seginfo->start_offset - section->VirtualAddress );

	if ( curr->next == NULL || curr->next->e.seginfo->lname_idx != i ) {
#if RAWSIZE_ROUND /* AntiVir TR/Crypt.XPACK Gen */
	    section->SizeOfRawData += cp->rawpagesize - 1;
	    section->SizeOfRawData &= ~(cp->rawpagesize - 1);
#endif
	    if ( section->Characteristics & IMAGE_SCN_MEM_EXECUTE ) {
		if ( codebase == 0 )
		    codebase = section->VirtualAddress;
		codesize += section->SizeOfRawData;
	    }
	    if ( section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA ) {
		if ( database == 0 )
		    database = section->VirtualAddress;
		datasize += section->SizeOfRawData;
	    }
	}
	if ( curr->next && curr->next->e.seginfo->lname_idx != i ) {
	    section++;
	}
    }


    if ( ModuleInfo.g.start_label ) {
#ifndef __ASMC64__
	if ( ModuleInfo.defOfssize == USE64 )
#endif
	    ph64->OptionalHeader.AddressOfEntryPoint = ((struct dsym *)ModuleInfo.g.start_label->segment)->e.seginfo->start_offset + ModuleInfo.g.start_label->offset;
#ifndef __ASMC64__
	else
	    ph32->OptionalHeader.AddressOfEntryPoint = ((struct dsym *)ModuleInfo.g.start_label->segment)->e.seginfo->start_offset + ModuleInfo.g.start_label->offset;
#endif
    } else {
	asmerr( 8009 );
    }

#ifndef __ASMC64__
    if ( ModuleInfo.defOfssize == USE64 ) {
#endif
#if IMGSIZE_ROUND
	/* round up the SizeOfImage field to page boundary */
	sizeimg = ( sizeimg + ph64->OptionalHeader.SectionAlignment - 1 ) & ~(ph64->OptionalHeader.SectionAlignment - 1);
#endif
	ph64->OptionalHeader.SizeOfCode = codesize;
	ph64->OptionalHeader.BaseOfCode = codebase;
	ph64->OptionalHeader.SizeOfImage = sizeimg;
	ph64->OptionalHeader.SizeOfHeaders = sizehdr;
	datadir = &ph64->OptionalHeader.DataDirectory[0];
#ifndef __ASMC64__
    } else {
#if IMGSIZE_ROUND
	/* round up the SizeOfImage field to page boundary */
	sizeimg = ( sizeimg + ph32->OptionalHeader.SectionAlignment - 1 ) & ~(ph32->OptionalHeader.SectionAlignment - 1);
#endif
	ph32->OptionalHeader.SizeOfCode = codesize;
	ph32->OptionalHeader.SizeOfInitializedData = datasize;
	ph32->OptionalHeader.BaseOfCode = codebase;
	ph32->OptionalHeader.BaseOfData = database;
	ph32->OptionalHeader.SizeOfImage = sizeimg;
	ph32->OptionalHeader.SizeOfHeaders = sizehdr;
	datadir = &ph32->OptionalHeader.DataDirectory[0];
    }
#endif

    /* set export directory data dir value */
    if ( curr = (struct dsym *)SymSearch( edataname ) ) {
	datadir[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_EXPORT].Size = curr->sym.max_offset;
    }

    /* set import directory and IAT data dir value */
    if ( curr = (struct dsym *)SymSearch( ".idata$" IMPDIRSUF ) ) {
	struct dsym *idata_null;
	struct dsym *idata_iat;
	uint_32 size;
	idata_null = (struct dsym *)SymSearch( ".idata$" IMPNDIRSUF ); /* final NULL import directory entry */
	idata_iat = (struct dsym *)SymSearch( ".idata$" IMPIATSUF ); /* IAT entries */
	size = idata_null->e.seginfo->start_offset + idata_null->sym.max_offset - curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = size;
	datadir[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress = idata_iat->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_IAT].Size = idata_iat->sym.max_offset;
    }

    /* set resource directory data dir value */
    if ( curr = (struct dsym *)SymSearch(".rsrc") ) {
	datadir[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress = curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_RESOURCE].Size = curr->sym.max_offset;
    }

    /* set relocation data dir value */
    if ( curr = (struct dsym *)SymSearch(".reloc") ) {
	datadir[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = curr->sym.max_offset;
    }

    /* fixme: TLS entry is not written because there exists a segment .tls, but
     * because a _tls_used symbol is found ( type: IMAGE_THREAD_DIRECTORY )
     */
    if ( curr = (struct dsym *)SymSearch(".tls") ) {
	datadir[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress = curr->e.seginfo->start_offset;
	datadir[IMAGE_DIRECTORY_ENTRY_TLS].Size = curr->sym.max_offset;
    }

#ifndef __ASMC64__
    if ( ModuleInfo.defOfssize == USE64 ) {
#endif
	if ( curr = (struct dsym *)SymSearch( ".pdata" ) ) {
	    datadir[IMAGE_DIRECTORY_ENTRY_EXCEPTION].VirtualAddress = curr->e.seginfo->start_offset;
	    datadir[IMAGE_DIRECTORY_ENTRY_EXCEPTION].Size = curr->sym.max_offset;
	}
	cp->imagebase64 = GHF( OptionalHeader.ImageBase );
#ifndef __ASMC64__
    } else
	cp->imagebase = GHF( OptionalHeader.ImageBase );
#endif
}

/* v2.11: this function is called when the END directive has been found.
 * Previously the code was run inside EndDirective() directly.
 */

static int pe_enddirhook( struct module_info *modinfo )
/**********************************************************/
{
    pe_create_MZ_header( modinfo );
    //pe_create_PE_header(); /* the PE header is created when the .MODEL directive is found */
    pe_emit_export_data();
    if ( modinfo->g.DllQueue )
	pe_emit_import_data();
    pe_create_section_table();
    return( NOT_ERROR );
}

/* write section contents
 * this is done after the last step only!
 */

static ret_code bin_write_module( struct module_info *modinfo )
/*************************************************************/
{
    struct dsym *curr;
    uint_32 size;
    uint_32 sizetotal;
    int i;
    int first;
    uint_32 sizeheap;
    struct IMAGE_DOS_HEADER *pMZ;
    uint_16 reloccnt;
    uint_32 sizemem;
    struct dsym *stack = NULL;
    uint_8  *hdrbuf;
    struct calc_param cp = { TRUE, 0 };

    for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	/* reset the offset fields of segments */
	/* it was used to store the size in there */
	curr->e.seginfo->start_offset = 0;
	/* set STACK segment type */
	if ( curr->e.seginfo->combine == COMB_STACK )
	    curr->e.seginfo->segtype = SEGTYPE_STACK;
    }

    /* calculate size of header */
    switch( modinfo->sub_format ) {
    case SFORMAT_MZ:
	reloccnt = GetSegRelocs( NULL );
	cp.sizehdr = (reloccnt * 4 + modinfo->mz_data.ofs_fixups + (modinfo->mz_data.alignment - 1)) & ~(modinfo->mz_data.alignment-1);
	break;
    default:
	cp.sizehdr = 0;
    }
    cp.fileoffset = cp.sizehdr;

    if ( cp.sizehdr ) {
	hdrbuf = (unsigned char *)LclAlloc( cp.sizehdr );
	memset( hdrbuf, 0, cp.sizehdr );
    }
    cp.entryoffset = -1;

    /* set starting offsets for all sections */

    cp.rva = 0;
    if ( modinfo->sub_format == SFORMAT_PE || modinfo->sub_format == SFORMAT_64BIT ) {
	if ( ModuleInfo.model == MODEL_NONE ) {
	    return( asmerr( 2013 ) );
	}
	pe_set_values( &cp );
    } else
    if ( modinfo->segorder == SEGORDER_DOSSEG ) {
	/* for .DOSSEG, regroup segments (CODE, UNDEF, DATA, BSS) */
	for ( i = 0 ; i < SIZE_DOSSEG; i++ ) {
	    for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
		if ( curr->e.seginfo->segtype != dosseg_order[i] )
		    continue;
		CalcOffset( curr, &cp );
	    }
	}
	SortSegments( 0 );
    } else { /* segment order .SEQ (default) and .ALPHA */

	if ( modinfo->segorder == SEGORDER_ALPHA ) {
	    SortSegments( 1 );
	}
	for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	    /* ignore absolute segments */
	    CalcOffset( curr, &cp );
	}
    }
    /* handle relocs */
    for( curr = SymTables[TAB_SEG].head; curr; curr = curr->next ) {
	DoFixup( curr, &cp );
	if ( stack == NULL &&
	    curr->e.seginfo->combine == COMB_STACK )
	    stack = curr;
    }
    /* v2.04: return if any errors occured during fixup handling */
    if ( modinfo->g.error_count )
	return( ERROR );

    /* for plain binaries make sure the start label is at
     * the beginning of the first segment */
    if ( modinfo->sub_format == SFORMAT_NONE ) {
	if ( modinfo->g.start_label ) {
	    if ( cp.entryoffset == -1 || cp.entryseg != modinfo->g.start_label->segment ) {
		return( asmerr( 3003 ) );
	    }
	}
    }

    sizetotal = GetImageSize( FALSE );

    /* for MZ|PE format, initialize the header */

    switch ( modinfo->sub_format ) {
    case SFORMAT_MZ:
	/* set fields in MZ header */
	pMZ = (struct IMAGE_DOS_HEADER *)hdrbuf;
	pMZ->e_magic   = 'M' + ('Z' << 8);
	pMZ->e_cblp    = sizetotal % 512; /* bytes last page */
	pMZ->e_cp      = sizetotal / 512 + (sizetotal % 512 ? 1 : 0); /* pages */
	pMZ->e_crlc    = reloccnt;
	pMZ->e_cparhdr = cp.sizehdr >> 4; /* size header in paras */
	sizeheap = GetImageSize( TRUE ) - sizetotal;
	pMZ->e_minalloc = sizeheap / 16 + ((sizeheap % 16) ? 1 : 0); /* heap min */
	if ( pMZ->e_minalloc < modinfo->mz_data.heapmin )
	    pMZ->e_minalloc = modinfo->mz_data.heapmin;
	pMZ->e_maxalloc = modinfo->mz_data.heapmax; /* heap max */
	if ( pMZ->e_maxalloc < pMZ->e_minalloc )
	    pMZ->e_maxalloc = pMZ->e_minalloc;

	/* set stack if there's one defined */

	if ( stack ) {
	    uint_32 addr = stack->e.seginfo->start_offset;
	    if ( stack->e.seginfo->group )
		addr += stack->e.seginfo->group->offset;
	    pMZ->e_ss = (addr >> 4) + ((addr & 0xF) ? 1 : 0); /* SS */
	    /* v2.11: changed sym.offset to sym.max_offset */
	    pMZ->e_sp = stack->sym.max_offset; /* SP */
	} else {
	    asmerr( 8010 );
	}
	pMZ->e_csum = 0; /* checksum */

	/* set entry CS:IP if defined */

	if ( modinfo->g.start_label ) {
	    uint_32 addr;
	    curr = (struct dsym *)modinfo->g.start_label->segment;
	    if ( curr->e.seginfo->group ) {
		addr = curr->e.seginfo->group->offset;
		pMZ->e_ip = (addr & 0xF ) + curr->e.seginfo->start_offset + modinfo->g.start_label->offset; /* IP */
		pMZ->e_cs = addr >> 4; /* CS */
	    } else {
		addr = curr->e.seginfo->start_offset;
		pMZ->e_ip = (addr & 0xF ) + modinfo->g.start_label->offset; /* IP */
		pMZ->e_cs = addr >> 4; /* CS */
	    }
	} else {
	    asmerr( 8009 );
	}
	pMZ->e_lfarlc = modinfo->mz_data.ofs_fixups;
	GetSegRelocs( (uint_16 *)( hdrbuf + pMZ->e_lfarlc ) );
	break;
    }

    if( CurrFile[LST] ) {
	/* go to EOF */
	fseek( CurrFile[LST], 0, SEEK_END );
	LstNL();
	LstNL();
	LstPrintf( szCaption );
	LstNL();
	LstNL();
	LstPrintf( szCaption2 );
	LstNL();
	LstPrintf( szSep );
	LstNL();
    }

    if ( cp.sizehdr ) {
	if ( fwrite( hdrbuf, 1, cp.sizehdr, CurrFile[OBJ] ) != cp.sizehdr )
	    WriteError();
	LstPrintf( szSegLine, szHeader, 0, 0, cp.sizehdr, 0 );
	LstNL();
    }

    /* write sections */
    for( curr = SymTables[TAB_SEG].head, first = TRUE; curr; curr = curr->next ) {
	if ( curr->e.seginfo->segtype == SEGTYPE_ABS ) {
	    continue;
	}
	if ( ( ModuleInfo.sub_format == SFORMAT_PE || ModuleInfo.sub_format == SFORMAT_64BIT ) &&
	    ( curr->e.seginfo->segtype == SEGTYPE_BSS || curr->e.seginfo->info ) )
	    size = 0;
	else
	size = curr->sym.max_offset - curr->e.seginfo->start_loc;
	sizemem = first ? size : curr->sym.max_offset;
	/* if no bytes have been written to the segment, check if there's
	 * any further segments with bytes set. If no, skip write! */
	if ( curr->e.seginfo->bytes_written == 0 ) {
	    struct dsym *dir;
	    for ( dir = curr->next; dir; dir = dir->next )
		if ( dir->e.seginfo->bytes_written )
		    break;
	    if ( !dir ) {
		size = 0;
	    }
	}
	LstPrintf( szSegLine, curr->sym.name, curr->e.seginfo->fileoffset, first ? curr->e.seginfo->start_offset + curr->e.seginfo->start_loc : curr->e.seginfo->start_offset, size, sizemem );
	LstNL();
	if ( size != 0 && curr->e.seginfo->CodeBuffer ) {
	    fseek( CurrFile[OBJ], curr->e.seginfo->fileoffset, SEEK_SET );
	    if ( fwrite( curr->e.seginfo->CodeBuffer, 1, size, CurrFile[OBJ] ) != size )
		WriteError();
	}
	first = FALSE;
    }
    if ( modinfo->sub_format == SFORMAT_PE || ModuleInfo.sub_format == SFORMAT_64BIT ) {
	size = ftell( CurrFile[OBJ] );
	if ( size & ( cp.rawpagesize - 1 ) ) {
	    char *tmp;
	    size = cp.rawpagesize - ( size & ( cp.rawpagesize - 1 ) );
	    tmp = myalloca( size );
	    memset( tmp, 0, size );
	    fwrite( tmp, 1, size, CurrFile[OBJ] );
	}
    }
    LstPrintf( szSep );
    LstNL();
    if ( modinfo->sub_format == SFORMAT_MZ )
	sizeheap += sizetotal - cp.sizehdr;
    else
    if ( modinfo->sub_format == SFORMAT_PE || ModuleInfo.sub_format == SFORMAT_64BIT )
	sizeheap = cp.rva;
    else
	sizeheap = GetImageSize( TRUE );
    LstPrintf( szTotal, " ", sizetotal, sizeheap );
    LstNL();

    return( NOT_ERROR );
}

static int bin_check_external( struct module_info *modinfo )
/***************************************************************/
{
    struct dsym *curr;
    for ( curr = SymTables[TAB_EXT].head; curr != NULL ; curr = curr->next )
	if( curr->sym.weak == FALSE || curr->sym.used == TRUE ) {
	    return( asmerr( 2014, curr->sym.name ) );
	}
    return( NOT_ERROR );
}


void bin_init( struct module_info *modinfo )
/******************************************/
{
    modinfo->g.WriteModule = bin_write_module;
    modinfo->g.Pass1Checks = bin_check_external;
    switch ( modinfo->sub_format ) {
#ifndef __ASMC64__
    case SFORMAT_MZ:
	memcpy( &modinfo->mz_data, &mzdata, sizeof( struct MZDATA ) );
	break;
#endif
    case SFORMAT_PE:
    case SFORMAT_64BIT:
	modinfo->g.EndDirHook = pe_enddirhook; /* v2.11 */
	break;
    }
    return;
}
