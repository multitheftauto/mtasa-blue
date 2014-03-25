/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.loader.h
*  PURPOSE:     Collision model entity
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _COLLISION_LOADER_
#define _COLLISION_LOADER_

// Collision loading routines.
void __cdecl LoadCollisionModel( const char *pBuffer, CColModelSAInterface *pCollision, const char *colName );
void __cdecl LoadCollisionModelVer2( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName );
void __cdecl LoadCollisionModelVer3( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName );
void __cdecl LoadCollisionModelVer4( const char *pBuffer, int bufferSize, CColModelSAInterface *pCollision, const char *colName );

// GTA:SA functions, only here temporarily.
typedef void* (__cdecl*_mallocNew_t)( size_t memSize );

extern _mallocNew_t         _mallocNew;

#endif //_COLLISION_LOADER_