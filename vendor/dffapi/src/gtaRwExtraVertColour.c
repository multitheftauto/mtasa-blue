//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwGeometryExtraVertColourInit(gtaRwGeometryExtraVertColour *extraColObj, gtaRwUInt32 VertexCount) {
    gtaRwGeometryExtraVertColourDestroy(extraColObj);
    extraColObj->enabled = rwTRUE;
    extraColObj->nightColors = gtaRwMemAllocAndZero(4 * VertexCount);
}

void gtaRwGeometryExtraVertColourDestroy(gtaRwGeometryExtraVertColour *extraColObj) {
    if (extraColObj) {
        if (extraColObj->nightColors)
            gtaRwMemFree(extraColObj->nightColors);
        gtaRwMemZero(extraColObj, sizeof(gtaRwGeometryExtraVertColour));
    }
}

gtaRwBool gtaRwGeometryExtraVertColourWrite(gtaRwGeometryExtraVertColour *extraColObj, gtaRwStream *stream, gtaRwUInt32 vertexCount) {
    if (extraColObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_EXTRAVERTCOLOUR, gtaRwGeometryExtraVertColourSize(extraColObj, vertexCount) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &extraColObj->nightColors, 4))
            return rwFALSE;
        if (extraColObj->nightColors && vertexCount > 0) {
            if (!gtaRwStreamWrite(stream, extraColObj->nightColors, 4 * vertexCount))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwGeometryExtraVertColourRead(gtaRwGeometryExtraVertColour *extraColObj, gtaRwStream *stream, gtaRwUInt32 vertexCount) {
    gtaRwGeometryExtraVertColourDestroy(extraColObj);
    if (gtaRwStreamRead(stream, &extraColObj->nightColors, 4) != 4) {
        gtaRwGeometryExtraVertColourDestroy(extraColObj);
        return rwFALSE;
    }
    if (extraColObj->nightColors && vertexCount > 0) {
        extraColObj->nightColors = gtaRwMemAllocAndZero(4 * vertexCount);
        if (gtaRwStreamRead(stream, extraColObj->nightColors, 4 * vertexCount) != 4 * vertexCount) {
            gtaRwGeometryExtraVertColourDestroy(extraColObj);
            return rwFALSE;
        }
    }
    extraColObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwGeometryExtraVertColourSize(gtaRwGeometryExtraVertColour *extraColObj, gtaRwUInt32 vertexCount) {
    if (extraColObj->enabled) {
        if (extraColObj->nightColors)
            return 16 + vertexCount * 4;
        return 16;
    }
    return 0;
}
