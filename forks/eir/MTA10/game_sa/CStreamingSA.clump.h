/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.clump.h
*  PURPOSE:     Atomic and clump file streaming.
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_CLUMP_
#define _STREAMING_CLUMP_

// Exported utilities
void _initAtomicNormals( RpAtomic *atom );

// Clump streaming exports.
bool __cdecl LoadClumpFile( RwStream *stream, modelId_t model );
bool __cdecl LoadClumpFilePersistent( RwStream *stream, modelId_t id );
bool __cdecl LoadClumpFileBigContinue( RwStream *stream, modelId_t id );

// Exports.
void RpClumpAtomicActivator( RpAtomic *atom, modelId_t replacerId );

#endif //_STREAMING_CLUMP_