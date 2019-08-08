//
// dffapi
// https://github.com/DK22Pac/dffapi
//

struct gtaRwMaterialList {
    gtaRwUInt32 materialReferencesCount;
    gtaRwInt32 *materialReferences;
    gtaRwMaterial *materials;
};

gtaRwBool gtaRwMaterialListRead(gtaRwMaterialList *matListObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialListWrite(gtaRwMaterialList *matListObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialListSize(gtaRwMaterialList *matListObj);
void gtaRwMaterialListInit(gtaRwMaterialList *matListObj, gtaRwUInt32 materialReferencesCount, gtaRwUInt32 materialCount);
void gtaRwMaterialListDestroy(gtaRwMaterialList *matListObj);
gtaRwUInt32 gtaRwMaterialListGetMaterialCount(gtaRwMaterialList *matListObj);
