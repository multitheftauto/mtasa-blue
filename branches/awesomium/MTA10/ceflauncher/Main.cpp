#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cef3/include/cef_app.h>

int WINAPI WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdShow, int nCmdShow )
{
    CefMainArgs mainArgs ( hInstance );

    return CefExecuteProcess ( mainArgs, NULL, NULL );
}
