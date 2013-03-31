/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rwapi.h
*  PURPOSE:     Header file for RenderWare native code implementation
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRENDERWARESA_RWAPI
#define __CRENDERWARESA_RWAPI

extern RwScene *const *p_gtaScene;

// Internal RenderWare functions
RwTexture*          RwFindTexture           ( const char *name, const char *secName );  // US exe: 0x007F3AC0
RwError*            RwSetError              ( RwError *info );                          // US exe: 0x00808820
RpLight*            RpLightCreate           ( unsigned char type );                     // US exe: 0x00752110
RpClump*            RpClumpCreate           ( void );                                   // US exe: 0x0074A290
RwCamera*           RwCameraCreate          ( void );                                   // US exe: 0x007EE4F0
RwTexDictionary*    RwTexDictionaryCreate   ( void );                                   // US exe: 0x007F3600

#endif //__CRENDERWARESA_RWAPI
