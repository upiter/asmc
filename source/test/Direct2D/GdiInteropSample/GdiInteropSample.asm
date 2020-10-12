;
; https://github.com/microsoft/Windows-classic-samples/tree/master/Samples/
;    Win7Samples/multimedia/Direct2D/GdiInteropSample
;
include GdiInteropSample.inc

.code
;******************************************************************
;*                                                                *
;*  WinMain                                                       *
;*                                                                *
;*  Application entrypoint                                        *
;*                                                                *
;******************************************************************

wWinMain proc hInstance:HINSTANCE, hPrevInstance:HINSTANCE, lpCmdLine:LPWSTR, nCmdShow:SINT

  local vtable:DemoAppVtbl

    ;; Ignore the return value because we want to run the program even in the
    ;; unlikely event that HeapSetInformation fails.

    HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0)

    CoInitialize(NULL)
    .if (SUCCEEDED(eax))

       .new app:DemoApp(&vtable)

        app.Initialize()

        .if (SUCCEEDED(eax))

            app.RunMessageLoop()
        .endif
        CoUninitialize()
    .endif

    .return 0

wWinMain endp

;******************************************************************
;*                                                                *
;*  DemoApp::DemoApp constructor                                  *
;*                                                                *
;*  Initialize member data                                        *
;*                                                                *
;******************************************************************

DemoApp::DemoApp proc uses rdi vtable:ptr

    mov [rcx].DemoApp.lpVtbl,rdx
    lea rdi,[rcx+8]
    xor eax,eax
    mov ecx,(DemoApp - 8) / 8
    rep stosq
    mov rdi,rdx
    for q,<Release,
           Initialize,
           RunMessageLoop,
           CreateDeviceIndependentResources,
           CreateDeviceResources,
           DiscardDeviceResources,
           OnRender>
        lea rax,DemoApp_&q
        stosq
        endm
    ret

DemoApp::DemoApp endp

    assume rsi:ptr DemoApp

;******************************************************************
;*                                                                *
;*  DemoApp::Release destructor                                   *
;*                                                                *
;*  Tear down resources                                           *
;*                                                                *
;******************************************************************

DemoApp::Release proc uses rsi

    mov rsi,rcx
    SafeRelease(&[rsi].m_pD2DFactory, ID2D1Factory)
    SafeRelease(&[rsi].m_pDCRT, ID2D1DCRenderTarget)
    SafeRelease(&[rsi].m_pBlackBrush, ID2D1SolidColorBrush)
    ret

DemoApp::Release endp

;*******************************************************************
;*                                                                 *
;*  DemoApp::Initialize                                            *
;*                                                                 *
;*  Create the application window and device-independent resources *
;*                                                                 *
;*******************************************************************

DemoApp::Initialize proc uses rsi

  local hr:HRESULT
  local wcex:WNDCLASSEX
  local dpiX:FLOAT, dpiY:FLOAT

    mov rsi,rcx

    ;; Initialize device-indpendent resources, such
    ;; as the Direct2D factory.

    mov hr,[rsi].CreateDeviceIndependentResources()

    .if (SUCCEEDED(hr))

        ;; Register the window class.
        mov wcex.cbSize,        WNDCLASSEX
        mov wcex.style,         CS_HREDRAW or CS_VREDRAW
        mov wcex.lpfnWndProc,   &WndProc
        mov wcex.cbClsExtra,    0
        mov wcex.cbWndExtra,    sizeof(LONG_PTR)
        mov wcex.hInstance,     HINST_THISCOMPONENT
        mov wcex.hIcon,         NULL
        mov wcex.hIconSm,       NULL
        mov wcex.hbrBackground, NULL
        mov wcex.lpszMenuName,  NULL
        mov wcex.hCursor,       LoadCursor(NULL, IDC_ARROW)
        mov wcex.lpszClassName, &@CStr(L"D2DDemoApp")

        RegisterClassEx(&wcex)

        ;; Create the application window.
        ;;
        ;; Because the CreateWindow function takes its size in pixels, we
        ;; obtain the system DPI and use it to scale the window size.

        mov rcx,[rsi].m_pD2DFactory
        [rcx].ID2D1Factory.GetDesktopDpi(&dpiX, &dpiY)

        movss xmm0,dpiX
        mulss xmm0,640.0
        divss xmm0,96.0
        ceilf(xmm0)
        cvtss2si eax,xmm0
        mov dpiX,eax

        movss xmm0,dpiY
        mulss xmm0,480.0
        divss xmm0,96.0
        ceilf(xmm0)
        cvtss2si eax,xmm0
        mov dpiY,eax

        mov [rsi].m_hwnd,CreateWindowEx(0,
            L"D2DDemoApp",
            L"Direct2D Demo App",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            dpiX,
            dpiY,
            NULL,
            NULL,
            HINST_THISCOMPONENT,
            rsi
            )

        mov eax,S_OK
        .if rax == [rsi].m_hwnd
            mov eax,E_FAIL
        .endif
        mov hr,eax
        .if (SUCCEEDED(eax))

            ShowWindow([rsi].m_hwnd, SW_SHOWNORMAL)
            UpdateWindow([rsi].m_hwnd)
        .endif
    .endif

    .return hr

DemoApp::Initialize endp

;******************************************************************
;*                                                                *
;*  DemoApp::CreateDeviceIndependentResources                     *
;*                                                                *
;*  This method is used to create resources which are not bound   *
;*  to any device. Their lifetime effectively extends for the     *
;*  duration of the app.                                          *
;*                                                                *
;******************************************************************

DemoApp::CreateDeviceIndependentResources proc

    ;; Create D2D factory
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &IID_ID2D1Factory, NULL, &[rcx].DemoApp.m_pD2DFactory)
    ret

DemoApp::CreateDeviceIndependentResources endp

;******************************************************************
;*                                                                *
;*  DemoApp::CreateDeviceResources                                *
;*                                                                *
;*  This method creates resources which are bound to a particular *
;*  D3D device. It's all centralized here, in case the resources  *
;*  need to be recreated in case of D3D device loss (eg. display  *
;*  change, remoting, removal of video card, etc).                *
;*                                                                *
;******************************************************************

DemoApp::CreateDeviceResources proc uses rsi

  local hr:HRESULT

    mov rsi,rcx
    mov hr,S_OK

    .if ![rsi].m_pDCRT

        ;; Create a DC render target.

        mov rdx,D2D1_PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)
        mov rdx,D2D1_RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT, rdx, 0, 0,
                D2D1_RENDER_TARGET_USAGE_NONE, D2D1_FEATURE_LEVEL_DEFAULT)

        mov rcx,[rsi].m_pD2DFactory
        mov hr,[rcx].ID2D1Factory.CreateDCRenderTarget(rdx, &[rsi].m_pDCRT)
        .if (SUCCEEDED(hr))

            ;; Create a black brush.

            mov rdx,D3DCOLORVALUE(Black, 1.0)
            mov rcx,[rsi].m_pDCRT
            mov hr,[rcx].ID2D1DCRenderTarget.CreateSolidColorBrush(rdx, NULL, &[rsi].m_pBlackBrush)
        .endif
    .endif

    .return hr

DemoApp::CreateDeviceResources endp

;******************************************************************
;*                                                                *
;*  DemoApp::DiscardDeviceResources                               *
;*                                                                *
;*  Discard device-specific resources which need to be recreated  *
;*  when a D3D device is lost                                     *
;*                                                                *
;******************************************************************

DemoApp::DiscardDeviceResources proc uses rsi

    mov rsi,rcx
    SafeRelease(&[rsi].m_pDCRT, ID2D1DCRenderTarget)
    SafeRelease(&[rsi].m_pBlackBrush, ID2D1SolidColorBrush)
    ret

DemoApp::DiscardDeviceResources endp

;******************************************************************
;*                                                                *
;*  DemoApp::RunMessageLoop                                       *
;*                                                                *
;*  Main window message loop                                      *
;*                                                                *
;******************************************************************

DemoApp::RunMessageLoop proc

  local msg:MSG

    .while GetMessage(&msg, NULL, 0, 0)

        TranslateMessage(&msg)
        DispatchMessage(&msg)
    .endw
    ret

DemoApp::RunMessageLoop endp


;*******************************************************************
;*                                                                 *
;*  DemoApp::OnRender                                              *
;*                                                                 *
;*  This method draws Direct2D content to a GDI HDC.               *
;*                                                                 *
;*  This method will automatically discard device-specific         *
;*  resources if the D3D device disappears during function         *
;*  invocation, and will recreate the resources the next time it's *
;*  invoked.                                                       *
;*                                                                 *
;*******************************************************************

DemoApp::OnRender proc uses rsi rdi rbx ps:PAINTSTRUCT


  local hr:HRESULT
  local rc:RECT
  local original:HGDIOBJ
  local blackPen:HPEN
  local pntArray1[2]:POINT
  local pntArray2[2]:POINT
  local pntArray3[2]:POINT
  local hdc:HDC

    mov rsi,rcx
    mov hdc,[rdx].PAINTSTRUCT.hdc

    ;; Get the dimensions of the client drawing area.

    GetClientRect([rsi].m_hwnd, &rc)

    ;;
    ;; Draw the pie chart with Direct2D.
    ;;

    ;; Create the DC render target.

    mov hr,[rsi].CreateDeviceResources()
    mov rdi,[rsi].m_pDCRT

    assume rdi:LPID2D1DCRenderTarget

    .if (SUCCEEDED(hr))

        ;; Bind the DC to the DC render target.

        mov hr,[rdi].BindDC(hdc, &rc)

        [rdi].BeginDraw()
        [rdi].SetTransform(Matrix3x2F::Identity())
        [rdi].Clear(D3DCOLORVALUE(White, 1.0))

        mov rbx,D2D1::Point2F(150.0, 150.0)

        [rdi].DrawEllipse(
            D2D1::Ellipse(
                rbx,
                100.0,
                100.0),
            [rsi].m_pBlackBrush,
            3.0,
            NULL
            )


        [rdi].DrawLine(
            rbx,
            D2D1::Point2F(
                (150.0 + 100.0 * 0.15425),
                (150.0 - 100.0 * 0.988)),
            [rsi].m_pBlackBrush,
            3.0,
            NULL
            )

        [rdi].DrawLine(
            rbx,
            D2D1::Point2F(
                (150.0 + 100.0 * 0.525),
                (150.0 + 100.0 * 0.8509)),
            [rsi].m_pBlackBrush,
            3.0,
            NULL
            )

        [rdi].DrawLine(
            rbx,
            D2D1::Point2F(
                (150.0 - 100.0 * 0.988),
                (150.0 - 100.0 * 0.15425)),
            [rsi].m_pBlackBrush,
            3.0,
            NULL
            )

        mov hr,[rdi].EndDraw(NULL, NULL)

        .if (SUCCEEDED(hr))

            ;;
            ;; Draw the pie chart with GDI.
            ;;

            ;; Save the original object.

            mov original,SelectObject(hdc, GetStockObject(DC_PEN))

            mov blackPen,CreatePen(PS_SOLID, 3, 0)
            SelectObject(hdc, blackPen)

            Ellipse(hdc, 300, 50, 500, 250)

            static_cast macro val
              local n
                n textequ <>
              % forc c,<@CatStr(%val)>
                ifidn <c>,<.>
                  exitm
                endif
                  n CatStr n,<c>
                  endm
                exitm<n>
                endm

            mov pntArray1[0*POINT].x,400
            mov pntArray1[0*POINT].y,150
            mov pntArray1[1*POINT].x,static_cast(400.0 + 100.0 * 0.15425)
            mov pntArray1[1*POINT].y,static_cast(150.0 - 100.0 * 0.9885)


            mov pntArray2[0*POINT].x,400
            mov pntArray2[0*POINT].y,150
            mov pntArray2[1*POINT].x,static_cast(400.0 + 100.0 * 0.525)
            mov pntArray2[1*POINT].y,static_cast(150.0 + 100.0 * 0.8509)

            mov pntArray3[0*POINT].x,400
            mov pntArray3[0*POINT].y,150
            mov pntArray3[1*POINT].x,static_cast(400.0 - 100.0 * 0.988)
            mov pntArray3[1*POINT].y,static_cast(150.0 - 100.0 * 0.15425)

            Polyline(hdc, &pntArray1, 2)
            Polyline(hdc, &pntArray2, 2)
            Polyline(hdc, &pntArray3, 2)

            DeleteObject(blackPen)

            ;; Restore the original object.
            SelectObject(hdc, original)
        .endif
    .endif

    .if (hr == D2DERR_RECREATE_TARGET)

        mov hr,S_OK
        [rsi].DiscardDeviceResources()
    .endif

    .return hr

DemoApp::OnRender endp


;******************************************************************
;*                                                                *
;*  WndProc                                                       *
;*                                                                *
;*  Window message handler                                        *
;*                                                                *
;******************************************************************

WndProc proc hwnd:HWND, message:UINT, wParam:WPARAM, lParam:LPARAM

  local result:LRESULT
  local pDemoApp:ptr DemoApp
  local wasHandled:BOOL
  local ps:PAINTSTRUCT

    mov wasHandled,FALSE
    mov result,0

    .if edx == WM_CREATE

        mov r8,[r9].CREATESTRUCT.lpCreateParams
        SetWindowLongPtrW(rcx, GWLP_USERDATA, PtrToUlong(r8))
        mov result,1

    .else

        mov pDemoApp,GetWindowLongPtrW(rcx, GWLP_USERDATA)
        mov wasHandled,FALSE

        .if rax

            .switch message

            .case WM_PAINT
            .case WM_DISPLAYCHANGE

                BeginPaint(hwnd, &ps)
                pDemoApp.OnRender(&ps)
                EndPaint(hwnd, &ps)

                mov result,0
                mov wasHandled,TRUE
                .endc

            .case WM_DESTROY

                PostQuitMessage(0)
                mov result,1
                mov wasHandled,TRUE
                .endc

            .case WM_CHAR
                .gotosw(WM_DESTROY) .if wParam == VK_ESCAPE
                .endc

            .endsw
        .endif

        .if (!wasHandled)

            mov result,DefWindowProc(hwnd, message, wParam, lParam)
        .endif
    .endif

    .return result

WndProc endp

    end _tstart