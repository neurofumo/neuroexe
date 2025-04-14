// NeuroFine.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "NeuroFine.h"
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define MAX_LOADSTRING 100
#include <windows.h>
#include <tchar.h>
#include <thread>
#include <chrono>

#define WINDOW_WIDTH 825
#define WINDOW_HEIGHT 612

const TCHAR g_szClassName[] = _T("MyWindowClass");
HBITMAP hImage = NULL;

// Function to load and display BMP image
void DrawImage(HWND hwnd, HDC hdc) {
    if (!hImage) return;

    HDC hdcMem = CreateCompatibleDC(hdc);
    HGDIOBJ oldBitmap = SelectObject(hdcMem, hImage);

    BITMAP bmp;
    GetObject(hImage, sizeof(BITMAP), &bmp);

    RECT rect;
    GetClientRect(hwnd, &rect);
    StretchBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
}

// Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        if (hImage)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hImage);

            BITMAP bmp;
            GetObject(hImage, sizeof(BITMAP), &bmp);

            // Get the size of the client area (window's drawable area)
            RECT rect;
            GetClientRect(hwnd, &rect);
            int winWidth = rect.right - rect.left;
            int winHeight = rect.bottom - rect.top;

            // Stretch the bitmap to fit the window
            StretchBlt(hdc, 0, 0, winWidth, winHeight,
                hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

            SelectObject(hdcMem, hOld);
            DeleteDC(hdcMem);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }


                 break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// Launch the neuro.exe window
void LaunchNeuroWindow() {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WndProc,
                      0, 0, GetModuleHandle(NULL), LoadIcon(NULL, IDI_APPLICATION),
                      LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1),
                      NULL, g_szClassName, LoadIcon(NULL, IDI_APPLICATION) };
    wc.hbrBackground = NULL; // Prevent background erase (flicker)


    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, g_szClassName, _T("neuro.exe"),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, GetModuleHandle(NULL), NULL);
    

    if (!hwnd) return;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    // Load the image
    hImage = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP2));

    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
    // Message loop
    MSG Msg;
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    if (hImage) DeleteObject(hImage);
}

// Monitor function to detect target window
void MonitorDeleteWindow() {
    while (true) {
        HWND hTarget = FindWindow(NULL, _T("Confirm File Delete"));
        if (hTarget) {
            LaunchNeuroWindow();
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow) {
    std::thread monitorThread(MonitorDeleteWindow);
    monitorThread.detach(); // Let it run in background

    // Idle loop to keep app alive
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}
