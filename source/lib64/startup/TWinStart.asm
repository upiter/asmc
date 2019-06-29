; TWINSTART.ASM--
;
; Copyright (c) The Asmc Contributors. All rights reserved.
; Consult your license regarding permissions and restrictions.
;
; Startup module for LIBC
;
include stdlib.inc
include twindow.inc

cmain     proto __cdecl :ptr, :dword, :ptr, :ptr
_initterm proto __cdecl :ptr, :ptr

;;
;; pointers to initialization sections
;;
externdef __xi_a:ptr
externdef __xi_z:ptr
externdef __xt_a:ptr
externdef __xt_z:ptr

    .code

    dd 495A440Ah
    dd 564A4A50h
    db __LIBC__ / 100 + '0','.',__LIBC__ mod 100 / 10 + '0',__LIBC__ mod 10 + '0'

TWinStart proc frame

  local hwnd:window_t
  local retval:int_t

    _initterm(&__xi_a, &__xi_z)

    mov retval,cmain( TWindow::TWindow(&hwnd), __argc, __argv, _environ )

    hwnd.Release()

    exit( retval )

TWinStart endp

    end TWinStart

