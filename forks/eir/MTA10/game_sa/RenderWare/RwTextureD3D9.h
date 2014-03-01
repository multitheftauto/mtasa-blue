/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTextureD3D9.h
*  PURPOSE:     RenderWare texture implementation for D3D9
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_TEXTURE_PLUGIN_D3D9_
#define _RENDERWARE_TEXTURE_PLUGIN_D3D9_

int __cdecl RwD3D9SetTexture( RwTexture *texture, unsigned int stageIndex );

void __cdecl RwD3D9RenderStateSetVertexAlphaEnabled( DWORD enabled );
DWORD __cdecl RwD3D9RenderStateIsVertexAlphaEnabled( void );

// TODO: think about improving my RenderWare definitions:
// * plugins should not plant OOP functions into the classes.
int __cdecl RwTextureHasAlpha( RwTexture *tex );

// Module initialization.
void RwTextureD3D9_Init( void );
void RwTextureD3D9_Shutdown( void );

#endif //_RENDERWARE_TEXTURE_PLUGIN_D3D9_