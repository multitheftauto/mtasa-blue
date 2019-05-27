//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialNormalMapWrite(gtaRwMaterialNormalMap *nmObj, gtaRwStream *stream) {
    if (nmObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_NORMALMAP, gtaRwMaterialNormalMapSize(nmObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &nmObj->flags, 4))
            return rwFALSE;
        if (nmObj->normalMapUsed) {
            if (!gtaRwMaterialTextureWrite(&nmObj->normalMapTexture, stream))
                return rwFALSE;
            if (nmObj->envMapUsed) {
                if (!gtaRwStreamWrite(stream, &nmObj->envMapCoefficient, 4))
                    return rwFALSE;
                if (!gtaRwMaterialTextureWrite(&nmObj->envMapTexture, stream))
                    return rwFALSE;
            }
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialNormalMapRead(gtaRwMaterialNormalMap *nmObj, gtaRwStream *stream) {
    gtaRwMaterialNormalMapDestroy(nmObj);
    if (gtaRwStreamRead(stream, &nmObj->flags, 4) != 4) {
        gtaRwMaterialNormalMapDestroy(nmObj);
        return rwFALSE;
    }
    if (nmObj->normalMapUsed) {
        if (!gtaRwMaterialTextureRead(&nmObj->normalMapTexture, stream)) {
            gtaRwMaterialNormalMapDestroy(nmObj);
            return rwFALSE;
        }
        if (nmObj->envMapUsed) {
            if (gtaRwStreamRead(stream, &nmObj->envMapCoefficient, 4) != 4) {
                gtaRwMaterialNormalMapDestroy(nmObj);
                return rwFALSE;
            }
            if (!gtaRwMaterialTextureRead(&nmObj->envMapTexture, stream)) {
                gtaRwMaterialNormalMapDestroy(nmObj);
                return rwFALSE;
            }
        }
    }
    nmObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialNormalMapSize(gtaRwMaterialNormalMap *nmObj) {
    if (nmObj->enabled) {
        gtaRwUInt32 size = 16;
        if (nmObj->normalMapUsed) {
            size += gtaRwMaterialTextureSize(&nmObj->normalMapTexture);
            if (nmObj->envMapUsed)
                size += gtaRwMaterialTextureSize(&nmObj->envMapTexture);
        }
        return size;
    }
    return 0;
}

void gtaRwMaterialNormalMapInit(gtaRwMaterialNormalMap *nmObj, gtaRwBool normalMapUsed, gtaRwBool envMapUsed, gtaRwReal envMapCoefficient, gtaRwBool envMapModulate) {
    gtaRwMaterialNormalMapDestroy(nmObj);
    nmObj->enabled = rwTRUE;
    nmObj->normalMapUsed = normalMapUsed;
    nmObj->envMapUsed = envMapUsed;
    nmObj->envMapCoefficient = envMapCoefficient;
    nmObj->envMapModulate = envMapModulate;
}

void gtaRwMaterialNormalMapDestroy(gtaRwMaterialNormalMap *nmObj) {
    if (nmObj) {
        gtaRwMaterialTextureDestroy(&nmObj->envMapTexture);
        gtaRwMaterialTextureDestroy(&nmObj->normalMapTexture);
        gtaRwMemZero(nmObj, sizeof(gtaRwMaterialNormalMap));
    }
}
