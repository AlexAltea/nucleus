/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "window.h"
#include "resource.h"

#include "nucleus/nucleus.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        //nucleus.task(NUCLEUS_EVENT_CLOSE);
        break;
    case WM_SIZE:
        nucleusOnResize(LOWORD(lParam), HIWORD(lParam), 100, 60);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

Window::Window(const std::string& title, int width, int height) : m_title(title), m_width(width), m_height(height)
{
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
    RECT rc = {0, 0, m_width, m_height};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

    // Create the Window
    m_hwnd = CreateWindowEx(
        WS_EX_APPWINDOW,
        lpszClassName,
        m_title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        NULL,
        NULL,
        hInstance,
        NULL);

    if (!m_hwnd) {
        MessageBox(NULL, "Window Creation Failed!", "Nucleus", MB_ICONEXCLAMATION | MB_OK);
        return;
    }

    ShowWindow(m_hwnd, SW_SHOWNORMAL);
    UpdateWindow(m_hwnd);
}

void Window::loop()
{
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
