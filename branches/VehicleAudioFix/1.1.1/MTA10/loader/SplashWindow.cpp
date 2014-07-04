//
// aru aru!
//

#include "StdInc.h"
#include "SplashWindow.h"
#include "resource.h"

int SplashWindow::m_iResult = 0;

void SplashWindow::Initialize(HINSTANCE hInst, int iWhichNag )
{
    m_bShutdown = false;
    m_iResult = 0;

    WNDCLASSEX wcx;

    ZeroMemory(&wcx, sizeof(wcx));
    wcx.cbSize = sizeof(wcx);
    wcx.style = 0;
    wcx.lpfnWndProc = (WNDPROC)WindowProc;
    wcx.cbClsExtra = 0;
    wcx.hInstance = hInst;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcx.lpszClassName = "MTASASplash";

    BOOL result = RegisterClassEx( &wcx );

    RECT rect;
    GetWindowRect( GetDesktopWindow(), &rect );

    rect.left += ( ( rect.right - rect.left ) - 640 )/2;
    rect.top += ( ( rect.bottom - rect.top ) - 480 )/2;

    m_hWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_STATICEDGE | WS_EX_TOOLWINDOW,
        "MTASASplash", "MTASA", WS_POPUP, rect.left, rect.top, 640, 480, 
        (HWND)NULL, (HMENU)NULL, hInst, NULL);

    SetWindowRgn( m_hWnd, NULL, FALSE );

    m_hBmp = (HBITMAP)LoadImage( hInst, iWhichNag == 0 ? MAKEINTRESOURCE( IDB_NAG1 ) : MAKEINTRESOURCE( IDB_NAG2 ), IMAGE_BITMAP, 0, 0, 0 );
    m_hRgn = CreateRgnFromFile( m_hBmp, RGB(255, 0, 0) ); 
    SetWindowRgn( m_hWnd, m_hRgn, FALSE );

    SetTimer( m_hWnd, 1, 100, 0 );
}

void SplashWindow::Shutdown()
{
    DeleteObject((HGDIOBJ)m_hBmp);

    DeleteObject((HGDIOBJ)m_hRgn);

    DestroyWindow( m_hWnd );
}

HRGN SplashWindow::CreateRgnFromFile( HBITMAP hBmp, COLORREF color )
{
    // From CodeProject:
    // http://www.codeproject.com/KB/GDI/coolrgn.aspx

    // get image properties
    BITMAP bmp = { 0 };
    GetObject( hBmp, sizeof(BITMAP), &bmp );
    // allocate memory for extended image information
    LPBITMAPINFO bi = (LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
    memset( bi, 0, sizeof(BITMAPINFO) + 8 );
    bi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    // set window size
    DWORD dwWidth	= bmp.bmWidth;		// bitmap width
    DWORD dwHeight	= bmp.bmHeight;		// bitmap height
    // create temporary dc
    HDC dc = CreateIC( "DISPLAY",NULL,NULL,NULL );
    // get extended information about image (length, compression, length of color table if exist, ...)
    DWORD res = GetDIBits( dc, hBmp, 0, bmp.bmHeight, 0, bi, DIB_RGB_COLORS );
    // allocate memory for image data (colors)
    LPBYTE pBits = new BYTE[ bi->bmiHeader.biSizeImage + 4 ];
    // allocate memory for color table
    if ( bi->bmiHeader.biBitCount == 8 )
    {
        // actually color table should be appended to this header(BITMAPINFO),
        // so we have to reallocate and copy it
        LPBITMAPINFO old_bi = bi;
        // 255 - because there is one in BITMAPINFOHEADER
        bi = (LPBITMAPINFO)new char[ sizeof(BITMAPINFO) + 255 * sizeof(RGBQUAD) ];
        memcpy( bi, old_bi, sizeof(BITMAPINFO) );
        // release old header
        delete old_bi;
    }
    // get bitmap info header
    BITMAPINFOHEADER& bih = bi->bmiHeader;
    // get color table (for 256 color mode contains 256 entries of RGBQUAD(=DWORD))
    LPDWORD clr_tbl = (LPDWORD)&bi->bmiColors;
    // fill bits buffer
    res = GetDIBits( dc, hBmp, 0, bih.biHeight, pBits, bi, DIB_RGB_COLORS );
    DeleteDC( dc );

    BITMAP bm;
    GetObject( hBmp, sizeof(BITMAP), &bm );
    // shift bits and byte per pixel (for comparing colors)
    LPBYTE pClr = (LPBYTE)&color;

    // swap red and blue components
    BYTE tmp = pClr[0]; pClr[0] = pClr[2]; pClr[2] = tmp;
    // convert color if curent DC is 16-bit (5:6:5) or 15-bit (5:5:5)
    if ( bih.biBitCount == 16 )
    {
        // for 16 bit
        color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
            ((DWORD)(pClr[1] & 0xfc) << 3) |
            ((DWORD)(pClr[2] & 0xf8) << 8);
        // for 15 bit
        //		color = ((DWORD)(pClr[0] & 0xf8) >> 3) |
        //				((DWORD)(pClr[1] & 0xf8) << 2) |
        //				((DWORD)(pClr[2] & 0xf8) << 7);
    }

    const DWORD RGNDATAHEADER_SIZE	= sizeof(RGNDATAHEADER);
    const DWORD ADD_RECTS_COUNT		= 40;			// number of rects to be appended
    // to region data buffer

    // BitPerPixel
    BYTE	Bpp = bih.biBitCount >> 3;				// bytes per pixel
    // bytes per line in pBits is DWORD aligned and bmp.bmWidthBytes is WORD aligned
    // so, both of them not
    DWORD m_dwAlignedWidthBytes = (bmp.bmWidthBytes & ~0x3) + (!!(bmp.bmWidthBytes & 0x3) << 2);
    // DIB image is flipped that's why we scan it from the last line
    LPBYTE	pColor = pBits + (bih.biHeight - 1) * m_dwAlignedWidthBytes;
    DWORD	dwLineBackLen = m_dwAlignedWidthBytes + bih.biWidth * Bpp;	// offset of previous scan line
    // (after processing of current)
    DWORD	dwRectsCount = bih.biHeight;			// number of rects in allocated buffer
    INT		i, j;									// current position in mask image
    INT		first = 0;								// left position of current scan line
    // where mask was found
    bool	wasfirst = false;						// set when mask has been found in current scan line
    bool	ismask;									// set when current color is mask color

    // allocate memory for region data
    // region data here is set of regions that are rectangles with height 1 pixel (scan line)
    // that's why first allocation is <bm.biHeight> RECTs - number of scan lines in image
    RGNDATAHEADER* pRgnData = 
        (RGNDATAHEADER*)new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
    // get pointer to RECT table
    LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
    // zero region data header memory (header  part only)
    memset( pRgnData, 0, RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) );
    // fill it by default
    pRgnData->dwSize	= RGNDATAHEADER_SIZE;
    pRgnData->iType		= RDH_RECTANGLES;

    for ( i = 0; i < bih.biHeight; i++ )
    {
        for ( j = 0; j < bih.biWidth; j++ )
        {
            // get color
            switch ( bih.biBitCount )
            {
            case 8:
                ismask = (clr_tbl[ *pColor ] != color);
                break;
            case 16:
                ismask = (*(LPWORD)pColor != (WORD)color);
                break;
            case 24:
                ismask = ((*(LPDWORD)pColor & 0x00ffffff) != color);
                break;
            case 32:
                ismask = (*(LPDWORD)pColor != color);
            }
            // shift pointer to next color
            pColor += Bpp;
            // place part of scan line as RECT region if transparent color found after mask color or
            // mask color found at the end of mask image
            if ( wasfirst )
            {
                if ( !ismask )
                {
                    // save current RECT
                    RECT r;
                    r.left = first;
                    r.top = i;
                    r.right = j;
                    r.bottom = i + 1;

                    pRects[ pRgnData->nCount++ ] = r;
                    // if buffer full reallocate it with more room
                    if ( pRgnData->nCount >= dwRectsCount )
                    {
                        dwRectsCount += ADD_RECTS_COUNT;
                        // allocate new buffer
                        LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
                        // copy current region data to it
                        memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
                        // delte old region data buffer
                        delete pRgnData;
                        // set pointer to new regiondata buffer to current
                        pRgnData = (RGNDATAHEADER*)pRgnDataNew;
                        // correct pointer to RECT table
                        pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
                    }
                    wasfirst = false;
                }
            }
            else if ( ismask )		// set wasfirst when mask is found
            {
                first = j;
                wasfirst = true;
            }
        }

        if ( wasfirst && ismask )
        {
            // save current RECT
            RECT r;
            r.left = first;
            r.top = i;
            r.right = j;
            r.bottom = i + 1;

            pRects[ pRgnData->nCount++ ] = r;
            // if buffer full reallocate it with more room
            if ( pRgnData->nCount >= dwRectsCount )
            {
                dwRectsCount += ADD_RECTS_COUNT;
                // allocate new buffer
                LPBYTE pRgnDataNew = new BYTE[ RGNDATAHEADER_SIZE + dwRectsCount * sizeof(RECT) ];
                // copy current region data to it
                memcpy( pRgnDataNew, pRgnData, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT) );
                // delte old region data buffer
                delete pRgnData;
                // set pointer to new regiondata buffer to current
                pRgnData = (RGNDATAHEADER*)pRgnDataNew;
                // correct pointer to RECT table
                pRects = (LPRECT)((LPBYTE)pRgnData + RGNDATAHEADER_SIZE);
            }
            wasfirst = false;
        }

        pColor -= dwLineBackLen;
    }
    // release image data
    delete pBits;
    delete bi;

    // create region
    HRGN hRgn = ExtCreateRegion( NULL, RGNDATAHEADER_SIZE + pRgnData->nCount * sizeof(RECT), (LPRGNDATA)pRgnData );
    // release region data
    delete pRgnData;

    return hRgn;
}

void SplashWindow::Show()
{
    ShowWindow( m_hWnd, SW_SHOW );
    UpdateWindow( m_hWnd );
}

int SplashWindow::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT paint;

            BeginPaint( hWnd, &paint );

            HDC hdc = CreateCompatibleDC( paint.hdc );
            SelectObject( hdc, GetInstance().m_hBmp );

            BitBlt( paint.hdc, 0, 0, 640, 480, hdc, 1, 1, SRCCOPY );

            DeleteDC( hdc );

            EndPaint( hWnd, &paint );

            return 0;
        }
        break;

        case WM_TIMER:
        {
            UINT nIDEvent = wParam;

            if ( nIDEvent == 1 )
            {
                HWND grcWindow = ::FindWindow("grcWindow", NULL);
                if ( grcWindow )
                {
                    KillTimer(GetInstance().m_hWnd, 1);
                    SetTimer(GetInstance().m_hWnd, 2, 2000, 0);
                }

                DWORD dwExitCode;
                HANDLE process = GetInstance().m_hWaitForProcess;
                if ( process != NULL && GetExitCodeProcess( process, &dwExitCode ) )
                {
                    if ( dwExitCode != STILL_ACTIVE )
                    {
                        GetInstance().m_bShutdown = true;
                    }
                }

                return 0;
            }
            else if ( nIDEvent == 2 )
            {
                GetInstance().m_bShutdown = true;

                return 0;
            }
        }
        break;

        case WM_KEYUP:
        {
            if ( wParam == VK_ESCAPE )
            {
                m_iResult = IDNO;
                return 0;
            }
        }
        break;

        case WM_LBUTTONUP:
        {
            int x = LOWORD(lParam); 
            int y = HIWORD(lParam); 

            m_iResult = 0;
            if ( x > 141 && y > 254 && x < 348 && y < 314 )
                m_iResult = IDYES;
            if ( x > 162 && y > 327 && x < 340 && y < 350 )
                m_iResult = IDNO;
        }
        break;

    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

int SplashWindow::PulseMessagePump()
{
    MSG msg;
    if (PeekMessage( &msg, m_hWnd, 0, 0, PM_REMOVE ))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if ( !IsWindowVisible(m_hWnd) )
        return IDNO;

    return m_iResult;

/*
    if (m_bShutdown || !IsWindowVisible(m_hWnd))
    {
        return false;
    }

    return true;
*/
}
