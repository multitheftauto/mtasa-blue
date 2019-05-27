//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwMaterialUVAnim gtaRwMaterialUVAnim;

struct gtaRwMaterialUVAnim {
    gtaRwBool enabled;
    union {
        gtaRwUInt32 animSlotsMap;
        struct {
            gtaRwUInt32 animSlots1Used : 1;
            gtaRwUInt32 animSlots2Used : 1;
            gtaRwUInt32 animSlots3Used : 1;
            gtaRwUInt32 animSlots4Used : 1;
            gtaRwUInt32 animSlots5Used : 1;
            gtaRwUInt32 animSlots6Used : 1;
            gtaRwUInt32 animSlots7Used : 1;
            gtaRwUInt32 animSlots8Used : 1;
        };
    };
    gtaRwChar *animNames;
    gtaRwUInt32 numAnimNames;
    gtaRwUInt32 currentSlot;
};

void gtaRwMaterialUVAnimInit(gtaRwMaterialUVAnim *uvAnimObj, gtaRwUInt32 numSlots);
void gtaRwMaterialUVAnimSetupAnim(gtaRwMaterialUVAnim *uvAnimObj, gtaRwUInt32 slot, gtaRwChar *animName);
void gtaRwMaterialUVAnimDestroy(gtaRwMaterialUVAnim *uvAnimObj);
gtaRwBool gtaRwMaterialUVAnimWrite(gtaRwMaterialUVAnim *uvAnimObj, gtaRwStream *stream);
gtaRwBool gtaRwMaterialUVAnimRead(gtaRwMaterialUVAnim *uvAnimObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwMaterialUVAnimSize(gtaRwMaterialUVAnim *uvAnimObj);
