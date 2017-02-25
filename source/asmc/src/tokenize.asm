include ctype.inc
include string.inc

include asmc.inc
include token.inc

GetTextLine		PROTO :DWORD
FindResWord		PROTO FASTCALL :DWORD, :DWORD
conditional_assembly_prepare PROTO :DWORD

INST_ALLOWED_PREFIX	equ 007h
INST_FIRST		equ 008h
INST_RM_INFO		equ 010h
INST_OPND_DIR		equ 0E0h

instr_item		STRUC
opclsidx		db ?	; v2.06: index for opnd_clstab
byte1_info		db ?	; flags for 1st byte
flags			db ?
reserved		db ?	; not used yet
cpu			dw ?
opcode			db ?	; opcode byte
rm_byte			db ?	; mod_rm_byte
instr_item		ENDS

externdef		InstrTable:instr_item
externdef		token_stringbuf:DWORD
externdef		commentbuffer:DWORD
externdef		CurrIfState:DWORD
externdef		optable_idx:WORD

special_item		STRUC
value			dd ?
sflags			dd ?
cpu			dw ?
bytval			db ?
_type			db ?
special_item		ENDS
externdef		SpecialTable:special_item


line_status		STRUC
input			dd ?
output			dd ?	; free space in token string buffer
start			dd ?	; start of line
index			dd ?	; index token array
flags			db ?	; v2.08: added
flags2			db ?	; v2.08: was global var g_flags
flags3			db ?	; v2.08: added
line_status		ENDS

TOK_DEFAULT		equ 0	; default mode - handle conditional assembly
TOK_RESCAN		equ 1	; retokenize after expansion - ignore conditional assembly
TOK_NOCURLBRACES	equ 2	; don't handle {}-literals
TOK_LINE		equ 4	; full line is retokenized

TF3_ISCONCAT		equ 1	; line was concatenated
TF3_EXPANSION		equ 2	; expansion operator % at pos 0

.data
;
; strings for token 0x28 - 0x2F
;
stokstr1	dw '(',')','*','+',',','-','.','/'
;
; strings for token 0x5B - 0x5D
;
stokstr2	dw '[',0,']'

__equ		db '=',0
__dcolon	db ':'
__colon		db ':',0
__amp		db '&',0
__percent	db '%',0
__quest		db '?',0
__null		db 0

_cstring	db 0	; allow \" in string

	.code

	OPTION PROCALIGN:4
	OPTION PROC: PRIVATE
	ASSUME	ecx: PTR asm_tok

IsMultiLine PROC FASTCALL tokenarray
	;
	; test line concatenation if last token is a comma.
	; dont concat EQU, macro invocations or
	; - ECHO
	; - FORC/IRPC (v2.0)
	; - INCLUDE (v2.8)
	; lines!
	; v2.05: don't concat if line's an instruction.
	;
	.if	[ecx+16].token == T_DIRECTIVE && [ecx+16].tokval == T_EQU

		xor eax,eax
		ret
	.endif

	.if	[ecx+16].token == T_COLON

		add ecx,16 * 2
	.endif

	movzx	eax,[ecx].token
	.switch eax

	  .case T_DIRECTIVE

		mov	ecx,[ecx].tokval
		.switch ecx

		  .case T_ECHO
		  .case T_INCLUDE
		  .case T_FORC
		  .case T_IRPC

			xor eax,eax
			ret
		.endsw
		.endc

	  .case T_INSTRUCTION

		xor eax,eax
		ret

	  .case T_ID
		;
		; don't concat macros
		;
		.if	SymFind( [ecx].string_ptr )

			.if	[eax].asym.state == SYM_MACRO && \
				!([eax].asym.mac_flag & SMAC_MULTILINE)

				xor eax,eax
				ret
			.endif
		.endif

	.endsw
	mov	eax,1
	ret
IsMultiLine ENDP

	ASSUME edx: PTR line_status

ConcatLine PROC USES esi edi edx ecx src, cnt, o, ls

	mov	eax,src
	mov	esi,eax
	add	eax,1

	.if	!M_EAT_SPACE(ecx, eax) || ecx == ';'

		mov	edi,o

		.if	GetTextLine(edi)

			M_SKIP_SPACE eax, edi
			strlen( edi )

			.if	!cnt

				mov BYTE PTR [esi],' '
				inc esi
			.endif

			mov	edx,ls
			mov	ecx,esi
			sub	ecx,[edx].start
			add	ecx,eax

			.if	ecx >= MAX_LINE_LEN

				asmerr( 2039 )
				mov	ecx,esi
				sub	ecx,[edx].start
				inc	ecx
				mov	eax,MAX_LINE_LEN
				sub	eax,ecx
				mov	BYTE PTR [edi+eax],0
			.endif

			lea	ecx,[eax+1]
			xchg	esi,edi
			rep	movsb
			mov	eax,NOT_ERROR
			jmp	toend
		.endif
	.endif

	mov	eax,EMPTY
toend:
	ret
ConcatLine ENDP

	ASSUME ecx: NOTHING
	ASSUME ebx: PTR asm_tok

get_string PROC USES esi edi ebx buf, p

  local symbol_c:	BYTE,
	symbol_o:	BYTE,
	delim:		BYTE,
	level:		DWORD,
	tdst:		DWORD,
	tsrc:		DWORD,
	tcount:		DWORD

	mov	edx,p
	mov	ebx,buf
	mov	esi,[edx].input
	mov	edi,[edx].output
	movzx	eax,BYTE PTR [esi]
	xor	ecx,ecx
	mov	symbol_o,al

	.switch eax

	  .case '"'
	  .case 27h

		mov	[ebx].string_delim,al
		mov	ah,al
		movsb

		.repeat
			mov	al,[esi]
			.switch
			  .case !al
				;
				; missing terminating quote, change to undelimited string
				;
				mov	 [ebx].string_delim,al
				;
				; count the first quote
				;
				add	 ecx,1
				.break

			  .case al == _cstring		; case \" ?
				cmp	BYTE PTR [esi-1],'\'
				je	@F
			  .case al == ah		; another quote?
				mov	[edi],al
				add	edi,1
				add	esi,1
				.break .if [esi] != al	; exit loop
				sub	edi,1
			  .default
			   @@:
				mov	[edi],al
				add	edi,1
				add	esi,1
				add	ecx,1
				.endc
			.endsw
		.until ecx >= MAX_STRING_LEN
		;
		; end of string marker is the same
		;
		.endc

	  .case '{'

		test	[edx].flags,TOK_NOCURLBRACES
		jnz	default

		mov	symbol_c,'}'

	  .case '<'

		mov	[ebx].string_delim,al
		mov	level,ecx

		.if	al == '<'

			mov symbol_c,'>'
		.endif
		add	esi,1

		.while	ecx < MAX_STRING_LEN

			mov	ax,[esi]
			.if	al == symbol_o		; < or { ?

				inc level
			.elseif al == symbol_c	; > or } ?

				.if	level

					dec level
				.else
					;
					; store the string delimiter unless it is <>
					; v2.08: don't store delimiters for {}-literals
					;
					add esi,1
					.break		; exit loop
				.endif

			.elseif ( al == '"' || al == 27h ) && !( [edx].flags2 & DF_STRPARM )
				;
				; a " or ' inside a <>/{} string? Since it's not a must that
				; [double-]quotes are paired in a literal it must be done
				; directive-dependant!
				; see: IFIDN <">,<">
				;
				mov	delim,al
				movsb
				add	ecx,1
				mov	tdst,edi
				mov	tsrc,esi
				mov	tcount,ecx
				mov	ax,[esi]

				.while	al != delim && al && ecx < MAX_STRING_LEN - 1

					.if	symbol_o == '<' && al == '!' && ah

						add esi,1
					.endif
					movsb
					add	ecx,1
					mov	ax,[esi]
				.endw

				.if	al != delim
					mov	esi,tsrc
					mov	edi,tdst
					mov	ecx,tcount
					.continue
				.endif
			.elseif al == '!' && symbol_o == '<' && ah
				;
				; handle literal-character operator '!'.
				; it makes the next char to enter the literal uninterpreted.
				;
				; v2.09: don't store the '!'
				;
				add esi,1
			.elseif al == '\'

				.if	ConcatLine(esi, ecx, edi, edx) != EMPTY

					or [edx].flags3,TF3_ISCONCAT
					.continue
				.endif
			.elseif !al || ( al == ';' && symbol_o == '{' )

				.if	[edx].flags == TOK_DEFAULT && !([edx].flags2 & DF_NOCONCAT)
					;
					; if last nonspace character was a comma
					; get next line and continue string scan
					;
					mov	tdst,edi
					mov	tcount,ecx
					sub	edi,1

					.if	M_EAT_SPACE_R(eax, edi) == ','

						strlen( [edx].output )
						add    eax,4
						and    eax,-4
						add    eax,[edx].output
						mov    edi,eax

						.if	GetTextLine(eax)

							M_SKIP_SPACE eax, edi
							strlen( edi )
							add	eax,tcount
							.if	eax >= MAX_LINE_LEN

								asmerr( 2039 )
								jmp    toend
							.endif
							strcpy( esi, edi )
							mov	edi,tdst
							mov	ecx,tcount
							mov	edx,p
							.continue
						.endif
					.endif
				.endif
				mov	edx,p
				mov	esi,[edx].input
				mov	edi,[edx].output
				movsb
				mov	ecx,1
				jmp	default
			.endif
			movsb
			add	ecx,1
		.endw
		.endc

	  .default
	   default:

		mov	[ebx].string_delim,0
		;
		; this is an undelimited string,
		; so just copy it until we hit something that looks like the end.
		; this format is used by the INCLUDE directive, but may also
		; occur inside the string macros!
		;
		; v2.05: also stop if a ')' is found - see literal2.asm regression test
		;
		.while	ecx < MAX_STRING_LEN

			movzx	eax,BYTE PTR [esi]

			.break .if !eax
			.break .if BYTE PTR _ctype[eax*2+2] & _SPACE
			.break .if eax == ','
			.break .if eax == ')'
			.break .if eax == '%'
			.break .if eax == ';' && [edx].line_status.flags == TOK_DEFAULT

			.if	eax == '\'

				.if	[edx].flags == TOK_DEFAULT || [edx].flags & TOK_LINE

					.if	ConcatLine(esi, ecx, edi, edx) != EMPTY

						or	[edx].flags3,TF3_ISCONCAT
						.continue .if ecx

						mov   eax,EMPTY
						jmp   toend
					.endif
				.endif
			.elseif eax == '!' && BYTE PTR [esi+1] && ecx < MAX_STRING_LEN - 1
				;
				; v2.08: handle '!' operator
				;
				movsb
			.endif
			movsb
			add	ecx,1
		.endw
	.endsw

	.if	ecx == MAX_STRING_LEN

		asmerr( 2041 )
	.else
		xor	eax,eax
		mov	[edi],al
		add	edi,1
		mov	[ebx].token,T_STRING
		mov	[ebx].stringlen,ecx
		mov	[edx].input,esi
		mov	[edx].output,edi
	.endif
toend:
	mov	_cstring,0
	ret
get_string ENDP

	ASSUME edx: NOTHING
	ASSUME esi: PTR line_status

get_special_symbol PROC FASTCALL USES esi edi ebx buf, p

	mov	ebx,buf
	mov	esi,p
	mov	edi,[esi].input
	mov	eax,[edi]

	.switch al

	  .case ':' ; T_COLON binary operator (0x3A)

		inc	[esi].input
		lea	ecx,__dcolon

		.if	ah == ':'

			inc	[esi].input
			mov	[ebx].token,T_DBL_COLON
			mov	[ebx].string_ptr,ecx
		.else
			inc	ecx
			mov	[ebx].token,T_COLON
			mov	[ebx].string_ptr,ecx
		.endif

		.endc

	  .case '%' ; T_PERCENT (0x25)

		shr	eax,8
		or	eax,202020h

		.if	eax == 'tuo'	; %OUT directive?

			mov	eax,[esi].input
			movzx	eax,BYTE PTR [eax+4]

			.if	!( __ltype[eax+1] & _LABEL or _DIGIT )

				mov	[ebx].token,T_DIRECTIVE
				mov	[ebx].tokval,T_ECHO
				mov	[ebx].dirtype,DRT_ECHO
				mov	edi,[esi].output
				mov	ecx,[esi].input
				add	[esi].input,4
				add	[esi].output,5
				mov	eax,[ecx]
				mov	[edi],eax
				xor	eax,eax
				mov	[edi+4],al
				.endc
			.endif
		.endif

		inc	[esi].input
		.if	[esi].flags == TOK_DEFAULT && [esi].index == 0

			or	[esi].flags3,TF3_EXPANSION
			mov	eax,EMPTY
			jmp	toend
		.endif

		lea	eax,__percent
		mov	[ebx].token,T_PERCENT
		mov	[ebx].string_ptr,eax

		.endc

	  .case '('
		;
		; 0x28: T_OP_BRACKET operator - needs a matching ')'
		; v2.11: reset c-expression flag if a macro function call is detected
		; v2.20: set _cstring to allow escape chars (\") in @CStr( string )
		; v2.20: removed auto off switch for asmc_syntax in macros
		; v2.20: added more test code for label() calls
		;
		.if	ah == ')' && [ebx-16].token == T_REG
			;
			; REG() expans as CALL REG
			;
			; TODO: [...]()
			;
			; ( [ebx-16].token == T_REG || [ebx-16].token == T_CL_SQ_BRACKET )
			;
			or	[ebx-16].hll_flags,T_HLL_PROC

		.elseif [esi].index && [ebx-16].token == T_ID

			.if	SymFind( [ebx-16].string_ptr )

				xor	ecx,ecx
				movzx	edx,[eax].asym.state
				.switch
				  .case !ModuleInfo.asmc_syntax
					.endc .if edx != SYM_MACRO
					.endc .if !([eax].asym.mac_flag & SMAC_ISFUNC)
					and	[esi].flags2,not DF_CEXPR
					.endc

				  .case edx == SYM_MACRO
					.if	[eax].asym.mac_flag & SMAC_ISFUNC
						and	[esi].flags2,not DF_CEXPR
						.if	[eax].asym.flag & SFL_PREDEFINED
							mov	edx,[eax].asym._name
							mov	edx,[edx]
							.if	edx == "tSC@"
								mov _cstring,'"'
								.endc
							.endif
						.endif
						mov	ecx,T_HLL_MACRO
						.endc
					.endif
					.endc	.if [eax].asym.flag & SFL_PREDEFINED
					.endc	.if ![eax].asym.string_ptr
					.endc	.if !SymFind( [eax].asym.string_ptr )
					xor	ecx,ecx
					.endc	.if !( [eax].asym.flag & SFL_ISPROC )
					mov	ecx,T_HLL_PROC
					.endc

				  .case edx == SYM_STACK
				  .case edx == SYM_INTERNAL
				  .case edx == SYM_EXTERNAL
				  .case [eax].asym.flag & SFL_ISPROC
					mov	ecx,T_HLL_PROC
					.endc
				  .case edx == SYM_UNDEFINED
					.endc	.if BYTE PTR [edi+1] != ')'
					mov	ecx,T_HLL_PROC
					.endc
				.endsw
				or	[ebx-16].hll_flags,cl

			.elseif BYTE PTR [edi+1] == ')' ;&& [ebx-32].token == T_DIRECTIVE
				;
				; undefined code label..
				;
				; label() or .if label()
				; ...
				; label:
				;
				or	[ebx-16].hll_flags,T_HLL_PROC
			.endif
			mov	eax,[edi]
		.endif
		;
		; no break
		;
	  .case ')'..'/'
		;
		; ')' : 0x29: T_CL_BRACKET
		; '*' : 0x2A: binary operator
		; '+' : 0x2B: unary|binary operator
		; ',' : 0x2C: T_COMMA
		; '-' : 0x2D: unary|binary operator
		; '.' : 0x2E: T_DOT binary operator
		; '/' : 0x2F: binary operator
		;
		; all of these are themselves a token
		;
		inc	[esi].input
		mov	[ebx].token,al
		mov	[ebx].specval,0 ; initialize, in case the token needs extra data
		movzx	eax,al		; v2.06: use constants for the token string
		sub	al,'('
		lea	eax,stokstr1[eax*2]
		mov	[ebx].string_ptr,eax
		.endc

	  .case '[' ; T_OP_SQ_BRACKET operator - needs a matching ']' (0x5B)
	  .case ']' ; T_CL_SQ_BRACKET (0x5D)

		inc	[esi].input
		mov	[ebx].token,al
		movzx	eax,al
		sub	al,'['
		lea	eax,stokstr2[eax*2]
		mov	[ebx].string_ptr,eax
		.endc

	  .case '=' ; (0x3D)

		.if	ah != '='
			mov	[ebx].token,T_DIRECTIVE
			mov	[ebx].tokval,T_EQU
			mov	[ebx].dirtype,DRT_EQUALSGN ;  to make it differ from EQU directive
			lea	eax,__equ
			mov	[ebx].string_ptr,eax
			inc	[esi].input
			.endc
		.endif
		;
		; fall thru
		;
	  .default
		;
		; detect C style operators.
		; DF_CEXPR is set if .IF, .WHILE, .ELSEIF or .UNTIL
		; has been detected in the current line.
		; will catch: '!', '<', '>', '&', '==', '!=', '<=', '>=', '&&', '||'
		; A single '|' will also be caught, although it isn't a valid
		; operator - it will cause a 'operator expected' error msg later.
		; the tokens are stored as one- or two-byte sized "strings".
		;
		mov	edx,eax
		strchr( "=!<>&|", eax )

		.if	eax && [esi].flags2 & DF_CEXPR

			mov	al,[eax]
			mov	ecx,[esi].output
			inc	[esi].output
			mov	[ecx],al
			inc	[esi].input
			mov	[ebx].stringlen,1
			mov	edx,[esi].input
			.if	al == '&' || al == '|'

				.if	[edx] == al

					add	ecx,1
					mov	[ecx],al
					inc	[esi].output
					inc	[esi].input
					mov	[ebx].stringlen,2
				.endif
			.elseif BYTE PTR [edx] == '='

				add	ecx,1
				mov	BYTE PTR [ecx],'='
				inc	[esi].output
				inc	[esi].input
				mov	[ebx].stringlen,2
			.endif
			xor	eax,eax
			mov	[ebx].token,T_STRING
			mov	[ebx].string_delim,al
			mov	[ecx+1],al
			inc	[esi].output
			.endc
		.endif

		.if	dl == '&'	; v2.08: ampersand is a special token
			inc	[esi].input
			mov	[ebx].token,dl
			lea	eax,__amp
			mov	[ebx].string_ptr,eax
			.endc
		.endif
		;
		; anything we don't recognise we will consider a string,
		; delimited by space characters, commas, newlines or nulls
		;
		get_string( ebx, esi )
		jmp	toend
	.endsw

	xor	eax,eax
toend:
	ret
get_special_symbol ENDP

CHEXPREFIX equ 1

get_number PROC FASTCALL USES esi edi ebx buf, p

	mov	ebx,buf
	mov	esi,p
	mov	edx,[esi].input
	xor	edi,edi

ifdef CHEXPREFIX
	push	ebp
	movzx	ebp,WORD PTR [edx]
	or	ebp,00002000h
	.if	ebp == 'x0'

		add	edx,2
	.endif
endif

	ALIGN	4
	;
	; Read numbers 0..9 and a..f
	;
	.while	1
		movzx	ecx,BYTE PTR [edx]
		sub	ecx,'A'			; tolower(ecx)
		cmp	ecx,'Z' - 'A' + 1
		sbb	eax,eax
		and	eax,'a' - 'A'
		add	ecx,eax
		add	ecx,'A'
		cmp	ecx,'a'			; break if cl > '0' && cl < 'a'
		sbb	eax,eax
		cmp	ecx,'9' + 1
		adc	eax,0
		.break .if !ZERO?
		cmp	ecx,'f' + 1		; 'a'..'f' --> 10..15
		sbb	eax,eax
		cmp	ecx,'a'
		adc	eax,0
		and	eax,'a' - '9' - 1
		sub	ecx,eax
		.break .if ecx < '0'		; validate --> 0..15
		.break .if ecx > '0' + 15
		sub	ecx,'0'
		mov	eax,1
		shl	eax,cl
		or	edi,eax
		add	edx,1
	.endw

	mov	eax,ecx
	mov	ecx,edx
ifdef	CHEXPREFIX
	.if	ebp == 'x0'
		mov	eax,16
		.if	!edi
			xor	eax,eax
		.endif
	.else
endif

	.switch eax

	  .case '.'
		;
		; valid floats look like: (int)[.(int)][e(int)]
		; Masm also allows hex format, terminated by 'r' (3F800000r)
		;
		xor	edi,edi
		mov	eax,1
		.while	al

			mov	al,[edx]
			.if	eax == '.'
				add	ah,1
			.elseif al < '0' || al > '9'
				or	al,20h
				.break .if al != 'e'
				.break .if edi
				mov	edi,1
				;
				; accept e+2 / e-4 /etc.
				;
				mov al,[edx+1]
				.if al == '+' || al == '-'
					add edx,1
				.endif
			.endif
			add	edx,1
		.endw
		mov	[ebx].token,T_FLOAT
		mov	[ebx].floattype,0
		jmp	number_done

	  .case 'r'

		mov    [ebx].token,T_FLOAT
		mov    [ebx].floattype,'r'
		inc    edx
		jmp    number_done

	  .case 'h'

		mov    eax,16
		inc    edx
		.endc

	  .case 'y'

		xor	eax,eax
		.endc .if !(edi & 3)
		mov	eax,2
		inc	edx
		.endc

	  .case 't'

		xor	eax,eax
		.endc .if !(edi & 03FFh)
		mov	eax,10
		inc	edx
		.endc

	  .case 'q'
	  .case 'o'

		xor	eax,eax
		.endc .if !(edi & 00FFh)
		mov	eax,8
		inc	edx
		.endc

	  .default

		mov	cl,ModuleInfo.radix
		mov	eax,1
		shl	eax,cl
		dec	edx
		mov	cl,[edx]
		or	cl,20h

		.if	(cl == 'b' || cl == 'd') && edi >= eax

			mov	eax,[esi].input
			mov	ch,'1'

			.if	cl != 'b'
				mov	ch,'9'
			.endif
			.while	eax < edx && BYTE PTR [eax] <= ch
				add	eax,1
			.endw
			.if	eax == edx
				mov	eax,2
				.if	cl != 'b'
					mov	eax,10
				.endif
				mov	ecx,edx
				add	edx,1
				.endc
			.endif
		.endif

		inc	edx
		mov	cl,ModuleInfo.radix
		mov	eax,1
		shl	eax,cl
		cmp	edi,eax
		mov	eax,0
		mov	ecx,edx
		.endc	.if !CARRY?
		movzx	eax,ModuleInfo.radix
	.endsw
ifdef	CHEXPREFIX
	.endif
endif
	.if	eax
		mov	[ebx].token,T_NUM
		mov	[ebx].numbase,al
		sub	ecx,[esi].input
		mov	[ebx].itemlen,ecx
	.else
		mov	[ebx].token,T_BAD_NUM
		movzx	eax,BYTE PTR [edx]
		.while	__ltype[eax+1] & _LABEL or _DIGIT
			add	edx,1
			mov	al,[edx]
		.endw
	.endif

number_done:
	mov	ecx,edx
	mov	edx,esi
	mov	edi,[edx].line_status.output
	mov	esi,[edx].line_status.input
	sub	ecx,esi
	rep	movsb
	xor	eax,eax
	mov	[edi],al
	add	edi,1
	mov	[edx].line_status.input,esi
	mov	[edx].line_status.output,edi
toend:
ifdef CHEXPREFIX
	pop	ebp
endif
	ret
get_number ENDP

	ASSUME edx: PTR line_status

get_id_in_backquotes PROC FASTCALL USES esi edi ebx buf, p

	mov	ebx,buf
	inc	[edx].input
	mov	esi,[edx].input
	mov	edi,[edx].output
	mov	[ebx].token,T_ID
	mov	[ebx].idarg,0
	movzx	eax,BYTE PTR [esi]
	.while	eax != '`'
		test	eax,eax
		jz	error
		cmp	eax,';'
		je	error
		lodsb
		stosb
		inc	[edx].input
	.endw

	inc	[edx].input
	xor	eax,eax
	stosb
	mov	[edx].output,edi
toend:
	ret
error:
	mov	BYTE PTR [edi],0
	asmerr( 2046 )
	jmp	toend
get_id_in_backquotes ENDP

get_id	PROC FASTCALL USES esi edi ebx buf, p

	mov	ebx,buf
	mov	esi,[edx].input
	mov	edi,[edx].output
	movzx	eax,BYTE PTR [esi]

	.repeat
		mov	[edi],al
		add	edi,1
		add	esi,1
		mov	al,[esi]
	.until !(__ltype[eax+1] & _LABEL or _DIGIT)

	mov	ecx,edi
	sub	ecx,[edx].output

	.if	ecx > MAX_ID_LEN

		asmerr( 2043 )
		mov	edi,[edx].output
		add	edi,MAX_ID_LEN
	.endif

	mov	BYTE PTR [edi],0
	add	edi,1
	mov	eax,[edx].output
	mov	al,[eax]

	.if	ecx == 1 && al == '?'

		mov	[edx].input,esi
		mov	[ebx].token,T_QUESTION_MARK
		mov	[ebx].string_ptr,offset __quest
		jmp	toend
	.endif

	push	edx
	mov	edx,[edx].output
	xchg	edx,ecx
	call	FindResWord
	pop	edx

	.if	!eax

		mov	eax,[edx].output
		mov	al,[eax]
		.if	al == '.' && !ModuleInfo.dotname

			mov	[ebx].token,T_DOT
			lea	eax,stokstr1[('.' - '(') * 2]
			mov	[ebx].string_ptr,eax
			inc	[edx].input
			jmp	toend
		.endif
		mov	[edx].input,esi
		mov	[edx].output,edi
		mov	[ebx].token,T_ID
		mov	[ebx].idarg,0
		jmp	toend
	.endif

	mov	[edx].input,esi
	mov	[edx].output,edi
	mov	[ebx].tokval,eax

	.if	eax == T_DOT_ELSEIF || eax == T_DOT_WHILE || eax == T_DOT_CASE

		mov	[ebx].hll_flags,T_HLL_DELAY
	.endif

	.if	eax >= SPECIAL_LAST

		.if	ModuleInfo.m510

			mov   eax,[ebx].tokval
			sub   eax,SPECIAL_LAST
			movzx eax,optable_idx[eax*2]
			movzx ecx,InstrTable[eax*sizeof(instr_item)].cpu
			mov   eax,ecx
			and   eax,P_CPU_MASK
			and   ecx,P_EXT_MASK
			mov   edi,ModuleInfo.curr_cpu
			mov   esi,edi
			and   edi,P_CPU_MASK
			and   esi,P_EXT_MASK
			.if	eax > edi || ecx > esi

				mov [ebx].token,T_ID
				mov [ebx].idarg,0
				jmp toend
			.endif
		.endif
		mov	[ebx].token,T_INSTRUCTION
		jmp	toend
	.endif

	mov	ecx,edx
	mov	edx,sizeof(special_item)
	mov	eax,[ebx].tokval
	mul	edx
	mov	edx,ecx
	mov	esi,eax
	mov	al,SpecialTable[esi].bytval
	mov	[ebx].bytval,al
	movzx	eax,SpecialTable[esi]._type

	.switch eax
	  .case RWT_REG
		mov	ecx,T_REG
		.endc
	  .case RWT_DIRECTIVE
		mov	ecx,T_DIRECTIVE
		.endc	.if [edx].flags2
		mov	eax,SpecialTable[esi].value
		mov	[edx].flags2,al
		.endc
	  .case RWT_UNARY_OP
		mov	ecx,T_UNARY_OPERATOR
		.endc
	  .case RWT_BINARY_OP
		mov	ecx,T_BINARY_OPERATOR
		.endc
	  .case RWT_STYPE
		mov	ecx,T_STYPE
		.endc
	  .case RWT_RES_ID
		mov	ecx,T_RES_ID
		.endc
	  .default
		mov	ecx,T_ID
		mov	[ebx].idarg,0
	.endsw
	mov	[ebx].token,cl
toend:
	xor	eax,eax
	ret
get_id	ENDP

;
; save symbols in IFDEF's so they don't get expanded
;
StartComment PROC FASTCALL p

	mov	eax,p
	.if	M_EAT_SPACE( ecx, eax )

		mov	ModuleInfo.inside_comment,cl
		add	eax,1
		.if	strchr( eax, ecx )

			mov ModuleInfo.inside_comment,0
		.endif
		ret
	.else
		asmerr( 2110 )
	.endif
	ret
StartComment ENDP

	OPTION PROC: PUBLIC
	ASSUME esi:  NOTHING

GetToken PROC FASTCALL tokenarray, p
	;
	; get one token.
	; possible return values: NOT_ERROR, ERROR, EMPTY.
	;
	; names beginning with '.' are difficult to detect,
	; because the dot is a binary operator. The rules to
	; accept a "dotted" name are:
	; 1.- a valid ID char is to follow the dot
	; 2.- if buffer index is > 0, then the previous item
	;     must not be a reg, ), ] or an ID.
	; [bx.abc]    -> . is an operator
	; ([bx]).abc  -> . is an operator
	; [bx].abc    -> . is an operator
	; varname.abc -> . is an operator
	;
	mov	[ecx].asm_tok.hll_flags,0
	mov	eax,[edx].input
	movzx	eax,BYTE PTR [eax]

	.switch
	  .case BYTE PTR _ctype[eax*2+2] & _DIGIT
		jmp	get_number
	  .case __ltype[eax+1] & _LABEL
		jmp	get_id
	  .case al == '`'
		.endc .if Options.strict_masm_compat
		jmp	get_id_in_backquotes
	  .case al == '.'
		mov	eax,[edx].input
		movzx	eax,BYTE PTR [eax+1]
		.endc .if !( __ltype[eax+1] & _LABEL or _DIGIT )
		movzx	eax,[ecx-16].asm_tok.token
		.if	![edx].index || \
			(eax != T_REG && eax != T_CL_BRACKET && eax != T_CL_SQ_BRACKET && eax != T_ID)

			jmp get_id
		.endif
	.endsw
	jmp	get_special_symbol
toend:
	ret
GetToken ENDP

Tokenize PROC USES esi edi ebx line, start, tokenarray:PTR asm_tok, flags

  local rc, p:line_status
;
; create tokens from a source line.
; line:	 the line which is to be tokenized
; start: where to start in the token buffer. If start == 0,
;	 then some variables are additionally initialized.
; flags: 1=if the line has been tokenized already.
;
	mov	eax,line
	mov	p.input,eax
	mov	p.start,eax
	mov	eax,flags
	mov	p.flags,al
	mov	eax,start
	mov	p.index,eax
	mov	p.flags2,0
	mov	p.flags3,0
	mov	_cstring,0

	.if	!eax

		mov	eax,token_stringbuf
		mov	p.output,eax
		.if	ModuleInfo.inside_comment

			.if	strchr( line, ModuleInfo.inside_comment )

				mov ModuleInfo.inside_comment,0
			.endif
			jmp	skipline
		.endif
	.else
		mov	eax,ModuleInfo.stringbufferend
		mov	p.output,eax
	.endif

	.while	1

		mov	eax,p.input
		M_SKIP_SPACE ecx, eax
		mov	edx,eax
		mov	p.input,eax

		.if	ecx == ';' && flags == TOK_DEFAULT

			.while	edx > line
				movzx	eax,BYTE PTR [edx-1]
				.break	.if !BYTE PTR _ctype[eax*2+2] & _SPACE
				sub	edx,1
			.endw

			mov	p.input,edx
			strcpy( commentbuffer, edx )
			mov	ModuleInfo.CurrComment,eax
			mov	BYTE PTR [edx],0
		.endif

		mov	ebx,p.index
		shl	ebx,4
		add	ebx,tokenarray
		mov	[ebx].tokpos,edx

		.if	BYTE PTR [edx] == 0
			;
			; if a comma is last token, concat lines ... with some exceptions
			; v2.05: moved from PreprocessLine(). Moved because the
			; concatenation may be triggered by a comma AFTER expansion.
			;
			.if	p.index > 1 && \
				([ebx-16].token == T_COMMA || [ebx-16].token == T_OP_BRACKET) && \
				( Parse_Pass == PASS_1 || !UseSavedState ) && !start

				.if	IsMultiLine( tokenarray )

					strlen( p.output )
					add	eax,4
					and	eax,-4
					add	eax,p.output
					mov	edi,eax

					.if	GetTextLine(eax)

						.if	M_EAT_SPACE(eax, edi)

							strcpy( p.input, edi )

							.if	strlen( p.start ) >= MAX_LINE_LEN

								asmerr( 2039 )
								mov	eax,start
								mov	p.index,eax
								.break
							.endif
							.continue
						.endif
					.endif
				.endif
			.endif
			.break
		.endif

		mov	eax,p.output
		mov	[ebx].string_ptr,eax
		GetToken( ebx, addr p )
		mov	rc,eax

		.continue .if eax == EMPTY

		.if	eax == ERROR
			;
			; skip this line
			;
			mov    eax,start
			mov    p.index,eax
			.break
		.endif

		;
		; v2.04: this has been moved here from condasm.c to
		; avoid problems with (conditional) listings. It also
		; avoids having to search for the first token twice.
		; Note: a conditional assembly directive within an
		;	inactive block and preceded by a label isn't detected!
		;	This is an exact copy of the Masm behavior, although
		;	it probably is just a bug!
		;
		.if	!(flags & TOK_RESCAN)

			mov	eax,tokenarray
			movzx	ecx,[eax+16].asm_tok.token
			mov	eax,p.index

			.if	!eax || (eax == 2 && (ecx == T_COLON || ecx == T_DBL_COLON))

				mov	ecx,[ebx].tokval
				.if	[ebx].token == T_DIRECTIVE && \
					([ebx].bytval == DRT_CONDDIR || ecx == T_DOT_ASSERT)

					.if	ecx == T_COMMENT
						;
						; p.index is 0 or 2
						;
						StartComment( p.input )
						.break
					.endif

					.if	ecx == T_DOT_ASSERT
						test	ModuleInfo.asmc_syntax,ASMCFLAG_ASSERT
						jnz	@F
						mov	eax,p.input
						M_SKIP_SPACE ecx, eax
						cmp	cl,':'
						jne	@F
						inc	eax
						M_SKIP_SPACE ecx, eax
						mov	eax,[eax]
						or	eax,20202020h
						cmp	eax,'sdne'
						jne	@F
						mov	ecx,T_ENDIF
					.endif

					conditional_assembly_prepare( ecx )
					.if	CurrIfState != BLOCK_ACTIVE
						;
						; p.index is 1 or 3
						;
						inc	p.index
						.break
					.endif

				.else
				@@:
					.if	CurrIfState != BLOCK_ACTIVE
						;
						; further processing skipped. p.index is 0
						;
						.break
					.endif
				.endif
			.endif
		.endif

		inc	p.index
		.if	p.index >= MAX_TOKEN

			asmerr( 2141 )
			mov	eax,start
			mov	p.index,eax
			jmp	skipline
		.endif

		mov	eax,p.output
		sub	eax,token_stringbuf
		add	eax,4
		and	eax,-4
		add	eax,token_stringbuf
		mov	p.output,eax

	.endw

	mov	eax,p.output
	sub	eax,token_stringbuf
	add	eax,4
	and	eax,-4
	add	eax,token_stringbuf
	mov	p.output,eax
	mov	ModuleInfo.stringbufferend,eax

skipline:
	mov	ebx,tokenarray
	mov	eax,p.index
	shl	eax,4
	add	ebx,eax
	mov	[ebx].token,T_FINAL
	mov	al,p.flags3
	mov	[ebx].bytval,al
	mov	[ebx].string_ptr,offset __null
	mov	eax,p.index
	ret
Tokenize ENDP

	END
