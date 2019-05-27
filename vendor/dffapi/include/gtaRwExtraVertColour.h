//
// dffapi
// https://github.com/DK22Pac/dffapi
//

typedef struct gtaRwGeometryExtraVertColour gtaRwGeometryExtraVertColour;

struct gtaRwGeometryExtraVertColour {
    gtaRwBool enabled;
    gtaRwRGBA *nightColors;
};

void gtaRwGeometryExtraVertColourInit(gtaRwGeometryExtraVertColour *extraColObj, gtaRwUInt32 VertexCount);
void gtaRwGeometryExtraVertColourDestroy(gtaRwGeometryExtraVertColour *extraColObj);
gtaRwBool gtaRwGeometryExtraVertColourWrite(gtaRwGeometryExtraVertColour *extraColObj, gtaRwStream *stream, gtaRwUInt32 vertexCount);
gtaRwBool gtaRwGeometryExtraVertColourRead(gtaRwGeometryExtraVertColour *extraColObj, gtaRwStream *stream, gtaRwUInt32 vertexCount);
gtaRwUInt32 gtaRwGeometryExtraVertColourSize(gtaRwGeometryExtraVertColour *extraColObj, gtaRwUInt32 vertexCount);
