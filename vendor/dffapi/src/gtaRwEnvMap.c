//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialEnvMapWrite(gtaRwMaterialEnvMap *envMapObj, gtaRwStream *stream) {
    if (envMapObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_ENVMAP, gtaRwMaterialEnvMapSize(envMapObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &envMapObj->scaleX, 24))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialEnvMapRead(gtaRwMaterialEnvMap *envMapObj, gtaRwStream *stream) {
    gtaRwMaterialEnvMapDestroy(envMapObj);
    if (gtaRwStreamRead(stream, &envMapObj->scaleX, 24) != 24) {
        gtaRwMaterialEnvMapDestroy(envMapObj);
        return rwFALSE;
    }
    envMapObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialEnvMapSize(gtaRwMaterialEnvMap *envMapObj) {
    if (envMapObj->enabled)
        return 36;
    return 0;
}

void gtaRwMaterialEnvMapInit(gtaRwMaterialEnvMap *envMapObj, gtaRwReal scaleX, gtaRwReal scaleY, gtaRwReal transSclX, gtaRwReal transSclY, gtaRwReal shininess) {
    gtaRwMaterialEnvMapDestroy(envMapObj);
    envMapObj->enabled = rwTRUE;
    envMapObj->scaleX = scaleX;
    envMapObj->scaleY = scaleY;
    envMapObj->transSclX = transSclX;
    envMapObj->transSclY = transSclY;
    envMapObj->shininess = shininess;
}

void gtaRwMaterialEnvMapDestroy(gtaRwMaterialEnvMap *envMapObj) {
    if (envMapObj)
        gtaRwMemZero(envMapObj, sizeof(gtaRwMaterialEnvMap));
}
