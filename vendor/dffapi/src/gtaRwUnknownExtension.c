//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwUnknownExtensionWrite(gtaRwUnknownExtension *extObj, gtaRwStream *stream) {
    if (extObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, extObj->id, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (extObj->data && extObj->dataSize > 0) {
            if (!gtaRwStreamWrite(stream, extObj->data, extObj->dataSize))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwUnknownExtensionRead(gtaRwUnknownExtension *extObj, gtaRwStream *stream, gtaRwUInt32 id, gtaRwUInt32 dataSize) {
    gtaRwUnknownExtensionDestroy(extObj);
    void *data;
    if (dataSize > 0) {
        data = gtaRwMemAlloc(dataSize);
        if (gtaRwStreamRead(stream, data, dataSize) != dataSize) {
            gtaRwMemFree(data);
            return rwFALSE;
        }
    }
    else
        data = rwNULL;
    extObj->enabled = rwTRUE;
    extObj->id = id;
    extObj->dataSize = dataSize;
    extObj->data = data;
    return rwTRUE;
}

gtaRwUInt32 gtaRwUnknownExtensionSize(gtaRwUnknownExtension *extObj) {
    if (extObj->enabled)
        return 12 + extObj->dataSize;
    return 0;
}

void gtaRwUnknownExtensionInit(gtaRwUnknownExtension *extObj, gtaRwUInt32 id, gtaRwUInt8 *data, gtaRwUInt32 dataSize) {
    gtaRwUnknownExtensionDestroy(extObj);
    extObj->enabled = rwTRUE;
    extObj->id = id;
    extObj->data = data;
    extObj->dataSize = dataSize;
}

void gtaRwUnknownExtensionDestroy(gtaRwUnknownExtension *extObj) {
    if (extObj) {
        if (extObj->data)
            gtaRwMemFree(extObj->data);
        gtaRwMemZero(extObj, sizeof(gtaRwUnknownExtension));
    }
}

gtaRwUInt32 gtaRwUnknownExtensionsSize(gtaRwUnknownExtension *extObjects, gtaRwUInt32 numExtensions) {
    gtaRwUInt32 result = 0;
    if (extObjects) {
        gtaRwUInt32 i = 0;
        for (i = 0; i < numExtensions; i++)
            result += gtaRwUnknownExtensionSize(&extObjects[i]);
    }
    return result;
}

gtaRwBool gtaRwUnknownExtensionsWrite(gtaRwUnknownExtension * extObjects, gtaRwUInt32 numExtensions, gtaRwStream * stream) {
    if (extObjects) {
        gtaRwUInt32 i = 0;
        for (i = 0; i < numExtensions; i++) {
            if (!gtaRwUnknownExtensionWrite(&extObjects[i], stream))
                return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwUnknownExtensionAllocateAndRead(gtaRwUnknownExtension * *ppExtObjects, gtaRwUInt32 * numExtensions, gtaRwUInt32 type, gtaRwUInt32 entryLength, gtaRwStream * stream) {
    if (gtaRwIgnoreUnknownExtensions) {
        if (!gtaRwStreamSkip(stream, entryLength))
            return rwFALSE;
    }
    else {
        if (*numExtensions > 0)
            *ppExtObjects = gtaRwMemResize(*ppExtObjects, sizeof(gtaRwUnknownExtension) * *numExtensions, sizeof(gtaRwUnknownExtension) * (*numExtensions + 1));
        else
            *ppExtObjects = gtaRwMemAllocAndZero(sizeof(gtaRwUnknownExtension));
        if (!*ppExtObjects)
            return rwFALSE;
        if (!gtaRwUnknownExtensionRead(&(*ppExtObjects)[*numExtensions], stream, type, entryLength))
            return rwFALSE;
        *numExtensions += 1;
    }
    return rwTRUE;
}
