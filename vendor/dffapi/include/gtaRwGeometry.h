struct gtaRwGeometry
{
	union{
		gtaRwInt32 format;  // Compression flags and number of texture coord sets
		struct{
			gtaRwUInt16 tristrip : 1;   // This geometry's meshes can be rendered as strips.
			gtaRwUInt16 positions : 1;  // This geometry has positions
			gtaRwUInt16 textured : 1;   // This geometry has only one set of texture coordinates. Texture coordinates are specified on a per vertex basis
			gtaRwUInt16 prelit : 1;     // This geometry has pre-light colors
			gtaRwUInt16 normals : 1;    // This geometry has vertex normals
			gtaRwUInt16 light : 1;      // This geometry will be lit
			gtaRwUInt16 modulateMaterialColor : 1; // Modulate material color with vertex colors (pre-lit + lit)
			gtaRwUInt16 textured2 : 1;  // This geometry has at least 2 sets of texture coordinates.
			gtaRwUInt8 numTexCoordSets;
			gtaRwUInt8 native : 1;
			gtaRwUInt8 nativeInstance : 1;
		};
	};
	gtaRwInt32 numTriangles;
	gtaRwInt32 numVertices;
	gtaRwInt32 numMorphTargets;
	gtaRwSurfaceProperties geometryLighting; // only if dff version is lower than 3.4.0.0.1
	gtaRwRGBA *preLitLum;          // The pre-lighting values
	gtaRwTexCoords *texCoords[8];  // Texture coordinates
	gtaRwTriangle *triangles;      // The triangles
	gtaRwMorphTarget *morphTarget; // The Morph Target
	gtaRwMaterialList matList;
	struct{
		gtaRwGeometryBinMesh mesh;
		gtaRwGeometryNative native;
		gtaRwGeometrySkin skin;
		gtaRwGeometryBreakable breakable;
		gtaRwGeometryExtraVertColour extraColour;
		//gtaRwGeometry2dEffect effect2d;
	} Extension;

};

gtaRwUInt8  gtaRwGeometryGetTexCoordsCount(gtaRwGeometry* geometryObj);
void        gtaRwGeometryInitialise(gtaRwGeometry* geometryObj, gtaRwInt32 NumTriangles, gtaRwInt32 NumVertices, gtaRwInt32 NumMorphTargets, gtaRwInt32 Format,
                                    gtaRwUInt8 NumTexCoordsCustom, gtaRwReal CenterX, gtaRwReal CenterY, gtaRwReal CenterZ, gtaRwReal Radius);
void        gtaRwGeometryDestroy(gtaRwGeometry* geometryObj);
gtaRwBool   gtaRwGeometryStreamRead(gtaRwGeometry* geometryObj, gtaRwStream* stream, gtaRwUInt32 ClumpVersion);
gtaRwBool   gtaRwGeometryStreamWrite(gtaRwGeometry* geometryObj, gtaRwStream* stream);
gtaRwUInt32 gtaRwGeometryGetStreamActualSize(gtaRwGeometry* geometryObj);
gtaRwUInt32 gtaRwGeometryGetStreamSize(gtaRwGeometry* geometryObj);
gtaRwBool   gtaRwGeometryConvertFromOGlNative(gtaRwGeometry* geometryObj);
void        gtaRwGeometryRecalculateFaces(gtaRwGeometry* geometryObj);
void        gtaRwGeometryConvertFromNative(gtaRwGeometry* geometryObj, gtaPlatformId Platform);