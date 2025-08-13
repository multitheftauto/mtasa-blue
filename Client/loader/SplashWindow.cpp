/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/SplashWindow.cpp
 *  PURPOSE:     This file implements the functionality to show a splash window
 *               before the game window appears. The splash window will be
 *               created and rendered on a separate thread to avoid blocking
                 the main thread.
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "resource.h"
#include <chrono>

static UINT GetWindowDpi(HWND window);
static SIZE GetPrimaryMonitorSize();
static LONG ScaleToDpi(LONG value, UINT dpi);

using hrc = std::chrono::high_resolution_clock;

///////////////////////////////////////////////////////////////////////////
//
// Splash window logic.
// 
///////////////////////////////////////////////////////////////////////////

class Splash final
{
public:
    bool CreateSplashWindow(HINSTANCE instance);
    void DestroySplashWindow();

    bool CreateDeviceResources();
    void ReleaseDeviceResources();

    bool Update();
    void Paint(HDC windowContext, bool drawBackground) const;
    void UpdateLoadingBar();
    void OnDestroy();

    void Show() const;
    void Hide() const;
    void BringToFront() const;

private:
    WNDCLASS m_windowClass{};
    HWND     m_window{};
    int      m_width{};
    int      m_height{};

    // Background bitmap
    HBITMAP m_bgBitmap{};
    HDC     m_bgContext{};

    // Loading bar bitmap
    hrc::time_point m_barLastUpdate{};
    HBITMAP         m_barBitmap{};
    HDC             m_barContext{};
    HRGN            m_barRegion{};
    int             m_barWidth{};
    int             m_barHeight{};
    int             m_barX{};
    int             m_barY{};
};

static Splash g_splash{};

/**
 * @brief The window procedure for the splash window.
 */
static long CALLBACK HandleSplashWindowMessage(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_ERASEBKGND)
        return 1;

    if (message == WM_PAINT)
    {
        PAINTSTRUCT ps{};
        HDC         windowContext = BeginPaint(window, &ps);
        {
            g_splash.Paint(windowContext, ps.rcPaint.top == 0);
        }
        EndPaint(window, &ps);
        return 0;
    }

    if (message == WM_DESTROY)
    {
        g_splash.OnDestroy();
        return 0;
    }

    return DefWindowProc(window, message, wParam, lParam);
}

/**
 * @brief Creates the splash window.
 * @param instance The instance handle of the loader DLL.
 * @return true if the window was created successfully, false otherwise.
 */
bool Splash::CreateSplashWindow(HINSTANCE instance)
{
    if (m_window != nullptr)
        return false;

    WNDCLASS windowClass{};
    windowClass.lpfnWndProc = &HandleSplashWindowMessage;
    windowClass.style = 0;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hIcon = LoadIconA(GetModuleHandle(nullptr), MAKEINTRESOURCE(110));            // IDI_ICON1 from Launcher
    windowClass.lpszClassName = TEXT("SplashWindow");

    if (!RegisterClass(&windowClass))
    {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
            return false;
    }

    HWND window = CreateWindow(windowClass.lpszClassName, "Multi Theft Auto Launcher", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                               NULL, NULL, instance, NULL);

    if (window == nullptr)
    {
        UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
        return false;
    }

    // The source bitmap is 1000x500, but we render it at half size.
    SIZE monitorSize = GetPrimaryMonitorSize();
    UINT dpi = GetWindowDpi(window);
    m_width = ScaleToDpi(500, dpi);
    m_height = ScaleToDpi(250, dpi);
    int x = (monitorSize.cx - m_width) / 2;
    int y = (monitorSize.cy - m_height) / 2;
    MoveWindow(window, x, y, m_width, m_height, TRUE);
    ShowWindow(window, SW_SHOW);

    // Calculate the position and size of the loading bar.
    m_barLastUpdate = hrc::now();
    m_barX = {};
    m_barY = ScaleToDpi(197, dpi);
    m_barWidth = m_width;
    m_barHeight = ScaleToDpi(5, dpi) + 1;            // We add 1 pixel because scaling can cause the bar to be too small.

    m_windowClass = windowClass;
    m_window = window;
    return true;
}

/**
 * @brief Destroys the splash window and unregisters its window class.
 */
void Splash::DestroySplashWindow()
{
    ReleaseDeviceResources();

    if (m_window != nullptr)
    {
        DestroyWindow(m_window);
        m_window = {};
    }

    if (m_windowClass.hInstance != nullptr)
    {
        UnregisterClass(m_windowClass.lpszClassName, m_windowClass.hInstance);
        m_windowClass = {};
    }
}

/**
 * @brief Creates the device context and bitmap with the stretched splash image for rendering.
 */
bool Splash::CreateDeviceResources()
{
    if (!m_window)
        return false;

    HBITMAP backgroundResource = LoadBitmap(m_windowClass.hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
    HBITMAP barResource = LoadBitmap(m_windowClass.hInstance, MAKEINTRESOURCE(IDB_BITMAP2));

    if (backgroundResource == nullptr || barResource == nullptr)
        return false;

    HDC windowContext = GetDC(m_window);

    // Background
    HDC sourceContext = CreateCompatibleDC(windowContext);
    SelectObject(sourceContext, backgroundResource);
    {
        HBITMAP backgroundBitmap = CreateCompatibleBitmap(windowContext, m_width, m_height);
        HDC     renderContext = CreateCompatibleDC(windowContext);
        SelectObject(renderContext, backgroundBitmap);

        BITMAP source{};
        GetObject(backgroundResource, sizeof(source), &source);

        SetStretchBltMode(renderContext, HALFTONE);
        StretchBlt(renderContext, 0, 0, m_width, m_height, sourceContext, 0, 0, source.bmWidth, source.bmHeight, SRCCOPY);

        m_bgBitmap = backgroundBitmap;
        m_bgContext = renderContext;
    }

    // Loading bar
    SelectObject(sourceContext, barResource);
    {
        HBITMAP barBitmap = CreateCompatibleBitmap(windowContext, m_barWidth, m_barHeight);
        HDC     renderContext = CreateCompatibleDC(windowContext);
        SelectObject(renderContext, barBitmap);

        BITMAP source{};
        GetObject(barResource, sizeof(source), &source);

        SetStretchBltMode(renderContext, HALFTONE);
        StretchBlt(renderContext, 0, 0, m_barWidth, m_barHeight, sourceContext, 0, 0, source.bmWidth, source.bmHeight, SRCCOPY);

        m_barBitmap = barBitmap;
        m_barContext = renderContext;
        m_barRegion = CreateRectRgn(0, m_barY, m_width, m_barY + m_barHeight);
    }

    DeleteDC(sourceContext);
    DeleteObject(barResource);
    DeleteObject(backgroundResource);

    ReleaseDC(m_window, windowContext);
    return true;
}

/**
 * @brief Releases the device context and bitmaps used for rendering.
 */
void Splash::ReleaseDeviceResources()
{
    if (m_bgContext)
    {
        DeleteDC(m_bgContext);
        m_bgContext = {};
    }

    if (m_bgBitmap)
    {
        DeleteObject(m_bgBitmap);
        m_bgBitmap = {};
    }

    if (m_barContext)
    {
        DeleteDC(m_barContext);
        m_barContext = {};
    }

    if (m_barBitmap)
    {
        DeleteObject(m_barBitmap);
        m_barBitmap = {};
    }

    if (m_barRegion != nullptr)
    {
        DeleteObject(m_barRegion);
        m_barRegion = {};
    }
}

/**
 * @brief Polls the splash window message queue and updates the loading bar.
 */
bool Splash::Update()
{
    if (!m_window)
        return false;

    MSG msg{};

    while (PeekMessage(&msg, m_window, 0, 0, PM_NOREMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        if (GetMessage(&msg, m_window, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    UpdateLoadingBar();
    return true;
}

/**
 * @brief Called when the splash window is destroyed.
 */
void Splash::OnDestroy()
{
    m_window = {};
}

/**
 * @brief Renders the splash window.
 * @param windowContext The device context of the window.
 * @param drawBackground If true, the background must be drawn.
 */
void Splash::Paint(HDC windowContext, bool drawBackground) const
{
    if (drawBackground)
    {
        SelectClipRgn(windowContext, nullptr);
        BitBlt(windowContext, 0, 0, m_width, m_height, m_bgContext, 0, 0, SRCCOPY);
    }

    SelectClipRgn(windowContext, m_barRegion);

    if (m_barX > 0)
        BitBlt(windowContext, 0, m_barY, m_width, m_height, m_barContext, m_width - m_barX, 0, SRCCOPY);

    BitBlt(windowContext, m_barX, m_barY, m_width, m_height, m_barContext, 0, 0, SRCCOPY);
}

/**
 * @brief Updates the progress of the loading bar and invalidates the loading bar region of the window.
 */
void Splash::UpdateLoadingBar()
{
    constexpr int       PIXELS_PER_UPDATE = 3;
    constexpr long long UPDATE_RATE_IN_MS = 16;

    const hrc::time_point now = hrc::now();
    const long long       elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_barLastUpdate).count();

    if (elapsed < UPDATE_RATE_IN_MS)
        return;

    const long long numUpdates = elapsed / UPDATE_RATE_IN_MS;
    m_barLastUpdate += std::chrono::milliseconds(numUpdates * UPDATE_RATE_IN_MS);

    m_barX += PIXELS_PER_UPDATE * numUpdates;

    if (m_barX >= m_width)
        m_barX = 0;

    // Only invalidate the loading bar region to avoid flickering.
    RECT invalidate{0, m_barY, m_width, m_barY + m_barHeight};
    InvalidateRect(m_window, &invalidate, FALSE);
}

/**
 * @brief Shows the splash window.
 */
void Splash::Show() const
{
    if (m_window)
    {
        ShowWindow(m_window, SW_SHOW);
    }
}

/**
 * @brief Hides the splash window.
 */
void Splash::Hide() const
{
    if (m_window)
    {
        ShowWindow(m_window, SW_HIDE);
    }
}

/**
 * @brief Brings the splash window to the front.
 */
void Splash::BringToFront() const
{
    if (m_window)
    {
        SetForegroundWindow(m_window);
        SetWindowPos(m_window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Splash thread logic.
//
///////////////////////////////////////////////////////////////////////////

class SplashThread final
{
public:
    void Create(HINSTANCE instance);
    void Destroy();

    bool Exists() const { return m_thread != nullptr; }
    bool ShouldExit() const { return m_exitEvent == nullptr || WaitForSingleObject(m_exitEvent, 0) == WAIT_OBJECT_0; }

    void Run(HINSTANCE instance);
    void PostRun();

private:
    HANDLE m_thread{};
    HANDLE m_exitEvent{};
};

static SplashThread g_splashThread{};

/**
 * @brief The entry point for the splash window thread.
 */
static DWORD WINAPI RunSplashThread(LPVOID instance)
{
    g_splashThread.Run(static_cast<HINSTANCE>(instance));
    g_splashThread.PostRun();
    return 0;
}

/**
 * @brief Creates the splash window thread.
 * @param instance The instance handle of the loader DLL.
 */
void SplashThread::Create(HINSTANCE instance)
{
    if (m_thread != nullptr)
        return;

    HANDLE exitEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    if (exitEvent == nullptr)
        return;

    HANDLE thread = CreateThread(nullptr, 0, RunSplashThread, instance, 0, nullptr);

    if (thread == nullptr)
    {
        CloseHandle(exitEvent);
        return;
    }

    m_thread = thread;
    m_exitEvent = exitEvent;
}

/**
 * @brief Destroys the splash window thread and the splash window, if it still exists.
 */
void SplashThread::Destroy()
{
    if (m_thread)
    {
        SetEvent(m_exitEvent);
        WaitForSingleObject(m_thread, 3000);
        CloseHandle(m_thread);
        m_thread = {};
    }

    if (m_exitEvent)
    {
        CloseHandle(m_exitEvent);
        m_exitEvent = {};
    }

    g_splash.DestroySplashWindow();
}

/**
 * @brief The main loop of the splash window thread.
 * @param instance The instance handle of the loader DLL.
 */
void SplashThread::Run(HINSTANCE instance)
{
    if (!g_splash.CreateSplashWindow(instance))
        return;

    if (!g_splash.CreateDeviceResources())
    {
        g_splash.DestroySplashWindow();
        return;
    }

    MSG msg{};

    while (true)
    {
        if (g_splashThread.ShouldExit())
            break;

        if (!g_splash.Update())
            break;

        Sleep(10);
    }

    g_splash.DestroySplashWindow();
}

/**
 * @brief Cleans up the splash window thread on thread exit.
 */
void SplashThread::PostRun()
{
    if (m_exitEvent)
    {
        CloseHandle(m_exitEvent);
        m_exitEvent = {};
    }

    if (m_thread)
    {
        CloseHandle(m_thread);
        m_thread = {};
    }
}

///////////////////////////////////////////////////////////////////////////
//
// Splash interface functions.
//
///////////////////////////////////////////////////////////////////////////

#ifdef MTA_DEBUG

void ShowSplash(HINSTANCE instance)
{
}

void HideSplash()
{
}

void SuspendSplash()
{
}

void ResumeSplash()
{
}

#else

void ShowSplash(HINSTANCE instance)
{
    if (g_splashThread.Exists())
    {
        g_splash.BringToFront();
    }
    else
    {
        g_splashThread.Create(instance);
    }
}

void HideSplash()
{
    g_splashThread.Destroy();
    g_splash.DestroySplashWindow();
}

void SuspendSplash()
{
    g_splash.Hide();
}

void ResumeSplash()
{
    g_splash.Show();
}

#endif

///////////////////////////////////////////////////////////////////////////
//
// Utility functions.
//
///////////////////////////////////////////////////////////////////////////

/**
 * @brief Returns the dots per inch (dpi) value for the specified window.
 */
static UINT GetWindowDpi(HWND window)
{
    // Minimum version: Windows 10, version 1607
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);

    static GetDpiForWindow_t Win32GetDpiForWindow = ([] {
        HMODULE user32 = LoadLibrary("user32");
        return user32 ? reinterpret_cast<GetDpiForWindow_t>(static_cast<void*>(GetProcAddress(user32, "GetDpiForWindow"))) : nullptr;
    })();

    if (Win32GetDpiForWindow)
        return Win32GetDpiForWindow(window);

    HDC  screenDC = GetDC(NULL);
    auto x = static_cast<UINT>(GetDeviceCaps(screenDC, LOGPIXELSX));
    ReleaseDC(NULL, screenDC);
    return x;
}

/**
 * @brief Returns the width and height of the primary monitor.
 */
static SIZE GetPrimaryMonitorSize()
{
    POINT    zero{};
    HMONITOR primaryMonitor = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(primaryMonitor, &monitorInfo);

    const RECT& work = monitorInfo.rcWork;
    return {work.right - work.left, work.bottom - work.top};
}

/**
 * @brief Scales the value from the default screen dpi (96) to the provided dpi.
 */
static LONG ScaleToDpi(LONG value, UINT dpi)
{
    return static_cast<LONG>(ceil(value * static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI));
}
