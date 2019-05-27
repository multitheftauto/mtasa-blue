//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwRasterLevel gtaRwRasterLevel;
typedef struct gtaRwTextureD3D gtaRwTextureD3D;

struct gtaRwRasterLevel {
    gtaRwUInt32 size;
    gtaRwUInt8 *pixels;
};

struct gtaRwTextureD3D {
    gtaRwUInt32 platformId;
    gtaRwUInt32 filterMode : 8;
    gtaRwUInt32 uAddressing : 4;
    gtaRwUInt32 vAddressing : 4;
    gtaRwChar name[32];
    gtaRwChar maskName[32];
    gtaRwUInt32 rasterFormat;
    union {
        gtaRwUInt32 d3dFormat; // SA
        gtaRwBool hasAlpha; // GTA 3 & VC
    };
    gtaRwInt16 width;
    gtaRwInt16 height;
    gtaRwUInt8 depth;
    gtaRwUInt8 numLevels;
    gtaRwUInt8 rasterType;
    union {
        gtaRwUInt8 compression; // GTA 3 & VC
        struct { // SA
            gtaRwUInt8 alpha : 1;
            gtaRwUInt8 cubeTexture : 1;
            gtaRwUInt8 autoMipMaps : 1;
            gtaRwUInt8 compressed : 1;
        };
    };
    gtaRwUInt8 *palette;
    gtaRwRasterLevel *levels;

    // extensions

    gtaRwTextureAnisot anisot;

    gtaRwUnknownExtension *unknownExtensions;
    gtaRwUInt32 numUnknownExtensions;
};

void gtaRwRasterLevelInit(gtaRwRasterLevel *levelObj, gtaRwUInt32 size);
void gtaRwRasterLevelDestroy(gtaRwRasterLevel *levelObj);

void gtaRwTextureD3DInitD3D8(gtaRwTextureD3D *texObj, gtaRwUInt32 platformId, gtaRwUInt32 filterMode, gtaRwUInt32 uAddressing, gtaRwUInt32 vAddressing,
    gtaRwChar *name, gtaRwChar *maskName, gtaRwUInt32 rasterFormat, gtaRwBool hasAlpha, gtaRwInt16 width, gtaRwInt16 height, gtaRwUInt8 depth,
    gtaRwUInt8 numLevels, gtaRwUInt8 rasterType, gtaRwUInt8 compression);
void gtaRwTextureD3DInitD3D9(gtaRwTextureD3D *texObj, gtaRwUInt32 platformId, gtaRwUInt32 filterMode, gtaRwUInt32 uAddressing, gtaRwUInt32 vAddressing,
    gtaRwChar *name, gtaRwChar *maskName, gtaRwUInt32 rasterFormat, gtaRwUInt32 d3dFormat, gtaRwInt16 width, gtaRwInt16 height, gtaRwUInt8 depth,
    gtaRwUInt8 numLevels, gtaRwUInt8 rasterType, gtaRwBool HasAlpha, gtaRwBool IsCubeTexture, gtaRwBool UseAutoMipMaps, gtaRwBool isCompressed);
void gtaRwTextureD3DDestroy(gtaRwTextureD3D *texObj);
gtaRwBool gtaRwTextureD3DRead(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureD3DReadD3D9(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureD3DReadD3D8(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureD3DWriteD3D8(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureD3DWriteD3D9(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwBool gtaRwTextureD3DWrite(gtaRwTextureD3D *texObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwTextureD3DSize(gtaRwTextureD3D *texObj);
