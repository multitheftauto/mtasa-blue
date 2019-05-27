//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwFrameNodeNameWrite(gtaRwFrameNodeName *nodeNameObj, gtaRwStream *stream) {
    if (nodeNameObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, gtaID_NODENAME, gtaRwFrameNodeNameSize(nodeNameObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (nodeNameObj->name) {
            if (!gtaRwStreamWrite(stream, nodeNameObj->name, strlen(nodeNameObj->name)))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwFrameNodeNameRead(gtaRwFrameNodeName *nodeNameObj, gtaRwStream *stream, gtaRwUInt32 length) {
    gtaRwFrameNodeNameDestroy(nodeNameObj);
    if (length > 0) {
        nodeNameObj->name = (gtaRwChar *)gtaRwMemAllocAndZero(length + 1);
        if (gtaRwStreamRead(stream, nodeNameObj->name, length) != length) {
            gtaRwFrameNodeNameDestroy(nodeNameObj);
            return rwFALSE;
        }
        if (nodeNameObj->name[0] == '\0') {
            gtaRwMemFree(nodeNameObj->name);
            nodeNameObj->name = rwNULL;
        }
    }
    nodeNameObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwFrameNodeNameSize(gtaRwFrameNodeName *nodeNameObj) {
    if (nodeNameObj->enabled) {
        gtaRwUInt32 size = 12;
        if (nodeNameObj->name)
            size += strlen(nodeNameObj->name);
        return size;
    }
    return 0;
}

void gtaRwFrameNodeNameInit(gtaRwFrameNodeName *nodeNameObj, gtaRwChar const *name) {
    gtaRwFrameNodeNameDestroy(nodeNameObj);
    nodeNameObj->enabled = rwTRUE;
    if (name && name[0] != '\0') {
        nodeNameObj->name = (gtaRwChar *)gtaRwMemAllocAndZero(strlen(name) + 1);
        strcpy(nodeNameObj->name, name);
    }
}

void gtaRwFrameNodeNameDestroy(gtaRwFrameNodeName *nodeNameObj) {
    if (nodeNameObj) {
        if (nodeNameObj->name)
            gtaRwMemFree(nodeNameObj->name);
        gtaRwMemZero(nodeNameObj, sizeof(gtaRwFrameNodeName));
    }
}
