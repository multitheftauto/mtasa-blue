//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwFrameInitWithMatrix(gtaRwFrame *frameObj, gtaRwMatrix *matrix, gtaRwInt32 parent, gtaRwUInt32 matrixFlags) {
    gtaRwFrameDestroy(frameObj);
    if (matrix) {
        gtaRwMemCopy(&frameObj->right, &matrix->right, sizeof(gtaRwV3d));
        gtaRwMemCopy(&frameObj->up, &matrix->up, sizeof(gtaRwV3d));
        gtaRwMemCopy(&frameObj->at, &matrix->at, sizeof(gtaRwV3d));
        gtaRwMemCopy(&frameObj->pos, &matrix->pos, sizeof(gtaRwV3d));
    }
    frameObj->parent = parent;
    frameObj->matrixFlags = matrixFlags;
}

void gtaRwFrameInitIdentity(gtaRwFrame *frameObj, gtaRwInt32 parent, gtaRwUInt32 matrixFlags) {
    gtaRwFrameDestroy(frameObj);
    frameObj->right.x = 1.0f;
    frameObj->up.y = 1.0f;
    frameObj->at.z = 1.0f;
    frameObj->parent = parent;
    frameObj->matrixFlags = matrixFlags;
}

void gtaRwFrameInit(gtaRwFrame *frameObj, gtaRwV3d *right, gtaRwV3d *up, gtaRwV3d *at, gtaRwV3d *pos, gtaRwInt32 parent, gtaRwUInt32 matrixFlags) {
    gtaRwFrameDestroy(frameObj);
    if (right)
        gtaRwMemCopy(&frameObj->right, &right, sizeof(gtaRwV3d));
    if (up)
        gtaRwMemCopy(&frameObj->up, &up, sizeof(gtaRwV3d));
    if (at)
        gtaRwMemCopy(&frameObj->at, &at, sizeof(gtaRwV3d));
    if (pos)
        gtaRwMemCopy(&frameObj->pos, &pos, sizeof(gtaRwV3d));
    frameObj->parent = parent;
    frameObj->matrixFlags = matrixFlags;
}

void gtaRwFrameDestroy(gtaRwFrame *frameObj) {
    if (frameObj) {
        gtaRwFrameHAnimDestroy(&frameObj->hAnim);
        gtaRwFrameNodeNameDestroy(&frameObj->nodeName);
        gtaRwMemZero(frameObj, sizeof(gtaRwFrame));
    }
}

void gtaRwFrameSetName(gtaRwFrame *frameObj, gtaRwChar const *name) {
    gtaRwFrameNodeNameInit(&frameObj->nodeName, name);
}

void gtaRwFrameListInit(gtaRwFrameList *frameListObj, gtaRwInt32 frameCount) {
    gtaRwFrameListDestroy(frameListObj);
    frameListObj->frameCount = frameCount;
    frameListObj->frames = gtaRwMemAllocAndZero(sizeof(gtaRwFrame) * frameCount);
}

void gtaRwFrameListDestroy(gtaRwFrameList *frameListObj) {
    if (frameListObj) {
        if (frameListObj->frames) {
            gtaRwUInt32 i;
            for (i = 0; i < frameListObj->frameCount; i++)
                gtaRwFrameDestroy(&frameListObj->frames[i]);
            gtaRwMemFree(frameListObj->frames);
        }
        gtaRwMemZero(frameListObj, sizeof(gtaRwFrameList));
    }
}

gtaRwBool gtaRwFrameListRead(gtaRwFrameList *frameListObj, gtaRwStream *stream) {
    gtaRwFrameListDestroy(frameListObj);
    gtaRwUInt32 type, length, entryLength;
    if (!gtaRwStreamFindChunk(stream, rwID_FRAMELIST, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL))
        return rwFALSE;
    if (gtaRwStreamRead(stream, &frameListObj->frameCount, 4) != 4) {
        gtaRwFrameListDestroy(frameListObj);
        return rwFALSE;
    }
    if (frameListObj->frameCount > 0) {
        frameListObj->frames = gtaRwMemAllocAndZero(sizeof(gtaRwFrame) * frameListObj->frameCount);
        gtaRwUInt32 i;
        for (i = 0; i < frameListObj->frameCount; i++) {
            if (gtaRwStreamRead(stream, &frameListObj->frames[i], 56) != 56) {
                gtaRwFrameListDestroy(frameListObj);
                return rwFALSE;
            }
        }
        for (i = 0; i < frameListObj->frameCount; i++) {
            if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, rwNULL, rwNULL)) {
                gtaRwFrameListDestroy(frameListObj);
                return rwFALSE;
            }
            while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, rwNULL, rwNULL)) {
                switch (type) {
                case rwID_HANIM:
                    if (! gtaRwFrameHAnimRead(&frameListObj->frames[i].hAnim, stream)) {
                        gtaRwFrameListDestroy(frameListObj);
                        return rwFALSE;
                    }
                    break;
                case gtaID_NODENAME:
                    if (!gtaRwFrameNodeNameRead(&frameListObj->frames[i].nodeName, stream, length)) {
                        gtaRwFrameListDestroy(frameListObj);
                        return rwFALSE;
                    }
                    break;
                default:
                    if (!gtaRwUnknownExtensionAllocateAndRead(&frameListObj->frames[i].unknownExtensions, &frameListObj->frames[i].numUnknownExtensions, type, entryLength, stream)) {
                        gtaRwFrameListDestroy(frameListObj);
                        return rwFALSE;
                    }
                }
                length += -12 - entryLength;
            }
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwFrameListWrite(gtaRwFrameList *frameListObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_FRAMELIST, gtaRwFrameListSize(frameListObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4 + frameListObj->frameCount * 56, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWrite(stream, &frameListObj->frameCount, 4))
        return rwFALSE;
    gtaRwUInt32 i;
    for (i = 0; i < frameListObj->frameCount; i++) {
        if (!gtaRwStreamWrite(stream, &frameListObj->frames[i], 56))
            return rwFALSE;
    }
    for (i = 0; i < frameListObj->frameCount; i++) {
        gtaRwUInt32 extensionsSize =
            gtaRwFrameHAnimSize(&frameListObj->frames[i].hAnim)
            + gtaRwFrameNodeNameSize(&frameListObj->frames[i].nodeName)
            + gtaRwUnknownExtensionsSize(frameListObj->frames[i].unknownExtensions, frameListObj->frames[i].numUnknownExtensions);
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, extensionsSize, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwFrameHAnimWrite(&frameListObj->frames[i].hAnim, stream))
            return rwFALSE;
        if (!gtaRwFrameNodeNameWrite(&frameListObj->frames[i].nodeName, stream))
            return rwFALSE;
        if (!gtaRwUnknownExtensionsWrite(frameListObj->frames[i].unknownExtensions, frameListObj->frames[i].numUnknownExtensions, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwFrameListSize(gtaRwFrameList *frameListObj) {
    gtaRwUInt32 size = 28;
    gtaRwUInt32 i;
    for (i = 0; i < frameListObj->frameCount; i++) {
        size += 68
            + gtaRwFrameHAnimSize(&frameListObj->frames[i].hAnim)
            + gtaRwFrameNodeNameSize(&frameListObj->frames[i].nodeName)
            + gtaRwUnknownExtensionsSize(frameListObj->frames[i].unknownExtensions, frameListObj->frames[i].numUnknownExtensions);
    }
    return size;
}
