//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterialTexture gtaRwMaterialTexture;

struct gtaRwMaterialTexture {
    gtaRwUInt8 filtering;
    gtaRwUInt8 uAddressing : 4;
    gtaRwUInt8 vAddressing : 4;
    gtaRwUInt8 generateMipMaps : 1;
    gtaRwUInt8 padding;
    gtaRwString name;
    gtaRwString maskName;

    // extensions

    gtaRwTextureSkyMipmap skyMipmap;
    gtaRwTextureAnisot anisot;

    gtaRwUnknownExtension *unknownExtensions;
    gtaRwUInt32 numUnknownExtensions;
};

void gtaRwMaterialTextureInit(gtaRwMaterialTexture *texObj, gtaRwTextureFilterMode filtering, gtaRwTextureAddressMode uAddressing, gtaRwTextureAddressMode vAddressing, gtaRwUInt8 generateMipMaps, gtaRwChar const *name, gtaRwChar const *maskName);
gtaRwBool gtaRwMaterialTextureRead(gtaRwMaterialTexture *texObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialTextureWrite(gtaRwMaterialTexture *texObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialTextureSize(gtaRwMaterialTexture *texObj);
void gtaRwMaterialTextureDestroy(gtaRwMaterialTexture *texObj);
