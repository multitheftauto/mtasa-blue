//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialRead(gtaRwMaterial *matObj, gtaRwStream *stream) {
    gtaRwMaterialDestroy(matObj);
    if (!gtaRwStreamFindChunk(stream, rwID_MATERIAL, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &matObj->flags, 28) != 28) {
        gtaRwMaterialDestroy(matObj);
        return rwFALSE;
    }
    if (matObj->textured) {
        if (!gtaRwMaterialTextureRead(&matObj->texture, stream)) {
            gtaRwMaterialDestroy(matObj);
            return rwFALSE;
        }
    }
    gtaRwUInt32 length;
    if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, rwNULL, rwNULL)) {
        gtaRwMaterialDestroy(matObj);
        return rwFALSE;
    }
    gtaRwUInt32 entryLength, type;
    while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, rwNULL, rwNULL)) {
        switch (type) {
        case rwID_UVANIM:
            if (!gtaRwMaterialUVAnimRead(&matObj->uvAnim, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
            break;
        case rwID_MATFX:
            if (!gtaRwMaterialMatFXRead(&matObj->matFx, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
            break;
        case rwID_NORMALMAP:
            if (!gtaRwMaterialNormalMapRead(&matObj->normalMap, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
            break;
        case gtaID_ENVMAP:
            if (!gtaRwMaterialEnvMapRead(&matObj->envMap, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
            break;
        case gtaID_SPECMAP:
            if (!gtaRwMaterialSpecMapRead(&matObj->specMap, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
            break;
        default:
            if (!gtaRwUnknownExtensionAllocateAndRead(&matObj->unknownExtensions, &matObj->numUnknownExtensions, type, entryLength, stream)) {
                gtaRwMaterialDestroy(matObj);
                return rwFALSE;
            }
        }
        length += -12 - entryLength;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialWrite(gtaRwMaterial *matObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATERIAL, gtaRwMaterialSize(matObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 28, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &matObj->flags, 28))
        return rwFALSE;
    if (matObj->textured) {
        if (!gtaRwMaterialTextureWrite(&matObj->texture, stream))
            return rwFALSE;
    }
    gtaRwUInt32 extensionsSize =
        gtaRwMaterialEnvMapSize(&matObj->envMap)
        + gtaRwMaterialMatFXSize(&matObj->matFx)
        + gtaRwMaterialNormalMapSize(&matObj->normalMap)
        + gtaRwMaterialSpecMapSize(&matObj->specMap)
        + gtaRwMaterialUVAnimSize(&matObj->uvAnim)
        + gtaRwUnknownExtensionsSize(matObj->unknownExtensions, matObj->numUnknownExtensions);
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, extensionsSize, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwMaterialMatFXWrite(&matObj->matFx, stream))
        return rwFALSE;
    if (!gtaRwMaterialUVAnimWrite(&matObj->uvAnim, stream))
        return rwFALSE;
    if (!gtaRwMaterialEnvMapWrite(&matObj->envMap, stream))
        return rwFALSE;
    if (!gtaRwMaterialSpecMapWrite(&matObj->specMap, stream))
        return rwFALSE;
    if (!gtaRwMaterialNormalMapWrite(&matObj->normalMap, stream))
        return rwFALSE;
    if (!gtaRwUnknownExtensionsWrite(matObj->unknownExtensions, matObj->numUnknownExtensions, stream))
        return rwFALSE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialSize(gtaRwMaterial *matObj) {
    gtaRwUInt32 size = 64
        + gtaRwMaterialEnvMapSize(&matObj->envMap)
        + gtaRwMaterialMatFXSize(&matObj->matFx)
        + gtaRwMaterialNormalMapSize(&matObj->normalMap)
        + gtaRwMaterialSpecMapSize(&matObj->specMap)
        + gtaRwMaterialUVAnimSize(&matObj->uvAnim)
        + gtaRwUnknownExtensionsSize(matObj->unknownExtensions, matObj->numUnknownExtensions);
    if (matObj->textured)
        size += gtaRwMaterialTextureSize(&matObj->texture);
    return size;
}

void gtaRwMaterialInit(gtaRwMaterial *matObj, gtaRwUInt8 red, gtaRwUInt8 green, gtaRwUInt8 blue, gtaRwUInt8 alpha, gtaRwBool textured, gtaRwReal ambient, gtaRwReal diffuse) {
    gtaRwMaterialDestroy(matObj);
    matObj->color.r = red;
    matObj->color.g = green;
    matObj->color.b = blue;
    matObj->color.a = alpha;
    matObj->textured = textured;
    matObj->surfaceProps.ambient = ambient;
    matObj->surfaceProps.diffuse = diffuse;
    matObj->surfaceProps.specular = 1.0f;
}

void gtaRwMaterialDestroy(gtaRwMaterial *matObj) {
    if (matObj) {
        gtaRwMaterialTextureDestroy(&matObj->texture);
        gtaRwMaterialEnvMapDestroy(&matObj->envMap);
        gtaRwMaterialMatFXDestroy(&matObj->matFx);
        gtaRwMaterialNormalMapDestroy(&matObj->normalMap);
        gtaRwMaterialSpecMapDestroy(&matObj->specMap);
        gtaRwMaterialUVAnimDestroy(&matObj->uvAnim);
        if (matObj->unknownExtensions)
            gtaRwMemFree(matObj->unknownExtensions);
        gtaRwMemZero(matObj, sizeof(gtaRwMaterial));
    }
}
