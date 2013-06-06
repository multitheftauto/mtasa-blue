/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.utils.h
*  PURPOSE:     Data streamer utilities
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_UTILITIES_
#define _STREAMING_UTILITIES_

namespace Streaming
{
    extern streamingLoadCallback_t  streamingLoadCallback;
};

// Imported textures scanning
namespace RwImportedScan
{
    void Apply( unsigned short model );
    void Unapply( void );
};

// Remap VEHICLE.TXD export
namespace RwRemapScan
{
    void Apply( void );
    void Unapply( void );
};

// This is an optimization because the computer binary system works that way.
// Removes the need for additional clock cycles by avoiding conversion to integer.
#define idOffset( num, off )                ((modelId_t)( (modelId_t)(num) - (modelId_t)(off) ))
#define idRangeCheck( num, off, range )     (idOffset((num),(off)) < (modelId_t)(range))
#define idRangeCheckEx( num, range )        ((modelId_t)(num) < (modelId_t)(range))

// Used by CRenderWareSA
void RpClumpAtomicActivator( RpAtomic *atom, modelId_t replacerId );

// Used by streaming
void __cdecl FreeCOLLibrary( unsigned char collId );
bool __cdecl CheckTXDDependency( modelId_t id );
bool __cdecl CheckAnimDependency( modelId_t id );
bool __cdecl LoadModel( void *buf, modelId_t id, unsigned int threadId );

modelId_t __cdecl ProcessLoadQueue( unsigned int offset, bool favorPriority );
bool __cdecl ProcessStreamingRequest( modelId_t id );
void __cdecl PulseStreamingRequests( void );
void __cdecl PulseStreamingRequest( unsigned int reqId );

#endif //_STREAMING_UTILITIES_