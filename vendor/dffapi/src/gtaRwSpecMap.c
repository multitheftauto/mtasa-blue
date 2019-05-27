//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialSpecMapWrite(gtaRwMaterialSpecMap *specMapObj, gtaRwStream *stream) {
    if (specMapObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_SPECMAP, gtaRwMaterialSpecMapSize(specMapObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &specMapObj->specularity, 28))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialSpecMapRead(gtaRwMaterialSpecMap *specMapObj, gtaRwStream *stream) {
    gtaRwMaterialSpecMapDestroy(specMapObj);
    if (gtaRwStreamRead(stream, &specMapObj->specularity, 28) != 28) {
        gtaRwMaterialSpecMapDestroy(specMapObj);
        return rwFALSE;
    }
    specMapObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialSpecMapSize(gtaRwMaterialSpecMap *specMapObj) {
    if (specMapObj->enabled)
        return 40;
    return 0;
}

void gtaRwMaterialSpecMapInit(gtaRwMaterialSpecMap *specMapObj, gtaRwReal specularity, gtaRwChar *textureName) {
    gtaRwMaterialSpecMapDestroy(specMapObj);
    specMapObj->enabled = rwTRUE;
    specMapObj->specularity = specularity;
    gtaRwMemZero(specMapObj->textureName, 24);
    if (textureName)
        strncpy(specMapObj->textureName, textureName, 24);
}

void gtaRwMaterialSpecMapDestroy(gtaRwMaterialSpecMap *specMapObj) {
    if (specMapObj)
        gtaRwMemZero(specMapObj, sizeof(gtaRwMaterialSpecMap));
}
