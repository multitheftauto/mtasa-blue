//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterialSpecMap gtaRwMaterialSpecMap;

struct gtaRwMaterialSpecMap {
    gtaRwBool enabled;
    gtaRwReal specularity;
    gtaRwChar textureName[24];
};

gtaRwBool gtaRwMaterialSpecMapWrite(gtaRwMaterialSpecMap *specMapObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialSpecMapRead(gtaRwMaterialSpecMap *specMapObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialSpecMapSize(gtaRwMaterialSpecMap *specMapObj);
void gtaRwMaterialSpecMapInit(gtaRwMaterialSpecMap *specMapObj, gtaRwReal specularity, gtaRwChar *textureName);
void gtaRwMaterialSpecMapDestroy(gtaRwMaterialSpecMap *specMapObj);
