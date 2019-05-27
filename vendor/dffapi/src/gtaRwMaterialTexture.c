//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialTextureRead(gtaRwMaterialTexture *texObj, gtaRwStream *stream) {
    gtaRwUInt32 length, entryLength, type;
    gtaRwMaterialTextureDestroy(texObj);
    if (!gtaRwStreamFindChunk(stream, rwID_TEXTURE, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamRead(stream, texObj, 4)) {
        gtaRwMaterialTextureDestroy(texObj);
        return rwFALSE;
    }
    if (!gtaRwStringRead(&texObj->name, stream)) {
        gtaRwMaterialTextureDestroy(texObj);
        return rwFALSE;
    }
    if (!gtaRwStringRead(&texObj->maskName, stream)) {
        gtaRwMaterialTextureDestroy(texObj);
        return rwFALSE;
    }
    if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, rwNULL, rwNULL)) {
        gtaRwMaterialTextureDestroy(texObj);
        return rwFALSE;
    }
    while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, rwNULL, rwNULL)) {
        switch (type) {
        case rwID_ANISOT:
            if (!gtaRwTextureAnisotRead(&texObj->anisot, stream)) {
                gtaRwMaterialTextureDestroy(texObj);
                return rwFALSE;
            }
            break;
        case rwID_SKYMIPMAP:
            if (!gtaRwTextureSkyMipmapRead(&texObj->skyMipmap, stream)) {
                gtaRwMaterialTextureDestroy(texObj);
                return rwFALSE;
            }
            break;
        default:
            if (!gtaRwUnknownExtensionAllocateAndRead(&texObj->unknownExtensions, &texObj->numUnknownExtensions, type, entryLength, stream)) {
                gtaRwMaterialTextureDestroy(texObj);
                return rwFALSE;
            }
        }
        length += -12 - entryLength;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialTextureWrite(gtaRwMaterialTexture *texObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_TEXTURE, gtaRwMaterialTextureSize(texObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, texObj, 4))
        return rwFALSE;
    if (!gtaRwStringWrite(&texObj->name, stream))
        return rwFALSE;
    if (!gtaRwStringWrite(&texObj->maskName, stream))
        return rwFALSE;
    gtaRwUInt32 extensionsSize =
        gtaRwTextureAnisotSize(&texObj->anisot)
        + gtaRwTextureSkyMipmapSize(&texObj->skyMipmap)
        + gtaRwUnknownExtensionsSize(texObj->unknownExtensions, texObj->numUnknownExtensions);
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, extensionsSize, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwTextureSkyMipmapWrite(&texObj->skyMipmap, stream))
        return rwFALSE;
    if (!gtaRwTextureAnisotWrite(&texObj->anisot, stream))
        return rwFALSE;
    if (!gtaRwUnknownExtensionsWrite(texObj->unknownExtensions, texObj->numUnknownExtensions, stream))
        return rwFALSE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialTextureSize(gtaRwMaterialTexture *texObj) {
    gtaRwUInt32 result = 40 + gtaRwStringSize(&texObj->name) + gtaRwStringSize(&texObj->maskName)
        + gtaRwTextureAnisotSize(&texObj->anisot)
        + gtaRwTextureSkyMipmapSize(&texObj->skyMipmap)
        + gtaRwUnknownExtensionsSize(texObj->unknownExtensions, texObj->numUnknownExtensions);
    return result;
}

void gtaRwMaterialTextureInit(gtaRwMaterialTexture *texObj, gtaRwTextureFilterMode filtering, gtaRwTextureAddressMode uAddressing, gtaRwTextureAddressMode vAddressing, gtaRwUInt8 generateMipMaps, gtaRwChar const *name, gtaRwChar const *maskName) {
    gtaRwMaterialTextureDestroy(texObj);
    texObj->filtering = filtering;
    texObj->uAddressing = uAddressing;
    texObj->vAddressing = vAddressing;
    texObj->generateMipMaps = generateMipMaps;
    gtaRwStringInit(&texObj->name, name, rwFALSE);
    gtaRwStringInit(&texObj->maskName, maskName, rwFALSE);
}

void gtaRwMaterialTextureDestroy(gtaRwMaterialTexture *texObj) {
    if (texObj) {
        gtaRwStringDestroy(&texObj->name);
        gtaRwStringDestroy(&texObj->maskName);
        gtaRwTextureAnisotDestroy(&texObj->anisot);
        gtaRwTextureSkyMipmapDestroy(&texObj->skyMipmap);
        if (texObj->unknownExtensions)
            gtaRwMemFree(texObj->unknownExtensions);
        gtaRwMemZero(texObj, sizeof(gtaRwMaterialTexture));
    }
}
