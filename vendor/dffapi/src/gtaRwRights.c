//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwRightsInit(gtaRwRights *rightsObj, gtaRwUInt32 pluginId, gtaRwUInt32 pluginData) {
    gtaRwRightsDestroy(rightsObj);
    rightsObj->enabled = rwTRUE;
    rightsObj->pluginId = pluginId;
    rightsObj->pluginData = pluginData;
}

void gtaRwRightsDestroy(gtaRwRights *rightsObj) {
    if (rightsObj)
        gtaRwMemZero(rightsObj, sizeof(gtaRwRights));
}

gtaRwBool gtaRwRightsWrite(gtaRwRights *rightsObj, gtaRwStream *stream) {
    if (rightsObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_RIGHTTORENDER, 8, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &rightsObj->pluginId, 8))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwRightsRead(gtaRwRights *rightsObj, gtaRwStream *stream) {
    gtaRwRightsDestroy(rightsObj);
    if (gtaRwStreamRead(stream, &rightsObj->pluginId, 4) != 4) {
        gtaRwRightsDestroy(rightsObj);
        return rwFALSE;
    }
    if (gtaRwStreamRead(stream, &rightsObj->pluginData, 4) != 4) {
        gtaRwRightsDestroy(rightsObj);
        return rwFALSE;
    }
    rightsObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwRightsSize(gtaRwRights *rightsObj) {
    if (rightsObj->enabled)
        return 20;
    return 0;
}
