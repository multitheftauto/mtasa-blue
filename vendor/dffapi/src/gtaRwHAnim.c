//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwHAnimNodeInit(gtaRwHAnimNode *nodeObj, gtaRwInt32 nodeID, gtaRwInt32 nodeIndex, gtaRwInt32 flags) {
    nodeObj->nodeID = nodeID;
    nodeObj->nodeIndex = nodeIndex;
    nodeObj->flags = flags;
}

void gtaRwHAnimNodeDestroy(gtaRwHAnimNode *nodeObj) {
    if (nodeObj)
        gtaRwMemZero(nodeObj, sizeof(gtaRwHAnimNode));
}

void gtaRwFrameHAnimInitEmpty(gtaRwFrameHAnim *hAnimObj, gtaRwInt32 id) {
    gtaRwFrameHAnimDestroy(hAnimObj);
    hAnimObj->enabled = rwTRUE;
    hAnimObj->hAnimVersion = rwHANIMSTREAMCURRENTVERSION;
    hAnimObj->id = id;
}

void gtaRwFrameHAnimInit(gtaRwFrameHAnim *hAnimObj, gtaRwInt32 id, gtaRwUInt32 numNodes, gtaRwUInt32 flags, gtaRwUInt32 keyFrameSize) {
    gtaRwFrameHAnimDestroy(hAnimObj);
    hAnimObj->enabled = rwTRUE;
    hAnimObj->id = id;
    hAnimObj->numNodes = numNodes;
    hAnimObj->flags = flags;
    hAnimObj->keyFrameSize = keyFrameSize;
    hAnimObj->hAnimVersion = rwHANIMSTREAMCURRENTVERSION;
    if (numNodes > 0)
        hAnimObj->nodes = gtaRwMemAllocAndZero(sizeof(gtaRwHAnimNode) * numNodes);
}

void gtaRwFrameHAnimDestroy(gtaRwFrameHAnim *hAnimObj) {
    if (hAnimObj) {
        if (hAnimObj->nodes) {
            gtaRwUInt32 i;
            for (i = 0; i < hAnimObj->numNodes; i++)
                gtaRwHAnimNodeDestroy(&hAnimObj->nodes[i]);
            gtaRwMemFree(hAnimObj->nodes);
        }
        gtaRwMemZero(hAnimObj, sizeof(gtaRwFrameHAnim));
    }
}

gtaRwBool gtaRwFrameHAnimWrite(gtaRwFrameHAnim *hAnimObj, gtaRwStream *stream) {
    if (hAnimObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_HANIM, gtaRwFrameHAnimSize(hAnimObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &hAnimObj->hAnimVersion, 12))
            return rwFALSE;
        if (hAnimObj->numNodes > 0) {
            if (!gtaRwStreamWrite(stream, &hAnimObj->flags, 8))
                return rwFALSE;
            gtaRwUInt32 i;
            for (i = 0; i < hAnimObj->numNodes; i++) {
                if (!gtaRwStreamWrite(stream, &hAnimObj->nodes[i].nodeID, 12))
                    return rwFALSE;
            }
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwFrameHAnimRead(gtaRwFrameHAnim *hAnimObj, gtaRwStream *stream) {
    gtaRwFrameHAnimDestroy(hAnimObj);
    if (gtaRwStreamRead(stream, &hAnimObj->hAnimVersion, 12) != 12) {
        gtaRwFrameHAnimDestroy(hAnimObj);
        return rwFALSE;
    }
    if (hAnimObj->numNodes > 0) {
        if (gtaRwStreamRead(stream, &hAnimObj->flags, 8) != 8) {
            gtaRwFrameHAnimDestroy(hAnimObj);
            return rwFALSE;
        }
        hAnimObj->nodes = gtaRwMemAllocAndZero(sizeof(gtaRwHAnimNode) * hAnimObj->numNodes);
        gtaRwUInt32 i;
        for (i = 0; i < hAnimObj->numNodes; i++) {
            if (gtaRwStreamRead(stream, &hAnimObj->nodes[i].nodeID, 12) != 12) {
                gtaRwFrameHAnimDestroy(hAnimObj);
                return rwFALSE;
            }
        }
    }
    hAnimObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwFrameHAnimSize(gtaRwFrameHAnim *hAnimObj) {
    if (hAnimObj->enabled) {
        gtaRwUInt32 size = 24;
        if (hAnimObj->numNodes > 0)
            size += 12 * hAnimObj->numNodes + 8;
        return size;
    }
    return 0;
}
