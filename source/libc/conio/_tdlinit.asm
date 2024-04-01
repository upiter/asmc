; _TDLINIT.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;
include conio.inc
include stdio.inc
include malloc.inc
include tchar.inc

    .code

    assume rbx:THWND
    assume rsi:THWND

_dlinit proc uses rsi rdi rbx hwnd:THWND, name:LPTSTR

    ldr rbx,hwnd
    mov rdi,[rbx].window
    mov rsi,[rbx].prev
    xor edx,edx

    .switch pascal [rbx].type

    .case T_PUSHBUTTON

        _at     BG_PBUTTON,FG_TITLE,' '
        movzx   ecx,[rbx].rc.col
        mov     rdx,rdi
        rep     stosd
        mov     al,[rdi+2]
        and     eax,0xF0
        or      al,at_foreground[FG_PBSHADE]
        shl     eax,16
        mov     ax,U_LOWER_HALF_BLOCK
        mov     [rdi],eax
        movzx   ecx,[rsi].rc.col
        lea     rdi,[rdx+rcx*4+4]
        movzx   ecx,[rbx].rc.col
        mov     ax,U_UPPER_HALF_BLOCK
        rep     stosd
        lea     rdi,[rdx+8]
        mov     eax,[rdi-4]
        mov     ecx,[rdi-2]
        and     ecx,0xF0
        or      cl,at_foreground[FG_TITLEKEY]
        mov     rdx,name

    .case T_RADIOBUTTON

        mov     eax,[rdi]
        mov     ax,'('
        mov     [rdi],ax
        mov     al,')'
        mov     [rdi+8],ax
        add     rdi,4*4
        mov     al,' '
        mov     ecx,[rdi+2]
        and     ecx,0xF0
        or      cl,at_foreground[FG_MENUKEY]
        mov     rdx,name

    .case T_CHECKBOX

        mov     eax,[rdi]
        mov     ax,'['
        mov     [rdi],ax
        mov     al,']'
        mov     [rdi+8],ax
        add     rdi,4*4
        mov     al,' '
        mov     ecx,[rdi+2]
        and     ecx,0xF0
        or      cl,at_foreground[FG_MENUKEY]
        mov     rdx,name

    .case T_XCELL
    .case T_TEXTBUTTON
    .case T_MENUITEM
        add     rdi,4
        _at     BG_MENU,FG_MENU,' '
        mov     ecx,eax
        shr     ecx,16
        and     ecx,0xF0
        or      cl,at_foreground[FG_MENUKEY]
        mov     rdx,name

    .case T_EDIT
        mov     eax,[rdi]
        mov     ax,U_MIDDLE_DOT
        movzx   ecx,[rbx].rc.col
        rep     stosd

    .case T_TEXTAREA
    .case T_MOUSERECT
        mov     eax,[rdi]
        mov     ax,' '
        mov     ecx,eax
        shr     ecx,16
        and     ecx,0xF0
        or      cl,at_foreground[FG_MENUKEY]
        mov     rdx,name
    .endsw

    .if ( rdx )

        .for ( rsi=rdx, edx=0 : dl < [rbx].rc.col : edx++ )

            _tlodsb
            .break .if !_tal

            .if ( _tal == '&' && [rbx].type != T_EDIT )

                _tlodsb
                .break .if !_tal

                mov [rdi],ax
                mov [rdi+2],cx
                add rdi,4

                or  al,0x20
                mov [rbx].syskey,al
            .else
                stosd
               .continue(0)
            .endif
        .endf
    .endif
    .return( rbx )

_dlinit endp

    end
