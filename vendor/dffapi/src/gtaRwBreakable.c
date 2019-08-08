//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwGeometryBreakableInit(gtaRwGeometryBreakable *breakableObj, gtaRwUInt32 vertexCount, gtaRwUInt32 faceCount, gtaRwUInt32 meshCount, gtaRwBool posnRelativeToCollision) {
    gtaRwGeometryBreakableDestroy(breakableObj);
    breakableObj->enabled = rwTRUE;
    if (vertexCount == 0 && faceCount == 0 && meshCount == 0)
        breakableObj->thisData = rwNULL;
    else
    {
        breakableObj->thisData = breakableObj;
        breakableObj->posnRelativeToCollision = posnRelativeToCollision;
        breakableObj->vertexCount = vertexCount;
        breakableObj->faceCount = faceCount;
        breakableObj->meshCount = meshCount;
        breakableObj->vertices = gtaRwMemAllocAndZero(vertexCount * 12);
        breakableObj->texCoors = gtaRwMemAllocAndZero(vertexCount * 8);
        breakableObj->colors = gtaRwMemAllocAndInit(vertexCount * 4, -1);
        breakableObj->faces = gtaRwMemAllocAndZero(faceCount * 6);
        breakableObj->faceMaterials = gtaRwMemAllocAndZero(faceCount * 2);
        breakableObj->texNames = gtaRwMemAllocAndZero(meshCount * 32);
        breakableObj->texMaskNames = gtaRwMemAllocAndZero(meshCount * 32);
        breakableObj->matColors = gtaRwMemAllocAndZero(meshCount * 12);
    }
}

gtaRwBool gtaRwGeometryBreakableDestroy(gtaRwGeometryBreakable *breakableObj) {
    if (breakableObj) {
        if (breakableObj->vertices)
            gtaRwMemFree(breakableObj->vertices);
        if (breakableObj->texCoors)
            gtaRwMemFree(breakableObj->texCoors);
        if (breakableObj->colors)
            gtaRwMemFree(breakableObj->colors);
        if (breakableObj->faces)
            gtaRwMemFree(breakableObj->faces);
        if (breakableObj->faceMaterials)
            gtaRwMemFree(breakableObj->faceMaterials);
        if (breakableObj->texNames)
            gtaRwMemFree(breakableObj->texNames);
        if (breakableObj->texMaskNames)
            gtaRwMemFree(breakableObj->texMaskNames);
        if (breakableObj->matColors)
            gtaRwMemFree(breakableObj->matColors);
        gtaRwMemZero(breakableObj, sizeof(gtaRwGeometryBreakable));
    }
    return rwTRUE;
}

gtaRwBool gtaRwGeometryBreakableWrite(gtaRwGeometryBreakable *breakableObj, gtaRwStream *stream) {
    if (breakableObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_BREAKABLE, gtaRwGeometryBreakableSize(breakableObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!breakableObj->thisData) {
            if (!gtaRwStreamWrite(stream, &breakableObj->thisData, 4))
                return rwFALSE;
        }
        else {
            if (!gtaRwStreamWrite(stream, &breakableObj->thisData, 56))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->vertices, breakableObj->vertexCount * 12))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->texCoors, breakableObj->vertexCount * 8))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->colors, breakableObj->vertexCount * 4))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->faces, breakableObj->faceCount * 6))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->faceMaterials, breakableObj->faceCount * 2))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->texNames, breakableObj->meshCount * 32))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->texMaskNames, breakableObj->meshCount * 32))
                return rwFALSE;
            if (!gtaRwStreamWrite(stream, breakableObj->matColors, breakableObj->meshCount * 12))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwGeometryBreakableRead(gtaRwGeometryBreakable *breakableObj, gtaRwStream *stream) {
    gtaRwGeometryBreakableDestroy(breakableObj);
    if (gtaRwStreamRead(stream, &breakableObj->thisData, 4) != 4) {
        gtaRwGeometryBreakableDestroy(breakableObj);
        return rwFALSE;
    }
    if (breakableObj->thisData) {
        breakableObj->enabled = rwTRUE;
        breakableObj->thisData = breakableObj;
        if (gtaRwStreamRead(stream, &breakableObj->posnRelativeToCollision, 52) != 52) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->vertices = gtaRwMemAlloc(breakableObj->vertexCount * 12);
        if (gtaRwStreamRead(stream, breakableObj->vertices, breakableObj->vertexCount * 12) != breakableObj->vertexCount * 12) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->texCoors = gtaRwMemAlloc(breakableObj->vertexCount * 8);
        if (gtaRwStreamRead(stream, breakableObj->texCoors, breakableObj->vertexCount * 8) != breakableObj->vertexCount * 8) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->colors = gtaRwMemAlloc(breakableObj->vertexCount * 4);
        if (gtaRwStreamRead(stream, breakableObj->colors, breakableObj->vertexCount * 4) != breakableObj->vertexCount * 4) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->faces = gtaRwMemAlloc(breakableObj->faceCount * 6);
        if (gtaRwStreamRead(stream, breakableObj->faces, breakableObj->faceCount * 6) != breakableObj->faceCount * 6) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->faceMaterials = gtaRwMemAlloc(breakableObj->faceCount * 2);
        if (gtaRwStreamRead(stream, breakableObj->faceMaterials, breakableObj->faceCount * 2) != breakableObj->faceCount * 2) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->texNames = gtaRwMemAlloc(breakableObj->meshCount * 32);
        if (gtaRwStreamRead(stream, breakableObj->texNames, breakableObj->meshCount * 32) != breakableObj->meshCount * 32) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->texMaskNames = gtaRwMemAlloc(breakableObj->meshCount * 32);
        if (gtaRwStreamRead(stream, breakableObj->texMaskNames, breakableObj->meshCount * 32) != breakableObj->meshCount * 32) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
        breakableObj->matColors = gtaRwMemAlloc(breakableObj->meshCount * 12);
        if (gtaRwStreamRead(stream, breakableObj->matColors, breakableObj->meshCount * 12) != breakableObj->meshCount * 12) {
            gtaRwGeometryBreakableDestroy(breakableObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryBreakableSize(gtaRwGeometryBreakable *breakableObj) {
    if (breakableObj->enabled) {
        if (breakableObj->thisData)
            return 68 + breakableObj->vertexCount * 24 + breakableObj->faceCount * 8 + breakableObj->meshCount * 76;
        return 16;
    }
    return 0;
}
