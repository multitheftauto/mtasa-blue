//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwTexDictionaryInitialise(gtaRwTexDictionary *txdObj, gtaRwUInt16 numTextures) {
    gtaRwTexDictionaryDestroy(txdObj);
    txdObj->_zero = 0;
    txdObj->deviceId = 2;
    numTextures = numTextures;
    if (numTextures > 0)
        txdObj->textures = gtaRwMemAllocAndZero(sizeof(gtaRwTextureD3D) * numTextures);
}

void gtaRwTexDictionaryDestroy(gtaRwTexDictionary *txdObj) {
    if (txdObj) {
        if (txdObj->textures) {
            gtaRwUInt32 i;
            for (i = 0; i < txdObj->numTextures; i++)
                gtaRwTextureD3DDestroy(&txdObj->textures[i]);
            gtaRwMemFree(txdObj->textures);
        }
    }
}

gtaRwBool gtaRwTexDictionaryRead(gtaRwTexDictionary *txdObj, gtaRwStream *stream) {
    gtaRwTexDictionaryDestroy(txdObj);
    if (!gtaRwStreamFindChunk(stream, rwID_TEXDICTIONARY, rwNULL, &txdObj->version, &txdObj->build)) {
        gtaRwTexDictionaryDestroy(txdObj);
        return rwFALSE;
    }
    if (!gtaRwStreamFindChunk(stream, rwID_STRUCT, rwNULL, rwNULL, rwNULL)) {
        gtaRwTexDictionaryDestroy(txdObj);
        return rwFALSE;
    }
    if (gtaRwStreamRead(stream, &txdObj->numTextures, 4) != 4) {
        gtaRwTexDictionaryDestroy(txdObj);
        return rwFALSE;
    }
    if (txdObj->numTextures > 0) {
        txdObj->textures = gtaRwMemAllocAndZero(sizeof(gtaRwTextureD3D) * txdObj->numTextures);
        for (gtaRwInt32 i = 0; i < txdObj->numTextures; i++) {
            if (!gtaRwTextureD3DRead(&txdObj->textures[i], stream)) {
                gtaRwTexDictionaryDestroy(txdObj);
                return rwFALSE;
            }
        }
    }
    if (!gtaRwIgnoreTxdExtensions) {
        gtaRwUInt32 length;
        if (!gtaRwStreamFindChunk(stream, rwID_EXTENSION, &length, rwNULL, rwNULL)) {
            gtaRwTexDictionaryDestroy(txdObj);
            return rwFALSE;
        }
        gtaRwUInt32 type, entryLength;
        while (length && gtaRwStreamReadChunkHeader(stream, &type, &entryLength, rwNULL, rwNULL)) {
            if (!gtaRwUnknownExtensionAllocateAndRead(&txdObj->unknownExtensions, &txdObj->numUnknownExtensions, type, entryLength, stream)) {
                gtaRwTexDictionaryDestroy(txdObj);
                return rwFALSE;
            }
            length += -12 - entryLength;
        }
    }
    return rwTRUE;
}

gtaRwBool gtaRwTexDictionaryWrite(gtaRwTexDictionary *txdObj, gtaRwStream *stream) {
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_TEXDICTIONARY, gtaRwTexDictionarySize(txdObj) - 12, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_STRUCT, 4, gtaRwVersion, gtaRwBuild))
        return rwFALSE;
    if (gtaRwVersion >= 0x36000) {
        if (!gtaRwStreamWrite(stream, &txdObj->numTextures, 4))
            return rwFALSE;
    }
    else {
        gtaRwUInt32 numTexturesOnly = txdObj->numTextures;
        if (!gtaRwStreamWrite(stream, &numTexturesOnly, 4))
            return rwFALSE;
    }
    gtaRwUInt32 i;
    for (i = 0; i < txdObj->numTextures; i++) {
        if (!gtaRwTextureD3DWrite(&txdObj->textures[i], stream))
            return rwFALSE;
    }
    if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_EXTENSION, 0, gtaRwVersion, gtaRwBuild))
        return rwFALSE;

    return rwTRUE;
}

gtaRwUInt32 gtaRwTexDictionarySize(gtaRwTexDictionary *txdObj) {
    gtaRwUInt32 size = 40;
    gtaRwInt32 i;
    for (i = 0; i < txdObj->numTextures; i++)
        size += gtaRwTextureD3DSize(&txdObj->textures[i]);
    return size;
}
