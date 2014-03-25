/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rwapi.h
*  PURPOSE:     Header file for RenderWare native code implementation
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRENDERWARESA_RWAPI
#define __CRENDERWARESA_RWAPI

extern RwScene *const *p_gtaScene;

// Internal RenderWare functions
RwTexture*          RwFindTexture( const char *name, const char *secName );     // US exe: 0x007F3AC0
RwError*            RwSetError( RwError *info );                                // US exe: 0x00808820

// Module initialization.
void RenderWareAPI_Init( void );
void RenderWareAPI_Shutdown( void );

#endif //__CRENDERWARESA_RWAPI
