//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwMaterialUVAnimInit(gtaRwMaterialUVAnim *uvAnimObj, gtaRwUInt32 numSlots) {
    gtaRwMaterialUVAnimDestroy(uvAnimObj);
    uvAnimObj->enabled = rwTRUE;
    if (numSlots > 8)
        numSlots = 8;
    uvAnimObj->numAnimNames = numSlots;
    if (numSlots > 0)
        uvAnimObj->animNames = gtaRwMemAllocAndZero(numSlots * 32);
}

void gtaRwMaterialUVAnimSetupAnim(gtaRwMaterialUVAnim *uvAnimObj, gtaRwUInt32 slot, gtaRwChar *animName) {
    uvAnimObj->animSlotsMap |= (1 << slot);
    strcpy(&uvAnimObj->animNames[32 * uvAnimObj->currentSlot], animName);
    uvAnimObj->currentSlot += 1;
}

void gtaRwMaterialUVAnimDestroy(gtaRwMaterialUVAnim *uvAnimObj) {
    if (uvAnimObj) {
        if (uvAnimObj->animNames)
            gtaRwMemFree(uvAnimObj->animNames);
        gtaRwMemZero(uvAnimObj, sizeof(gtaRwMaterialUVAnim));
    }
}

gtaRwBool gtaRwMaterialUVAnimWrite(gtaRwMaterialUVAnim *uvAnimObj, gtaRwStream *stream) {
    if (uvAnimObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_UVANIM, gtaRwMaterialUVAnimSize(uvAnimObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, gtaRwMaterialUVAnimSize(uvAnimObj) - 24, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &uvAnimObj->animSlotsMap, 4))
            return rwFALSE;
        if (uvAnimObj->animNames) {
            if (!gtaRwStreamWrite(stream, uvAnimObj->animNames, uvAnimObj->numAnimNames * 32))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialUVAnimRead(gtaRwMaterialUVAnim *uvAnimObj, gtaRwStream *stream) {
    gtaRwMaterialUVAnimDestroy(uvAnimObj);
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &uvAnimObj->animSlotsMap, 4) != 4) {
        gtaRwMaterialUVAnimDestroy(uvAnimObj);
        return rwFALSE;
    }
    gtaRwInt32 i;
    for (i = 0; i < 8; i++) {
        if (uvAnimObj->animSlotsMap & (1 << i))
            uvAnimObj->numAnimNames += 1;
    }
    if (uvAnimObj->numAnimNames > 0) {
        uvAnimObj->animNames = (gtaRwChar *)gtaRwMemAllocAndZero(uvAnimObj->numAnimNames * 32);
        if (gtaRwStreamRead(stream, uvAnimObj->animNames, uvAnimObj->numAnimNames * 32) != uvAnimObj->numAnimNames * 32) {
            gtaRwMaterialUVAnimDestroy(uvAnimObj);
            return rwFALSE;
        }
    }
    uvAnimObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialUVAnimSize(gtaRwMaterialUVAnim *uvAnimObj) {
    if (uvAnimObj->enabled)
        return 28 + uvAnimObj->numAnimNames * 32;
    return 0;
}
