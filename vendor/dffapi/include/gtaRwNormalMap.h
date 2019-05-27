//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterialNormalMap gtaRwMaterialNormalMap;

struct gtaRwMaterialNormalMap {
    gtaRwBool enabled;
    union {
        gtaRwUInt32 flags;
        struct {
            gtaRwUInt32 normalMapUsed : 1; // do we use this section at all?
            gtaRwUInt32 notUsed : 3; // this field is not used
            gtaRwUInt32 envMapUsed : 1; // do we use env map
            gtaRwUInt32 envMapModulate : 1; // combine bump and env (bump * env)
        };
    };
    gtaRwMaterialTexture normalMapTexture; // normal map texture
    gtaRwReal envMapCoefficient; // env map intensity
    gtaRwMaterialTexture envMapTexture; // env map texture
};

gtaRwBool gtaRwMaterialNormalMapWrite(gtaRwMaterialNormalMap *nmObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialNormalMapRead(gtaRwMaterialNormalMap *nmObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialNormalMapSize(gtaRwMaterialNormalMap *nmObj);
void gtaRwMaterialNormalMapInit(gtaRwMaterialNormalMap *nmObj, gtaRwBool normalMapUsed, gtaRwBool envMapUsed, gtaRwReal envMapCoefficient, gtaRwBool envMapModulate);
void gtaRwMaterialNormalMapDestroy(gtaRwMaterialNormalMap *nmObj);
