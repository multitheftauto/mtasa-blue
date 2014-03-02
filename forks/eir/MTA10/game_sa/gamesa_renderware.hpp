/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/gamesa_renderware.hpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

void InitRwFunctions( eGameVersion version )
{
    // Version dependant addresses
    switch ( version )
    {
    // VERSION 1.0 EU ADDRESSES
    case VERSION_EU_10:
        // Utility functions
        RwDeviceSystemRequest               = (RwDeviceSystemRequest_t)                 0x007F2AF0;
        RwPrefetch                          = (RwPrefetch_t)                            0x0072F480;
        RwFlushLoader                       = (RwFlushLoader_t)                         0x0072E700;
        RwErrorGet                          = (RwErrorGet_t)                            0x008088C0;
        RwAllocAligned                      = (RwAllocAligned_t)                        0x0072F4C0;
        RwFreeAligned                       = (RwFreeAligned_t)                         0x0072F4F0;
        RwPluginRegistryReadDataChunks      = (RwPluginRegistryReadDataChunks_t)        0x008089C0;
        RwCreateExtension                   = (RwCreateExtension_t)                     0x007CCE80;

        RwIm3DTransform                     = (RwIm3DTransform_t)                       0x007EF490;
        RwIm3DRenderIndexedPrimitive        = (RwIm3DRenderIndexedPrimitive_t)          0x007EF590;
        RwIm3DEnd                           = (RwIm3DEnd_t)                             0x007EF560;

        // Vector functions
        RwV3dNormalize                      = (RwV3dNormalize_t)                        0x007ED9F0;
        RwV3dTransformVector                = (RwV3dTransformVector_t)                  0x007EDE00;

        // Matrix functions
        RwMatrixCreate                      = (RwMatrixCreate_t)                        0x007F2A90;
        RwMatrixInvert                      = (RwMatrixInvert_t)                        0x007F20B0;
        RwMatrixTranslate                   = (RwMatrixTranslate_t)                     0x007F2490;
        RwMatrixScale                       = (RwMatrixScale_t)                         0x007F2300;
        RwMatrixUnknown                     = (RwMatrixUnknown_t)                       0x007326D0;

        // Object functions
        RwObjectRegister                    = (RwObjectRegister_t)                      0x00808720;

        // Stream functions
        RwStreamReadChunkHeaderInfo         = (RwStreamReadChunkHeaderInfo_t)           0x007ED5D0;
        RwStreamFindChunk                   = (RwStreamFindChunk_t)                     0x007ED310;

        // Frame functions
        RwFrameCreate                       = (RwFrameCreate_t)                         0x007F0450;
        RwFrameCloneRecursive               = (RwFrameCloneRecursive_t)                 0x007F0090;
        RwFrameCloneHierarchy               = (RwFrameCloneHierarchy_t)                 0x007F0290;
        RwFrameSetIdentity                  = (RwFrameSetIdentity_t)                    0x007F10F0;
        RwFrameAddChild                     = (RwFrameAddChild_t)                       0x007F0B40;
        RwFrameRemoveChild                  = (RwFrameRemoveChild_t)                    0x007F0D10; 
        RwFrameForAllObjects                = (RwFrameForAllObjects_t)                  0x007F1240; 
        RwFrameGetLTM                       = (RwFrameGetLTM_t)                         0x007F09D0;
        RwFrameTranslate                    = (RwFrameTranslate_t)                      0x007F0E70;
        RwFrameOrient                       = (RwFrameOrient_t)                         0x007F2010;
        RwFrameScale                        = (RwFrameScale_t)                          0x007F0F10;
        RwFrameDestroy                      = (RwFrameDestroy_t)                        0x007F04B0;
        RwFrameRegisterPlugin               = (RwFrameRegisterPlugin_t)                 0x007F12A0;
        RwFrameRegisterPluginStream         = (RwFrameRegisterPluginStream_t)           0x008075C0;

        // Material functions
        RpMaterialCreate                    = (RpMaterialCreate_t)                      0x0074D9E0;
        RpMaterialDestroy                   = (RpMaterialDestroy_t)                     0x0074DA70;
        RpMaterialRegisterPlugin            = (RpMaterialRegisterPlugin_t)              0x0074DC40;
        RpMaterialRegisterPluginStream      = (RpMaterialRegisterPluginStream_t)        0x0074DC70;

        // Geometry functions
        RpGeometryCreate                    = (RpGeometryCreate_t)                      0x0074CAE0;
        RpGeometryGetSkeleton               = (RpGeometryGetSkeleton_t)                 0x007C7590;
        RpGeometryTriangleSetVertexIndices  = (RpGeometryTriangleSetVertexIndices_t)    0x0074C6E0;
        RpGeometryTriangleSetMaterial       = (RpGeometryTriangleSetMaterial_t)         0x0074C710;
        RpGeometryUnlock                    = (RpGeometryUnlock_t)                      0x0074C850;
        RpGeometryLock                      = (RpGeometryLock_t)                        0x0074C820;
        RpGeometryTransform                 = (RpGeometryTransform_t)                   0x0074C030;
        RpGeometryRegisterPlugin            = (RpGeometryRegisterPlugin_t)              0x0074CDC0;
        RpGeometryDestroy                   = (RpGeometryDestroy_t)                     0x0074CD10;
        RpGeometryRegisterPlugin            = (RpGeometryRegisterPlugin_t)              0x0074CDC0;
        RpGeometryRegisterPluginStream      = (RpGeometryRegisterPluginStream_t)        0x0074CDF0;

        // Atomic functions
        RpAtomicCreate                      = (RpAtomicCreate_t)                        0x00749CA0;
        RpAtomicClone                       = (RpAtomicClone_t)                         0x00749EB0;
        RpAtomicSetFrame                    = (RpAtomicSetFrame_t)                      0x0074BF70;
        RpAtomicSetGeometry                 = (RpAtomicSetGeometry_t)                   0x00749D90;
        RpAtomicGetWorldBoundingSphere      = (RpAtomicGetWorldBoundingSphere_t)        0x00749380;
        RpAtomicSetupObjectPipeline         = (RpAtomicSetupObjectPipeline_t)           0x005D7F00;
        RpAtomicSetupVehiclePipeline        = (RpAtomicSetupVehiclePipeline_t)          0x005D5B20;
        RpAtomicRender                      = (RpAtomicRender_t)                        0x00749210;
        RpAtomicDestroy                     = (RpAtomicDestroy_t)                       0x00749E10;
        RpAtomicRegisterPlugin              = (RpAtomicRegisterPlugin_t)                0x0074BDF0;
        RpAtomicRegisterPluginStream        = (RpAtomicRegisterPluginStream_t)          0x0074BE50;

        // Light functions
        RpLightSetRadius                    = (RpLightSetRadius_t)                      0x00751AC0;
        RpLightSetColor                     = (RpLightSetColor_t)                       0x00751AE0;
        RpLightGetConeAngle                 = (RpLightGetConeAngle_t)                   0x00751B30;
        RpLightDestroy                      = (RpLightDestroy_t)                        0x00752120;
        RpLightRegisterPlugin               = (RpLightRegisterPlugin_t)                 0x00751DB0;
        RpLightRegisterPluginStream         = (RpLightRegisterPluginStream_t)           0x00751DE0;

        // Camera functions
        RwCameraClone                       = (RwCameraClone_t)                         0x007EF3F0;
        RwCameraDestroy                     = (RwCameraDestroy_t)                       0x007EE4F0;
        RwCameraRegisterPlugin              = (RwCameraRegisterPlugin_t)                0x007EE490;
        RwCameraRegisterPluginStream        = (RwCameraRegisterPluginStream_t)          0x00808CD0;

        // Clump functions
        RpClumpClone                        = (RpClumpClone_t)                          0x00749FC0;
        RpClumpAddAtomic                    = (RpClumpAddAtomic_t)                      0x0074A4E0;
        RpClumpRemoveAtomic                 = (RpClumpRemoveAtomic_t)                   0x0074A510;
        RpClumpAddLight                     = (RpClumpAddLight_t)                       0x0074A540;
        RpClumpGetNumAtomics                = (RpClumpGetNumAtomics_t)                  0x00749930;
        RpClumpForAllAtomics                = (RpClumpForAllAtomics_t)                  0x00749BC0;
        RpClumpGetBoneTransform             = (RpClumpGetBoneTransform_t)               0x00735360;
        RpClumpSetupFrameCallback           = (RpClumpSetupFrameCallback_t)             0x00733750;
        RpClumpStreamRead                   = (RpClumpStreamRead_t)                     0x0074B470;
        RpClumpDestroy                      = (RpClumpDestroy_t)                        0x0074A360;
        RpClumpRegisterPlugin               = (RpClumpRegisterPlugin_t)                 0x0074BE20;
        RpClumpRegisterPluginStream         = (RpClumpRegisterPluginStream_t)           0x0074BEC0;

        // Raster functions
        RwRasterCreate                      = (RwRasterCreate_t)                        0x007FB270;
        RwRasterUnlock                      = (RwRasterUnlock_t)                        0x007FAF00;
        RwRasterLock                        = (RwRasterLock_t)                          0x007FB310;
        RwRasterDestroy                     = (RwRasterDestroy_t)                       0x007FB060;
        RwRasterRegisterPlugin              = (RwRasterRegisterPlugin_t)                0x007FB0F0;

        // Texture functions
        RwTextureCreate                     = (RwTextureCreate_t)                       0x007F3800;
        RwTextureDestroy                    = (RwTextureDestroy_t)                      0x007F3860;
        _RpD3D9SetTexture                   = (RpD3D9SetTexture_t)                      0x007FDEB0;
        RwTextureRegisterPlugin             = (RwTextureRegisterPlugin_t)               0x007F3BF0;
        RwTextureRegisterPluginStream       = (RwTextureRegisterPluginStream_t)         0x00804590;

        // TexDictionary functions
        RwTexDictionaryAddTexture           = (RwTexDictionaryAddTexture_t)             0x007F39C0;
        RwTexDictionaryGetCurrent           = (RwTexDictionaryGetCurrent_t)             0x007F3AD0;
        RwTexDictionarySetCurrent           = (RwTexDictionarySetCurrent_t)             0x007F3AB0;
        RwTexDictionaryForAllTextures       = (RwTexDictionaryForAllTextures_t)         0x007F3770;
        RwTexDictionaryFindNamedTexture     = (RwTexDictionaryFindNamedTexture_t)       0x007F3A30;
        RwTexDictionaryStreamRead           = (RwTexDictionaryStreamRead_t)             0x00804C70; 
        RwTexDictionaryGtaStreamRead        = (RwTexDictionaryGtaStreamRead_t)          0x00730FC0;
        RwTexDictionaryDestroy              = (RwTexDictionaryDestroy_t)                0x007F36E0;
        RwTexDictionaryRegisterPlugin       = (RwTexDictionaryRegisterPlugin_t)         0x007F3C50;
        RwTexDictionaryRegisterPluginStream = (RwTexDictionaryRegisterPluginStream_t)   0x00804920;

        // Scene functions
        RwSceneRender                       = (RwSceneRender_t)                         0x0074F5C0;
        RwSceneAddAtomic                    = (RwSceneAddAtomic_t)                      0x00750FE0;
        RwSceneAddClump                     = (RwSceneAddClump_t)                       0x00751350;
        RwSceneAddLight                     = (RwSceneAddLight_t)                       0x00751960;
        RwSceneRemoveLight                  = (RwSceneRemoveLight_t)                    0x007519B0;
        RwSceneDestroy                      = (RwSceneDestroy_t)                        0x0074F660;
        RwSceneRegisterPlugin               = (RwSceneRegisterPlugin_t)                 0x0074FD20;
        RwSceneRegisterPluginStream         = (RwSceneRegisterPluginStream_t)           0x0074FD50;

        // Dict functions
        RtDictSchemaStreamReadDict          = (RtDictSchemaStreamReadDict_t)            0x007CF280;
        RtDictDestroy                       = (RtDictDestroy_t)                         0x007CF170;

        // Animation functions
        RwAnimationInit                     = (RwAnimationInit_t)                       0x007CD5E0;
        RwSkeletonUpdate                    = (RwSkeletonUpdate_t)                      0x007C51D0;
        break;

    // VERSION 1.0 US ADDRESSES
    case VERSION_US_10:
        // Utility functions
        RwDeviceSystemRequest               = (RwDeviceSystemRequest_t)                 0x007F2AB0;
        RwAllocAligned                      = (RwAllocAligned_t)                        0x0072F4C0;
        RwFreeAligned                       = (RwFreeAligned_t)                         0x0072F4F0;
        RwCreateExtension                   = (RwCreateExtension_t)                     0x007CCE80;
        RwErrorGet                          = (RwErrorGet_t)                            0x00808880;
        RwPrefetch                          = (RwPrefetch_t)                            0x0072F480;
        RwFlushLoader                       = (RwFlushLoader_t)                         0x0072E700;
        RwPluginRegistryReadDataChunks      = (RwPluginRegistryReadDataChunks_t)        0x00808980;

        RwIm3DTransform                     = (RwIm3DTransform_t)                       0x007EF450;
        RwIm3DRenderIndexedPrimitive        = (RwIm3DRenderIndexedPrimitive_t)          0x007EF550;
        RwIm3DEnd                           = (RwIm3DEnd_t)                             0x007EF520;

        // Vector functions
        RwV3dNormalize                      = (RwV3dNormalize_t)                        0x007ED9B0;
        RwV3dTransformVector                = (RwV3dTransformVector_t)                  0x007EDDC0;

        // Matrix functions
        RwMatrixCreate                      = (RwMatrixCreate_t)                        0x007F2A50;
        RwMatrixInvert                      = (RwMatrixInvert_t)                        0x007F2070;
        RwMatrixTranslate                   = (RwMatrixTranslate_t)                     0x007F2450;
        RwMatrixScale                       = (RwMatrixScale_t)                         0x007F22C0;
        RwMatrixUnknown                     = (RwMatrixUnknown_t)                       0x007326D0;

        // Object functions
        RwObjectRegister                    = (RwObjectRegister_t)                      0x008086E0;

        // Stream functions
        RwStreamFindChunk                   = (RwStreamFindChunk_t)                     0x007ED2D0;
        RwStreamReadChunkHeaderInfo         = (RwStreamReadChunkHeaderInfo_t)           0x007ED590;

        // Frame functions
        RwFrameCreate                       = (RwFrameCreate_t)                         0x007F0410;
        RwFrameCloneRecursive               = (RwFrameCloneRecursive_t)                 0x007F0050;
        RwFrameCloneHierarchy               = (RwFrameCloneHierarchy_t)                 0x007f0250;
        RwFrameAddChild                     = (RwFrameAddChild_t)                       0x007F0B00;
        RwFrameRemoveChild                  = (RwFrameRemoveChild_t)                    0x007F0CD0;
        RwFrameForAllObjects                = (RwFrameForAllObjects_t)                  0x007F1200;
        RwFrameSetIdentity                  = (RwFrameSetIdentity_t)                    0x007F10B0;
        RwFrameGetLTM                       = (RwFrameGetLTM_t)                         0x007F0990;
        RwFrameTranslate                    = (RwFrameTranslate_t)                      0x007F0E30;
        RwFrameScale                        = (RwFrameScale_t)                          0x007F0ED0;
        RwFrameOrient                       = (RwFrameOrient_t)                         0x007F2010;
        RwFrameDestroy                      = (RwFrameDestroy_t)                        0x007F05A0;
        RwFrameRegisterPlugin               = (RwFrameRegisterPlugin_t)                 0x007F1260;
        RwFrameRegisterPluginStream         = (RwFrameRegisterPluginStream_t)           0x00807580;

        // Material functions
        RpMaterialCreate                    = (RpMaterialCreate_t)                      0x0074D990;
        RpMaterialDestroy                   = (RpMaterialDestroy_t)                     0x0074DA20;
        RpMaterialRegisterPlugin            = (RpMaterialRegisterPlugin_t)              0x0074DBF0;
        RpMaterialRegisterPluginStream      = (RpMaterialRegisterPluginStream_t)        0x0074DC20;

        // Geometry functions
        RpGeometryCreate                    = (RpGeometryCreate_t)                      0x0074CA90;
        RpGeometryTransform                 = (RpGeometryTransform_t)                   0x0074BFE0;
        RpGeometryGetSkeleton               = (RpGeometryGetSkeleton_t)                 0x007C7550;
        RpGeometryTriangleSetVertexIndices  = (RpGeometryTriangleSetVertexIndices_t)    0x0074C690;
        RpGeometryTriangleSetMaterial       = (RpGeometryTriangleSetMaterial_t)         0x0074C6C0;
        RpGeometryUnlock                    = (RpGeometryUnlock_t)                      0x0074C800;
        RpGeometryLock                      = (RpGeometryLock_t)                        0x0074C7D0;
        RpGeometryDestroy                   = (RpGeometryDestroy_t)                     0x0074CCC0;
        RpGeometryRegisterPlugin            = (RpGeometryRegisterPlugin_t)              0x0074CD70;
        RpGeometryRegisterPluginStream      = (RpGeometryRegisterPluginStream_t)        0x0074CDA0;

        // Atomic functions
        RpAtomicCreate                      = (RpAtomicCreate_t)                        0x00749C50;
        RpAtomicClone                       = (RpAtomicClone_t)                         0x00749E60;
        RpAtomicSetFrame                    = (RpAtomicSetFrame_t)                      0x0074BF20;
        RpAtomicSetGeometry                 = (RpAtomicSetGeometry_t)                   0x00749D40;
        RpAtomicGetWorldBoundingSphere      = (RpAtomicGetWorldBoundingSphere_t)        0x00749330;
        RpAtomicSetupObjectPipeline         = (RpAtomicSetupObjectPipeline_t)           0x005D7F00;
        RpAtomicSetupVehiclePipeline        = (RpAtomicSetupVehiclePipeline_t)          0x005D5B20;
        RpAtomicRender                      = (RpAtomicRender_t)                        0x007491C0;
        RpAtomicDestroy                     = (RpAtomicDestroy_t)                       0x00749DC0;
        RpAtomicRegisterPlugin              = (RpAtomicRegisterPlugin_t)                0x0074BDA0;
        RpAtomicRegisterPluginStream        = (RpAtomicRegisterPluginStream_t)          0x0074BE00;

        // Light functions
        RpLightSetRadius                    = (RpLightSetRadius_t)                      0x00751A70;
        RpLightSetColor                     = (RpLightSetColor_t)                       0x00751A90;
        RpLightGetConeAngle                 = (RpLightGetConeAngle_t)                   0x00751AE0;
        RpLightDestroy                      = (RpLightDestroy_t)                        0x007520D0;
        RpLightRegisterPlugin               = (RpLightRegisterPlugin_t)                 0x00751D60;
        RpLightRegisterPluginStream         = (RpLightRegisterPluginStream_t)           0x00751D90;

        // Camera functions
        RwCameraClone                       = (RwCameraClone_t)                         0x007EF3B0;
        RwCameraDestroy                     = (RwCameraDestroy_t)                       0x007EE4B0;
        RwCameraRegisterPlugin              = (RwCameraRegisterPlugin_t)                0x007EE450;
        RwCameraRegisterPluginStream        = (RwCameraRegisterPluginStream_t)          0x00808C90;

        // Clump functions
        RpClumpClone                        = (RpClumpClone_t)                          0x00749F70;
        RpClumpAddAtomic                    = (RpClumpAddAtomic_t)                      0x0074A490;
        RpClumpRemoveAtomic                 = (RpClumpRemoveAtomic_t)                   0x0074A4C0;
        RpClumpAddLight                     = (RpClumpAddLight_t)                       0x0074A4F0;
        RpClumpForAllAtomics                = (RpClumpForAllAtomics_t)                  0x00749B70;
        RpClumpGetNumAtomics                = (RpClumpGetNumAtomics_t)                  0x007498E0;
        RpClumpGetBoneTransform             = (RpClumpGetBoneTransform_t)               0x00735360;
        RpClumpSetupFrameCallback           = (RpClumpSetupFrameCallback_t)             0x00733750;
        RpClumpStreamRead                   = (RpClumpStreamRead_t)                     0x0074B420;
        RpClumpDestroy                      = (RpClumpDestroy_t)                        0x0074A310;
        RpClumpRegisterPlugin               = (RpClumpRegisterPlugin_t)                 0x0074bdd0;
        RpClumpRegisterPluginStream         = (RpClumpRegisterPluginStream_t)           0x0074BE70;

        // Raster functions
        RwRasterCreate                      = (RwRasterCreate_t)                        0x007FB230;
        RwRasterUnlock                      = (RwRasterUnlock_t)                        0x007FAEC0;
        RwRasterLock                        = (RwRasterLock_t)                          0x007FB2D0;
        RwRasterDestroy                     = (RwRasterDestroy_t)                       0x007FB020;
        RwRasterRegisterPlugin              = (RwRasterRegisterPlugin_t)                0x007FB0B0;

        // Texture functions
        RwTextureCreate                     = (RwTextureCreate_t)                       0x007F37C0;
        RwTextureDestroy                    = (RwTextureDestroy_t)                      0x007F3820;
        _RpD3D9SetTexture                   = (RpD3D9SetTexture_t)                      0x007FDE70;
        RwTextureRegisterPlugin             = (RwTextureRegisterPlugin_t)               0x007f3bb0;
        RwTextureRegisterPluginStream       = (RwTextureRegisterPluginStream_t)         0x00804550;

        // TexDictionary functions
        RwTexDictionaryAddTexture           = (RwTexDictionaryAddTexture_t)             0x007F3980;
        RwTexDictionaryGetCurrent           = (RwTexDictionaryGetCurrent_t)             0x007F3A90;
        RwTexDictionarySetCurrent           = (RwTexDictionarySetCurrent_t)             0x007F3A70;
        RwTexDictionaryForAllTextures       = (RwTexDictionaryForAllTextures_t)         0x007F3730;
        RwTexDictionaryFindNamedTexture     = (RwTexDictionaryFindNamedTexture_t)       0x007F39F0;
        RwTexDictionaryStreamRead           = (RwTexDictionaryStreamRead_t)             0x00804C30;
        RwTexDictionaryGtaStreamRead        = (RwTexDictionaryGtaStreamRead_t)          0x00730FC0;
        RwTexDictionaryDestroy              = (RwTexDictionaryDestroy_t)                0x007F36A0;
        RwTexDictionaryRegisterPlugin       = (RwTexDictionaryRegisterPlugin_t)         0x007F3C10;
        RwTexDictionaryRegisterPluginStream = (RwTexDictionaryRegisterPluginStream_t)   0x008048E0;

        // Scene functions
        RwSceneRender                       = (RwSceneRender_t)                         0x0074F570;
        RwSceneAddClump                     = (RwSceneAddClump_t)                       0x00751300;
        RwSceneAddLight                     = (RwSceneAddLight_t)                       0x00751910;
        RwSceneAddAtomic                    = (RwSceneAddAtomic_t)                      0x00750F90;
        RwSceneRemoveLight                  = (RwSceneRemoveLight_t)                    0x00751960;
        RwSceneDestroy                      = (RwSceneDestroy_t)                        0x0074F610;
        RwSceneRegisterPlugin               = (RwSceneRegisterPlugin_t)                 0x0074FCD0;
        RwSceneRegisterPluginStream         = (RwSceneRegisterPluginStream_t)           0x0074FD00;

        // Dict functions
        RtDictSchemaStreamReadDict          = (RtDictSchemaStreamReadDict_t)            0x007CF240;
        RtDictDestroy                       = (RtDictDestroy_t)                         0x007CF130;

        // Animation functions
        RwAnimationInit                     = (RwAnimationInit_t)                       0x007CD5E0;
        RwSkeletonUpdate                    = (RwSkeletonUpdate_t)                      0x007C5210;
        break;
    }

    // Shared addresses
    LoadCollisionModel                  = (LoadCollisionModel_t)                    0x00537580;
    LoadCollisionModelVer2              = (LoadCollisionModelVer2_t)                0x00537EE0;
    LoadCollisionModelVer3              = (LoadCollisionModelVer3_t)                0x00537CE0;
    LoadCollisionModelVer4              = (LoadCollisionModelVer4_t)                0x00537AE0;
    CTxdStore_LoadTxd                   = (CTxdStore_LoadTxd_t)                     0x00731DD0;
    CTxdStore_GetTxd                    = (CTxdStore_GetTxd_t)                      0x00408340;
    CTxdStore_RemoveTxd                 = (CTxdStore_RemoveTxd_t)                   0x00731E90;
    CTxdStore_RemoveRef                 = (CTxdStore_RemoveRef_t)                   0x00731A30;
    CTxdStore_AddRef                    = (CTxdStore_AddRef_t)                      0x00731A00;
    CClothesBuilder_CopyTexture         = (CClothesBuilder_CopyTexture_t)           0x005A5730;
}
