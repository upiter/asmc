; _INITTERM.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;
ifdef __UNIX__

include stdlib.inc

externdef __ImageBase:size_t

    .code

_initterm proc uses rbx pfbegin:ptr, pfend:ptr

   .new count:int_t

    ldr rbx,pfbegin
    ldr rax,pfend
    sub rax,rbx
    shr rax,3
    .ifnz

        .for ( count = eax : count : count--, rbx+=8 )

            mov rax,[rbx]
            .break .if !rax

ifdef _WIN64
            add  rax,__ImageBase
endif
            call rax
        .endf
    .endif
    ret

_initterm endp

else

include winbase.inc

MAXENTRIES equ 256

    .code

_initterm proc uses rsi rdi rbx pfbegin:ptr, pfend:ptr

   .new entries[MAXENTRIES]:uint64_t

    ldr rcx,pfbegin
    ldr rdx,pfend

    mov rax,rdx
    sub rax,rcx

    ; walk the table of function pointers from the bottom up, until
    ; the end is encountered.  Do not skip the first entry.  The initial
    ; value of pfbegin points to the first valid entry.  Do not try to
    ; execute what pfend points to.  Only entries before pfend are valid.

    .ifnz

        .for ( rsi = rcx,
               rdi = &entries,
               edx = 0,
               rcx += rax : rsi < rcx && edx < MAXENTRIES : rsi += 8 )

            mov eax,[rsi]
            .if ( eax )

                stosd
                mov eax,[rsi+4]
                stosd
                inc edx
            .endif
        .endf

        .for ( rsi = &entries, edi = edx :: )

            .for ( ecx = MAXENTRIES,
                   rbx = rsi,
                   edx = 0,
                   eax = edi : eax : eax--, rbx+=8 )

                .if ( dword ptr [rbx] != 0 && ecx >= [rbx+4] )

                    mov ecx,[rbx+4]
                    mov rdx,rbx
                .endif
            .endf
            .break .if !rdx

            mov  ecx,[rdx]
            mov  dword ptr [rdx],0
ifdef _WIN64
            lea  rax,_initterm
            mov  rdx,imagerel _initterm
            sub  rax,rdx
            add  rcx,rax
endif
            call rcx
        .endf
    .endif
    ret

_initterm endp
endif
    end
