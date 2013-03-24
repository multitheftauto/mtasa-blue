/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_renderware.h
*  PURPOSE:     RenderWare interface mappings to Grand Theft Auto: San Andreas
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#ifndef __GAMESA_RENDERWARE
#define __GAMESA_RENDERWARE

#define WIN32_LEAN_AND_MEAN

#include "RenderWare.h"
#include <windows.h>
#include <stdio.h>

class CColModelSAInterface;

/*****************************************************************************/
/** Renderware functions                                                    **/
/*****************************************************************************/

/* RenderWare macros */
#define RpGetFrame(__c)            ((RwFrame*)(((RwObject *)(__c))->parent))
#define RpSetFrame(__c,__f)        ((((RwObject *)(__c))->parent) = (void *)(__f))

/* RenderWare function defines */
typedef RpAtomic *              (__cdecl *RpAtomicCreate_t)                     (void);
typedef RpAtomic *              (__cdecl *RpAtomicClone_t)                      (RpAtomic * atomic);
typedef int                     (__cdecl *RpAtomicDestroy_t)                    (RpAtomic * atomic);
typedef RpAtomic *              (__cdecl *RpAtomicSetGeometry_t)                (RpAtomic * atomic, RpGeometry * geometry, unsigned int flags);
typedef RpAtomic *              (__cdecl *RpAtomicSetFrame_t)                   (RpAtomic * atomic, RwFrame * frame);
typedef RpClump *               (__cdecl *RpClumpAddAtomic_t)                   (RpClump * clump, RpAtomic * atomic);
typedef RpClump *               (__cdecl *RpClumpAddLight_t)                    (RpClump * clump, RpLight * light);
typedef int                     (__cdecl *RpClumpGetNumAtomics_t)               (RpClump * clump);
typedef RpClump *               (__cdecl *RpClumpRemoveAtomic_t)                (RpClump * clump, RpAtomic * atomic);
typedef RwFrame *               (__cdecl *RwFrameAddChild_t)                    (RwFrame * parent, RwFrame * child);
typedef RwFrame *               (__cdecl *RwFrameRemoveChild_t)                 (RwFrame * child);
typedef RwFrame *               (__cdecl *RwFrameForAllObjects_t)               (RwFrame * frame, void * callback, void * data);
typedef RwFrame *               (__cdecl *RwFrameTranslate_t)                   (RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *               (__cdecl *RwFrameScale_t)                       (RwFrame * frame, const RwV3d * v, RwTransformOrder order);
typedef RwFrame *               (__cdecl *RwFrameCreate_t)                      (void);
typedef RwFrame *               (__cdecl *RwFrameSetIdentity_t)                 (RwFrame * frame);
typedef RpGeometry *            (__cdecl *RpGeometryCreate_t)                   (int numverts, int numtriangles, unsigned int format);
typedef const RpGeometry *      (__cdecl *RpGeometryTriangleSetVertexIndices_t) (const RpGeometry * geo, RpTriangle * tri, unsigned short v1, unsigned short v2, unsigned short v3);
typedef RpGeometry *            (__cdecl *RpGeometryUnlock_t)                   (RpGeometry * geo);
typedef RpGeometry *            (__cdecl *RpGeometryLock_t)                     (RpGeometry * geo, int lockmode);
typedef RpGeometry *            (__cdecl *RpGeometryTransform_t)                (RpGeometry * geo, const RwMatrix * matrix);
typedef RpGeometry *            (__cdecl *RpGeometryTriangleSetMaterial_t)      (RpGeometry * geo, RpTriangle * tri, RpMaterial * mat);
typedef int                     (__cdecl *RpGeometryDestroy_t)                  (RpGeometry * geo);
typedef void *                  (__cdecl *RwIm3DTransform_t)                    (RwVertex *pVerts, unsigned int numVerts, RwMatrix *ltm, unsigned int flags);
typedef int                     (__cdecl *RwIm3DRenderIndexedPrimitive_t)       (RwPrimitiveType primType, unsigned short *indices, int numIndices);
typedef int                     (__cdecl *RwIm3DEnd_t)                          (void);
typedef RpLight *               (__cdecl *RpLightCreate_t)                      (int type);
typedef RpLight *               (__cdecl *RpLightSetRadius_t)                   (RpLight * light, float radius);
typedef RpLight *               (__cdecl *RpLightSetColor_t)                    (RpLight * light, const RwColorFloat * color);
typedef RwMatrix *              (__cdecl *RwMatrixCreate_t)                     (void);
typedef RwMatrix *              (__cdecl *RwMatrixInvert_t)                     (RwMatrix *dst, const RwMatrix *src);
typedef RwMatrix *              (__cdecl *RwMatrixTranslate_t)                  (RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RwMatrix *              (__cdecl *RwMatrixScale_t)                      (RwMatrix * matrix, const RwV3d * translation, RwTransformOrder order);
typedef RpMaterial *            (__cdecl *RpMaterialCreate_t)                   (void);
typedef int                     (__cdecl *RpMaterialDestroy_t)                  (RpMaterial * mat);
typedef RwTexDictionary *       (__cdecl *RwTexDictionarySetCurrent_t)          (RwTexDictionary * dict);
typedef const RwTexDictionary * (__cdecl *RwTexDictionaryForAllTextures_t)      (const RwTexDictionary * dict, void * callback, void * data);
typedef RwTexture *             (__cdecl *RwTexDictionaryAddTexture_t)          (RwTexDictionary * dict, RwTexture * texture);
typedef RwTexDictionary *       (__cdecl *RwTexDictionaryGetCurrent_t)          (void);
typedef RwTexture *             (__cdecl *RwTexDictionaryFindNamedTexture_t)    (RwTexDictionary * dict, const char* name);
typedef void                    (__cdecl *RpPrtStdGlobalDataSetStreamEmbedded_t)(void * value);
typedef RpWorld *               (__cdecl *RpWorldAddAtomic_t)                   (RpWorld * world, RpAtomic * atomic);
typedef RpWorld *               (__cdecl *RpWorldAddClump_t)                    (RpWorld * world, RpClump * clump);
typedef RpWorld *               (__cdecl *RpWorldAddLight_t)                    (RpWorld * world, RpLight * light);
typedef float                   (__cdecl *RwV3dNormalize_t)                     (RwV3d * out, const RwV3d * in);
typedef RwV3d *                 (__cdecl *RwV3dTransformVector_t)               (RwV3d * out, const RwV3d * in, const RwMatrix * matrix);
typedef void                    (__cdecl *_rwObjectHasFrameSetFrame_t)          (void *object, RwFrame *frame);
typedef RwCamera *              (__cdecl *RwCameraClone_t)                      (RwCamera *camera);
typedef RpClump *               (__cdecl *RpClumpClone_t)                       (RpClump *clone);
typedef int                     (__cdecl *RwTexDictionaryDestroy_t)             (RwTexDictionary *txd);
typedef int                     (__cdecl *RwTextureDestroy_t)                   (RwTexture *texture);
typedef int                     (__cdecl *RwStreamFindChunk_t)                  (RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);
typedef RpClump*                (__cdecl *RpClumpStreamRead_t)                  (RwStream *stream);
typedef RwError*                (__cdecl *RwErrorGet_t)                         (RwError *code);
typedef RwStream*               (__cdecl *RwStreamOpen_t)                       (RwStreamType type, RwStreamMode mode, const void *pData);
typedef int                     (__cdecl *RwStreamClose_t)                      (RwStream *stream, void *pData);
typedef int                     (__cdecl *RpClumpDestroy_t)                     (RpClump *clump);
typedef RpClump*                (__cdecl *RpClumpForAllAtomics_t)               (RpClump *clump, void* callback, void* pData);
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryStreamRead_t)          (RwStream *stream);
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryGtaStreamRead_t)       (RwStream *stream);
typedef RwRaster*               (__cdecl *RwRasterUnlock_t)                     (RwRaster *raster);
typedef RwRaster*               (__cdecl *RwRasterLock_t)                       (RwRaster *raster, unsigned char level, int lockmode);
typedef RwRaster*               (__cdecl *RwRasterCreate_t)                     (int width, int height, int depth, int flags);
typedef RwTexture*              (__cdecl *RwTextureCreate_t)                    (RwRaster *raster);

// Utility functions
typedef void*                   (__cdecl *RwAllocAligned_t)                     (size_t size, unsigned int align);
typedef void                    (__cdecl *RwFreeAligned_t)                      (void *ptr);

// Object functions
typedef void                    (__cdecl *RwObjectRegister_t)                   (void *group, RwObject *obj);

// Frame functions
typedef RwFrame*                (__cdecl *RwFrameCloneRecursive_t)              (const RwFrame *frame, const RwFrame *root);
typedef const RwMatrix*         (__cdecl *RwFrameGetLTM_t)                      (RwFrame *frame);

// Atomic functions
typedef bool                    (__cdecl *RpAtomicRender_t)                     (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupObjectPipeline_t)        (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupVehiclePipeline_t)       (RpAtomic *atomic);

// Scene functions
typedef RwScene*                (__cdecl *RwSceneRender_t)                      (RwScene *scene);
typedef RwScene*                (__cdecl *RwSceneAddAtomic_t)                   (RwScene *scene, RpAtomic *atomic);
typedef RwScene*                (__cdecl *RwSceneAddClump_t)                    (RwScene *scene, RpClump *clump);
typedef RwScene*                (__cdecl *RwSceneAddLight_t)                    (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneRemoveLight_t)                 (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneDestroy_t)                     (RwScene *scene);

/*****************************************************************************/
/** Renderware function mappings                                            **/
/*****************************************************************************/

#ifdef RWFUNC_IMPLEMENT
    #define RWFUNC(a,b) a = b;
#else
    #define RWFUNC(a,b) extern a;
#endif

static void __declspec(naked)    invalid_ptr()
{
    __asm int 3;
}

// US Versions
RWFUNC ( RwStreamFindChunk_t                     RwStreamFindChunk                       , (RwStreamFindChunk_t)                     invalid_ptr )
RWFUNC ( RpClumpStreamRead_t                     RpClumpStreamRead                       , (RpClumpStreamRead_t)                     invalid_ptr )
RWFUNC ( RwErrorGet_t                            RwErrorGet                              , (RwErrorGet_t)                            invalid_ptr )
RWFUNC ( RwStreamOpen_t                          RwStreamOpen                            , (RwStreamOpen_t)                          invalid_ptr )
RWFUNC ( RwStreamClose_t                         RwStreamClose                           , (RwStreamClose_t)                         invalid_ptr )
RWFUNC ( RpClumpDestroy_t                        RpClumpDestroy                          , (RpClumpDestroy_t)                        invalid_ptr )
RWFUNC ( RpClumpGetNumAtomics_t                  RpClumpGetNumAtomics                    , (RpClumpGetNumAtomics_t)                  invalid_ptr )
RWFUNC ( RwFrameTranslate_t                      RwFrameTranslate                        , (RwFrameTranslate_t)                      invalid_ptr )
RWFUNC ( RpClumpForAllAtomics_t                  RpClumpForAllAtomics                    , (RpClumpForAllAtomics_t)                  invalid_ptr )
RWFUNC ( RwFrameAddChild_t                       RwFrameAddChild                         , (RwFrameAddChild_t)                       invalid_ptr )
RWFUNC ( RpClumpAddAtomic_t                      RpClumpAddAtomic                        , (RpClumpAddAtomic_t)                      invalid_ptr )
RWFUNC ( RpAtomicSetFrame_t                      RpAtomicSetFrame                        , (RpAtomicSetFrame_t)                      invalid_ptr )
RWFUNC ( RwTexDictionaryStreamRead_t             RwTexDictionaryStreamRead               , (RwTexDictionaryStreamRead_t)             invalid_ptr )
RWFUNC ( RwTexDictionaryGtaStreamRead_t          RwTexDictionaryGtaStreamRead            , (RwTexDictionaryGtaStreamRead_t)          invalid_ptr )
RWFUNC ( RwTexDictionaryGetCurrent_t             RwTexDictionaryGetCurrent               , (RwTexDictionaryGetCurrent_t)             invalid_ptr )
RWFUNC ( RwTexDictionarySetCurrent_t             RwTexDictionarySetCurrent               , (RwTexDictionarySetCurrent_t)             invalid_ptr )
RWFUNC ( RwTexDictionaryForAllTextures_t         RwTexDictionaryForAllTextures           , (RwTexDictionaryForAllTextures_t)         invalid_ptr )
RWFUNC ( RwTexDictionaryAddTexture_t             RwTexDictionaryAddTexture               , (RwTexDictionaryAddTexture_t)             invalid_ptr )
RWFUNC ( RpPrtStdGlobalDataSetStreamEmbedded_t   RpPrtStdGlobalDataSetStreamEmbedded     , (RpPrtStdGlobalDataSetStreamEmbedded_t)   invalid_ptr )
RWFUNC ( RpClumpRemoveAtomic_t                   RpClumpRemoveAtomic                     , (RpClumpRemoveAtomic_t)                   invalid_ptr )
RWFUNC ( RpAtomicClone_t                         RpAtomicClone                           , (RpAtomicClone_t)                         invalid_ptr )
RWFUNC ( RwTexDictionaryFindNamedTexture_t       RwTexDictionaryFindNamedTexture         , (RwTexDictionaryFindNamedTexture_t)       invalid_ptr )
RWFUNC ( RwFrameRemoveChild_t                    RwFrameRemoveChild                      , (RwFrameRemoveChild_t)                    invalid_ptr )
RWFUNC ( RwFrameForAllObjects_t                  RwFrameForAllObjects                    , (RwFrameForAllObjects_t)                  invalid_ptr )
RWFUNC ( RpAtomicDestroy_t                       RpAtomicDestroy                         , (RpAtomicDestroy_t)                       invalid_ptr )
RWFUNC ( RpAtomicSetGeometry_t                   RpAtomicSetGeometry                     , (RpAtomicSetGeometry_t)                   invalid_ptr )
RWFUNC ( RpWorldAddAtomic_t                      RpWorldAddAtomic                        , (RpWorldAddAtomic_t)                      invalid_ptr )
RWFUNC ( RpGeometryCreate_t                      RpGeometryCreate                        , (RpGeometryCreate_t)                      invalid_ptr )
RWFUNC ( RpGeometryTriangleSetVertexIndices_t    RpGeometryTriangleSetVertexIndices      , (RpGeometryTriangleSetVertexIndices_t)    invalid_ptr )
RWFUNC ( RpGeometryUnlock_t                      RpGeometryUnlock                        , (RpGeometryUnlock_t)                      invalid_ptr )
RWFUNC ( RpGeometryLock_t                        RpGeometryLock                          , (RpGeometryLock_t)                        invalid_ptr )
RWFUNC ( RpAtomicCreate_t                        RpAtomicCreate                          , (RpAtomicCreate_t)                        invalid_ptr )
RWFUNC ( RwFrameCreate_t                         RwFrameCreate                           , (RwFrameCreate_t)                         invalid_ptr )
RWFUNC ( RpGeometryTransform_t                   RpGeometryTransform                     , (RpGeometryTransform_t)                   invalid_ptr )
RWFUNC ( RwFrameSetIdentity_t                    RwFrameSetIdentity                      , (RwFrameSetIdentity_t)                    invalid_ptr )
RWFUNC ( RwMatrixCreate_t                        RwMatrixCreate                          , (RwMatrixCreate_t)                        invalid_ptr )
RWFUNC ( RwMatrixTranslate_t                     RwMatrixTranslate                       , (RwMatrixTranslate_t)                     invalid_ptr )
RWFUNC ( RwMatrixScale_t                         RwMatrixScale                           , (RwMatrixScale_t)                         invalid_ptr )
RWFUNC ( RpGeometryTriangleSetMaterial_t         RpGeometryTriangleSetMaterial           , (RpGeometryTriangleSetMaterial_t)         invalid_ptr )
RWFUNC ( RpMaterialCreate_t                      RpMaterialCreate                        , (RpMaterialCreate_t)                      invalid_ptr )
RWFUNC ( RpGeometryDestroy_t                     RpGeometryDestroy                       , (RpGeometryDestroy_t)                     invalid_ptr )
RWFUNC ( RpMaterialDestroy_t                     RpMaterialDestroy                       , (RpMaterialDestroy_t)                     invalid_ptr )
RWFUNC ( RwV3dNormalize_t                        RwV3dNormalize                          , (RwV3dNormalize_t)                        invalid_ptr )
RWFUNC ( RwIm3DTransform_t                       RwIm3DTransform                         , (RwIm3DTransform_t)                       invalid_ptr )
RWFUNC ( RwIm3DRenderIndexedPrimitive_t          RwIm3DRenderIndexedPrimitive            , (RwIm3DRenderIndexedPrimitive_t)          invalid_ptr )
RWFUNC ( RwIm3DEnd_t                             RwIm3DEnd                               , (RwIm3DEnd_t)                             invalid_ptr )
RWFUNC ( RwMatrixInvert_t                        RwMatrixInvert                          , (RwMatrixInvert_t)                        invalid_ptr )
RWFUNC ( RpWorldAddClump_t                       RpWorldAddClump                         , (RpWorldAddClump_t)                       invalid_ptr )
RWFUNC ( RwFrameScale_t                          RwFrameScale                            , (RwFrameScale_t)                          invalid_ptr )
RWFUNC ( RwV3dTransformVector_t                  RwV3dTransformVector                    , (RwV3dTransformVector_t)                  invalid_ptr )
RWFUNC ( RpClumpAddLight_t                       RpClumpAddLight                         , (RpClumpAddLight_t)                       invalid_ptr )
RWFUNC ( _rwObjectHasFrameSetFrame_t             _rwObjectHasFrameSetFrame               , (_rwObjectHasFrameSetFrame_t)             invalid_ptr )
RWFUNC ( RpLightSetRadius_t                      RpLightSetRadius                        , (RpLightSetRadius_t)                      invalid_ptr )
RWFUNC ( RpWorldAddLight_t                       RpWorldAddLight                         , (RpWorldAddLight_t)                       invalid_ptr )
RWFUNC ( RpLightSetColor_t                       RpLightSetColor                         , (RpLightSetColor_t)                       invalid_ptr )
RWFUNC ( RwCameraClone_t                         RwCameraClone                           , (RwCameraClone_t)                         invalid_ptr )
RWFUNC ( RpClumpClone_t                          RpClumpClone                            , (RpClumpClone_t)                          invalid_ptr )
RWFUNC ( RwTexDictionaryDestroy_t                RwTexDictionaryDestroy                  , (RwTexDictionaryDestroy_t)                invalid_ptr )
RWFUNC ( RwTextureDestroy_t                      RwTextureDestroy                        , (RwTextureDestroy_t)                      invalid_ptr )
RWFUNC ( RwRasterUnlock_t                        RwRasterUnlock                          , (RwRasterUnlock_t)                        invalid_ptr )
RWFUNC ( RwRasterLock_t                          RwRasterLock                            , (RwRasterLock_t)                          invalid_ptr )
RWFUNC ( RwRasterCreate_t                        RwRasterCreate                          , (RwRasterCreate_t)                        invalid_ptr )
RWFUNC ( RwTextureCreate_t                       RwTextureCreate                         , (RwTextureCreate_t)                       invalid_ptr )

// Object functions
RWFUNC ( RwObjectRegister_t                      RwObjectRegister                        , (RwObjectRegister_t)                      invalid_ptr )

// Frame functions
RWFUNC ( RwFrameGetLTM_t                         RwFrameGetLTM                           , (RwFrameGetLTM_t)                         invalid_ptr )
RWFUNC ( RwFrameCloneRecursive_t                 RwFrameCloneRecursive                   , (RwFrameCloneRecursive_t)                 invalid_ptr )

// Atomic functions
RWFUNC ( RpAtomicRender_t                        RpAtomicRender                          , (RpAtomicRender_t)                        invalid_ptr )
RWFUNC ( RpAtomicSetupObjectPipeline_t           RpAtomicSetupObjectPipeline             , (RpAtomicSetupObjectPipeline_t)           invalid_ptr )
RWFUNC ( RpAtomicSetupVehiclePipeline_t          RpAtomicSetupVehiclePipeline            , (RpAtomicSetupVehiclePipeline_t)          invalid_ptr )

// Scene functions
RWFUNC ( RwSceneAddAtomic_t                      RwSceneAddAtomic                        , (RwSceneAddAtomic_t)                      invalid_ptr )
RWFUNC ( RwSceneAddLight_t                       RwSceneAddLight                         , (RwSceneAddLight_t)                       invalid_ptr )
RWFUNC ( RwSceneAddClump_t                       RwSceneAddClump                         , (RwSceneAddClump_t)                       invalid_ptr )
RWFUNC ( RwSceneRemoveLight_t                    RwSceneRemoveLight                      , (RwSceneRemoveLight_t)                    invalid_ptr )

// Utility functions
RWFUNC ( RwAllocAligned_t                        RwAllocAligned                          , (RwAllocAligned_t)                        invalid_ptr )
RWFUNC ( RwFreeAligned_t                         RwFreeAligned                           , (RwFreeAligned_t)                         invalid_ptr )

/*****************************************************************************/
/** GTA function definitions and mappings                                   **/
/*****************************************************************************/

typedef bool                (__cdecl *SetTextureDict_t)                 (unsigned short id);
typedef bool                (__cdecl *LoadClumpFile_t)                  (RwStream *stream, unsigned int id);        // (stream, model id)
typedef bool                (__cdecl *LoadModel_t)                      (RwBuffer *filename, unsigned int id);      // (memory chunk, model id)
typedef void                (__cdecl *LoadCollisionModel_t)             (unsigned char*, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer2_t)         (unsigned char*, unsigned int, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer3_t)         (unsigned char*, unsigned int, CColModelSAInterface*, const char*); // buf, bufsize, ccolmodel&, keyname
typedef bool                (__cdecl *CTxdStore_LoadTxd_t)              (unsigned int id, RwStream *filename);
typedef void                (__cdecl *CTxdStore_RemoveTxd_t)            (unsigned int id);
typedef void                (__cdecl *CTxdStore_RemoveRef_t)            (unsigned int id);
typedef void                (__cdecl *CTxdStore_AddRef_t)               (unsigned int id);
typedef RwTexDictionary*    (__cdecl *CTxdStore_GetTxd_t)               (unsigned int id);
typedef RwTexture*          (__cdecl *CClothesBuilder_CopyTexture_t)    (RwTexture *texture);

RWFUNC ( SetTextureDict_t                SetTextureDict                  , (SetTextureDict_t)                invalid_ptr )
RWFUNC ( LoadClumpFile_t                 LoadClumpFile                   , (LoadClumpFile_t)                 invalid_ptr )
RWFUNC ( LoadModel_t                     LoadModel                       , (LoadModel_t)                     invalid_ptr )
RWFUNC ( LoadCollisionModel_t            LoadCollisionModel              , (LoadCollisionModel_t)            invalid_ptr )
RWFUNC ( LoadCollisionModelVer2_t        LoadCollisionModelVer2          , (LoadCollisionModelVer2_t)        invalid_ptr )
RWFUNC ( LoadCollisionModelVer3_t        LoadCollisionModelVer3          , (LoadCollisionModelVer3_t)        invalid_ptr )
RWFUNC ( CTxdStore_LoadTxd_t             CTxdStore_LoadTxd               , (CTxdStore_LoadTxd_t)             invalid_ptr )
RWFUNC ( CTxdStore_GetTxd_t              CTxdStore_GetTxd                , (CTxdStore_GetTxd_t)              invalid_ptr )
RWFUNC ( CTxdStore_RemoveTxd_t           CTxdStore_RemoveTxd             , (CTxdStore_RemoveTxd_t)           invalid_ptr )
RWFUNC ( CTxdStore_RemoveRef_t           CTxdStore_RemoveRef             , (CTxdStore_RemoveRef_t)           invalid_ptr )
RWFUNC ( CTxdStore_AddRef_t              CTxdStore_AddRef                , (CTxdStore_AddRef_t)              invalid_ptr )
RWFUNC ( CClothesBuilder_CopyTexture_t   CClothesBuilder_CopyTexture     , (CClothesBuilder_CopyTexture_t)   invalid_ptr )

/*****************************************************************************/
/** Function inlines                                                        **/
/*****************************************************************************/

// Matrix copying
inline void RwFrameCopyMatrix ( RwFrame * dst, RwFrame * src )
{
    dst->modelling = src->modelling;
    dst->ltm = src->ltm;
}

// Recursive RwFrame children searching function
inline RwFrame * RwFrameFindFrame ( RwFrame * parent, const char * name ) {
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
