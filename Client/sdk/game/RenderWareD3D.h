/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/RenderWareD3D.h
 *  PURPOSE:     RenderWare-compatible definitions for
 *               Grand Theft Auto: San Andreas
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

// From SA::Render
struct RwD3D9Raster
{
    union
    {
        IDirect3DTexture9* texture;
        IDirect3DSurface9* surface;
    };
    unsigned char*       palette;
    unsigned char        alpha;
    unsigned char        cubeTextureFlags; /* 0x01 IS_CUBEMAP_TEX */
    unsigned char        textureFlags;     /* 0x10 IS_COMPRESSED */
    unsigned char        lockedLevel;
    IDirect3DSurface9*   lockedSurface;
    D3DLOCKED_RECT       lockedRect;
    D3DFORMAT            format;
    IDirect3DSwapChain9* swapChain;
    HWND*                hwnd;
};

// From gtamodding.com
struct NativeTexturePC_Header
{
    struct
    {
        unsigned int platformId;
        unsigned int filterMode : 8;
        unsigned int uAddressing : 4;
        unsigned int vAddressing : 4;
        unsigned int pad : 16;
        char         name[32];
        char         maskName[32];
    } TextureFormat;

    struct
    {
        unsigned int   rasterFormat;
        D3DFORMAT      d3dFormat;
        unsigned short width;
        unsigned short height;
        unsigned char  depth;
        unsigned char  numLevels;
        unsigned char  rasterType;
        unsigned char  alpha : 1;
        unsigned char  cubeTexture : 1;
        unsigned char  autoMipMaps : 1;
        unsigned char  compressed : 1;
        unsigned char  pad : 4;
    } RasterFormat;
};
