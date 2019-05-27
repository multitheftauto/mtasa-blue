//
// dffapi
// https://github.com/DK22Pac/dffapi
//
#include "StdInc.h"

void gtaRwMatFXBumpInit(gtaRwMatFXBump *bumpObj, gtaRwReal bumpiness, gtaRwBool hasTexture, gtaRwBool hasBumpedTexture) {
    gtaRwMatFXBumpDestroy(bumpObj);
    bumpObj->bumpiness = bumpiness;
    bumpObj->hasTexture = hasTexture;
    bumpObj->hasBumpedTexture = hasBumpedTexture;
}

gtaRwBool gtaRwMatFXBumpWrite(gtaRwMatFXBump *bumpObj, gtaRwStream *stream) {
    if (!gtaRwStreamWrite(stream, &bumpObj->bumpiness, 8))
        return rwFALSE;
    if (bumpObj->hasTexture) {
        if (!gtaRwMaterialTextureWrite(&bumpObj->texture, stream))
            return rwFALSE;
    }
    if (!gtaRwStreamWrite(stream, &bumpObj->hasBumpedTexture, 4))
        return rwFALSE;
    if (bumpObj->hasBumpedTexture) {
        if (!gtaRwMaterialTextureWrite(&bumpObj->bumpedTexture, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMatFXBumpRead(gtaRwMatFXBump *bumpObj, gtaRwStream *stream) {
    gtaRwMatFXBumpDestroy(bumpObj);
    if (gtaRwStreamRead(stream, &bumpObj->bumpiness, 8) != 8) {
        gtaRwMatFXBumpDestroy(bumpObj);
        return rwFALSE;
    }
    if (bumpObj->hasTexture) {
        if (!gtaRwMaterialTextureRead(&bumpObj->texture, stream)) {
            gtaRwMatFXBumpDestroy(bumpObj);
            return rwFALSE;
        }
    }
    if (gtaRwStreamRead(stream, &bumpObj->hasBumpedTexture, 4) != 4) {
        gtaRwMatFXBumpDestroy(bumpObj);
        return rwFALSE;
    }
    if (bumpObj->hasBumpedTexture) {
        if (!gtaRwMaterialTextureRead(&bumpObj->bumpedTexture, stream)) {
            gtaRwMatFXBumpDestroy(bumpObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwMatFXBumpSize(gtaRwMatFXBump *bumpObj) {
    gtaRwUInt32 size = 12;
    if (bumpObj->hasTexture)
        size += gtaRwMaterialTextureSize(&bumpObj->texture);
    if (bumpObj->hasBumpedTexture)
        size += gtaRwMaterialTextureSize(&bumpObj->bumpedTexture);
    return size;
}

void gtaRwMatFXBumpDestroy(gtaRwMatFXBump *bumpObj) {
    if (bumpObj) {
        gtaRwMaterialTextureDestroy(&bumpObj->texture);
        gtaRwMaterialTextureDestroy(&bumpObj->bumpedTexture);
        gtaRwMemZero(bumpObj, sizeof(gtaRwMatFXBump));
    }
}

void gtaRwMatFXEnvInit(gtaRwMatFXEnv *envObj, gtaRwReal coefficient, gtaRwBool useFrameBufferAlpha, gtaRwBool hasTexture) {
    gtaRwMatFXEnvDestroy(envObj);
    envObj->coefficient = coefficient;
    envObj->useFrameBufferAlpha = useFrameBufferAlpha;
    envObj->hasTexture = hasTexture;
}

gtaRwBool gtaRwMatFXEnvWrite(gtaRwMatFXEnv *envObj, gtaRwStream *stream) {
    if (!gtaRwStreamWrite(stream, &envObj->coefficient, 12))
        return rwFALSE;
    if (envObj->hasTexture) {
        if (!gtaRwMaterialTextureWrite(&envObj->texture, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMatFXEnvRead(gtaRwMatFXEnv *envObj, gtaRwStream *stream) {
    gtaRwMatFXEnvDestroy(envObj);
    if (gtaRwStreamRead(stream, &envObj->coefficient, 12) != 12) {
        gtaRwMatFXEnvDestroy(envObj);
        return rwFALSE;
    }
    if (envObj->hasTexture) {
        if (!gtaRwMaterialTextureRead(&envObj->texture, stream)) {
            gtaRwMatFXEnvDestroy(envObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwMatFXEnvSize(gtaRwMatFXEnv *envObj) {
    gtaRwUInt32 size = 12;
    if (envObj->hasTexture)
        size += gtaRwMaterialTextureSize(&envObj->texture);
    return size;
}

void gtaRwMatFXEnvDestroy(gtaRwMatFXEnv *envObj) {
    if (envObj) {
        gtaRwMaterialTextureDestroy(&envObj->texture);
        gtaRwMemZero(envObj, sizeof(gtaRwMatFXEnv));
    }
}

void gtaRwMatFXDualInit(gtaRwMatFXDual *dualObj, gtaRwUInt32 srcBlendMode, gtaRwUInt32 dstBlendMode, gtaRwBool hasTexture) {
    gtaRwMatFXDualDestroy(dualObj);
    dualObj->srcBlendMode = srcBlendMode;
    dualObj->dstBlendMode = dstBlendMode;
    dualObj->hasTexture = hasTexture;
}

gtaRwBool gtaRwMatFXDualWrite(gtaRwMatFXDual *dualObj, gtaRwStream *stream) {
    if (!gtaRwStreamWrite(stream, &dualObj->srcBlendMode, 12))
        return rwFALSE;
    if (dualObj->hasTexture)
    {
        if (!gtaRwMaterialTextureWrite(&dualObj->texture, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMatFXDualRead(gtaRwMatFXDual *dualObj, gtaRwStream *stream) {
    gtaRwMatFXDualDestroy(dualObj);
    if (gtaRwStreamRead(stream, &dualObj->srcBlendMode, 12) != 12) {
        gtaRwMatFXDualDestroy(dualObj);
        return rwFALSE;
    }
    if (dualObj->hasTexture) {
        if (!gtaRwMaterialTextureRead(&dualObj->texture, stream)) {
            gtaRwMatFXDualDestroy(dualObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwMatFXDualSize(gtaRwMatFXDual *dualObj) {
    gtaRwUInt32 size = 12;
    if (dualObj->hasTexture)
        size += gtaRwMaterialTextureSize(&dualObj->texture);
    return size;
}

void gtaRwMatFXDualDestroy(gtaRwMatFXDual *dualObj) {
    if (dualObj) {
        gtaRwMaterialTextureDestroy(&dualObj->texture);
        gtaRwMemZero(dualObj, sizeof(gtaRwMatFXDual));
    }
}

void gtaRwMatFXEffectInit(gtaRwMatFXEffect *effectObj) {
    gtaRwMatFXEffectSetupNull(effectObj);
}

void gtaRwMatFXEffectSetupNull(gtaRwMatFXEffect *effectObj) {
    gtaRwMatFXEffectDestroy(effectObj);
    effectObj->type = rwMATFXEFFECTNULL;
}

void gtaRwMatFXEffectSetupBump(gtaRwMatFXEffect *effectObj, gtaRwReal bumpiness, gtaRwBool hasTexture, gtaRwBool hasBumpedTexture) {
    gtaRwMatFXEffectDestroy(effectObj);
    effectObj->type = rwMATFXEFFECTBUMPMAP;
    gtaRwMatFXBumpInit(&effectObj->bump, bumpiness, hasTexture, hasBumpedTexture);
}

void gtaRwMatFXEffectSetupEnv(gtaRwMatFXEffect *effectObj, gtaRwReal coefficient, gtaRwBool useFrameBufferAlpha, gtaRwBool hasTexture) {
    gtaRwMatFXEffectDestroy(effectObj);
    effectObj->type = rwMATFXEFFECTENVMAP;
    gtaRwMatFXEnvInit(&effectObj->env, coefficient, useFrameBufferAlpha, hasTexture);
}

void gtaRwMatFXEffectSetupDual(gtaRwMatFXEffect *effectObj, gtaRwUInt32 srcBlendMode, gtaRwUInt32 dstBlendMode, gtaRwBool hasTexture) {
    gtaRwMatFXEffectDestroy(effectObj);
    effectObj->type = rwMATFXEFFECTDUAL;
    gtaRwMatFXDualInit(&effectObj->dual, srcBlendMode, dstBlendMode, hasTexture);
}

void gtaRwMatFXEffectSetupUvTransform(gtaRwMatFXEffect *effectObj) {
    gtaRwMatFXEffectDestroy(effectObj);
    effectObj->type = rwMATFXEFFECTUVTRANSFORM;
}

gtaRwBool gtaRwMatFXEffectWrite(gtaRwMatFXEffect *effectObj, gtaRwStream *stream) {
    if (!gtaRwStreamWrite(stream, &effectObj->type, 4))
        return rwFALSE;
    switch (effectObj->type) {
    case rwMATFXEFFECTBUMPMAP:
        if (!gtaRwMatFXBumpWrite(&effectObj->bump, stream))
            return rwFALSE;
        break;
    case rwMATFXEFFECTENVMAP:
        if (!gtaRwMatFXEnvWrite(&effectObj->env, stream))
            return rwFALSE;
        break;
    case rwMATFXEFFECTDUAL:
        if (!gtaRwMatFXDualWrite(&effectObj->dual, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMatFXEffectRead(gtaRwMatFXEffect *effectObj, gtaRwStream *stream) {
    gtaRwMatFXEffectDestroy(effectObj);
    if (gtaRwStreamRead(stream, &effectObj->type, 4) != 4) {
        gtaRwMatFXEffectDestroy(effectObj);
        return rwFALSE;
    }
    switch (effectObj->type)
    {
    case rwMATFXEFFECTBUMPMAP:
        if (!gtaRwMatFXBumpWrite(&effectObj->bump, stream)) {
            gtaRwMatFXEffectDestroy(effectObj);
            return rwFALSE;
        }
        break;
    case rwMATFXEFFECTENVMAP:
        if (!gtaRwMatFXEnvWrite(&effectObj->env, stream)) {
            gtaRwMatFXEffectDestroy(effectObj);
            return rwFALSE;
        }
        break;
    case rwMATFXEFFECTDUAL:
        if (!gtaRwMatFXDualWrite(&effectObj->dual, stream)) {
            gtaRwMatFXEffectDestroy(effectObj);
            return rwFALSE;
        }
    }
    return rwTRUE;
}

gtaRwUInt32 gtaRwMatFXEffectSize(gtaRwMatFXEffect *effectObj) {
    gtaRwUInt32 size = 4;
    switch (effectObj->type)
    {
    case rwMATFXEFFECTBUMPMAP:
        size += gtaRwMatFXBumpSize(&effectObj->bump);
        break;
    case rwMATFXEFFECTENVMAP:
        size += gtaRwMatFXEnvSize(&effectObj->env);
        break;
    case rwMATFXEFFECTDUAL:
        size += gtaRwMatFXDualSize(&effectObj->dual);
    }
    return size;
}

void gtaRwMatFXEffectDestroy(gtaRwMatFXEffect *effectObj) {
    if (effectObj) {
        gtaRwMatFXBumpDestroy(&effectObj->bump);
        gtaRwMatFXEnvDestroy(&effectObj->env);
        gtaRwMatFXDualDestroy(&effectObj->dual);
        gtaRwMemZero(effectObj, sizeof(gtaRwMatFXEffect));
    }
}

void gtaRwMaterialMatFXInit(gtaRwMaterialMatFX *fxObj, gtaRwMatFXMaterialFlags effectType) {
    gtaRwMaterialMatFXDestroy(fxObj);
    fxObj->enabled = rwTRUE;
    fxObj->effectType = effectType;
    switch (effectType) {
    case rwMATFXEFFECTBUMPMAP:
        fxObj->effect1.type = rwMATFXEFFECTBUMPMAP;
        fxObj->effect2.type = rwMATFXEFFECTNULL;
        break;
    case rwMATFXEFFECTENVMAP:
        fxObj->effect1.type = rwMATFXEFFECTENVMAP;
        fxObj->effect2.type = rwMATFXEFFECTNULL;
        break;
    case rwMATFXEFFECTBUMPENVMAP:
        fxObj->effect1.type = rwMATFXEFFECTBUMPMAP;
        fxObj->effect2.type = rwMATFXEFFECTENVMAP;
        break;
    case rwMATFXEFFECTDUAL:
        fxObj->effect1.type = rwMATFXEFFECTDUAL;
        fxObj->effect2.type = rwMATFXEFFECTNULL;
        break;
    case rwMATFXEFFECTUVTRANSFORM:
        fxObj->effect1.type = rwMATFXEFFECTUVTRANSFORM;
        fxObj->effect2.type = rwMATFXEFFECTNULL;
        break;
    case rwMATFXEFFECTDUALUVTRANSFORM:
        fxObj->effect1.type = rwMATFXEFFECTDUAL;
        fxObj->effect2.type = rwMATFXEFFECTUVTRANSFORM;
        break;
    default:
        fxObj->effect1.type = rwMATFXEFFECTNULL;
        fxObj->effect2.type = rwMATFXEFFECTNULL;
    }
}

void gtaRwMaterialMatFXDestroy(gtaRwMaterialMatFX *fxObj) {
    if (fxObj) {
        gtaRwMatFXEffectDestroy(&fxObj->effect1);
        gtaRwMatFXEffectDestroy(&fxObj->effect2);
        gtaRwMemZero(fxObj, sizeof(gtaRwMaterialMatFX));
    }
}

gtaRwBool gtaRwMaterialMatFXWrite(gtaRwMaterialMatFX *fxObj, gtaRwStream *stream) {
    if (fxObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATFX, gtaRwMaterialMatFXSize(fxObj) - 12, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &fxObj->effectType, 4))
            return rwFALSE;
        if (!gtaRwMatFXEffectWrite(&fxObj->effect1, stream))
            return rwFALSE;
        if (!gtaRwMatFXEffectWrite(&fxObj->effect2, stream))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwMaterialMatFXRead(gtaRwMaterialMatFX *fxObj, gtaRwStream *stream) {
    gtaRwMaterialMatFXDestroy(fxObj);
    if (gtaRwStreamRead(stream, &fxObj->effectType, 4) != 4) {
        gtaRwMaterialMatFXDestroy(fxObj);
        return rwFALSE;
    }
    if (!gtaRwMatFXEffectRead(&fxObj->effect1, stream)) {
        gtaRwMaterialMatFXDestroy(fxObj);
        return rwFALSE;
    }
    if (!gtaRwMatFXEffectRead(&fxObj->effect2, stream)) {
        gtaRwMaterialMatFXDestroy(fxObj);
        return rwFALSE;
    }
    fxObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwMaterialMatFXSize(gtaRwMaterialMatFX *fxObj) {
    if (fxObj->enabled)
        return 16 + gtaRwMatFXEffectSize(&fxObj->effect1) + gtaRwMatFXEffectSize(&fxObj->effect2);
    return 0;
}

void gtaRwAtomicMatFXInit(gtaRwAtomicMatFX *atomicFxObj, gtaRwUInt32 matFxEnabled) {
    gtaRwAtomicMatFXDestroy(atomicFxObj);
    atomicFxObj->enabled = rwTRUE;
    atomicFxObj->matFxEnabled = matFxEnabled;
}

gtaRwBool gtaRwAtomicMatFXWrite(gtaRwAtomicMatFX *atomicFxObj, gtaRwStream *stream) {
    if (atomicFxObj->enabled) {
        if (!gtaRwStreamWriteVersionedChunkHeader(stream, rwID_MATFX, 4, gtaRwVersion, gtaRwBuild))
            return rwFALSE;
        if (!gtaRwStreamWrite(stream, &atomicFxObj->matFxEnabled, 4))
            return rwFALSE;
    }
    return rwTRUE;
}

gtaRwBool gtaRwAtomicMatFXRead(gtaRwAtomicMatFX *atomicFxObj, gtaRwStream *stream) {
    gtaRwAtomicMatFXDestroy(atomicFxObj);
    if (gtaRwStreamRead(stream, &atomicFxObj->matFxEnabled, 4) != 4) {
        gtaRwAtomicMatFXDestroy(atomicFxObj);
        return rwFALSE;
    }
    atomicFxObj->enabled = rwTRUE;
    return rwTRUE;
}

gtaRwUInt32 gtaRwAtomicMatFXSize(gtaRwAtomicMatFX *atomicFxObj) {
    if (atomicFxObj->enabled)
        return 16;
    return 0;
}

void gtaRwAtomicMatFXDestroy(gtaRwAtomicMatFX *atomicFxObj) {
    if (atomicFxObj)
        gtaRwMemZero(atomicFxObj, sizeof(gtaRwAtomicMatFX));
}
