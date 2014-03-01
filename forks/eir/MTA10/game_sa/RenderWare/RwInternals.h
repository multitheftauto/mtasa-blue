/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwInternals.h
*  PURPOSE:     RenderWare internal definitions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_INTERNALS_
#define _RENDERWARE_INTERNALS_

#define MAX_SAMPLERS    8

// padlevel: 2
struct d3d9RasterStage  //size: 24 bytes
{
    RwRaster *raster;                   // 0
    unsigned int u_addressMode;         // 4
    unsigned int v_addressMode;         // 8
    unsigned int filterType;            // 12
    RwColor borderColor;                // 16
    unsigned int maxAnisotropy;         // 20
};

inline d3d9RasterStage& GetRasterStageInfo( unsigned int index )
{
    return ((d3d9RasterStage*)0x00C9A508)[index];
}

// Template parameters for easier management.
struct deviceStateValue
{
    deviceStateValue( void )
    {
        // We have to initialize this parameter this way.
        // Later in the engine we have to set up these values properly
        // That is looping through all TSS and applying them to our
        // internal table.
        value = 0xFFFFFFFF;
    }

    void operator = ( DWORD value )
    {
        this->value = value;
    }

    operator DWORD ( void ) const
    {
        return value;
    }

    operator DWORD& ( void )
    {
        return value;
    }

    DWORD value;
};

#endif //_RENDERWARE_INTERNALS_