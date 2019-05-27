//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMatFXBump gtaRwMatFXBump;
typedef struct gtaRwMatFXEnv gtaRwMatFXEnv;
typedef struct gtaRwMatFXDual gtaRwMatFXDual;
typedef struct gtaRwMatFXEffect gtaRwMatFXEffect;
typedef struct gtaRwMaterialMatFX gtaRwMaterialMatFX;
typedef struct gtaRwAtomicMatFX gtaRwAtomicMatFX;

struct gtaRwMatFXBump {
    gtaRwReal bumpiness;
    gtaRwBool hasTexture;
    gtaRwMaterialTexture texture;
    gtaRwBool hasBumpedTexture;
    gtaRwMaterialTexture bumpedTexture;
};

struct gtaRwMatFXEnv {
    gtaRwReal coefficient;
    gtaRwBool useFrameBufferAlpha;
    gtaRwBool hasTexture;
    gtaRwMaterialTexture texture;
};

struct gtaRwMatFXDual {
    gtaRwUInt32 srcBlendMode;
    gtaRwUInt32 dstBlendMode;
    gtaRwBool hasTexture;
    gtaRwMaterialTexture texture;
};

struct gtaRwMatFXEffect {
    gtaRwUInt32 type;
    gtaRwMatFXBump bump;
    gtaRwMatFXEnv env;
    gtaRwMatFXDual dual;
};

struct gtaRwMaterialMatFX {
    gtaRwBool enabled;
    gtaRwUInt32 effectType;
    gtaRwMatFXEffect effect1;
    gtaRwMatFXEffect effect2; // if second effect is not used, it must be effect with rpMATFXEFFECTNULL type
};

struct gtaRwAtomicMatFX {
    gtaRwBool enabled;
    gtaRwUInt32 matFxEnabled;
};

void gtaRwMatFXBumpInit(gtaRwMatFXBump *bumpObj, gtaRwReal bumpiness, gtaRwBool hasTexture, gtaRwBool hasBumpedTexture);
gtaRwBool gtaRwMatFXBumpWrite(gtaRwMatFXBump *bumpObj, gtaRwStream *stream);
gtaRwBool gtaRwMatFXBumpRead(gtaRwMatFXBump *bumpObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMatFXBumpSize(gtaRwMatFXBump *bumpObj);
void gtaRwMatFXBumpDestroy(gtaRwMatFXBump *bumpObj);

void gtaRwMatFXEnvInit(gtaRwMatFXEnv *envObj, gtaRwReal coefficient, gtaRwBool useFrameBufferAlpha, gtaRwBool hasTexture);
gtaRwBool gtaRwMatFXEnvWrite(gtaRwMatFXEnv *envObj, gtaRwStream *stream);
gtaRwBool gtaRwMatFXEnvRead(gtaRwMatFXEnv *envObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMatFXEnvSize(gtaRwMatFXEnv *envObj);
void gtaRwMatFXEnvDestroy(gtaRwMatFXEnv *envObj);

void gtaRwMatFXDualInit(gtaRwMatFXDual *dualObj, gtaRwUInt32 srcBlendMode, gtaRwUInt32 dstBlendMode, gtaRwBool hasTexture);
gtaRwBool gtaRwMatFXDualWrite(gtaRwMatFXDual *dualObj, gtaRwStream *stream);
gtaRwBool gtaRwMatFXDualRead(gtaRwMatFXDual *dualObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMatFXDualSize(gtaRwMatFXDual *dualObj);
void gtaRwMatFXDualDestroy(gtaRwMatFXDual *dualObj);

void gtaRwMatFXEffectInit(gtaRwMatFXEffect *effectObj);
void gtaRwMatFXEffectSetupNull(gtaRwMatFXEffect *effectObj);
void gtaRwMatFXEffectSetupBump(gtaRwMatFXEffect *effectObj, gtaRwReal bumpiness, gtaRwBool hasTexture, gtaRwBool hasBumpedTexture);
void gtaRwMatFXEffectSetupEnv(gtaRwMatFXEffect *effectObj, gtaRwReal coefficient, gtaRwBool useFrameBufferAlpha, gtaRwBool hasTexture);
void gtaRwMatFXEffectSetupDual(gtaRwMatFXEffect *effectObj, gtaRwUInt32 srcBlendMode, gtaRwUInt32 dstBlendMode, gtaRwBool hasTexture);
void gtaRwMatFXEffectSetupUvTransform(gtaRwMatFXEffect *effectObj);
gtaRwBool gtaRwMatFXEffectWrite(gtaRwMatFXEffect *effectObj, gtaRwStream *stream);
gtaRwBool gtaRwMatFXEffectRead(gtaRwMatFXEffect *effectObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMatFXEffectSize(gtaRwMatFXEffect *effectObj);
void gtaRwMatFXEffectDestroy(gtaRwMatFXEffect *effectObj);

void gtaRwMaterialMatFXInit(gtaRwMaterialMatFX *fxObj, gtaRwMatFXMaterialFlags effectType);
gtaRwBool gtaRwMaterialMatFXWrite(gtaRwMaterialMatFX *fxObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialMatFXRead(gtaRwMaterialMatFX *fxObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialMatFXSize(gtaRwMaterialMatFX *fxObj);
void gtaRwMaterialMatFXDestroy(gtaRwMaterialMatFX *fxObj);

void gtaRwAtomicMatFXInit(gtaRwAtomicMatFX *atomicFxObj, gtaRwUInt32 matFxEnabled);
gtaRwBool gtaRwAtomicMatFXWrite(gtaRwAtomicMatFX *atomicFxObj, gtaRwStream *stream);
gtaRwBool gtaRwAtomicMatFXRead(gtaRwAtomicMatFX *atomicFxObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwAtomicMatFXSize(gtaRwAtomicMatFX *atomicFxObj);
void gtaRwAtomicMatFXDestroy(gtaRwAtomicMatFX *atomicFxObj);
