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

/* RenderWare function defines */
typedef RwError*                (__cdecl *RwErrorGet_t)                         (RwError *code);

// Utility functions
typedef void                    (__cdecl *RwDeviceSystemRequest_t)              (RwRenderSystem& rend, int objectID, unsigned int& result, int, int);
typedef void                    (__cdecl *RwPrefetch_t)                         (void);
typedef void                    (__cdecl *RwFlushLoader_t)                      (void);
typedef unsigned int            (__cdecl *RwPluginRegistryReadDataChunks_t)     (void *unk, RwStream *stream, void *obj);
typedef void*                   (__cdecl *RwAllocAligned_t)                     (size_t size, unsigned int align);
typedef void                    (__cdecl *RwFreeAligned_t)                      (void *ptr);
typedef RwExtension*            (__cdecl *RwCreateExtension_t)                  (unsigned int id, unsigned int count, size_t size, int unk3);

typedef void*                   (__cdecl *RwIm3DTransform_t)                    (RwVertex *pVerts, unsigned int numVerts, RwMatrix *ltm, unsigned int flags);
typedef int                     (__cdecl *RwIm3DRenderIndexedPrimitive_t)       (RwPrimitiveType primType, unsigned short *indices, int numIndices);
typedef int                     (__cdecl *RwIm3DEnd_t)                          (void);

// Vector functions
typedef float                   (__cdecl *RwV3dNormalize_t)                     (RwV3d *out, const RwV3d *in);
typedef RwV3d *                 (__cdecl *RwV3dTransformVector_t)               (RwV3d *out, const RwV3d *in, const RwMatrix *matrix);

// Matrix functions
typedef RwMatrix*               (__cdecl *RwMatrixCreate_t)                     (void);
typedef RwMatrix*               (__cdecl *RwMatrixInvert_t)                     (RwMatrix *dst, const RwMatrix *src);
typedef RwMatrix*               (__cdecl *RwMatrixTranslate_t)                  (RwMatrix *matrix, const RwV3d *translation, RwTransformOrder order);
typedef RwMatrix*               (__cdecl *RwMatrixScale_t)                      (RwMatrix *matrix, const RwV3d *translation, RwTransformOrder order);
typedef float                   (__cdecl *RwMatrixUnknown_t)                    (const RwMatrix& matrix, const RwMatrix& matrix2, unsigned short flags);

// Object functions
typedef void                    (__cdecl *RwObjectRegister_t)                   (void *group, RwObject *obj);

// Stream functions
typedef RwStream*               (__cdecl *RwStreamReadChunkHeaderInfo_t)        (RwStream *stream, RwChunkHeader& header);
typedef int                     (__cdecl *RwStreamFindChunk_t)                  (RwStream *stream, unsigned int type, unsigned int *lengthOut, unsigned int *versionOut);

// Frame functions
typedef RwFrame*                (__cdecl *RwFrameCreate_t)                      (void);
typedef RwFrame*                (__cdecl *RwFrameCloneRecursive_t)              (const RwFrame *frame, const RwFrame *root);
typedef void                    (__cdecl *RwFrameCloneHierarchy_t)              (RwFrame *frame);
typedef const RwMatrix*         (__cdecl *RwFrameGetLTM_t)                      (RwFrame *frame);
typedef RwFrame*                (__cdecl *RwFrameTranslate_t)                   (RwFrame *frame, const RwV3d *v, RwTransformOrder order);
typedef RwFrame*                (__cdecl *RwFrameScale_t)                       (RwFrame *frame, const RwV3d *v, RwTransformOrder order);
typedef void                    (__cdecl *RwFrameOrient_t)                      (RwFrame *frame, float unk, float unk2, CVector& unk3);
typedef RwFrame*                (__cdecl *RwFrameSetIdentity_t)                 (RwFrame *frame);
typedef void                    (__cdecl *RwFrameDestroy_t)                     (RwFrame *frame);

// Material functions
typedef RpMaterial*             (__cdecl *RpMaterialCreate_t)                   (void);
typedef int                     (__cdecl *RpMaterialDestroy_t)                  (RpMaterial *mat);
typedef void                    (__cdecl *RpD3D9SetSurfaceProperties_t)         (RpMaterialLighting& matLight, RwColor& matColor, unsigned int renderFlags);

// Geometry functions
typedef RpGeometry*             (__cdecl *RpGeometryCreate_t)                   (int numverts, int numtriangles, unsigned int format);
typedef RpSkeleton*             (__cdecl *RpGeometryGetSkeleton_t)              (RpGeometry *geom);
typedef const RpGeometry*       (__cdecl *RpGeometryTriangleSetVertexIndices_t) (const RpGeometry *geo, RpTriangle *tri, unsigned short v1, unsigned short v2, unsigned short v3);
typedef RpGeometry*             (__cdecl *RpGeometryUnlock_t)                   (RpGeometry *geo);
typedef RpGeometry*             (__cdecl *RpGeometryLock_t)                     (RpGeometry *geo, int lockmode);
typedef RpGeometry*             (__cdecl *RpGeometryTransform_t)                (RpGeometry *geo, const RwMatrix *matrix);
typedef RpGeometry*             (__cdecl *RpGeometryTriangleSetMaterial_t)      (RpGeometry *geo, RpTriangle *tri, RpMaterial *mat);
typedef int                     (__cdecl *RpGeometryDestroy_t)                  (RpGeometry *geo);

// Atomic functions
typedef RpAtomic*               (__cdecl *RpAtomicCreate_t)                     (void);
typedef RpAtomic*               (__cdecl *RpAtomicClone_t)                      (const RpAtomic *atomic);
typedef RpAtomic*               (__cdecl *RpAtomicSetGeometry_t)                (RpAtomic *atomic, RpGeometry *geometry, unsigned int flags);
typedef RpAtomic*               (__cdecl *RpAtomicRender_t)                     (RpAtomic *atomic);
typedef const RwSphere&         (__cdecl *RpAtomicGetWorldBoundingSphere_t)     (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupObjectPipeline_t)        (RpAtomic *atomic);
typedef void                    (__cdecl *RpAtomicSetupVehiclePipeline_t)       (RpAtomic *atomic);
typedef int                     (__cdecl *RpAtomicDestroy_t)                    (RpAtomic *atomic);

// Light functions
typedef RpLight*                (__cdecl *RpLightSetRadius_t)                   (RpLight *light, float radius);
typedef float                   (__cdecl *RpLightGetConeAngle_t)                (const RpLight *light);
typedef void                    (__cdecl *RpLightDestroy_t)                     (RpLight *light);

// Camera functions
typedef RwCamera*               (__cdecl *RwCameraClone_t)                      (RwCamera *camera);
typedef void                    (__cdecl *RwCameraDestroy_t)                    (RwCamera *camera);

// Clump functions
typedef RpClump *               (__cdecl *RpClumpClone_t)                       (const RpClump *clone);
typedef void                    (__cdecl *RpClumpGetBoneTransform_t)            (RpClump *clump, CVector *offsets);
typedef void                    (__cdecl *RpClumpSetupFrameCallback_t)          (RpClump *clump, unsigned int hierarchyId);
typedef RpClump*                (__cdecl *RpClumpStreamRead_t)                  (RwStream *stream);
typedef int                     (__cdecl *RpClumpDestroy_t)                     (RpClump *clump);

// Raster functions
typedef RwRaster*               (__cdecl *RwRasterCreate_t)                     (int width, int height, int depth, int flags);
typedef RwRaster*               (__cdecl *RwRasterUnlock_t)                     (RwRaster *raster);
typedef RwRaster*               (__cdecl *RwRasterLock_t)                       (RwRaster *raster, unsigned char level, int lockmode);
typedef void                    (__cdecl *RwRasterDestroy_t)                    (RwRaster *raster);

// Texture functions
typedef RwTexture*              (__cdecl *RwTextureCreate_t)                    (RwRaster *raster);
typedef int                     (__cdecl *RwTextureDestroy_t)                   (RwTexture *texture);
typedef int                     (__cdecl *RpD3D9SetTexture_t)                   (RwTexture *texture, unsigned int index);

// TexDictionary functions
typedef RwTexDictionary*        (__cdecl *RwTexDictionaryStreamRead_t)          (RwStream *stream);
typedef int                     (__cdecl *RwTexDictionaryDestroy_t)             (RwTexDictionary *txd);

// Scene functions
typedef RwScene*                (__cdecl *RwSceneRender_t)                      (RwScene *scene);
typedef RwScene*                (__cdecl *RwSceneAddAtomic_t)                   (RwScene *scene, RpAtomic *atomic);
typedef RwScene*                (__cdecl *RwSceneAddClump_t)                    (RwScene *scene, RpClump *clump);
typedef RwScene*                (__cdecl *RwSceneAddLight_t)                    (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneRemoveLight_t)                 (RwScene *scene, RpLight *light);
typedef void                    (__cdecl *RwSceneDestroy_t)                     (RwScene *scene);

// Dict functions
typedef RtDict*                 (__cdecl *RtDictSchemaStreamReadDict_t)         (RtDictSchema *schema, RwStream *stream);
typedef void                    (__cdecl *RtDictDestroy_t)                      (RtDict *dict);

// Animation functions
typedef bool                    (__cdecl *RwAnimationInit_t)                    (RpAnimation *anim, RwExtension *ext);
typedef bool                    (__cdecl *RwSkeletonUpdate_t)                   (RpSkeleton *skel);

// Memory functions
typedef void*                   (__cdecl *RwMalloc_t)                           (size_t memSize);
typedef void*                   (__cdecl *RwRealloc_t)                          (void *memptr, size_t newSize);
typedef void                    (__cdecl *RwFree_t)                             (void *memptr);
typedef void                    (__cdecl *RwCalloc_t)                           (unsigned int count, unsigned int blockSize);

/*****************************************************************************/
/** Renderware function mappings                                            **/
/*****************************************************************************/

#ifdef RWFUNC_IMPLEMENT
    #define RWFUNC(a,b) a = b;
#else
    #define RWFUNC(a,b) extern a;
#endif

// Utility functions
RWFUNC ( RwDeviceSystemRequest_t                    RwDeviceSystemRequest                   , (RwDeviceSystemRequest_t)                 invalid_ptr )
RWFUNC ( RwErrorGet_t                               RwErrorGet                              , (RwErrorGet_t)                            invalid_ptr )
RWFUNC ( RwAllocAligned_t                           RwAllocAligned                          , (RwAllocAligned_t)                        invalid_ptr )
RWFUNC ( RwFreeAligned_t                            RwFreeAligned                           , (RwFreeAligned_t)                         invalid_ptr )
RWFUNC ( RwCreateExtension_t                        RwCreateExtension                       , (RwCreateExtension_t)                     invalid_ptr )
RWFUNC ( RwPrefetch_t                               RwPrefetch                              , (RwPrefetch_t)                            invalid_ptr )
RWFUNC ( RwFlushLoader_t                            RwFlushLoader                           , (RwFlushLoader_t)                         invalid_ptr )
RWFUNC ( RwPluginRegistryReadDataChunks_t           RwPluginRegistryReadDataChunks          , (RwPluginRegistryReadDataChunks_t)        invalid_ptr )

RWFUNC ( RwIm3DTransform_t                          RwIm3DTransform                         , (RwIm3DTransform_t)                       invalid_ptr )
RWFUNC ( RwIm3DRenderIndexedPrimitive_t             RwIm3DRenderIndexedPrimitive            , (RwIm3DRenderIndexedPrimitive_t)          invalid_ptr )
RWFUNC ( RwIm3DEnd_t                                RwIm3DEnd                               , (RwIm3DEnd_t)                             invalid_ptr )

// Vector functions
RWFUNC ( RwV3dNormalize_t                           RwV3dNormalize                          , (RwV3dNormalize_t)                        invalid_ptr )
RWFUNC ( RwV3dTransformVector_t                     RwV3dTransformVector                    , (RwV3dTransformVector_t)                  invalid_ptr )

// Matrix functions
RWFUNC ( RwMatrixCreate_t                           RwMatrixCreate                          , (RwMatrixCreate_t)                        invalid_ptr )
RWFUNC ( RwMatrixInvert_t                           RwMatrixInvert                          , (RwMatrixInvert_t)                        invalid_ptr )
RWFUNC ( RwMatrixTranslate_t                        RwMatrixTranslate                       , (RwMatrixTranslate_t)                     invalid_ptr )
RWFUNC ( RwMatrixScale_t                            RwMatrixScale                           , (RwMatrixScale_t)                         invalid_ptr )
RWFUNC ( RwMatrixUnknown_t                          RwMatrixUnknown                         , (RwMatrixUnknown_t)                       invalid_ptr )

// Object functions
RWFUNC ( RwObjectRegister_t                         RwObjectRegister                        , (RwObjectRegister_t)                      invalid_ptr )

// Stream functions
RWFUNC ( RwStreamFindChunk_t                        RwStreamFindChunk                       , (RwStreamFindChunk_t)                     invalid_ptr )
RWFUNC ( RwStreamReadChunkHeaderInfo_t              RwStreamReadChunkHeaderInfo             , (RwStreamReadChunkHeaderInfo_t)           invalid_ptr )

// Frame functions
RWFUNC ( RwFrameCreate_t                            RwFrameCreate                           , (RwFrameCreate_t)                         invalid_ptr )
RWFUNC ( RwFrameCloneRecursive_t                    RwFrameCloneRecursive                   , (RwFrameCloneRecursive_t)                 invalid_ptr )
RWFUNC ( RwFrameCloneHierarchy_t                    RwFrameCloneHierarchy                   , (RwFrameCloneHierarchy_t)                 invalid_ptr )
RWFUNC ( RwFrameGetLTM_t                            RwFrameGetLTM                           , (RwFrameGetLTM_t)                         invalid_ptr )
RWFUNC ( RwFrameSetIdentity_t                       RwFrameSetIdentity                      , (RwFrameSetIdentity_t)                    invalid_ptr )
RWFUNC ( RwFrameScale_t                             RwFrameScale                            , (RwFrameScale_t)                          invalid_ptr )
RWFUNC ( RwFrameOrient_t                            RwFrameOrient                           , (RwFrameOrient_t)                         invalid_ptr )
RWFUNC ( RwFrameTranslate_t                         RwFrameTranslate                        , (RwFrameTranslate_t)                      invalid_ptr )
RWFUNC ( RwFrameDestroy_t                           RwFrameDestroy                          , (RwFrameDestroy_t)                        invalid_ptr )
RWFUNC ( RwFrameRegisterPlugin_t                    RwFrameRegisterPlugin                   , (RwFrameRegisterPlugin_t)                 invalid_ptr )
RWFUNC ( RwFrameRegisterPluginStream_t              RwFrameRegisterPluginStream             , (RwFrameRegisterPluginStream_t)           invalid_ptr )

// Material functions
RWFUNC ( RpMaterialCreate_t                         RpMaterialCreate                        , (RpMaterialCreate_t)                      invalid_ptr )
RWFUNC ( RpMaterialDestroy_t                        RpMaterialDestroy                       , (RpMaterialDestroy_t)                     invalid_ptr )
RWFUNC ( RpD3D9SetSurfaceProperties_t               RpD3D9SetSurfaceProperties              , (RpD3D9SetSurfaceProperties_t)            invalid_ptr )
RWFUNC ( RpMaterialRegisterPlugin_t                 RpMaterialRegisterPlugin                , (RpMaterialRegisterPlugin_t)              invalid_ptr )
RWFUNC ( RpMaterialRegisterPluginStream_t           RpMaterialRegisterPluginStream          , (RpMaterialRegisterPluginStream_t)        invalid_ptr )

// Geometry functions
RWFUNC ( RpGeometryCreate_t                         RpGeometryCreate                        , (RpGeometryCreate_t)                      invalid_ptr )
RWFUNC ( RpGeometryGetSkeleton_t                    RpGeometryGetSkeleton                   , (RpGeometryGetSkeleton_t)                 invalid_ptr )
RWFUNC ( RpGeometryTriangleSetVertexIndices_t       RpGeometryTriangleSetVertexIndices      , (RpGeometryTriangleSetVertexIndices_t)    invalid_ptr )
RWFUNC ( RpGeometryTriangleSetMaterial_t            RpGeometryTriangleSetMaterial           , (RpGeometryTriangleSetMaterial_t)         invalid_ptr )
RWFUNC ( RpGeometryUnlock_t                         RpGeometryUnlock                        , (RpGeometryUnlock_t)                      invalid_ptr )
RWFUNC ( RpGeometryLock_t                           RpGeometryLock                          , (RpGeometryLock_t)                        invalid_ptr )
RWFUNC ( RpGeometryTransform_t                      RpGeometryTransform                     , (RpGeometryTransform_t)                   invalid_ptr )
RWFUNC ( RpGeometryDestroy_t                        RpGeometryDestroy                       , (RpGeometryDestroy_t)                     invalid_ptr )
RWFUNC ( RpGeometryRegisterPlugin_t                 RpGeometryRegisterPlugin                , (RpGeometryRegisterPlugin_t)              invalid_ptr )
RWFUNC ( RpGeometryRegisterPluginStream_t           RpGeometryRegisterPluginStream          , (RpGeometryRegisterPluginStream_t)        invalid_ptr )

// Atomic functions
RWFUNC ( RpAtomicCreate_t                           RpAtomicCreate                          , (RpAtomicCreate_t)                        invalid_ptr )
RWFUNC ( RpAtomicClone_t                            RpAtomicClone                           , (RpAtomicClone_t)                         invalid_ptr )
RWFUNC ( RpAtomicSetGeometry_t                      RpAtomicSetGeometry                     , (RpAtomicSetGeometry_t)                   invalid_ptr )
RWFUNC ( RpAtomicGetWorldBoundingSphere_t           RpAtomicGetWorldBoundingSphere          , (RpAtomicGetWorldBoundingSphere_t)        invalid_ptr )
RWFUNC ( RpAtomicSetupObjectPipeline_t              RpAtomicSetupObjectPipeline             , (RpAtomicSetupObjectPipeline_t)           invalid_ptr )
RWFUNC ( RpAtomicSetupVehiclePipeline_t             RpAtomicSetupVehiclePipeline            , (RpAtomicSetupVehiclePipeline_t)          invalid_ptr )
RWFUNC ( RpAtomicRender_t                           RpAtomicRender                          , (RpAtomicRender_t)                        invalid_ptr )
RWFUNC ( RpAtomicDestroy_t                          RpAtomicDestroy                         , (RpAtomicDestroy_t)                       invalid_ptr )
RWFUNC ( RpAtomicRegisterPlugin_t                   RpAtomicRegisterPlugin                  , (RpAtomicRegisterPlugin_t)                invalid_ptr )
RWFUNC ( RpAtomicRegisterPluginStream_t             RpAtomicRegisterPluginStream            , (RpAtomicRegisterPluginStream_t)          invalid_ptr )

// Light functions
RWFUNC ( RpLightSetRadius_t                         RpLightSetRadius                        , (RpLightSetRadius_t)                      invalid_ptr )
RWFUNC ( RpLightGetConeAngle_t                      RpLightGetConeAngle                     , (RpLightGetConeAngle_t)                   invalid_ptr )
RWFUNC ( RpLightDestroy_t                           RpLightDestroy                          , (RpLightDestroy_t)                        invalid_ptr )
RWFUNC ( RpLightRegisterPlugin_t                    RpLightRegisterPlugin                   , (RpLightRegisterPlugin_t)                 invalid_ptr )
RWFUNC ( RpLightRegisterPluginStream_t              RpLightRegisterPluginStream             , (RpLightRegisterPluginStream_t)           invalid_ptr )

// Camera functions
RWFUNC ( RwCameraClone_t                            RwCameraClone                           , (RwCameraClone_t)                         invalid_ptr )
RWFUNC ( RwCameraDestroy_t                          RwCameraDestroy                         , (RwCameraDestroy_t)                       invalid_ptr )
RWFUNC ( RwCameraRegisterPlugin_t                   RwCameraRegisterPlugin                  , (RwCameraRegisterPlugin_t)                invalid_ptr )
RWFUNC ( RwCameraRegisterPluginStream_t             RwCameraRegisterPluginStream            , (RwCameraRegisterPluginStream_t)          invalid_ptr )

// Clump functions
RWFUNC ( RpClumpClone_t                             RpClumpClone                            , (RpClumpClone_t)                          invalid_ptr )
RWFUNC ( RpClumpGetBoneTransform_t                  RpClumpGetBoneTransform                 , (RpClumpGetBoneTransform_t)               invalid_ptr )
RWFUNC ( RpClumpSetupFrameCallback_t                RpClumpSetupFrameCallback               , (RpClumpSetupFrameCallback_t)             invalid_ptr )
RWFUNC ( RpClumpStreamRead_t                        RpClumpStreamRead                       , (RpClumpStreamRead_t)                     invalid_ptr )
RWFUNC ( RpClumpDestroy_t                           RpClumpDestroy                          , (RpClumpDestroy_t)                        invalid_ptr )
RWFUNC ( RpClumpRegisterPlugin_t                    RpClumpRegisterPlugin                   , (RpClumpRegisterPlugin_t)                 invalid_ptr )
RWFUNC ( RpClumpRegisterPluginStream_t              RpClumpRegisterPluginStream             , (RpClumpRegisterPluginStream_t)           invalid_ptr )

// Raster functions
RWFUNC ( RwRasterCreate_t                           RwRasterCreate                          , (RwRasterCreate_t)                        invalid_ptr )
RWFUNC ( RwRasterUnlock_t                           RwRasterUnlock                          , (RwRasterUnlock_t)                        invalid_ptr )
RWFUNC ( RwRasterLock_t                             RwRasterLock                            , (RwRasterLock_t)                          invalid_ptr )
RWFUNC ( RwRasterDestroy_t                          RwRasterDestroy                         , (RwRasterDestroy_t)                       invalid_ptr )
RWFUNC ( RwRasterRegisterPlugin_t                   RwRasterRegisterPlugin                  , (RwRasterRegisterPlugin_t)                invalid_ptr )
RWFUNC ( RwRasterRegisterPluginStream_t             RwRasterRegisterPluginStream            , (RwRasterRegisterPluginStream_t)          invalid_ptr )

// Texture functions
RWFUNC ( RwTextureCreate_t                          RwTextureCreate                         , (RwTextureCreate_t)                       invalid_ptr )
RWFUNC ( RwTextureDestroy_t                         RwTextureDestroy                        , (RwTextureDestroy_t)                      invalid_ptr )
RWFUNC ( RpD3D9SetTexture_t                         _RpD3D9SetTexture                       , (RpD3D9SetTexture_t)                      invalid_ptr )
RWFUNC ( RwTextureRegisterPlugin_t                  RwTextureRegisterPlugin                 , (RwTextureRegisterPlugin_t)               invalid_ptr )
RWFUNC ( RwTextureRegisterPluginStream_t            RwTextureRegisterPluginStream           , (RwTextureRegisterPluginStream_t)         invalid_ptr )

// TexDictionary functions
RWFUNC ( RwTexDictionaryStreamRead_t                RwTexDictionaryStreamRead               , (RwTexDictionaryStreamRead_t)             invalid_ptr )
RWFUNC ( RwTexDictionaryDestroy_t                   RwTexDictionaryDestroy                  , (RwTexDictionaryDestroy_t)                invalid_ptr )
RWFUNC ( RwTexDictionaryRegisterPlugin_t            RwTexDictionaryRegisterPlugin           , (RwTexDictionaryRegisterPlugin_t)         invalid_ptr )
RWFUNC ( RwTexDictionaryRegisterPluginStream_t      RwTexDictionaryRegisterPluginStream     , (RwTexDictionaryRegisterPluginStream_t)   invalid_ptr )

// Scene functions
RWFUNC ( RwSceneRender_t                            RwSceneRender                           , (RwSceneRender_t)                         invalid_ptr )
RWFUNC ( RwSceneAddAtomic_t                         RwSceneAddAtomic                        , (RwSceneAddAtomic_t)                      invalid_ptr )
RWFUNC ( RwSceneAddLight_t                          RwSceneAddLight                         , (RwSceneAddLight_t)                       invalid_ptr )
RWFUNC ( RwSceneAddClump_t                          RwSceneAddClump                         , (RwSceneAddClump_t)                       invalid_ptr )
RWFUNC ( RwSceneRemoveLight_t                       RwSceneRemoveLight                      , (RwSceneRemoveLight_t)                    invalid_ptr )
RWFUNC ( RwSceneDestroy_t                           RwSceneDestroy                          , (RwSceneDestroy_t)                        invalid_ptr )
RWFUNC ( RwSceneRegisterPlugin_t                    RwSceneRegisterPlugin                   , (RwSceneRegisterPlugin_t)                 invalid_ptr )
RWFUNC ( RwSceneRegisterPluginStream_t              RwSceneRegisterPluginStream             , (RwSceneRegisterPluginStream_t)           invalid_ptr )

// Dict functions
RWFUNC ( RtDictSchemaStreamReadDict_t               RtDictSchemaStreamReadDict              , (RtDictSchemaStreamReadDict_t)            invalid_ptr )
RWFUNC ( RtDictDestroy_t                            RtDictDestroy                           , (RtDictDestroy_t)                         invalid_ptr )

// Animation functions
RWFUNC ( RwAnimationInit_t                          RwAnimationInit                         , (RwAnimationInit_t)                       invalid_ptr )
RWFUNC ( RwSkeletonUpdate_t                         RwSkeletonUpdate                        , (RwSkeletonUpdate_t)                      invalid_ptr )

// Memory functions
RWFUNC ( RwMalloc_t                                 RwMalloc                                , (RwMalloc_t)                              invalid_ptr )
RWFUNC ( RwRealloc_t                                RwRealloc                               , (RwRealloc_t)                             invalid_ptr )
RWFUNC ( RwFree_t                                   RwFree                                  , (RwFree_t)                                invalid_ptr )
RWFUNC ( RwCalloc_t                                 RwCalloc                                , (RwCalloc_t)                              invalid_ptr )

/*****************************************************************************/
/** GTA function definitions and mappings                                   **/
/*****************************************************************************/

typedef void                (__cdecl *LoadCollisionModel_t)             (const char*, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer2_t)         (const char*, unsigned int, CColModelSAInterface*, const char*);
typedef void                (__cdecl *LoadCollisionModelVer3_t)         (const char*, unsigned int, CColModelSAInterface*, const char*); // buf, bufsize, ccolmodel&, keyname
typedef void                (__cdecl *LoadCollisionModelVer4_t)         (const char*, unsigned int, CColModelSAInterface*, const char*);    // undocumented?
typedef RwTexture*          (__cdecl *CClothesBuilder_CopyTexture_t)    (RwTexture *texture);

RWFUNC ( LoadCollisionModel_t           LoadCollisionModel                , (LoadCollisionModel_t)              invalid_ptr )
RWFUNC ( LoadCollisionModelVer2_t       LoadCollisionModelVer2            , (LoadCollisionModelVer2_t)          invalid_ptr )
RWFUNC ( LoadCollisionModelVer3_t       LoadCollisionModelVer3            , (LoadCollisionModelVer3_t)          invalid_ptr )
RWFUNC ( LoadCollisionModelVer4_t       LoadCollisionModelVer4            , (LoadCollisionModelVer4_t)          invalid_ptr )
RWFUNC ( CClothesBuilder_CopyTexture_t  CClothesBuilder_CopyTexture       , (CClothesBuilder_CopyTexture_t)     invalid_ptr )

#endif //__GAMESA_RENDERWARE