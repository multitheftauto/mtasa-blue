//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwRasterLevelInit(gtaRwRasterLevel *levelObj, gtaRwUInt32 size) {
    gtaRwRasterLevelDestroy(levelObj);
    levelObj->pixels = gtaRwMemAllocAndInit(size, -1);
    levelObj->size = size;
}

void gtaRwRasterLevelDestroy(gtaRwRasterLevel *levelObj) {
    if (levelObj) {
        if (levelObj->pixels)
            gtaRwMemFree(levelObj->pixels);
        gtaRwMemZero(levelObj, sizeof(gtaRwRasterLevel));
    }
}

void gtaRwTextureD3DInitD3D8(gtaRwTextureD3D *texObj, gtaRwUInt32 platformId, gtaRwUInt32 filterMode, gtaRwUInt32 uAddressing,
    gtaRwUInt32 vAddressing, gtaRwChar *name, gtaRwChar *maskName, gtaRwUInt32 rasterFormat, gtaRwBool hasAlpha, gtaRwInt16 width,
    gtaRwInt16 height, gtaRwUInt8 depth, gtaRwUInt8 numLevels, gtaRwUInt8 rasterType, gtaRwUInt8 compression)
{
    gtaRwTextureD3DDestroy(texObj);
    texObj->platformId = platformId;
    texObj->filterMode = filterMode;
    texObj->uAddressing = uAddressing;
    texObj->vAddressing = vAddressing;
    if (name)
        strcpy_s(texObj->name, 32, name);
    if (maskName)
        strcpy_s(texObj->maskName, 32, maskName);
    texObj->rasterFormat = rasterFormat;
    texObj->hasAlpha = hasAlpha;
    texObj->width = width;
    texObj->height = height;
    texObj->depth = depth;
    texObj->numLevels = numLevels;
    if (numLevels)
        texObj->levels = gtaRwMemAlloc(numLevels * sizeof(gtaRwRasterLevel));
    texObj->rasterType = rasterType;
    texObj->compression = compression;
}

void gtaRwTextureD3DInitD3D9(gtaRwTextureD3D *texObj, gtaRwUInt32 platformId, gtaRwUInt32 filterMode, gtaRwUInt32 uAddressing,
    gtaRwUInt32 vAddressing, gtaRwChar *name, gtaRwChar *maskName, gtaRwUInt32 rasterFormat, gtaRwUInt32 d3dFormat, gtaRwInt16 width,
    gtaRwInt16 height, gtaRwUInt8 depth, gtaRwUInt8 numLevels, gtaRwUInt8 rasterType, gtaRwBool hasAlpha, gtaRwBool isCubeTexture,
    gtaRwBool useAutoMipMaps, gtaRwBool isCompressed)
{
    gtaRwTextureD3DDestroy(texObj);
    texObj->platformId = platformId;
    texObj->filterMode = filterMode;
    texObj->uAddressing = uAddressing;
    texObj->vAddressing = vAddressing;
    if (name)
        strcpy_s(texObj->name, 32, name);
    if (maskName)
        strcpy_s(texObj->maskName, 32, maskName);
    texObj->rasterFormat = rasterFormat;
    texObj->d3dFormat = d3dFormat;
    texObj->width = width;
    texObj->height = height;
    texObj->depth = depth;
    texObj->numLevels = numLevels;
    gtaRwUInt32 totalLevels = numLevels * (isCubeTexture ? 6 : 1);
    if (numLevels)
        texObj->levels = gtaRwMemAlloc(totalLevels * sizeof(gtaRwRasterLevel));
    texObj->rasterType = rasterType;
    texObj->alpha = (gtaRwUInt8)hasAlpha;
    texObj->cubeTexture = (gtaRwUInt8)isCubeTexture;
    texObj->autoMipMaps = (gtaRwUInt8)useAutoMipMaps;
    texObj->compressed = (gtaRwUInt8)isCompressed;
}

void gtaRwTextureD3DDestroy(gtaRwTextureD3D *texObj) {
    if (texObj) {
        if (texObj->palette)
            gtaRwMemFree(texObj->palette);
        if (texObj->levels) {
            gtaRwUInt32 i;
            for (i = 0; i < texObj->numLevels; i++)
                gtaRwRasterLevelDestroy(&texObj->levels[i]);
            gtaRwMemFree(texObj->levels);
        }
        gtaRwTextureAnisotDestroy(&texObj->anisot);
        gtaRwMemZero(texObj, sizeof(gtaRwTextureD3D));
    }
}

gtaRwBool gtaRwTextureD3DRead(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    gtaRwTextureD3DDestroy(texObj);
    if (!gtaRwStreamFindChunk(stream, rwID_TEXTURENATIVE, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &texObj->platformId, 88) != 88) {
        gtaRwTextureD3DDestroy(texObj);
        return rwFALSE;
    }
    switch (texObj->platformId) {
    case rwID_PCD3D9:
        if (!gtaRwTextureD3DReadD3D9(texObj, stream)) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
        break;
    case rwID_PCD3D8:
        if (!gtaRwTextureD3DReadD3D8(texObj, stream)) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
        break;
    default:
        gtaRwTextureD3DDestroy(texObj);
        gtaRwError("Reding Native Texture: This platform Id (%d) is not supported", texObj->platformId);
        return rwFALSE;
    }
    gtaRwUInt32 length;
    if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, rwNULL, rwNULL)) {
        gtaRwTextureD3DDestroy(texObj);
        return rwFALSE;
    }
    gtaRwUInt32 type, entryLength;
    while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, rwNULL, rwNULL)) {
        switch (type) {
        case rwID_ANISOT:
            if (!gtaRwTextureAnisotRead(&texObj->anisot, stream)) {
                gtaRwTextureD3DDestroy(texObj);
                return rwFALSE;
            }
            break;
        default:
            if (!gtaRwUnknownExtensionAllocateAndRead(&texObj->unknownExtensions, &texObj->numUnknownExtensions, type, entryLength, stream)) {
                gtaRwTextureD3DDestroy(texObj);
                return rwFALSE;
            }
        }
        length += -12 - entryLength;
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureD3DReadD3D9(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    if (texObj->rasterFormat & rwRASTERFORMATPAL4) {
        texObj->palette = gtaRwMemAlloc(128);
        if (gtaRwStreamRead(stream, texObj->palette, 128) != 128) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    else if (texObj->rasterFormat & rwRASTERFORMATPAL8) {
        texObj->palette = gtaRwMemAlloc(1024);
        if (gtaRwStreamRead(stream, texObj->palette, 1024) != 1024) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    gtaRwUInt32 totalLevels = texObj->numLevels * (texObj->cubeTexture ? 6 : 1);
    texObj->levels = gtaRwMemAlloc(totalLevels * sizeof(gtaRwRasterLevel));
    gtaRwUInt32 i;
    for (i = 0; i < totalLevels; i++) {
        if (gtaRwStreamRead(stream, &texObj->levels[i].size, 4) != 4) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
        texObj->levels[i].pixels = gtaRwMemAlloc(texObj->levels[i].size);
        if (gtaRwStreamRead(stream, texObj->levels[i].pixels, texObj->levels[i].size) != texObj->levels[i].size) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureD3DReadD3D8(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    if (texObj->rasterFormat & rwRASTERFORMATPAL4) {
        texObj->palette = gtaRwMemAlloc(128);
        if (gtaRwStreamRead(stream, texObj->palette, 128) != 128) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    else if (texObj->rasterFormat & rwRASTERFORMATPAL8) {
        texObj->palette = gtaRwMemAlloc(1024);
        if (gtaRwStreamRead(stream, texObj->palette, 1024) != 1024) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    texObj->levels = gtaRwMemAlloc(texObj->numLevels * sizeof(gtaRwRasterLevel));
    gtaRwUInt32 i;
    for (i = 0; i < texObj->numLevels; i++) {
        if (gtaRwStreamRead(stream, &texObj->levels[i].size, 4) != 4) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
        texObj->levels[i].pixels = gtaRwMemAlloc(texObj->levels[i].size);
        if (gtaRwStreamRead(stream, texObj->levels[i].pixels, texObj->levels[i].size) != texObj->levels[i].size) {
            gtaRwTextureD3DDestroy(texObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureD3DWriteD3D8(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    gtaRwUInt32 i;
    for (i = 0; i < texObj->numLevels; i++) {
        if (!gtaRwStreamWrite(stream, &texObj->levels[i].size, 4))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, texObj->levels[i].pixels, texObj->levels[i].size))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureD3DWriteD3D9(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    gtaRwUInt32 totalLevels = texObj->numLevels * (texObj->cubeTexture ? 6 : 1);
    gtaRwUInt32 i;
    for (i = 0; i < totalLevels; i++) {
        if (!gtaRwStreamWrite(stream, &texObj->levels[i].size, 4))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, texObj->levels[i].pixels, texObj->levels[i].size))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureD3DWrite(gtaRwTextureD3D *texObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_TEXTURENATIVE, gtaRwTextureD3DSize(texObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, gtaRwTextureD3DSize(texObj) - 36, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &texObj->platformId, 88))
        return rwFALSE;
    if (texObj->rasterFormat & rwRASTERFORMATPAL4) {
        if (!gtaRwStreamWrite(stream, texObj->palette, 128))
            return rwFALSE;
    }
    else if (texObj->rasterFormat & rwRASTERFORMATPAL8) {
        if (!gtaRwStreamWrite(stream, texObj->palette, 1024))
            return rwFALSE;
    }
    switch (texObj->platformId) {
    case rwID_PCD3D9:
        if (!gtaRwTextureD3DWriteD3D9(texObj, stream))
            return rwFALSE;
        break;
    case rwID_PCD3D8:
        if (!gtaRwTextureD3DWriteD3D8(texObj, stream))
            return rwFALSE;
        break;
    default:
        gtaRwError("Writing Native Texture: This platform Id (%d) is not supported", texObj->platformId);
        return rwFALSE;
    }
    gtaRwUInt32 extensionsSize = gtaRwTextureAnisotSize(&texObj->anisot) + gtaRwUnknownExtensionsSize(texObj->unknownExtensions, texObj->numUnknownExtensions);
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, extensionsSize, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwTextureAnisotWrite(&texObj->anisot, stream))
        return rwFALSE;
    if (!gtaRwUnknownExtensionsWrite(texObj->unknownExtensions, texObj->numUnknownExtensions, stream))
        return rwFALSE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwTextureD3DSize(gtaRwTextureD3D *texObj) {
    gtaRwUInt32 size = 124; // 88 + (struct) 12 + (header) 12 + (extension) 12
    gtaRwUInt32 totalLevels;
    gtaRwUInt32 i;
    if (texObj->rasterFormat & rwRASTERFORMATPAL4)
        size += 128;
    else if (texObj->rasterFormat & rwRASTERFORMATPAL8)
        size += 1024;
    size += gtaRwTextureAnisotSize(&texObj->anisot) + gtaRwUnknownExtensionsSize(texObj->unknownExtensions, texObj->numUnknownExtensions);
    switch (texObj->platformId) {
    case rwID_PCD3D9:
        totalLevels = texObj->numLevels * (texObj->cubeTexture ? 6 : 1);
        for (i = 0; i < totalLevels; i++)
            size += 4 + texObj->levels[i].size;
        break;
    case rwID_PCD3D8:
        for (i = 0; i < texObj->numLevels; i++)
            size += 4 + texObj->levels[i].size;
        break;
    default:
        size = 0;
        gtaRwError("Native Texture Size: This platform Id (%d) is not supported", texObj->platformId);
    }
    return size;
}
