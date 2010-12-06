#pragma once

#include <Windows.h>

class SplashWindow
{
public:
    void Initialize(HINSTANCE hInst, int iWhichNag);
    void Show();
    void Shutdown();

    void SetWaitForProcess(HANDLE hProcess) { m_hWaitForProcess = hProcess; }

    int PulseMessagePump();

    static SplashWindow & GetInstance()
    {
        static SplashWindow w;
        return w;
    }

private:
    static int WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );
    HWND m_hWnd;
    HRGN m_hRgn;
    HBITMAP m_hBmp;
    bool m_bShutdown;
    HANDLE m_hWaitForProcess;
    static int m_iResult;
};