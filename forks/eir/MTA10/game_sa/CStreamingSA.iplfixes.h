/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.iplfixes.h
*  PURPOSE:     IPL sector streaming
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_IPLFIXES_
#define _STREAMING_IPLFIXES_

// Nasty hooks.
void __cdecl HOOK_LoadIPLInstance ( SIPLInst *pAddInst );
void __cdecl HOOK_CWorld_LOD_SETUP ( CEntitySAInterface *pLodEntity );

// Module initialization.
void StreamingIPLFIXES_Init( void );
void StreamingIPLFIXES_Shutdown( void );

#endif //_STREAMING_IPLFIXES_