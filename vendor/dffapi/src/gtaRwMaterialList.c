//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwMaterialListRead(gtaRwMaterialList *matListObj, gtaRwStream *stream) {
    gtaRwMaterialListDestroy(matListObj);
    if (!gtaRwStreamFindChunk(stream, rwID_MATLIST, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &matListObj->materialReferencesCount, 4) != 4) {
        gtaRwMaterialListDestroy(matListObj);
        return rwFALSE;
    }
    if (matListObj->materialReferencesCount > 0) {
        matListObj->materialReferences = (gtaRwInt32 *)gtaRwMemAllocAndInit(matListObj->materialReferencesCount * 4, -1);
        if (gtaRwStreamRead(stream, matListObj->materialReferences, matListObj->materialReferencesCount * 4) != matListObj->materialReferencesCount * 4) {
            gtaRwMaterialListDestroy(matListObj);
            return rwFALSE;
        }
        matListObj->materials = (gtaRwMaterial *)gtaRwMemAllocAndZero(matListObj->materialReferencesCount * sizeof(gtaRwMaterial));
        gtaRwUInt32 i;
        for (i = 0; i < gtaRwMaterialListGetMaterialCount(matListObj); i++) {
            if (!gtaRwMaterialRead(&matListObj->materials[i], stream)) {
                gtaRwMaterialListDestroy(matListObj);
                return rwFALSE;
            }
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialListWrite(gtaRwMaterialList *matListObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATLIST, gtaRwMaterialListSize(matListObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4 + matListObj->materialReferencesCount * 4, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &matListObj->materialReferencesCount, 4))
        return rwFALSE;
    if (matListObj->materialReferencesCount > 0) {
        if (!gtaRwStreamWrite(stream, matListObj->materialReferences, matListObj->materialReferencesCount * 4))
            return rwFALSE;
        gtaRwUInt32 i;
        for (i = 0; i < gtaRwMaterialListGetMaterialCount(matListObj); i++) {
            if (!gtaRwMaterialWrite(&matListObj->materials[i], stream))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialListSize(gtaRwMaterialList *matListObj) {
    unsigned int size = 28 + 4 * matListObj->materialReferencesCount;
    gtaRwUInt32 i;
    for (i = 0; i < gtaRwMaterialListGetMaterialCount(matListObj); i++)
        size += gtaRwMaterialSize(&matListObj->materials[i]);
    return size;
}

void gtaRwMaterialListInit(gtaRwMaterialList *matListObj, gtaRwUInt32 materialReferencesCount, gtaRwUInt32 materialCount) {
    gtaRwMaterialListDestroy(matListObj);
    matListObj->materialReferencesCount = materialReferencesCount;
    if (materialReferencesCount > 0 && materialCount > 0 && materialReferencesCount >= materialCount) {
        matListObj->materialReferences = (gtaRwInt32 *)gtaRwMemAllocAndInit(4 * materialReferencesCount, -1);
        matListObj->materials = (gtaRwMaterial *)gtaRwMemAllocAndZero(sizeof(gtaRwMaterial) * materialCount);
    }
}

void gtaRwMaterialListDestroy(gtaRwMaterialList *matListObj) {
    if (matListObj) {
        if (matListObj->materialReferences) {
            if (matListObj->materials) {
                gtaRwUInt32 i;
                for (i = 0; i < gtaRwMaterialListGetMaterialCount(matListObj); i++)
                    gtaRwMaterialDestroy(&matListObj->materials[i]);
                gtaRwMemFree(matListObj->materials);
            }
            gtaRwMemFree(matListObj->materialReferences);
        }
        gtaRwMemZero(matListObj, sizeof(gtaRwMaterialList));
    }
}

gtaRwUInt32 gtaRwMaterialListGetMaterialCount(gtaRwMaterialList *matListObj) {
    gtaRwUInt32 materialCount = 0;
    gtaRwUInt32 i;
    for (i = 0; i < matListObj->materialReferencesCount; i++) {
        if (matListObj->materialReferences[i] < 0)
            materialCount += 1;
    }
    return materialCount;
}
