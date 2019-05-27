//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

gtaRwBool gtaRwTextureAnisotWrite(gtaRwTextureAnisot *anisotObj, gtaRwStream *stream) {
    if (anisotObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_ANISOT, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &anisotObj->anisotropyLevel, 4))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwTextureAnisotRead(gtaRwTextureAnisot *anisotObj, gtaRwStream *stream) {
    gtaRwTextureAnisotDestroy(anisotObj);
    if (gtaRwStreamRead(stream, &anisotObj->anisotropyLevel, 4) != 4) {
        gtaRwTextureAnisotDestroy(anisotObj);
        return rwFALSE;
    }
    anisotObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwTextureAnisotSize(gtaRwTextureAnisot *anisotObj) {
    if (anisotObj->enabled)
        return 16;
    return 0;
}

void gtaRwTextureAnisotInit(gtaRwTextureAnisot *anisotObj, gtaRwUInt32 anisotLevel) {
    gtaRwTextureAnisotDestroy(anisotObj);
    anisotObj->enabled = rwTRUE;
    anisotObj->anisotropyLevel = anisotLevel;
}

void gtaRwTextureAnisotDestroy(gtaRwTextureAnisot *anisotObj) {
    if (anisotObj)
        gtaRwMemZero(anisotObj, sizeof(gtaRwTextureAnisot));
}
