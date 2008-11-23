/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CDirect3DEvents9.cpp
*  PURPOSE:     Handler implementations for Direct3D 9 events
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <libpng/png.h>

BYTE** ppScreenData = NULL;

void CDirect3DEvents9::OnDirect3DDeviceCreate  ( IDirect3DDevice9 *pDevice )
{
	WriteDebugEvent ( "CDirect3DEvents9::OnDirect3DDeviceCreate" );

    // Create the GUI manager
	CCore::GetSingleton ( ).InitGUI ( pDevice );

    // Create all our fonts n stuff
    CGraphics::GetSingleton ().OnDeviceCreate ( pDevice );

    // Create the GUI elements
	CLocalGUI::GetSingleton().CreateObjects ( pDevice );    
}

void CDirect3DEvents9::OnDirect3DDeviceDestroy ( IDirect3DDevice9 *pDevice )
{
	WriteDebugEvent ( "CDirect3DEvents9::OnDirect3DDeviceDestroy" );

    // Unload the current mod
    CModManager::GetSingleton ().Unload ();

    // Destroy the GUI elements
    CLocalGUI::GetSingleton().DestroyObjects ();

    // De-initialize the GUI manager (destroying is done on Exit)
	CCore::GetSingleton ( ).DeinitGUI ();
}

void CDirect3DEvents9::OnBeginScene ( IDirect3DDevice9 *pDevice )
{
    // Notify core
    CCore::GetSingleton ().DoPreFramePulse ();
}

bool CDirect3DEvents9::OnEndScene ( IDirect3DDevice9 *pDevice )
{
    return true;
}

void CDirect3DEvents9::OnInvalidate ( IDirect3DDevice9 *pDevice )
{
	WriteDebugEvent ( "CDirect3DEvents9::OnInvalidate" );

	// Invalidate the VMR9 Manager
	CVideoManager::GetSingleton ().OnLostDevice ();

	// Invalidate the main menu
	CLocalGUI::GetSingleton().GetMainMenu ()->OnInvalidate ( pDevice );

	// Notify gui
    CLocalGUI::GetSingleton().Invalidate ();

	// Clean up the screen data buffer
	if ( ppScreenData ) {
		unsigned short height = (unsigned short)CDirect3DData::GetSingleton ().GetViewportHeight ();
		for ( unsigned short y = 0; y < height; y++ ) {
			delete [] ppScreenData[y];
		}
		delete [] ppScreenData;
	}

    CGraphics::GetSingleton ().OnDeviceInvalidate ( pDevice );
}

void CDirect3DEvents9::OnRestore ( IDirect3DDevice9 *pDevice )
{
	WriteDebugEvent ( "CDirect3DEvents9::OnRestore" );

	// Create the screen data buffer
	unsigned short width_channels = (unsigned short)(CDirect3DData::GetSingleton ().GetViewportWidth () * 4);
	unsigned short height = (unsigned short)(CDirect3DData::GetSingleton ().GetViewportHeight ());

	ppScreenData = new BYTE* [ height ];

	for ( unsigned short y = 0; y < height; y++ ) {
		ppScreenData[y] = new BYTE [ width_channels ];
	}

	// Restore the VMR9 manager
	CVideoManager::GetSingleton ().OnResetDevice ( pDevice );

	// Restore the main menu
	CLocalGUI::GetSingleton().GetMainMenu ()->OnRestore ( pDevice );

    // Restore the GUI
	CLocalGUI::GetSingleton().Restore ();

	// Restore the graphics manager
    CGraphics::GetSingleton ().OnDeviceRestore ( pDevice );
}

void CDirect3DEvents9::OnPresent ( IDirect3DDevice9 *pDevice )
{
    // Start a new scene. This isn't ideal and is not really recommended by MSDN.
    // I tried disabling EndScene from GTA and just end it after this code ourselves
    // before present, but that caused graphical issues randomly with the sky.
    pDevice->BeginScene ();

    // Notify core
    CCore::GetSingleton ().DoPostFramePulse ();

    // Create a state block.
    IDirect3DStateBlock9 * pDeviceState = NULL;
    pDevice->CreateStateBlock ( D3DSBT_ALL, &pDeviceState );

    // Draw pre-GUI primitives
    CGraphics::GetSingleton ().DrawPreGUIQueue ();

    // Draw the GUI
    CLocalGUI::GetSingleton().Draw ();

    // Draw post-GUI primitives
    CGraphics::GetSingleton ().DrawPostGUIQueue ();

    // Redraw the mouse cursor so it will always be over other elements
    CLocalGUI::GetSingleton().DrawMouseCursor();

    // Restore the render states
    if ( pDeviceState )
    {
        pDeviceState->Apply ( );
        pDeviceState->Release ( );
    }
    
    // End the scene that we started.
    pDevice->EndScene ();
    
	// Make a screenshot if needed
	if ( CCore::GetSingleton().bScreenShot ) {
		RECT ScreenSize;

		D3DLOCKED_RECT LockedRect;
		IDirect3DSurface9 *pSurface, *pLockSurface;
		char szFileName[MAX_PATH] = {0};

		// Define a screen rectangle
		ScreenSize.top = ScreenSize.left = 0;
		ScreenSize.right = CDirect3DData::GetSingleton ().GetViewportWidth ();
		ScreenSize.bottom = CDirect3DData::GetSingleton ().GetViewportHeight ();		
		pDevice->GetRenderTarget ( 0, &pSurface );

		// Create a new render target
		if ( pDevice->CreateRenderTarget ( ScreenSize.right, ScreenSize.bottom, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, TRUE, &pLockSurface, NULL ) != D3D_OK ) {
			CCore::GetSingleton ().GetConsole ()->Printf("Couldn't create a new render target.");
		} else {
			unsigned long ulBeginTime = GetTickCount ();

			// Copy data from surface to surface
			if ( pDevice->StretchRect ( pSurface, &ScreenSize, pLockSurface, &ScreenSize, D3DTEXF_NONE ) != D3D_OK ) {
				CCore::GetSingleton ().GetConsole ()->Printf("Couldn't copy the surface.");
			}

			// Lock the surface
			if ( pLockSurface->LockRect ( &LockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_NOSYSLOCK ) != D3D_OK ) {
				CCore::GetSingleton ().GetConsole ()->Printf("Couldn't lock the surface.");
			}

			// Call the pre-screenshot function	
			CScreenShot::PreScreenShot ( szFileName, MAX_PATH - 1 );

			unsigned long ulScreenHeight = ScreenSize.bottom - ScreenSize.top;
			unsigned long ulScreenWidth = ScreenSize.right - ScreenSize.left;

			// Copy the surface data into a row-based buffer for libpng
			#define BYTESPERPIXEL 4
			unsigned long ulLineWidth = ulScreenWidth * 4;
			for ( unsigned int i = 0; i < ulScreenHeight; i++ ) {
				memcpy ( ppScreenData[i], (BYTE*) LockedRect.pBits + i* LockedRect.Pitch, ulLineWidth );
				for ( unsigned int j = 3; j < ulLineWidth; j += BYTESPERPIXEL ) {
					ppScreenData[i][j] = 0xFF;
				}
			}

			// Unlock and release the surface
			pLockSurface->UnlockRect ();
			pLockSurface->Release ();

			// Save to png (strip the output alpha channel and read as BGR)
			FILE *file = fopen (szFileName, "wb");
				png_struct* png_ptr = png_create_write_struct ( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
				png_info* info_ptr = png_create_info_struct ( png_ptr );
				png_init_io ( png_ptr, file );
				png_set_filter ( png_ptr, 0, PNG_FILTER_NONE );
				png_set_compression_level ( png_ptr, 1 );
				png_set_IHDR ( png_ptr, info_ptr, ScreenSize.right, ScreenSize.bottom, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );
				png_set_rows ( png_ptr, info_ptr, ppScreenData );
				png_write_png ( png_ptr, info_ptr, PNG_TRANSFORM_BGR | PNG_TRANSFORM_STRIP_ALPHA, NULL );
				png_write_end ( png_ptr, info_ptr );
				png_destroy_write_struct ( &png_ptr, &info_ptr );
			fclose(file);

			// Call the post-screenshot function
			CScreenShot::PostScreenShot ( szFileName );

			CCore::GetSingleton ().GetConsole ()->Printf ( "Screenshot capture took %.2f seconds.", (float)(GetTickCount () - ulBeginTime) / 1000.0f );
		}

		CCore::GetSingleton().bScreenShot = false;
	}
}
