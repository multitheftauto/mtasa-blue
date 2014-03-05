/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.textures.h
*  PURPOSE:     Texture streaming routines
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_TEXTURES_
#define _STREAMING_TEXTURES_

// Imported textures scanning
namespace RwImportedScan
{
    RwTexture* scanMethodEx( int id, const char *name );

    void Apply( unsigned short model );
    void Unapply( void );
};

// Remap VEHICLE.TXD export
namespace RwRemapScan
{
    void Apply( void );
    void Unapply( void );
};

// Texture streaming exports.
bool __cdecl LoadTXDFirstHalf( unsigned int id, RwStream *stream );
bool __cdecl LoadTXDContinue( unsigned int id, RwStream *stream );

#endif //_STREAMING_TEXTURES_