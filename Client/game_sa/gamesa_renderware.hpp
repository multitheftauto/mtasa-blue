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

void InitRwFunctions()
{
    RwStreamFindChunk = (RwStreamFindChunk_t)0x007ED2D0;
    RpClumpStreamRead = (RpClumpStreamRead_t)0x0074B420;
    RwErrorGet = (RwErrorGet_t)0x00808880;
    RwStreamOpen = (RwStreamOpen_t)0x007ECEF0;
    RwStreamClose = (RwStreamClose_t)0x007ECE20;
    RwStreamRead = (RwStreamRead_t)0x007EC9D0;
    RwStreamSkip = (RwStreamSkip_t)0x007ECD00;
    RpClumpDestroy = (RpClumpDestroy_t)0x0074A310;
    RpClumpGetNumAtomics = (RpClumpGetNumAtomics_t)0x007498E0;
    RwFrameTransform = (RwFrameTransform_t)0x007F0F70;
    RwFrameTranslate = (RwFrameTranslate_t)0x007F0E30;
    RpClumpForAllAtomics = (RpClumpForAllAtomics_t)0x00749B70;
    RwFrameAddChild = (RwFrameAddChild_t)0x007F0B00;
    RpClumpAddAtomic = (RpClumpAddAtomic_t)0x0074A490;
    RpAtomicSetFrame = (RpAtomicSetFrame_t)0x0074BF20;
    RwTexDictionaryStreamRead = (RwTexDictionaryStreamRead_t)0x00804C30;
    RwTexDictionaryGtaStreamRead = (RwTexDictionaryGtaStreamRead_t)0x00730FC0;
    RwTexDictionaryGetCurrent = (RwTexDictionaryGetCurrent_t)0x007F3A90;
    RwTexDictionarySetCurrent = (RwTexDictionarySetCurrent_t)0x007F3A70;
    RwTexDictionaryForAllTextures = (RwTexDictionaryForAllTextures_t)0x007F3730;
    RwTexDictionaryAddTexture = (RwTexDictionaryAddTexture_t)0x007F3980;
    RwTexDictionaryStreamWrite = (RwTexDictionaryStreamWrite_t)0x008049F0;
    rwD3D9NativeTextureRead = (rwD3D9NativeTextureRead_t)0x004CD820;
    RpPrtStdGlobalDataSetStreamEmbedded = (RpPrtStdGlobalDataSetStreamEmbedded_t)0x0041B350;
    RpClumpRender = (RpClumpRender_t)0x00749B20;
    RpClumpRemoveAtomic = (RpClumpRemoveAtomic_t)0x0074A4C0;
    RpAtomicClone = (RpAtomicClone_t)0x00749E60;
    RwTexDictionaryFindNamedTexture = (RwTexDictionaryFindNamedTexture_t)0x007F39F0;
    RwFrameRemoveChild = (RwFrameRemoveChild_t)0x007F0CD0;
    RwFrameForAllObjects = (RwFrameForAllObjects_t)0x007F1200;
    RpAtomicDestroy = (RpAtomicDestroy_t)0x00749DC0;
    RpAtomicSetGeometry = (RpAtomicSetGeometry_t)0x00749D40;
    RpWorldAddAtomic = (RpWorldAddAtomic_t)0x00750F90;
    RpGeometryCreate = (RpGeometryCreate_t)0x0074CA90;
    RpGeometryTriangleSetVertexIndices = (RpGeometryTriangleSetVertexIndices_t)0x0074C690;
    RpGeometryUnlock = (RpGeometryUnlock_t)0x0074C800;
    RpGeometryLock = (RpGeometryLock_t)0x0074C7D0;
    RpAtomicCreate = (RpAtomicCreate_t)0x00749C50;
    RwFrameCreate = (RwFrameCreate_t)0x007F0410;
    RwFrameDestroy = (RwFrameDestroy_t)0x007F05A0;
    RpGeometryTransform = (RpGeometryTransform_t)0x0074BFE0;
    RwFrameSetIdentity = (RwFrameSetIdentity_t)0x007F10B0;
    RwMatrixCreate = (RwMatrixCreate_t)0x007F2A50;
    RwMatrixTranslate = (RwMatrixTranslate_t)0x007F2450;
    RwMatrixScale = (RwMatrixScale_t)0x007F22C0;
    RpGeometryTriangleSetMaterial = (RpGeometryTriangleSetMaterial_t)0x0074C6C0;
    RpMaterialCreate = (RpMaterialCreate_t)0x0074D990;
    RpGeometryDestroy = (RpGeometryDestroy_t)0x0074CCC0;
    RpMaterialDestroy = (RpMaterialDestroy_t)0x0074DA20;
    RwV3dNormalize = (RwV3dNormalize_t)0x007ED9B0;
    RwIm3DTransform = (RwIm3DTransform_t)0x007EF450;
    RwIm3DRenderIndexedPrimitive = (RwIm3DRenderIndexedPrimitive_t)0x007EF550;
    RwIm3DEnd = (RwIm3DEnd_t)0x007EF520;
    RwMatrixInvert = (RwMatrixInvert_t)0x007F2070;
    RpWorldAddClump = (RpWorldAddClump_t)0x00751300;
    RwFrameScale = (RwFrameScale_t)0x007F0ED0;
    RwFrameUpdateObjects = (RwFrameUpdateObjects_t)0x7F0910;
    RwV3dTransformVector = (RwV3dTransformVector_t)0x007EDDC0;
    RpLightCreate = (RpLightCreate_t)0x00752110;
    RpClumpAddLight = (RpClumpAddLight_t)0x0074A4F0;
    _rwObjectHasFrameSetFrame = (_rwObjectHasFrameSetFrame_t)0x00804EF0;
    RpLightSetRadius = (RpLightSetRadius_t)0x00751A70;
    RpWorldAddLight = (RpWorldAddLight_t)0x00751910;
    RpLightSetColor = (RpLightSetColor_t)0x00751A90;
    RwCameraClone = (RwCameraClone_t)0x007EF3B0;
    RpClumpClone = (RpClumpClone_t)0x00749F70;
    RwTexDictionaryDestroy = (RwTexDictionaryDestroy_t)0x007F36A0;
    RwTextureDestroy = (RwTextureDestroy_t)0x007F3820;
    RwRasterUnlock = (RwRasterUnlock_t)0x007FAEC0;
    RwRasterLock = (RwRasterLock_t)0x007FB2D0;
    RwRasterCreate = (RwRasterCreate_t)0x007FB230;
    RwTextureCreate = (RwTextureCreate_t)0x007F37C0;
    RpMaterialSetTexture = (RpMaterialSetTexture_t)0x0074DBC0;
    GetAnimHierarchyFromClump = (GetAnimHierarchyFromClump_t)0x734B10;
    GetAnimHierarchyFromSkinClump = (GetAnimHierarchyFromSkinClump_t)0x734A40;
    RpHAnimIDGetIndex = (RpHAnimIDGetIndex_t)0x7C51A0;
    RpHAnimHierarchyGetMatrixArray = (RpHAnimHierarchyGetMatrixArray_t)0x7C5120;
    RtQuatRotate = (RtQuatRotate_t)0x7EB7C0;
    
    SetTextureDict = (SetTextureDict_t)0x007319C0;
    LoadClumpFile = (LoadClumpFile_t)0x005371F0;
    LoadModel = (LoadModel_t)0x0040C6B0;
    LoadCollisionModel = (LoadCollisionModel_t)0x00537580;
    LoadCollisionModelVer2 = (LoadCollisionModelVer2_t)0x00537EE0;
    LoadCollisionModelVer3 = (LoadCollisionModelVer3_t)0x00537CE0;
    CTxdStore_LoadTxd = (CTxdStore_LoadTxd_t)0x00731DD0;
    CTxdStore_GetTxd = (CTxdStore_GetTxd_t)0x00408340;
    CTxdStore_RemoveTxd = (CTxdStore_RemoveTxd_t)0x00731E90;
    CTxdStore_RemoveRef = (CTxdStore_RemoveRef_t)0x00731A30;
    CTxdStore_AddRef = (CTxdStore_AddRef_t)0x00731A00;
    CTxdStore_GetNumRefs = (CTxdStore_GetNumRefs_t)0x00731AA0;
    CClothesBuilder_CopyTexture = (CClothesBuilder_CopyTexture_t)0x005A5730;
}
