/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window.h"
#include "resource.h"

// Nucleus
#include "nucleus/nucleus.h"

#include <Windowsx.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    // Lifecycle
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        nucleusFinalize();
        PostQuitMessage(0);
        break;

    // Window events
    case WM_SIZE:
        nucleusOnWindowSizeChanged(LOWORD(lParam), HIWORD(lParam));    
        break;

    // Mouse events
    case WM_LBUTTONDOWN:
        nucleusOnMouseClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_LBUTTONDBLCLK:
        nucleusOnMouseDoubleClick(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;
    case WM_MOUSEMOVE:
        nucleusOnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        break;

    // Keyboard events
    case WM_KEYDOWN:
        nucleusOnKeyDown(wParam);
        break;
    case WM_KEYUP:
        nucleusOnKeyDown(wParam);
        break;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Window::Window(const std::string& title, int width, int height) :
    title(title), width(width), height(height) {
    WNDCLASSEX wc;
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char lpszClassName[] = "NucleusWindowClass";

    // Register the Window class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(hInstance, (LPCTSTR)IDI_NUCLEUS);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = lpszClassName;
    wc.hIconSm       = LoadIcon(hInstance, (LPCTSTR)IDI_NUCLEUS);

    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Window Registration Failed!", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    // Set initial size
    RECT rc = {0, 0, width, height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the Window
    hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        lpszClassName,
        title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!hwnd) {
        MessageBox(NULL, "Window creation failed!", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    // Get device context
    hdc = GetDC(hwnd);
    if (!hdc) {
        MessageBox(hwnd, "Failed to get a device context", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
        return;
    }
}

void Window::loop() {
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
