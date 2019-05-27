//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterialEnvMap gtaRwMaterialEnvMap;

struct gtaRwMaterialEnvMap {
    gtaRwBool enabled;
    gtaRwReal scaleX;
    gtaRwReal scaleY;
    gtaRwReal transSclX;
    gtaRwReal transSclY;
    gtaRwReal shininess;
    gtaRwInt32 texture; // always NULL
};

gtaRwBool gtaRwMaterialEnvMapWrite(gtaRwMaterialEnvMap *envMapObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialEnvMapRead(gtaRwMaterialEnvMap *envMapObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialEnvMapSize(gtaRwMaterialEnvMap *envMapObj);
void gtaRwMaterialEnvMapInit(gtaRwMaterialEnvMap *envMapObj, gtaRwReal scaleX, gtaRwReal scaleY, gtaRwReal transSclX, gtaRwReal transSclY, gtaRwReal shininess);
void gtaRwMaterialEnvMapDestroy(gtaRwMaterialEnvMap *envMapObj);
