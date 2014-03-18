/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwTexture.h
*  PURPOSE:     RenderWare native function implementations
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef _RENDERWARE_TEXTURE_
#define _RENDERWARE_TEXTURE_

struct RwRaster
{
    RwRaster*               parent;                             // 0
    unsigned char*          pixels;                             // 4
    unsigned char*          palette;                            // 8
    int                     width, height, depth;               // 12, 16 / 0x10, 20
    int                     stride;                             // 24 / 0x18
    short                   u, v;                               // 28
    unsigned char           type;                               // 32
    unsigned char           flags;                              // 33
    unsigned char           privateFlags;                       // 34
    unsigned char           format;                             // 35
    unsigned char*          origPixels;                         // 36
    int                     origWidth, origHeight, origDepth;   // 40

    // Begin of Direct3D9_Raster extension
    IDirect3DBaseTexture9*  renderResource;                     // 52, Direct3D texture resource
    BYTE                    pad[4];                             // 56
    bool                    isAlpha;                            // 60
};
struct RwTexture
{
    RwRaster*                   raster;                         // 0
    RwTexDictionary*            txd;                            // 4
    RwListEntry <RwTexture>     TXDList;                        // 8
    char                        name[RW_TEXTURE_NAME_LENGTH];   // 16
    char                        mask[RW_TEXTURE_NAME_LENGTH];   // 48

    union
    {
        unsigned int            flags;                          // 80

        struct
        {
            unsigned char       flags_a;                        // 80
            unsigned char       flags_b;                        // 81
            unsigned char       flags_c;                        // 82
            unsigned char       flags_d;                        // 83
        };
    };
    unsigned int                refs;                           // 84
    char                        anisotropy;                     // 88

    // Methods.
    void                        SetName                 ( const char *name );

    void                        AddToDictionary         ( RwTexDictionary *txd );
    void                        RemoveFromDictionary    ( void );

    // Flag derived from research by MTA team.
    void                        SetFiltering            ( bool filter )     { BOOL_FLAG( flags, 0x1102, filter ); }
    bool                        IsFiltering             ( void ) const      { return IS_FLAG( flags, 0x1102 ); }
};

struct RwTextureCoordinates
{
    float u,v;
};

// Texture API.
RwTexture*          RwTextureStreamReadEx           ( RwStream *stream );

#endif //_RENDERWARE_TEXTURE_