include windows.inc
include gl/glut.inc
include tchar.inc

    .code

WndProc proc uses rdi hWnd:HWND, message:UINT, wParam:WPARAM, lParam:LPARAM

    .switch edx

    .case WM_CREATE

        .new dc:HDC
        .new pf:PIXELFORMATDESCRIPTOR

        mov dc,GetDC(rcx)

        lea rdi,pf
        mov ecx,PIXELFORMATDESCRIPTOR
        xor eax,eax
        rep stosb

        mov pf.nSize,       PIXELFORMATDESCRIPTOR
        mov pf.nVersion,    1
        mov pf.dwFlags,     PFD_DRAW_TO_WINDOW or PFD_SUPPORT_OPENGL or PFD_DOUBLEBUFFER
        mov pf.iPixelType,  PFD_TYPE_RGBA
        mov pf.cColorBits,  32
        mov pf.cDepthBits,  24
        mov pf.cStencilBits,8
        mov pf.iLayerType,  PFD_MAIN_PLANE

        SetPixelFormat(dc, ChoosePixelFormat(dc, &pf), &pf)
        wglMakeCurrent(dc, wglCreateContext(dc))
        MessageBox(0, glGetString(GL_VERSION), "OPENGL VERSION",0)

        wglDeleteContext(dc)
        PostQuitMessage(0)
        .endc

    .case WM_DESTROY
        PostQuitMessage(0)
        .endc
    .default
        .return DefWindowProc(rcx, edx, r8, r9)
    .endsw
    xor eax,eax
    ret

WndProc endp

_tWinMain proc hInstance:HINSTANCE, hPrevInstance:HINSTANCE, lpCmdLine:LPTSTR, nShowCmd:SINT

  local wc:WNDCLASSEX, msg:MSG

    mov wc.cbSize,          WNDCLASSEX
    mov wc.style,           CS_OWNDC
    mov wc.hbrBackground,   COLOR_BACKGROUND
    mov wc.hInstance,       rcx
    mov wc.lpfnWndProc,     &WndProc
    mov wc.lpszClassName,   &@CStr("GLVersionCheck")

    .ifd RegisterClassEx(&wc)

        .if CreateWindowEx(0, "GLVersionCheck", "GLVersionCheck", WS_OVERLAPPEDWINDOW or WS_VISIBLE,
                100, 100, 640, 480, NULL, NULL, hInstance, 0)

            .while GetMessage(&msg,0,0,0)
                DispatchMessage(&msg)
            .endw
            xor eax,eax
        .endif
    .endif
    ret

_tWinMain endp

    end _tstart
