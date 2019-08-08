//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwGeometryBreakableMaterialColor gtaRwGeometryBreakableMaterialColor;
typedef struct gtaRwGeometryBreakableFace gtaRwGeometryBreakableFace;
typedef struct gtaRwGeometryBreakable gtaRwGeometryBreakable;

struct gtaRwGeometryBreakableFace {
    gtaRwUInt16 a, b, c;
};

struct gtaRwGeometryBreakableMaterialColor {
    gtaRwReal red, green, blue;
};

struct gtaRwGeometryBreakable {
    gtaRwBool enabled;
    gtaRwGeometryBreakable *thisData;
    gtaRwBool posnRelativeToCollision;
    gtaRwUInt32 vertexCount;
    gtaRwV3d *vertices;
    gtaRwTexCoords *texCoors;
    gtaRwRGBA *colors;
    gtaRwUInt32 faceCount;
    gtaRwGeometryBreakableFace *faces;
    gtaRwUInt16 *faceMaterials;
    gtaRwUInt32 meshCount;
    gtaRwChar(*texNames)[32];
    gtaRwChar(*texMaskNames)[32];
    gtaRwGeometryBreakableMaterialColor *matColors;
};

void gtaRwGeometryBreakableInit(gtaRwGeometryBreakable *breakableObj, gtaRwUInt32 vertexCount, gtaRwUInt32 faceCount, gtaRwUInt32 meshCount, gtaRwBool posnRelativeToCollision);
gtaRwBool gtaRwGeometryBreakableDestroy(gtaRwGeometryBreakable *breakableObj);
gtaRwBool gtaRwGeometryBreakableWrite(gtaRwGeometryBreakable *breakableObj, gtaRwStream *stream);
gtaRwBool gtaRwGeometryBreakableRead(gtaRwGeometryBreakable *breakableObj, gtaRwStream *stream);
gtaRwUInt32 gtaRwGeometryBreakableSize(gtaRwGeometryBreakable *breakableObj);
