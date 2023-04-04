; _TCONTROLA.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;
; void _tcontrolW( THWND hwnd, UINT count, char * string);
;
include conio.inc
include malloc.inc
include ltype.inc
include tchar.inc

    .code

    option proc:private

ClipIsSelected proto fastcall :PTEDIT {
    mov eax,[rcx].clip_eo
    sub eax,[rcx].clip_so
    }


ClipSet proto fastcall :PTEDIT {
    mov eax,[rcx].boffs
    add eax,[rcx].xoffs
    mov [rcx].clip_so,eax
    mov [rcx].clip_eo,eax
    }

    assume rcx:PTEDIT

IncreaseX proc fastcall ti:PTEDIT

    mov eax,[rcx].boffs
    add eax,[rcx].xoffs
    inc eax
    .if eax >= [rcx].bcols
        xor eax,eax
    .else
        mov edx,[rcx].boffs
        mov eax,[rcx].xoffs
        inc eax
        .if eax >= [rcx].scols
            mov eax,[rcx].scols
            dec eax
            inc edx
        .endif
        mov [rcx].xoffs,eax
        mov [rcx].boffs,edx
    .endif
    ret

IncreaseX endp


tinocando proc fastcall ti:PTEDIT

    .if ( [rcx].flags & O_USEBEEP )

        _cout("\x7")
    .endif
    .return( 0 ) ; operation fail (end of line/buffer)

tinocando endp


tistripend proc fastcall uses rbx string:LPSTR

    mov rbx,rcx
    .ifd strlen(rcx)

        mov ecx,eax
        add rbx,rax
        .repeat

            sub rbx,char_t
            mov al,[rbx]
            .break .if ( al != ' ' && al != 9 )
            mov char_t ptr [rbx],0
        .untilcxz
        mov rax,rcx
    .endif
    ret

tistripend endp


    assume rbx:PTEDIT

getline proc fastcall uses rbx ti:PTEDIT

    mov rax,[rcx].base
    .if rax
        mov     rbx,rcx
        mov     rdi,rax
        mov     edx,[rcx].bcols     ; terminate line
        xor     eax,eax             ; set length of line
        mov     [rdi+rdx-char_t],al
        mov     ecx,-1
        repne   scasb
        not     ecx
        dec     ecx
        sub     rdi,char_t
        mov     [rbx].bcount,ecx
        sub     ecx,[rbx].bcols     ; clear rest of line
        neg     ecx
        rep     stosb
        mov     rcx,rbx
        mov     edx,[rcx].bcount
        mov     rax,[rcx].base
    .endif
    ret

getline endp


curlptr proc fastcall ti:PTEDIT

    .if getline(rcx)

        mov edx,[rcx].boffs
        add edx,[rcx].xoffs
        add rax,rdx
    .endif
    ret

curlptr endp


event_home proc fastcall ti:PTEDIT

    .if getline(rcx)

        xor eax,eax
        mov [rcx].boffs,eax
        mov [rcx].xoffs,eax
        inc eax
    .endif
    ret

event_home endp


event_toend proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
    event_home(rcx)

    .if getline(rcx)

        tistripend(rax)

        .if ecx

            mov eax,[rbx].scols
            dec eax
            .ifs ecx <= eax
                mov eax,ecx
            .endif
            mov [rbx].xoffs,eax
            mov ecx,[rbx].bcount
            sub ecx,[rbx].scols
            inc ecx
            xor eax,eax
            .ifs eax <= ecx
                mov eax,ecx
            .endif
            mov [rbx].boffs,eax
            add eax,[rbx].xoffs
            .if eax > [rbx].bcount
                dec [rbx].boffs
            .endif
        .endif
        mov rcx,rbx
        mov eax,1
    .endif
    ret

event_toend endp


event_left proc fastcall ti:PTEDIT

    .if ( [rcx].xoffs )

        dec [rcx].xoffs
       .return( 1 )
    .endif

    .if ( [rcx].boffs )

        dec [rcx].boffs
       .return( 1 )
    .endif
    .return( 0 )

event_left endp


event_right proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
    .if curlptr(rcx)

        mov rcx,rax
        mov ax,[rax]
        mov edx,[rbx].xoffs
        shl edx,1
        sub rcx,rdx
        .if ax

            mov eax,[rbx].scols
            dec eax
            .if eax > [rbx].xoffs
                inc [rbx].xoffs
               .return( 1 )
            .endif
        .endif

        strlen(rcx)
        .if eax >= [rbx].scols
            inc [rbx].boffs
           .return( 1 )
        .endif
        xor eax,eax
    .endif
    ret

event_right endp


event_delete proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
    .if curlptr(rcx)

        tistripend(rax)
        .if ( ecx )

            curlptr(rbx)
            .if char_t ptr [rax]
                dec [rbx].bcount
                mov rcx,rax
                strcpy(rcx, &[rax+char_t])
                or  [rbx].flags,O_MODIFIED
               .return( 1 )
            .endif
        .endif
    .endif
    .return(tinocando(rbx))

event_delete endp


event_back proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
    .if getline(rcx)

        mov eax,[rbx].xoffs
        add eax,[rbx].boffs
        .if ( !eax || ![rbx].bcount )
            .return( 0 )
        .endif

        event_left(rbx)
        .if tistripend(curlptr(rbx))

            event_delete(rbx)
        .endif
    .endif
    ret

event_back endp


event_add proc fastcall uses rbx ti:PTEDIT, c:char_t

    mov rbx,rcx
    movzx esi,dx

    .if ( edx == 0 || edx == 10 || edx == 13 )

        .return( 0 )
    .endif

    .if ( getline(rcx) == NULL )

        .return( 0 )
    .endif

    lea rcx,_ltype[1]
    mov eax,esi

    .if ( byte ptr [rcx+rax] & _CONTROL )
        .if ( !( [rbx].flags & O_CONTROL ) )
            .return( 0 )
        .endif
    .endif

    mov eax,[rbx].bcount
    inc eax

    .if ( eax < [rbx].bcols )

        .ifd IncreaseX(rbx)

            inc [rbx].bcount
            .if getline(rbx)

                or      [rbx].flags,O_MODIFIED
                mov     eax,[rbx].boffs
                add     eax,[rbx].xoffs
                mov     rcx,[rbx].base
                lea     rdi,[rcx+rax-char_t]
                mov     edx,esi
                mov     ecx,-1
                xor     eax,eax
                repnz   scasb
                not     ecx
                inc     ecx
                mov     rsi,rdi
                inc     rdi
                std
                rep     movsb
                cld
                mov     [rdi],dl

               .return( 1 )
            .endif
        .endif
    .endif

    mov eax,[rbx].bcols
    dec eax
    mov [rbx].bcount,eax
   .return(tinocando(rbx))

event_add endp


event_nextword proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
   .return .if !curlptr(rcx)

    lea rsi,_ltype[1]
    mov rdx,rax
    mov rcx,rax
    movzx eax,char_t ptr [rcx]

    .while ( byte ptr [rsi+rax] & _LABEL )

        add rcx,char_t
        mov al,[rcx]
    .endw

    .while ( eax && !( byte ptr [rsi+rax] & _LABEL ) )

        add rcx,char_t
        mov al,[rcx]
    .endw

   .return .ifd !eax

    sub rcx,rdx
    mov eax,[rbx].boffs
    add eax,[rbx].xoffs
    add eax,ecx
   .return( 0 ) .if ( eax > [rbx].bcount )

    sub eax,[rbx].boffs
    mov ecx,[rbx].scols
    .if ( eax >= ecx )
        dec ecx
        sub eax,ecx
        add [rbx].boffs,eax
        mov [rbx].xoffs,ecx
    .else
        mov [rbx].xoffs,eax
    .endif
    .return( 1 )

event_nextword endp


event_prevword proc fastcall uses rbx ti:PTEDIT

    mov rbx,rcx
   .return .if !curlptr(rcx)

    mov rcx,rax
    mov eax,[rbx].boffs
    add eax,[rbx].xoffs
   .return .ifz

    lea rsi,_ltype[1]
    movzx eax,char_t ptr [rcx]

    .if ( char_t ptr [rsi+rax] & _LABEL )

        sub rcx,char_t
    .endif

    mov rdx,[rbx].base
    mov al,[rcx]

    .while ( rcx > rdx && !( char_t ptr [rsi+rax] & _LABEL ) )

        sub rcx,char_t
        mov al,[rcx]
    .endw

    .while ( rcx > rdx && ( char_t ptr [rsi+rax] & _LABEL ) )

        sub rcx,char_t
        mov al,[rcx]
    .endw

    .if ( !( char_t ptr [rsi+rax] & _LABEL ) )

        mov al,[rcx+char_t]
        .if ( char_t ptr [rsi+rax] & _LABEL )
            add rcx,char_t
        .endif
        movzx eax,char_t ptr [rcx]
    .endif

    sub rcx,[rbx].base
    .if ( ecx > [rbx].xoffs )

        sub ecx,[rbx].xoffs
        mov [rbx].xoffs,0
        mov [rbx].boffs,ecx
    .else
        mov [rbx].xoffs,ecx
    .endif
    .return( 1 )

event_prevword endp


ClipDelete proc fastcall ti:PTEDIT

    .if ClipIsSelected(rcx)

        mov edi,[rcx].clip_so
        mov esi,[rcx].xoffs
        add esi,[rcx].boffs

        .if ( esi < edi )

            .repeat
               .break .ifd !IncreaseX(rcx)
                inc esi
            .until ( edi == esi )

        .elseif ( esi > edi )

            .repeat
                .if [rcx].xoffs
                    dec [rcx].xoffs
                .elseif [rcx].boffs
                    dec [rcx].boffs
                .else
                    .break
                .endif
                dec esi
            .until ( edi == esi )
        .endif

        or  [rcx].flags,O_MODIFIED
        mov rdi,[rcx].base
        mov eax,[rcx].clip_so
        mov edx,[rcx].clip_eo
        lea rax,[rdi+rax]
        lea rdx,[rdi+rdx]
        mov rsi,rcx

        strcpy(rax, rdx)
        getline(rsi)
        ClipSet(rcx)
        mov eax,1
    .endif
    ret

ClipDelete endp


ClipCopy proc fastcall uses rbx r12 ti:PTEDIT, Cut:BOOL

    mov rbx,rcx
    mov r12d,edx

    .if ClipIsSelected(rcx) ; get size of selection

        mov edx,eax
        mov eax,[rbx].clip_so
        mov rcx,[rbx].base
        lea rcx,[rcx+rax]

        .if _clipsetA(rcx, edx)
            .if r12d
                ClipDelete(rbx)
            .endif
        .endif
        ClipSet(rbx)
    .endif
    .return( 0 )

ClipCopy endp


ClipPaste proc fastcall uses rbx r12 r13 ti:PTEDIT

   .new bo:int_t
   .new xo:int_t
    mov rbx,rcx
    .if [rcx].flags & O_OVERWRITE
        ClipDelete(rcx)
    .else
        ClipSet(rcx)
    .endif

    .if _clipgetA()

        mov r12,rax
        mov eax,[rbx].xoffs
        mov ecx,[rbx].boffs
        mov r13d,[rbx].bcols
        sub r13d,ecx
        mov xo,eax
        mov bo,ecx

        .fors ( : r13d > 0 : r13d-- )

            movzx edx,char_t ptr [r12]
            add r12,char_t
            .break .if !event_add(rbx, dl)
        .endf
        mov [rbx].boffs,bo
        mov [rbx].xoffs,xo
    .endif
    ClipSet(rbx)
   .return( 0 )

ClipPaste endp


PrintString proc fastcall uses rbx r12 r13 ti:PTEDIT

   .new ci[MAXSCRLINE]:CHAR_INFO

    mov rbx,rcx
    lea rdi,ci
    mov ecx,[rbx].scols
    mov eax,[rbx].clrattrib
    rep stosd

    mov r12,[rbx].base
    strlen(r12)

    .if ( eax > [rbx].boffs )

        mov eax,[rbx].boffs
        lea r12,[r12+rax]
        mov ecx,[rbx].scols
        lea r13,ci

        .repeat
            movzx eax,char_t ptr [r12]
            .break .if !eax
            mov [r13],ax
            add r12,char_t
            add r13,4
        .untilcxz
    .endif

    mov edi,[rbx].boffs
    mov ecx,[rbx].clip_eo

    .if ( edi < ecx )

        sub ecx,edi
        xor eax,eax
        .if ( ecx >= [rbx].scols )

            mov ecx,[rbx].scols
        .endif
        .if ( [rbx].clip_so >= edi )

            mov eax,[rbx].clip_so
            sub eax,edi
            .if eax <= ecx
                sub ecx,eax
            .else
                xor ecx,ecx
            .endif
        .endif
        .if ecx

            lea rdi,ci
            lea rdi,[rdi+rax*4+2]
            mov al,BG_INVERSE shl 4
            .repeat
                mov [rdi],al
                add rdi,4
            .untilcxz
        .endif
    .endif

    .for ( r12=&ci, r13d=0 : r13d < [rbx].scols : r13d++, r12+=4 )

        mov edi,[rbx].xpos
        add edi,r13d
        _scputw(dil, byte ptr [rbx].ypos, 1, [r12])
    .endf
    .return( 0 )

PrintString endp


    assume r12:THWND

define VK_SETCLIP 0x07

wm_char proc uses rbx r12 r13 hwnd:THWND, wParam:UINT, lParam:UINT

   .new clip_clear:char_t = true
   .new clip_delete:char_t = false
   .new add_char:char_t = false

    mov r12,hwnd
    mov rbx,[r12].context.object

    .ifd !ClipIsSelected(rbx)

        ClipSet(rbx) ; reset clipboard if not selected
    .endif

    ; Shift-Insert  -- Paste
    ; Shift-Delete  -- Cut
    ; Ctrl-Insert   -- Copy
    ; Ctrl-C        -- Copy
    ; Ctrl-V        -- Paste
    ; Ctrl-X        -- Cut

    xor r12,r12
    mov ecx,lParam
    mov eax,wParam
    .if ( ecx & SHIFTKEY_DOWN )
        mov r12d,VK_SHIFT
    .elseif ( ecx & CTRLKEY_DOWN )
        mov r12d,VK_CONTROL
    .endif

    .if ( ecx & KEY_EXTENDED )

        .switch eax
        .case VK_UP
        .case VK_DOWN
            ClipSet(rbx)
           .return( 1 ) ; return current event

        .case <vk_delete> VK_DELETE
            .if ( r12d == VK_SHIFT )
                ClipCopy(rbx, 1)
               .endc
            .endif
            .endc .ifd ClipDelete(rbx)
             event_delete(rbx)
            .endc

        .case <vk_insert> VK_INSERT
            .if ( r12d == VK_CONTROL )
                ClipCopy(rbx, 0)
               .endc
            .endif
            .if ( r12d == VK_SHIFT )
                ClipPaste(rbx)
               .endc
            .endif
            .gotosw(VK_UP)

        .case VK_SETCLIP
            .if ( eax && r12d == VK_SHIFT )

                mov clip_clear,false
                mov eax,[rbx].boffs
                add eax,[rbx].xoffs

                .if ( eax >= [rbx].clip_so )

                    mov [rbx].clip_eo,eax
                   .endc
                .endif
                mov [rbx].clip_so,eax
            .endif
           .endc

        .case VK_LEFT
            .if ( r12d == VK_CONTROL )
                event_prevword(rbx)
               .endc
            .endif
            .gotosw(VK_UP) .ifd !event_left(rbx)
            .gotosw(VK_SETCLIP)

        .case VK_RIGHT
            .if ( r12d == VK_CONTROL )
                event_nextword(rbx)
               .endc
            .endif
            .gotosw(VK_UP) .ifd !event_right(rbx)

            .if ( r12d == VK_SHIFT )

                mov clip_clear,false
                mov eax,[rbx].boffs
                add eax,[rbx].xoffs

                .if ( eax >= [rbx].clip_so )

                    mov edx,eax
                    dec edx
                    .if ( edx == [rbx].clip_so )
                        .if ( edx != [rbx].clip_eo )
                            mov [rbx].clip_so,eax
                           .endc
                        .endif
                    .endif
                    mov [rbx].clip_eo,eax
                   .endc
                .endif
                mov [rbx].clip_so,eax
            .endif
            .endc

        .case VK_HOME
            event_home(rbx)
           .gotosw(VK_SETCLIP)
        .case VK_END
            event_toend(rbx)
           .gotosw(VK_SETCLIP)
        .endsw

    .else

        .switch eax
        .case VK_ESCAPE
        .case VK_RETURN
            ClipSet(rbx)
           .return( 1 )
        .case VK_BACK
            event_back(rbx)
           .endc
        .case VK_TAB
            .gotosw(VK_RETURN) .if !( [rbx].flags & O_CONTROL )
        .default
            .if ( r12d == VK_CONTROL )

                .if ( cx == 'C' ) ; Copy
                    jmp vk_insert
                .endif
                 mov r12d,VK_SHIFT
                .if ( cx == 'V' ) ; Paste
                    jmp vk_insert
                .endif
                .if ( cx == 'X' ) ; Cut
                    jmp vk_delete
                .endif
                .gotosw(VK_RETURN)
            .endif
            mov add_char,true
            mov clip_delete,true
           .endc
        .endsw
    .endif

    .if ( clip_delete )
        ClipDelete(rbx)
    .elseif ( clip_clear )
        ClipSet(rbx)
    .endif
    .if ( add_char )
        mov eax,wParam
        event_add(rbx, al)
    .endif

    mov ecx,[rbx].xpos
    add ecx,[rbx].xoffs
    _gotoxy(ecx, [rbx].ypos)

   .return(PrintString(rbx))

wm_char endp


wndproc proc uses rbx r12 r13 hwnd:THWND, uiMsg:UINT, wParam:UINT, lParam:UINT

    mov eax,lParam
    mov r12,hwnd
    mov rbx,[r12].context.object

    .switch uiMsg
    .case WM_SETFOCUS
        xor eax,eax
        xor ecx,ecx
        mov [rbx].xoffs,eax
        mov [rbx].boffs,eax
        mov [rbx].clip_so,eax
        mov [rbx].clip_eo,eax
        mov rdx,[r12].prev
        mov al,[r12].rc.y
        add al,[rdx].TCLASS.rc.y
        mov [rbx].ypos,eax
        mov cl,[r12].rc.x
        add cl,[rdx].TCLASS.rc.x
        mov [rbx].xpos,ecx
        add ecx,[rbx].xoffs
        _gotoxy(ecx, eax)
        _cursoron()
        .if ( [rbx].flags & O_SELECT )
            mov [rbx].clip_eo,strlen([rbx].base)
        .endif
        .return( PrintString(rbx) )
    .case WM_KILLFOCUS
        xor eax,eax
        mov [rbx].xoffs,eax
        mov [rbx].boffs,eax
        mov [rbx].clip_so,eax
        mov [rbx].clip_eo,eax
        mov rdx,[r12].prev
        mov al,[r12].rc.y
        add al,[rdx].TCLASS.rc.y
        mov [rbx].ypos,eax
        mov al,[r12].rc.x
        add al,[rdx].TCLASS.rc.x
        mov [rbx].xpos,eax
       .return( PrintString(rbx) )
    .case WM_LBUTTONDOWN
        .endc
    .case WM_CHAR
        .if ( edx )
            mov esi,edx
            .return wm_char(r12, esi, eax)
        .endif
        .endc
    .case WM_KEYDOWN
        .endc .if !( eax & KEY_EXTENDED )
        .switch edx
        .case VK_UP
        .case VK_DOWN
        .case VK_RETURN
        .case VK_INSERT
        .case VK_DELETE
        .case VK_HOME
        .case VK_END
        .case VK_LEFT
        .case VK_RIGHT
        .gotosw(1:WM_CHAR)
        .endsw
    .endsw
    .return( 1 )
    ret

wndproc endp

    assume rcx:THWND

_tcontrolA proc public uses rbx r12 hwnd:THWND, count:UINT, char:WORD, string:LPSTR

    mov r12,hwnd
    mov rbx,[r12].context.object
    mov rcx,[r12].prev

    .if ( !rbx )

        mov rbx,[rcx].buffer
        mov [r12].context.object,rbx
        add [rcx].buffer,TEDIT
        mov [r12].buffer,[rcx].buffer
        mov [rbx].base,rax
        mov [rbx].bcols,count
        add [rcx].buffer,rax
        shr eax,4
        mov [r12].count,al
    .endif

    mov [r12].winproc,&wndproc
    mov [rbx].flags,[r12].flags
    mov [rbx].scols,[r12].rc.col
    mov eax,_getattrib(BG_EDIT, FG_EDIT)
    mov ax,char
    .if ( ax == 0 )
        mov ax,U_MIDDLE_DOT
    .endif
    mov [rbx].clrattrib,eax

    mov rcx,[r12].prev
    mov al,[r12].rc.y
    add al,[rcx].rc.y
    mov [rbx].ypos,eax
    mov al,[r12].rc.x
    add al,[rcx].rc.x
    mov [rbx].xpos,eax
    mov rsi,string
    .if ( rsi )
        strncpy([rbx].base, rsi, [rbx].bcols)
    .endif
    mov rcx,[r12].prev
    .if ( [rcx].flags & W_VISIBLE )
        PrintString(rbx)
    .endif
    .return(rbx)

_tcontrolA endp

    end