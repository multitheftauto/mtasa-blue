/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCacheSA.h
*  PURPOSE:     Engine caching system
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _ENGINE_CACHING_
#define _ENGINE_CACHING_

// Exported utilities.
bool __cdecl Cache_IsWritingMode( void );
void __cdecl Cache_StoreCollision( unsigned int id, CColModelSAInterface *col );
void __cdecl Cache_WriteCollision( void );
void __cdecl Cache_RestoreColFile( CColFileSA *colFile );
void __cdecl Cache_StoreColFile( CColFileSA colFile );
void __cdecl Cache_RestoreIPLFile( CIPLFileSA& iplFile, unsigned int iplIndex );
void __cdecl Cache_StoreIPLFile( unsigned int iplIndex, CIPLFileSA iplFile );
void __cdecl Cache_LoadCollision( void );
void __cdecl Cache_IncrementBySector( CEntitySAInterface **buildingArray );

// Module Initialization.
void Cache_Init( void );
void Cache_Shutdown( void );

#endif //_ENGINE_CACHING_