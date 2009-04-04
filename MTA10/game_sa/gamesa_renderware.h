/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		game_sa/gamesa_renderware.h
*  PURPOSE:		RenderWare interface mappings to Grand Theft Auto: San Andreas
*  DEVELOPERS:	Cecill Etheredge <ijsf@gmx.net>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef __GAMESA_RENDERWARE
#define __GAMESA_RENDERWARE

#define WIN32_LEAN_AND_MEAN

#include <game/RenderWare.h>
#include <windows.h>
#include <stdio.h>

#ifndef __CGAMESA_COLMODELSA
	typedef void CColModelSAInterface;
#endif

/*****************************************************************************/
/** Renderware functions                                                    **/
/*****************************************************************************/

/* RenderWare macros */
#define RpGetFrame(__c)            ((RwFrame*)(((RwObject *)(__c))->parent))
#define RpSetFrame(__c,__f)        ((((RwObject *)(__c))->parent) = (void *)(__f))

/* RenderWare function defines */
typedef RpAtomic *				(__cdecl *RpAtomicCreate_t)						(void);
typedef RpAtomic *				(__cdecl *RpAtomicClone_t)						(RpAtomic * atomic);
typedef int					(__cdecl *RpAtomicDestroy_t)					(RpAtomic * atomic);
typedef RpAtomic *				(__cdecl *RpAtomicSetGeometry_t)				(RpAtomic * atomic, RpGeometry * geometry, unsigned int flags);
typedef RpAtomic *				(__cdecl *RpAtomicSetFrame_t)					(RpAtomic * atomic, RwFrame * frame);
typedef RpClump *				(__cdecl *RpClumpAddAtomic_t)					(RpClump * clump, RpAtomic * atomic);
typedef RpClump *				(__cdecl *RpClumpAddLight_t)					(RpClump * clump, RpLight * light);
typedef int					(__cdecl *RpClumpGetNumAtomics_t)				(RpClump * clump);
typedef RpClump *				(__cdecl *RpClumpRemoveAtomic_t)				(RpClump * clump, RpAtomic * atomic);
typedef RwFrame *				(__cdecl *RwFrameAddChild_t)					(RwFrame * parent, RwFrame * child);
typedef RwFrame *				(__cdecl *RwFrameRemoveChild_t)					(RwFrame * child);
typedef RwFrame *				(__cdecl *RwFrameForAllObjects_t)				(RwFrame * frame, void * callback, void * data);
typedef RwFrame *				(__cdecl *RwFrameTranslate_t)					(RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *				(__cdecl *RwFrameScale_t)						(RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *				(__cdecl *RwFrameCreate_t)						(void);
typedef	RwFrame *				(__cdecl *RwFrameSetIdentity_t)					(RwFrame * frame);
typedef RpGeometry *			(__cdecl *RpGeometryCreate_t)					(int numverts, int numtriangles, unsigned int format);
typedef const RpGeometry *		(__cdecl *RpGeometryTriangleSetVertexIndices_t)	(const RpGeometry * geo, RpTriangle * tri, unsigned short v1, unsigned short v2, unsigned short v3);
typedef RpGeometry *			(__cdecl *RpGeometryUnlock_t)					(RpGeometry * geo);
typedef RpGeometry *			(__cdecl *RpGeometryLock_t)						(RpGeometry * geo, int lockmode);
typedef RpGeometry *			(__cdecl *RpGeometryTransform_t)				(RpGeometry * geo, const RwMatrix * matrix);
typedef RpGeometry *			(__cdecl *RpGeometryTriangleSetMaterial_t)		(RpGeometry * geo, RpTriangle * tri, RpMaterial * mat);
typedef int					(__cdecl *RpGeometryDestroy_t)					(RpGeometry * geo);
typedef void *					(__cdecl *RwIm3DTransform_t)					(RwVertex *pVerts, unsigned int numVerts, RwMatrix *ltm, unsigned int flags);
typedef int					(__cdecl *RwIm3DRenderIndexedPrimitive_t)		(RwPrimitiveType primType, unsigned short *indices, int numIndices);
typedef int					(__cdecl *RwIm3DEnd_t)							(void);
typedef RpLight *				(__cdecl *RpLightCreate_t)						(int type);
typedef RpLight *				(__cdecl *RpLightSetRadius_t)					(RpLight * light, float radius);
typedef RpLight *				(__cdecl *RpLightSetColor_t)					(RpLight * light, const RwColorFloat * color);
typedef RwMatrix *				(__cdecl *RwMatrixCreate_t)						(void);
typedef RwMatrix *				(__cdecl *RwMatrixInvert_t)						(RwMatrix *dst, const RwMatrix *src);
typedef RwMatrix *				(__cdecl *RwMatrixTranslate_t)					(RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RwMatrix *				(__cdecl *RwMatrixScale_t)						(RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RpMaterial *			(__cdecl *RpMaterialCreate_t)					(void);
typedef int					(__cdecl *RpMaterialDestroy_t)					(RpMaterial * mat);
typedef RwTexDictionary *		(__cdecl *RwTexDictionarySetCurrent_t)			(RwTexDictionary * dict);
typedef const RwTexDictionary * (__cdecl *RwTexDictionaryForAllTextures_t)		(const RwTexDictionary * dict, void * callback, void * data);
typedef RwTexture *				(__cdecl *RwTexDictionaryAddTexture_t)			(RwTexDictionary * dict, RwTexture * texture);
typedef RwTexDictionary *		(__cdecl *RwTexDictionaryGetCurrent_t)			(void);
typedef RwTexture *				(__cdecl *RwTexDictionaryFindNamedTexture_t)	(RwTexDictionary * dict, const char* name);
typedef void					(__cdecl *RpPrtStdGlobalDataSetStreamEmbedded_t)(void * value);
typedef RpWorld *				(__cdecl *RpWorldAddAtomic_t)					(RpWorld * world, RpAtomic * atomic);
typedef RpWorld *				(__cdecl *RpWorldAddClump_t)					(RpWorld * world, RpClump * clump);
typedef RpWorld *				(__cdecl *RpWorldAddLight_t)					(RpWorld * world, RpLight * light);
typedef float					(__cdecl *RwV3dNormalize_t)						(RwV3d * out, const RwV3d * in);
typedef RwV3d *					(__cdecl *RwV3dTransformVector_t)				(RwV3d * out, const RwV3d * in, const RwMatrix * matrix);
typedef void					(__cdecl *_rwObjectHasFrameSetFrame_t)			(void *object, RwFrame *frame);
typedef RwCamera *				(__cdecl *RwCameraClone_t)						(RwCamera *camera);
typedef RpClump *				(__cdecl *RpClumpClone_t)						(RpClump *clone);
typedef int					(__cdecl *RwTexDictionaryDestroy_t)				(RwTexDictionary *txd);
typedef int					(__cdecl *RwTextureDestroy_t)					(RwTexture *texture);
typedef int					(__cdecl *RwStreamFindChunk_t)					(RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);
typedef RpClump*				(__cdecl *RpClumpStreamRead_t)					(RwStream *stream);
typedef RwError*				(__cdecl *RwErrorGet_t)							(RwError *code);
typedef RwStream*				(__cdecl *RwStreamOpen_t)						(RwStreamType type, RwStreamMode mode, const void *pData);
typedef int					(__cdecl *RwStreamClose_t)						(RwStream *stream, void *pData);
typedef int					(__cdecl *RpClumpDestroy_t)						(RpClump *clump);
typedef RpClump*				(__cdecl *RpClumpForAllAtomics_t)				(RpClump *clump, void* callback, void* pData);
typedef RwTexDictionary*		(__cdecl *RwTexDictionaryStreamRead_t)			(RwStream *stream);
typedef RwRaster*				(__cdecl *RwRasterUnlock_t)						(RwRaster *raster);
typedef RwRaster*				(__cdecl *RwRasterLock_t)						(RwRaster *raster, unsigned char level, int lockmode);
typedef RwRaster*				(__cdecl *RwRasterCreate_t)						(int width, int height, int depth, int flags);
typedef RwTexture*				(__cdecl *RwTextureCreate_t)					(RwRaster *raster);

/*****************************************************************************/
/** Renderware function mappings                                            **/
/*****************************************************************************/

// US Versions
RwStreamFindChunk_t						RwStreamFindChunk						= (RwStreamFindChunk_t)						0xDEAD;
RpClumpStreamRead_t						RpClumpStreamRead						= (RpClumpStreamRead_t)						0xDEAD;
RwErrorGet_t							RwErrorGet								= (RwErrorGet_t)							0xDEAD;
RwStreamOpen_t							RwStreamOpen							= (RwStreamOpen_t)							0xDEAD;
RwStreamClose_t							RwStreamClose							= (RwStreamClose_t)							0xDEAD;
RpClumpDestroy_t						RpClumpDestroy							= (RpClumpDestroy_t)						0xDEAD;
RpClumpGetNumAtomics_t					RpClumpGetNumAtomics					= (RpClumpGetNumAtomics_t)					0xDEAD;
RwFrameTranslate_t						RwFrameTranslate						= (RwFrameTranslate_t)						0xDEAD;
RpClumpForAllAtomics_t					RpClumpForAllAtomics					= (RpClumpForAllAtomics_t)					0xDEAD;
RwFrameAddChild_t						RwFrameAddChild							= (RwFrameAddChild_t)						0xDEAD;
RpClumpAddAtomic_t						RpClumpAddAtomic						= (RpClumpAddAtomic_t)						0xDEAD;
RpAtomicSetFrame_t						RpAtomicSetFrame						= (RpAtomicSetFrame_t)						0xDEAD;
RwTexDictionaryStreamRead_t				RwTexDictionaryStreamRead				= (RwTexDictionaryStreamRead_t)				0xDEAD;
RwTexDictionaryGetCurrent_t				RwTexDictionaryGetCurrent				= (RwTexDictionaryGetCurrent_t)				0xDEAD;
RwTexDictionarySetCurrent_t				RwTexDictionarySetCurrent				= (RwTexDictionarySetCurrent_t)				0xDEAD;
RwTexDictionaryForAllTextures_t			RwTexDictionaryForAllTextures			= (RwTexDictionaryForAllTextures_t)			0xDEAD;
RwTexDictionaryAddTexture_t				RwTexDictionaryAddTexture				= (RwTexDictionaryAddTexture_t)				0xDEAD;
RpPrtStdGlobalDataSetStreamEmbedded_t	RpPrtStdGlobalDataSetStreamEmbedded		= (RpPrtStdGlobalDataSetStreamEmbedded_t)	0xDEAD;
RpClumpRemoveAtomic_t					RpClumpRemoveAtomic						= (RpClumpRemoveAtomic_t)					0xDEAD;
RpAtomicClone_t							RpAtomicClone							= (RpAtomicClone_t)							0xDEAD;
RwTexDictionaryFindNamedTexture_t		RwTexDictionaryFindNamedTexture			= (RwTexDictionaryFindNamedTexture_t)		0xDEAD;
RwFrameRemoveChild_t					RwFrameRemoveChild						= (RwFrameRemoveChild_t)					0xDEAD;
RwFrameForAllObjects_t					RwFrameForAllObjects					= (RwFrameForAllObjects_t)					0xDEAD;
RpAtomicDestroy_t						RpAtomicDestroy							= (RpAtomicDestroy_t)						0xDEAD;
RpAtomicSetGeometry_t					RpAtomicSetGeometry						= (RpAtomicSetGeometry_t)					0xDEAD;
RpWorldAddAtomic_t						RpWorldAddAtomic						= (RpWorldAddAtomic_t)						0xDEAD;
RpGeometryCreate_t						RpGeometryCreate						= (RpGeometryCreate_t)						0xDEAD;
RpGeometryTriangleSetVertexIndices_t	RpGeometryTriangleSetVertexIndices		= (RpGeometryTriangleSetVertexIndices_t)	0xDEAD;
RpGeometryUnlock_t						RpGeometryUnlock						= (RpGeometryUnlock_t)						0xDEAD;
RpGeometryLock_t						RpGeometryLock							= (RpGeometryLock_t)						0xDEAD;
RpAtomicCreate_t						RpAtomicCreate							= (RpAtomicCreate_t)						0xDEAD;
RwFrameCreate_t							RwFrameCreate							= (RwFrameCreate_t)							0xDEAD;
RpGeometryTransform_t					RpGeometryTransform						= (RpGeometryTransform_t)					0xDEAD;
RwFrameSetIdentity_t					RwFrameSetIdentity						= (RwFrameSetIdentity_t)					0xDEAD;
RwMatrixCreate_t						RwMatrixCreate							= (RwMatrixCreate_t)						0xDEAD;
RwMatrixTranslate_t						RwMatrixTranslate						= (RwMatrixTranslate_t)						0xDEAD;
RwMatrixScale_t							RwMatrixScale							= (RwMatrixScale_t)							0xDEAD;
RpGeometryTriangleSetMaterial_t			RpGeometryTriangleSetMaterial			= (RpGeometryTriangleSetMaterial_t)			0xDEAD;
RpMaterialCreate_t						RpMaterialCreate						= (RpMaterialCreate_t)						0xDEAD;
RpGeometryDestroy_t						RpGeometryDestroy						= (RpGeometryDestroy_t)						0xDEAD;
RpMaterialDestroy_t						RpMaterialDestroy						= (RpMaterialDestroy_t)						0xDEAD;
RwV3dNormalize_t						RwV3dNormalize							= (RwV3dNormalize_t)						0xDEAD;
RwIm3DTransform_t						RwIm3DTransform							= (RwIm3DTransform_t)						0xDEAD;
RwIm3DRenderIndexedPrimitive_t			RwIm3DRenderIndexedPrimitive			= (RwIm3DRenderIndexedPrimitive_t)			0xDEAD;
RwIm3DEnd_t								RwIm3DEnd								= (RwIm3DEnd_t)								0xDEAD;
RwMatrixInvert_t						RwMatrixInvert							= (RwMatrixInvert_t)						0xDEAD;
RpWorldAddClump_t						RpWorldAddClump							= (RpWorldAddClump_t)						0xDEAD;
RwFrameScale_t							RwFrameScale							= (RwFrameScale_t)							0xDEAD;
RwV3dTransformVector_t					RwV3dTransformVector					= (RwV3dTransformVector_t)					0xDEAD;
RpLightCreate_t							RpLightCreate							= (RpLightCreate_t)							0xDEAD;
RpClumpAddLight_t						RpClumpAddLight							= (RpClumpAddLight_t)						0xDEAD;
_rwObjectHasFrameSetFrame_t				_rwObjectHasFrameSetFrame				= (_rwObjectHasFrameSetFrame_t)				0xDEAD;
RpLightSetRadius_t						RpLightSetRadius						= (RpLightSetRadius_t)						0xDEAD;
RpWorldAddLight_t						RpWorldAddLight							= (RpWorldAddLight_t)						0xDEAD;
RpLightSetColor_t						RpLightSetColor							= (RpLightSetColor_t)						0xDEAD;
RwCameraClone_t							RwCameraClone							= (RwCameraClone_t)							0xDEAD;
RpClumpClone_t							RpClumpClone							= (RpClumpClone_t)							0xDEAD;
RwTexDictionaryDestroy_t				RwTexDictionaryDestroy					= (RwTexDictionaryDestroy_t)				0xDEAD;
RwTextureDestroy_t						RwTextureDestroy						= (RwTextureDestroy_t)						0xDEAD;
RwRasterUnlock_t						RwRasterUnlock							= (RwRasterUnlock_t)						0xDEAD;
RwRasterLock_t							RwRasterLock							= (RwRasterLock_t)							0xDEAD;
RwRasterCreate_t						RwRasterCreate							= (RwRasterCreate_t)						0xDEAD;
RwTextureCreate_t						RwTextureCreate							= (RwTextureCreate_t)						0xDEAD;

/*****************************************************************************/
/** GTA function definitions and mappings                                   **/
/*****************************************************************************/

typedef bool				(__cdecl *SetTextureDict_t)					(unsigned short id);
typedef bool				(__cdecl *LoadClumpFile_t)					(RwStream *stream, unsigned int id);		// (stream, model id)
typedef bool				(__cdecl *LoadModel_t)						(RwBuffer *filename, unsigned int id);		// (memory chunk, model id)
typedef void				(__cdecl *LoadCollisionModelVer3_t)			(unsigned char*, unsigned int, CColModelSAInterface*, const char*);	// buf, bufsize, ccolmodel&, keyname
typedef bool				(__cdecl *CTxdStore_LoadTxd_t)				(unsigned int id, RwStream *filename);
typedef void				(__cdecl *CTxdStore_RemoveTxd_t)			(unsigned int id);
typedef void				(__cdecl *CTxdStore_RemoveRef_t)			(unsigned int id);
typedef void				(__cdecl *CTxdStore_AddRef_t)				(unsigned int id);
typedef RwTexDictionary*	(__cdecl *CTxdStore_GetTxd_t)				(unsigned int id);
typedef RwTexture*			(__cdecl *CClothesBuilder_CopyTexture_t)	(RwTexture *texture);

SetTextureDict_t				SetTextureDict					= (SetTextureDict_t)				0xDEAD;
LoadClumpFile_t					LoadClumpFile					= (LoadClumpFile_t)					0xDEAD;
LoadModel_t						LoadModel						= (LoadModel_t)						0xDEAD;
LoadCollisionModelVer3_t		LoadCollisionModelVer3			= (LoadCollisionModelVer3_t)		0xDEAD;
CTxdStore_LoadTxd_t				CTxdStore_LoadTxd				= (CTxdStore_LoadTxd_t)				0xDEAD;
CTxdStore_GetTxd_t				CTxdStore_GetTxd				= (CTxdStore_GetTxd_t)				0xDEAD;
CTxdStore_RemoveTxd_t			CTxdStore_RemoveTxd				= (CTxdStore_RemoveTxd_t)			0xDEAD;
CTxdStore_RemoveRef_t			CTxdStore_RemoveRef				= (CTxdStore_RemoveRef_t)			0xDEAD;
CTxdStore_AddRef_t				CTxdStore_AddRef				= (CTxdStore_AddRef_t)				0xDEAD;
CClothesBuilder_CopyTexture_t	CClothesBuilder_CopyTexture		= (CClothesBuilder_CopyTexture_t)	0xDEAD;

#define			COL3_HEADER_SIZE	32

/*****************************************************************************/
/** Function inlines                                                        **/
/*****************************************************************************/

// Matrix copying
void RwFrameCopyMatrix ( RwFrame * dst, RwFrame * src ) {
	if ( dst == NULL || src == NULL ) return;
	memcpy(&dst->modelling,&src->modelling,sizeof(RwMatrix));
	memcpy(&dst->ltm,&src->ltm,sizeof(RwMatrix));
}

// Recursive RwFrame children searching function
RwFrame * RwFrameFindFrame ( RwFrame * parent, const char * name ) {
	RwFrame * ret = parent->child, * buf;
	while ( ret != NULL ) {
		// recurse into the child
		if ( ret->child != NULL ) {			
			buf = RwFrameFindFrame ( ret, name );
			if ( buf != NULL ) return buf;
		}

		// search through the children frames
		if ( strncmp ( &ret->szName[0], name, 16 ) == 0 ) {
			// found it
			return ret;
		}
		ret = ret->next;
	}
	return NULL;
}

#endif