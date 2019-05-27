//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwTextureSkyMipmapSetK(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwReal k) {
    skyMipmapObj->kValue = (gtaRwInt32)(k * 16.0f);
    if (skyMipmapObj->kValue >= -2048) {
        if (skyMipmapObj->kValue > 2047)
            skyMipmapObj->kValue = 2047;
    }
    else
        skyMipmapObj->kValue = -2048;
}

void gtaRwTextureSkyMipmapSetL(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwUInt32 l) {
    if (l > 3)
        l = 3;
    skyMipmapObj->lValue = l;
}

gtaRwReal gtaRwTextureSkyMipmapGetK(gtaRwTextureSkyMipmap *skyMipmapObj) {
    if (skyMipmapObj->kValue < 0)
        return 0.0f;
    else
        return (gtaRwReal)skyMipmapObj->kValue *0.0625f;
}

gtaRwUInt32 gtaRwTextureSkyMipmapGetL(gtaRwTextureSkyMipmap *skyMipmapObj) {
    return skyMipmapObj->lValue;
}

gtaRwBool gtaRwTextureSkyMipmapWrite(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwStream *stream) {
    if (skyMipmapObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_SKYMIPMAP, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &skyMipmapObj->klValues, 4))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureSkyMipmapRead(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwStream *stream) {
    gtaRwTextureSkyMipmapDestroy(skyMipmapObj);
    if (gtaRwStreamRead(stream, &skyMipmapObj->klValues, 4) != 4) {
        gtaRwTextureSkyMipmapDestroy(skyMipmapObj);
        return rwFALSE;
    }
    skyMipmapObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwTextureSkyMipmapSize(gtaRwTextureSkyMipmap *skyMipmapObj) {
    if (skyMipmapObj->enabled)
        return 16;
    return 0;
}

void gtaRwTextureSkyMipmapInit(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwInt32 kValue, gtaRwUInt32 lValue) {
    gtaRwTextureSkyMipmapDestroy(skyMipmapObj);
    skyMipmapObj->enabled = rwTRUE;
    skyMipmapObj->kValue = kValue;
    skyMipmapObj->lValue = lValue;
}

void gtaRwTextureSkyMipmapInitWithUncompressedKL(gtaRwTextureSkyMipmap *skyMipmapObj, gtaRwReal kValue, gtaRwUInt32 lValue) {
    gtaRwTextureSkyMipmapDestroy(skyMipmapObj);
    skyMipmapObj->enabled = rwTRUE;
    gtaRwTextureSkyMipmapSetK(skyMipmapObj, kValue);
    gtaRwTextureSkyMipmapSetL(skyMipmapObj, lValue);
}

void gtaRwTextureSkyMipmapDestroy(gtaRwTextureSkyMipmap *skyMipmapObj) {
    if (skyMipmapObj)
        gtaRwMemZero(skyMipmapObj, sizeof(gtaRwTextureSkyMipmap));
}
