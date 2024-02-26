; _TSTRCAT.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;

include string.inc
include tmacro.inc

    .code

    assume rdx:ptr TCHAR

_tcscat proc dst:LPTSTR, src:LPTSTR

    ldr     rcx,dst
    ldr     rdx,src
ifdef _WIN64
    mov     r8,rcx
endif
    xor     eax,eax
.0:
    cmp     __a,[rcx]
    je      .1
    add     rcx,TCHAR
    jmp     .0
.1:
    mov     [rcx],[rdx]
    add     rcx,TCHAR
    add     rdx,TCHAR
    test    eax,eax
    jnz     .1
ifdef _WIN64
    mov     rax,r8
else
    mov     eax,dst
endif
    ret

_tcscat endp

    end
