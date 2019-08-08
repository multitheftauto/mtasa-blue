struct gtaRwGeometryList
{
	gtaRwUInt32 geometryCount;
	gtaRwGeometry *geometries;
};

void gtaRwGeometryListInitialise(gtaRwGeometryList* geometryList, gtaRwInt32 GeometryCount);
void gtaRwGeometryListDestroy(gtaRwGeometryList* geometryList);
gtaRwBool gtaRwGeometryListStreamRead(gtaRwGeometryList* geometryList, gtaRwStream* stream, gtaRwUInt32 ClumpVersion);
gtaRwBool gtaRwGeometryListStreamWrite(gtaRwGeometryList* geometryList, gtaRwStream* stream);
gtaRwUInt32 gtaRwGeometryListGetStreamSize(gtaRwGeometryList* geometryList);