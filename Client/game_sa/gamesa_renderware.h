/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/gamesa_renderware.h
 *  PURPOSE:     RenderWare interface mappings to Grand Theft Auto: San Andreas
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *  RenderWare is © Criterion Software
 *
 *****************************************************************************/

#pragma once

#include <game/RenderWare.h>

struct CColModelSAInterface;

/*****************************************************************************/
/** Renderware functions                                                    **/
/*****************************************************************************/

/* RenderWare macros */
#define RpGetFrame(__c)            ((RwFrame*)(((RwObject *)(__c))->parent))
#define RpSetFrame(__c,__f)        ((((RwObject *)(__c))->parent) = (void *)(__f))

/* RenderWare function defines */
typedef RpAtomic*(__cdecl* RpAtomicCreate_t)();
typedef RpAtomic*(__cdecl* RpAtomicClone_t)(RpAtomic* atomic);
typedef int(__cdecl* RpAtomicDestroy_t)(RpAtomic* atomic);
typedef RpAtomic*(__cdecl* RpAtomicSetGeometry_t)(RpAtomic* atomic, RpGeometry* geometry, unsigned int flags);
typedef RpAtomic*(__cdecl* RpAtomicSetFrame_t)(RpAtomic* atomic, RwFrame* frame);
typedef RpClump*(__cdecl* RpClumpAddAtomic_t)(RpClump* clump, RpAtomic* atomic);
typedef RpClump*(__cdecl* RpClumpAddLight_t)(RpClump* clump, RpLight* light);
typedef int(__cdecl* RpClumpGetNumAtomics_t)(RpClump* clump);
typedef RpClump*(__cdecl* RpClumpRemoveAtomic_t)(RpClump* clump, RpAtomic* atomic);
typedef RwFrame*(__cdecl* RwFrameAddChild_t)(RwFrame* parent, RwFrame* child);
typedef RwFrame*(__cdecl* RwFrameRemoveChild_t)(RwFrame* child);
typedef RwFrame*(__cdecl* RwFrameForAllObjects_t)(RwFrame* frame, void* callback, void* data);
typedef RwFrame*(__cdecl* RwFrameTranslate_t)(RwFrame* frame, const RwV3d* v, RwTransformOrder order);
typedef RwFrame*(__cdecl* RwFrameTransform_t)(RwFrame* frame, const RwMatrix* m, RwOpCombineType combine);
typedef RwFrame*(__cdecl* RwFrameScale_t)(RwFrame* frame, const RwV3d* v, RwTransformOrder order);
typedef RwFrame*(__cdecl* RwFrameUpdateObjects_t)(RwFrame*);
typedef RwFrame*(__cdecl* RwFrameCreate_t)();
typedef int(__cdecl* RwFrameDestroy_t)(RwFrame* frame);
typedef RwFrame*(__cdecl* RwFrameSetIdentity_t)(RwFrame* frame);
typedef RpGeometry*(__cdecl* RpGeometryCreate_t)(int numverts, int numtriangles, unsigned int format);
typedef const RpGeometry*(__cdecl* RpGeometryTriangleSetVertexIndices_t)(const RpGeometry* geo, RpTriangle* tri, unsigned short v1, unsigned short v2,
                                                                         unsigned short v3);
typedef RpGeometry*(__cdecl* RpGeometryUnlock_t)(RpGeometry* geo);
typedef RpGeometry*(__cdecl* RpGeometryLock_t)(RpGeometry* geo, int lockmode);
typedef RpGeometry*(__cdecl* RpGeometryTransform_t)(RpGeometry* geo, const RwMatrix* matrix);
typedef RpGeometry*(__cdecl* RpGeometryTriangleSetMaterial_t)(RpGeometry* geo, RpTriangle* tri, RpMaterial* mat);
typedef int(__cdecl* RpGeometryDestroy_t)(RpGeometry* geo);
typedef void*(__cdecl* RwIm3DTransform_t)(RwVertex* pVerts, unsigned int numVerts, RwMatrix* ltm, unsigned int flags);
typedef int(__cdecl* RwIm3DRenderIndexedPrimitive_t)(RwPrimitiveType primType, unsigned short* indices, int numIndices);
typedef int(__cdecl* RwIm3DEnd_t)();
typedef RpLight*(__cdecl* RpLightCreate_t)(int type);
typedef RpLight*(__cdecl* RpLightSetRadius_t)(RpLight* light, float radius);
typedef RpLight*(__cdecl* RpLightSetColor_t)(RpLight* light, const RwColorFloat* color);
typedef RwMatrix*(__cdecl* RwMatrixCreate_t)();
typedef RwMatrix*(__cdecl* RwMatrixInvert_t)(RwMatrix* dst, const RwMatrix* src);
typedef RwMatrix*(__cdecl* RwMatrixTranslate_t)(RwMatrix* matrix, const RwV3d* translation, RwTransformOrder order);
typedef RwMatrix*(__cdecl* RwMatrixScale_t)(RwMatrix* matrix, const RwV3d* translation, RwTransformOrder order);
typedef RpMaterial*(__cdecl* RpMaterialCreate_t)();
typedef int(__cdecl* RpMaterialDestroy_t)(RpMaterial* mat);
typedef RwTexDictionary*(__cdecl* RwTexDictionarySetCurrent_t)(RwTexDictionary* dict);
typedef const RwTexDictionary*(__cdecl* RwTexDictionaryForAllTextures_t)(const RwTexDictionary* dict, void* callback, void* data);
typedef RwTexture*(__cdecl* RwTexDictionaryAddTexture_t)(RwTexDictionary* dict, RwTexture* texture);
typedef RwTexDictionary*(__cdecl* RwTexDictionaryGetCurrent_t)();
typedef RwTexture*(__cdecl* RwTexDictionaryFindNamedTexture_t)(RwTexDictionary* dict, const char* name);
typedef void(__cdecl* RpPrtStdGlobalDataSetStreamEmbedded_t)(void* value);
typedef RpClump*(__cdecl* RpClumpRender_t)(RpClump* clump);
typedef RpWorld*(__cdecl* RpWorldAddAtomic_t)(RpWorld* world, RpAtomic* atomic);
typedef RpWorld*(__cdecl* RpWorldAddClump_t)(RpWorld* world, RpClump* clump);
typedef RpWorld*(__cdecl* RpWorldAddLight_t)(RpWorld* world, RpLight* light);
typedef float(__cdecl* RwV3dNormalize_t)(RwV3d* out, const RwV3d* in);
typedef RwV3d*(__cdecl* RwV3dTransformVector_t)(RwV3d* out, const RwV3d* in, const RwMatrix* matrix);
typedef void(__cdecl* _rwObjectHasFrameSetFrame_t)(void* object, RwFrame* frame);
typedef RwCamera*(__cdecl* RwCameraClone_t)(RwCamera* camera);
typedef RpClump*(__cdecl* RpClumpClone_t)(RpClump* clone);
typedef int(__cdecl* RwTexDictionaryDestroy_t)(RwTexDictionary* txd);
typedef int(__cdecl* RwTextureDestroy_t)(RwTexture* texture);
typedef int(__cdecl* RwStreamFindChunk_t)(RwStream* stream, unsigned int type, unsigned int* lengthOut, unsigned int* versionOut);
typedef RpClump*(__cdecl* RpClumpStreamRead_t)(RwStream* stream);
typedef RwError*(__cdecl* RwErrorGet_t)(RwError* code);
typedef RwStream*(__cdecl* RwStreamOpen_t)(RwStreamType type, RwStreamMode mode, const void* pData);
typedef int(__cdecl* RwStreamClose_t)(RwStream* stream, void* pData);
typedef int(__cdecl* RwStreamRead_t)(RwStream* stream, void* pData, uint size);
typedef int(__cdecl* RwStreamSkip_t)(RwStream* stream, uint size);
typedef int(__cdecl* RpClumpDestroy_t)(RpClump* clump);
using RpClumpForAllAtomicsCB_t = bool(__cdecl*)(RpAtomic*, void*);
typedef RpClump*(__cdecl* RpClumpForAllAtomics_t)(RpClump* clump, RpClumpForAllAtomicsCB_t callback, void* pData);
typedef RwTexDictionary*(__cdecl* RwTexDictionaryStreamRead_t)(RwStream* stream);
typedef RwTexDictionary*(__cdecl* RwTexDictionaryGtaStreamRead_t)(RwStream* stream);
typedef int(__cdecl* RwTexDictionaryStreamWrite_t)(RwTexDictionary* txd, RwStream* stream);
typedef int(__cdecl* rwD3D9NativeTextureRead_t)(RwStream* stream, RwTexture** textureOut);
typedef RwRaster*(__cdecl* RwRasterUnlock_t)(RwRaster* raster);
typedef RwRaster*(__cdecl* RwRasterLock_t)(RwRaster* raster, unsigned char level, int lockmode);
typedef RwRaster*(__cdecl* RwRasterCreate_t)(int width, int height, int depth, int flags);
typedef RwTexture*(__cdecl* RwTextureCreate_t)(RwRaster* raster);
typedef RpMaterial*(__cdecl* RpMaterialSetTexture_t)(RpMaterial* mat, RwTexture* tex);
typedef RpHAnimHierarchy*(__cdecl* GetAnimHierarchyFromClump_t)(RpClump*);
typedef RpHAnimHierarchy*(__cdecl* GetAnimHierarchyFromSkinClump_t)(RpClump*);
typedef int(__cdecl* RpHAnimIDGetIndex_t)(RpHAnimHierarchy*, int);
typedef RwMatrix*(__cdecl* RpHAnimHierarchyGetMatrixArray_t)(RpHAnimHierarchy*);
typedef RtQuat*(__cdecl* RtQuatRotate_t)(RtQuat* quat, const RwV3d* axis, float angle, RwOpCombineType combineOp);

/*****************************************************************************/
/** Renderware function mappings                                            **/
/*****************************************************************************/

#ifdef RWFUNC_IMPLEMENT
    #define RWFUNC(a,b) a = b;
#else
    #define RWFUNC(a,b) extern a;
#endif

// US Versions
RWFUNC(RwStreamFindChunk_t RwStreamFindChunk, (RwStreamFindChunk_t)0xDEAD)
RWFUNC(RpClumpStreamRead_t RpClumpStreamRead, (RpClumpStreamRead_t)0xDEAD)
RWFUNC(RwErrorGet_t RwErrorGet, (RwErrorGet_t)0xDEAD)
RWFUNC(RwStreamOpen_t RwStreamOpen, (RwStreamOpen_t)0xDEAD)
RWFUNC(RwStreamClose_t RwStreamClose, (RwStreamClose_t)0xDEAD)
RWFUNC(RwStreamRead_t RwStreamRead, (RwStreamRead_t)0xDEAD)
RWFUNC(RwStreamSkip_t RwStreamSkip, (RwStreamSkip_t)0xDEAD)
RWFUNC(RpClumpDestroy_t RpClumpDestroy, (RpClumpDestroy_t)0xDEAD)
RWFUNC(RpClumpGetNumAtomics_t RpClumpGetNumAtomics, (RpClumpGetNumAtomics_t)0xDEAD)
RWFUNC(RwFrameTranslate_t RwFrameTranslate, (RwFrameTranslate_t)0xDEAD)
RWFUNC(RwFrameTransform_t RwFrameTransform, (RwFrameTransform_t)0xDEAD)
RWFUNC(RpClumpForAllAtomics_t RpClumpForAllAtomics, (RpClumpForAllAtomics_t)0xDEAD)
RWFUNC(RwFrameAddChild_t RwFrameAddChild, (RwFrameAddChild_t)0xDEAD)
RWFUNC(RpClumpAddAtomic_t RpClumpAddAtomic, (RpClumpAddAtomic_t)0xDEAD)
RWFUNC(RpAtomicSetFrame_t RpAtomicSetFrame, (RpAtomicSetFrame_t)0xDEAD)
RWFUNC(RwTexDictionaryStreamRead_t RwTexDictionaryStreamRead, (RwTexDictionaryStreamRead_t)0xDEAD)
RWFUNC(RwTexDictionaryGtaStreamRead_t RwTexDictionaryGtaStreamRead, (RwTexDictionaryGtaStreamRead_t)0xDEAD)
RWFUNC(RwTexDictionaryGetCurrent_t RwTexDictionaryGetCurrent, (RwTexDictionaryGetCurrent_t)0xDEAD)
RWFUNC(RwTexDictionarySetCurrent_t RwTexDictionarySetCurrent, (RwTexDictionarySetCurrent_t)0xDEAD)
RWFUNC(RwTexDictionaryForAllTextures_t RwTexDictionaryForAllTextures, (RwTexDictionaryForAllTextures_t)0xDEAD)
RWFUNC(RwTexDictionaryAddTexture_t RwTexDictionaryAddTexture, (RwTexDictionaryAddTexture_t)0xDEAD)
RWFUNC(RwTexDictionaryStreamWrite_t RwTexDictionaryStreamWrite, (RwTexDictionaryStreamWrite_t)0xDEAD)
RWFUNC(rwD3D9NativeTextureRead_t rwD3D9NativeTextureRead, (rwD3D9NativeTextureRead_t)0xDEAD)
RWFUNC(RpPrtStdGlobalDataSetStreamEmbedded_t RpPrtStdGlobalDataSetStreamEmbedded, (RpPrtStdGlobalDataSetStreamEmbedded_t)0xDEAD)
RWFUNC(RpClumpRender_t RpClumpRender, (RpClumpRender_t)0xDEAD)
RWFUNC(RpClumpRemoveAtomic_t RpClumpRemoveAtomic, (RpClumpRemoveAtomic_t)0xDEAD)
RWFUNC(RpAtomicClone_t RpAtomicClone, (RpAtomicClone_t)0xDEAD)
RWFUNC(RwTexDictionaryFindNamedTexture_t RwTexDictionaryFindNamedTexture, (RwTexDictionaryFindNamedTexture_t)0xDEAD)
RWFUNC(RwFrameRemoveChild_t RwFrameRemoveChild, (RwFrameRemoveChild_t)0xDEAD)
RWFUNC(RwFrameForAllObjects_t RwFrameForAllObjects, (RwFrameForAllObjects_t)0xDEAD)
RWFUNC(RpAtomicDestroy_t RpAtomicDestroy, (RpAtomicDestroy_t)0xDEAD)
RWFUNC(RpAtomicSetGeometry_t RpAtomicSetGeometry, (RpAtomicSetGeometry_t)0xDEAD)
RWFUNC(RpWorldAddAtomic_t RpWorldAddAtomic, (RpWorldAddAtomic_t)0xDEAD)
RWFUNC(RpGeometryCreate_t RpGeometryCreate, (RpGeometryCreate_t)0xDEAD)
RWFUNC(RpGeometryTriangleSetVertexIndices_t RpGeometryTriangleSetVertexIndices, (RpGeometryTriangleSetVertexIndices_t)0xDEAD)
RWFUNC(RpGeometryUnlock_t RpGeometryUnlock, (RpGeometryUnlock_t)0xDEAD)
RWFUNC(RpGeometryLock_t RpGeometryLock, (RpGeometryLock_t)0xDEAD)
RWFUNC(RpAtomicCreate_t RpAtomicCreate, (RpAtomicCreate_t)0xDEAD)
RWFUNC(RwFrameCreate_t RwFrameCreate, (RwFrameCreate_t)0xDEAD)
RWFUNC(RwFrameDestroy_t RwFrameDestroy, (RwFrameDestroy_t)0xDEAD)
RWFUNC(RpGeometryTransform_t RpGeometryTransform, (RpGeometryTransform_t)0xDEAD)
RWFUNC(RwFrameSetIdentity_t RwFrameSetIdentity, (RwFrameSetIdentity_t)0xDEAD)
RWFUNC(RwMatrixCreate_t RwMatrixCreate, (RwMatrixCreate_t)0xDEAD)
RWFUNC(RwMatrixTranslate_t RwMatrixTranslate, (RwMatrixTranslate_t)0xDEAD)
RWFUNC(RwMatrixScale_t RwMatrixScale, (RwMatrixScale_t)0xDEAD)
RWFUNC(RpGeometryTriangleSetMaterial_t RpGeometryTriangleSetMaterial, (RpGeometryTriangleSetMaterial_t)0xDEAD)
RWFUNC(RpMaterialCreate_t RpMaterialCreate, (RpMaterialCreate_t)0xDEAD)
RWFUNC(RpGeometryDestroy_t RpGeometryDestroy, (RpGeometryDestroy_t)0xDEAD)
RWFUNC(RpMaterialDestroy_t RpMaterialDestroy, (RpMaterialDestroy_t)0xDEAD)
RWFUNC(RwV3dNormalize_t RwV3dNormalize, (RwV3dNormalize_t)0xDEAD)
RWFUNC(RwIm3DTransform_t RwIm3DTransform, (RwIm3DTransform_t)0xDEAD)
RWFUNC(RwIm3DRenderIndexedPrimitive_t RwIm3DRenderIndexedPrimitive, (RwIm3DRenderIndexedPrimitive_t)0xDEAD)
RWFUNC(RwIm3DEnd_t RwIm3DEnd, (RwIm3DEnd_t)0xDEAD)
RWFUNC(RwMatrixInvert_t RwMatrixInvert, (RwMatrixInvert_t)0xDEAD)
RWFUNC(RpWorldAddClump_t RpWorldAddClump, (RpWorldAddClump_t)0xDEAD)
RWFUNC(RwFrameScale_t RwFrameScale, (RwFrameScale_t)0xDEAD)
RWFUNC(RwFrameUpdateObjects_t RwFrameUpdateObjects, (RwFrameUpdateObjects_t)0xDAED)
RWFUNC(RwV3dTransformVector_t RwV3dTransformVector, (RwV3dTransformVector_t)0xDEAD)
RWFUNC(RpLightCreate_t RpLightCreate, (RpLightCreate_t)0xDEAD)
RWFUNC(RpClumpAddLight_t RpClumpAddLight, (RpClumpAddLight_t)0xDEAD)
RWFUNC(_rwObjectHasFrameSetFrame_t _rwObjectHasFrameSetFrame, (_rwObjectHasFrameSetFrame_t)0xDEAD)
RWFUNC(RpLightSetRadius_t RpLightSetRadius, (RpLightSetRadius_t)0xDEAD)
RWFUNC(RpWorldAddLight_t RpWorldAddLight, (RpWorldAddLight_t)0xDEAD)
RWFUNC(RpLightSetColor_t RpLightSetColor, (RpLightSetColor_t)0xDEAD)
RWFUNC(RwCameraClone_t RwCameraClone, (RwCameraClone_t)0xDEAD)
RWFUNC(RpClumpClone_t RpClumpClone, (RpClumpClone_t)0xDEAD)
RWFUNC(RwTexDictionaryDestroy_t RwTexDictionaryDestroy, (RwTexDictionaryDestroy_t)0xDEAD)
RWFUNC(RwTextureDestroy_t RwTextureDestroy, (RwTextureDestroy_t)0xDEAD)
RWFUNC(RwRasterUnlock_t RwRasterUnlock, (RwRasterUnlock_t)0xDEAD)
RWFUNC(RwRasterLock_t RwRasterLock, (RwRasterLock_t)0xDEAD)
RWFUNC(RwRasterCreate_t RwRasterCreate, (RwRasterCreate_t)0xDEAD)
RWFUNC(RwTextureCreate_t RwTextureCreate, (RwTextureCreate_t)0xDEAD)
RWFUNC(RpMaterialSetTexture_t RpMaterialSetTexture, (RpMaterialSetTexture_t)0xDEAD)
RWFUNC(GetAnimHierarchyFromClump_t GetAnimHierarchyFromClump, (GetAnimHierarchyFromClump_t)0xDEAD)
RWFUNC(GetAnimHierarchyFromSkinClump_t GetAnimHierarchyFromSkinClump, (GetAnimHierarchyFromSkinClump_t)0xDEAD)
RWFUNC(RpHAnimIDGetIndex_t RpHAnimIDGetIndex, (RpHAnimIDGetIndex_t)0xDEAD)
RWFUNC(RpHAnimHierarchyGetMatrixArray_t RpHAnimHierarchyGetMatrixArray, (RpHAnimHierarchyGetMatrixArray_t)0xDEAD)
RWFUNC(RtQuatRotate_t RtQuatRotate, (RtQuatRotate_t)0xDEAD)

/*****************************************************************************/
/** GTA function definitions and mappings                                   **/
/*****************************************************************************/

typedef bool(__cdecl* SetTextureDict_t)(unsigned short id);
typedef bool(__cdecl* LoadClumpFile_t)(RwStream* stream, unsigned int id);            // (stream, model id)
typedef bool(__cdecl* LoadModel_t)(RwBuffer* filename, unsigned int id);              // (memory chunk, model id)
typedef void(__cdecl* LoadCollisionModel_t)(unsigned char*, CColModelSAInterface*, const char*);
typedef void(__cdecl* LoadCollisionModelVer2_t)(unsigned char*, unsigned int, CColModelSAInterface*, const char*);
typedef void(__cdecl* LoadCollisionModelVer3_t)(unsigned char*, unsigned int, CColModelSAInterface*,
                                                const char*);            // buf, bufsize, ccolmodel&, keyname
typedef bool(__cdecl* CTxdStore_LoadTxd_t)(unsigned int id, RwStream* filename);
typedef void(__cdecl* CTxdStore_RemoveTxd_t)(unsigned int id);
typedef void(__cdecl* CTxdStore_RemoveRef_t)(unsigned int id);
typedef void(__cdecl* CTxdStore_AddRef_t)(unsigned int id);
typedef int(__cdecl* CTxdStore_GetNumRefs_t)(unsigned int id);
typedef RwTexDictionary*(__cdecl* CTxdStore_GetTxd_t)(unsigned int id);
typedef RwTexture*(__cdecl* CClothesBuilder_CopyTexture_t)(RwTexture* texture);

RWFUNC(SetTextureDict_t SetTextureDict, (SetTextureDict_t)0xDEAD)
RWFUNC(LoadClumpFile_t LoadClumpFile, (LoadClumpFile_t)0xDEAD)
RWFUNC(LoadModel_t LoadModel, (LoadModel_t)0xDEAD)
RWFUNC(LoadCollisionModel_t LoadCollisionModel, (LoadCollisionModel_t)0xDEAD)
RWFUNC(LoadCollisionModelVer2_t LoadCollisionModelVer2, (LoadCollisionModelVer2_t)0xDEAD)
RWFUNC(LoadCollisionModelVer3_t LoadCollisionModelVer3, (LoadCollisionModelVer3_t)0xDEAD)
RWFUNC(CTxdStore_LoadTxd_t CTxdStore_LoadTxd, (CTxdStore_LoadTxd_t)0xDEAD)
RWFUNC(CTxdStore_GetTxd_t CTxdStore_GetTxd, (CTxdStore_GetTxd_t)0xDEAD)
RWFUNC(CTxdStore_RemoveTxd_t CTxdStore_RemoveTxd, (CTxdStore_RemoveTxd_t)0xDEAD)
RWFUNC(CTxdStore_RemoveRef_t CTxdStore_RemoveRef, (CTxdStore_RemoveRef_t)0xDEAD)
RWFUNC(CTxdStore_AddRef_t CTxdStore_AddRef, (CTxdStore_AddRef_t)0xDEAD)
RWFUNC(CTxdStore_GetNumRefs_t CTxdStore_GetNumRefs, (CTxdStore_GetNumRefs_t)0xDEAD)
RWFUNC(CClothesBuilder_CopyTexture_t CClothesBuilder_CopyTexture, (CClothesBuilder_CopyTexture_t)0xDEAD)

/*****************************************************************************/
/** Function inlines                                                        **/
/*****************************************************************************/

// Matrix copying
inline void RwFrameCopyMatrix(RwFrame* dst, RwFrame* src)
{
    if (dst == NULL || src == NULL)
        return;
    MemCpyFast(&dst->modelling, &src->modelling, sizeof(RwMatrix));
    MemCpyFast(&dst->ltm, &src->ltm, sizeof(RwMatrix));
}

// Recursive RwFrame children searching function
inline RwFrame* RwFrameFindFrame(RwFrame* parent, const char* name)
{
    RwFrame *ret = parent->child, *buf;
    while (ret != NULL)
    {
        // recurse into the child
        if (ret->child != NULL)
        {
            buf = RwFrameFindFrame(ret, name);
            if (buf != NULL)
                return buf;
        }

        // search through the children frames
        if (strncmp(&ret->szName[0], name, 16) == 0)
        {
            // found it
            return ret;
        }
        ret = ret->next;
    }
    return NULL;
}
