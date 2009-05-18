/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.cpp
*  PURPOSE:     RenderWare mapping to Grand Theft Auto: San Andreas
*               and miscellaneous rendering functions
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               arc_
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*  RenderWare is © Criterion Software
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CGameSA * pGame;

// RwFrameForAllObjects struct and callback used to replace dynamic vehicle parts
struct SReplaceParts
{
	const char *szName;                         // name of the part you want to replace (e.g. 'door_lf' or 'door_rf')
	unsigned char ucIndex;                      // index counter for internal usage (0 is the 'ok' part model, 1 is the 'dam' part model)
	RpAtomicContainer *pReplacements;		    // replacement atomics
	unsigned int uiReplacements;                // number of replacements
};
static RwObject* ReplacePartsCB ( RwObject * object, SReplaceParts * data )
{
	RpAtomic * Atomic = (RpAtomic*) object;
	char szAtomicName[16] = {0};

	// iterate through our loaded atomics
	for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
		// get the correct atomic name based on the object # in our parent frame
		if ( data->ucIndex == 0 )
			_snprintf ( &szAtomicName[0], 16, "%s_ok", data->szName );
		else
			_snprintf ( &szAtomicName[0], 16, "%s_dam", data->szName );

		// see if we have such an atomic in our replacement atomics array
		if ( strncmp ( &data->pReplacements[i].szName[0], &szAtomicName[0], 16 ) == 0 ) {
			// if so, override the geometry
			RpAtomicSetGeometry ( Atomic, data->pReplacements[i].atomic->geometry, 0 );

			// and copy the matrices
			RwFrameCopyMatrix ( RpGetFrame ( Atomic ), RpGetFrame ( data->pReplacements[i].atomic ) );

			object = (RwObject*) data->pReplacements[i].atomic;
			data->ucIndex++;
		}
	}

	return object;
}

// RpClumpForAllAtomics callback used to add atomics to a vehicle
static RpAtomic* AddAllAtomicsCB (RpAtomic * atomic, RpClump * data)
{
	RwFrame * pFrame = RpGetFrame ( data );

	// add the atomic to the frame
	RpAtomicSetFrame ( atomic, pFrame );
	RpClumpAddAtomic ( data, atomic );

	return atomic;
}

// RpClumpForAllAtomics struct and callback used to replace all wheels with a given wheel model
struct SReplaceWheels
{
	const char *szName;                         // name of the new wheel model
	RpClump *pClump;                            // the vehicle's clump
	RpAtomicContainer *pReplacements;           // replacement atomics
	unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* ReplaceWheelsCB (RpAtomic * atomic, SReplaceWheels * data)
{
	RwFrame * Frame = RpGetFrame ( atomic );

	// find our wheel atomics
	if ( strncmp ( &Frame->szName[0], "wheel_lf_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rf_dummy", 16 ) == 0 || 
		 strncmp ( &Frame->szName[0], "wheel_lm_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rm_dummy", 16 ) == 0 || 
		 strncmp ( &Frame->szName[0], "wheel_lb_dummy", 16 ) == 0 || strncmp ( &Frame->szName[0], "wheel_rb_dummy", 16 ) == 0 )
	{
		// find a replacement atomic
		for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
			// see if it's name is equal to the specified wheel
			if ( strncmp ( &data->pReplacements[i].szName[0], data->szName, 16 ) == 0 ) {
				// clone our wheel atomic
				RpAtomic * WheelAtomic = RpAtomicClone ( data->pReplacements[i].atomic );
				RpAtomicSetFrame ( WheelAtomic, Frame );

				// add it to the clump
				RpClumpAddAtomic ( data->pClump, WheelAtomic );

				// delete the current atomic
				RpClumpRemoveAtomic ( data->pClump, atomic );
			}
		}
	}

	return atomic;
}

// RpClumpForAllAtomics struct and callback used to replace all atomics for a vehicle
struct SReplaceAll
{
	RpClump *pClump;                            // the vehicle's clump
	RpAtomicContainer *pReplacements;           // replacement atomics
	unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* ReplaceAllCB (RpAtomic * atomic, SReplaceAll * data)
{
	RwFrame * Frame = RpGetFrame ( atomic );
	if ( Frame == NULL ) return atomic;

	// find a replacement atomic
	for ( unsigned int i = 0; i < data->uiReplacements; i++ ) {
		// see if we can find an atomic with the same name
		if ( strncmp ( &data->pReplacements[i].szName[0], &Frame->szName[0], 16 ) == 0 ) {
			// copy the matrices
			RwFrameCopyMatrix ( RpGetFrame ( atomic ), RpGetFrame ( data->pReplacements[i].atomic ) );

			// set the new atomic's frame to the current one
			RpAtomicSetFrame ( data->pReplacements[i].atomic, Frame );

			// override all engine and material related callbacks and pointers
			data->pReplacements[i].atomic->renderCallback	= atomic->renderCallback;
			data->pReplacements[i].atomic->frame			= atomic->frame;
			data->pReplacements[i].atomic->render			= atomic->render;
			data->pReplacements[i].atomic->interpolation	= atomic->interpolation;
			data->pReplacements[i].atomic->info 			= atomic->info;

			// add the new atomic to the vehicle clump
			RpClumpAddAtomic ( data->pClump, data->pReplacements[i].atomic );

			// remove the current atomic
			RpClumpRemoveAtomic ( data->pClump, atomic );
		}
	}

	return atomic;
}

// RpClumpForAllAtomics struct and callback used to load the atomics from a specific clump into a container
struct SLoadAtomics
{
	RpAtomicContainer *pReplacements;           // replacement atomics
	unsigned int uiReplacements;                // number of replacements
};
static RpAtomic* LoadAtomicsCB (RpAtomic * atomic, SLoadAtomics * data)
{
	RwFrame * Frame = RpGetFrame(atomic);

	// add the atomic to the container
	data->pReplacements [ data->uiReplacements ].atomic = atomic;
	strncpy ( &data->pReplacements [ data->uiReplacements ].szName[0], &Frame->szName[0], 16 );

	// and increment the counter
	data->uiReplacements++;

	return atomic;
}

CRenderWareSA::CRenderWareSA ( eGameVersion version )
{
    // Version dependant addresses
    switch ( version )
    {
        // VERSION 1.0 EU ADDRESSES
        case VERSION_EU_10:
        {
            RwStreamFindChunk					= (RwStreamFindChunk_t)						0x007ED310;
            RpClumpStreamRead					= (RpClumpStreamRead_t)						0x0074B470;
            RwErrorGet							= (RwErrorGet_t)							0x008088C0;
            RwStreamOpen						= (RwStreamOpen_t)							0x007ECF30;
            RwStreamClose						= (RwStreamClose_t)							0x007ECE60;
            RpClumpDestroy						= (RpClumpDestroy_t)						0x0074A360;
            RpClumpGetNumAtomics				= (RpClumpGetNumAtomics_t)					0x00749930;
            RwFrameTranslate					= (RwFrameTranslate_t)						0x007F0E70;
            RpClumpForAllAtomics				= (RpClumpForAllAtomics_t)					0x00749BC0;
            RwFrameAddChild						= (RwFrameAddChild_t)						0x007F0B40;
            RpClumpAddAtomic					= (RpClumpAddAtomic_t)						0x0074A4E0;
            RpAtomicSetFrame					= (RpAtomicSetFrame_t)						0x0074BF70;
            RwTexDictionaryStreamRead		    = (RwTexDictionaryStreamRead_t)				0x00804C70; 
            RwTexDictionaryGetCurrent			= (RwTexDictionaryGetCurrent_t)				0x007F3AD0;
            RwTexDictionarySetCurrent		    = (RwTexDictionarySetCurrent_t)				0x007F3AB0;
            RwTexDictionaryForAllTextures	    = (RwTexDictionaryForAllTextures_t)			0x007F3770;
            RwTexDictionaryAddTexture		    = (RwTexDictionaryAddTexture_t)				0x007F39C0;
            RpPrtStdGlobalDataSetStreamEmbedded = (RpPrtStdGlobalDataSetStreamEmbedded_t)	0x0041B350;
            RpClumpRemoveAtomic					= (RpClumpRemoveAtomic_t)					0x0074A510;
            RpAtomicClone						= (RpAtomicClone_t)							0x00749EB0; 
            RwTexDictionaryFindNamedTexture		= (RwTexDictionaryFindNamedTexture_t)		0x007F3A30;
            RwFrameRemoveChild					= (RwFrameRemoveChild_t)					0x007F0D10; 
            RwFrameForAllObjects				= (RwFrameForAllObjects_t)					0x007F1240; 
            RpAtomicDestroy						= (RpAtomicDestroy_t)						0x00749E10; 
            RpAtomicSetGeometry					= (RpAtomicSetGeometry_t)					0x00749D90;
            RpWorldAddAtomic					= (RpWorldAddAtomic_t)						0x00750FE0;
            RpGeometryCreate					= (RpGeometryCreate_t)						0x0074CAE0;
            RpGeometryTriangleSetVertexIndices	= (RpGeometryTriangleSetVertexIndices_t)	0x0074C6E0;
            RpGeometryUnlock					= (RpGeometryUnlock_t)						0x0074C850;
            RpGeometryLock						= (RpGeometryLock_t)						0x0074C820;
            RpAtomicCreate						= (RpAtomicCreate_t)						0x00749CA0;
            RwFrameCreate						= (RwFrameCreate_t)							0x007F0450;
            RpGeometryTransform					= (RpGeometryTransform_t)					0x0074C030;
            RwFrameSetIdentity					= (RwFrameSetIdentity_t)					0x007F10F0;
            RwMatrixCreate						= (RwMatrixCreate_t)						0x007F2A90; 
            RwMatrixTranslate					= (RwMatrixTranslate_t)						0x007F2490;
            RwMatrixScale						= (RwMatrixScale_t)							0x007F2300;
            RpGeometryTriangleSetMaterial		= (RpGeometryTriangleSetMaterial_t)			0x0074C710;
            RpMaterialCreate					= (RpMaterialCreate_t)						0x0074D9E0;
            RpGeometryDestroy					= (RpGeometryDestroy_t)						0x0074CD10;
            RpMaterialDestroy					= (RpMaterialDestroy_t)						0x0074DA70;
            RwV3dNormalize						= (RwV3dNormalize_t)						0x007ED9F0;
            RwIm3DTransform						= (RwIm3DTransform_t)						0x007EF490; 
            RwIm3DRenderIndexedPrimitive		= (RwIm3DRenderIndexedPrimitive_t)			0x007EF590;
            RwIm3DEnd							= (RwIm3DEnd_t)								0x007EF560;
            RwMatrixInvert						= (RwMatrixInvert_t)						0x007F20B0;
            RpWorldAddClump						= (RpWorldAddClump_t)						0x00751350;
            RwFrameScale						= (RwFrameScale_t)							0x007F0F10;
            RwV3dTransformVector				= (RwV3dTransformVector_t)					0x007EDE00;
            RpLightCreate						= (RpLightCreate_t)							0x00752160;
            RpClumpAddLight						= (RpClumpAddLight_t)						0x0074A540;
            _rwObjectHasFrameSetFrame			= (_rwObjectHasFrameSetFrame_t)				0x00804F30;
            RpLightSetRadius					= (RpLightSetRadius_t)						0x00751AC0;
            RpWorldAddLight						= (RpWorldAddLight_t)						0x00751960;
            RpLightSetColor						= (RpLightSetColor_t)						0x00751AE0;
            RwCameraClone						= (RwCameraClone_t)							0x007EF3F0;
            RpClumpClone						= (RpClumpClone_t)							0x00749FC0;
            RwTexDictionaryDestroy				= (RwTexDictionaryDestroy_t)				0x007F36E0;
            RwTextureDestroy					= (RwTextureDestroy_t)						0x007F3860;
            RwRasterUnlock						= (RwRasterUnlock_t)						0x007FAF00;
            RwRasterLock						= (RwRasterLock_t)							0x007FB310; 
            RwRasterCreate						= (RwRasterCreate_t)						0x007FB270;
            RwTextureCreate						= (RwTextureCreate_t)						0x007F3800;

            SetTextureDict					    = (SetTextureDict_t)				        0x007319C0;
            LoadClumpFile					    = (LoadClumpFile_t)					        0x005371F0;
            LoadModel						    = (LoadModel_t)						        0x0040C6B0;
            LoadCollisionModel	    		    = (LoadCollisionModel_t)    		        0x00537580;
            LoadCollisionModelVer2			    = (LoadCollisionModelVer2_t)		        0x00537EE0;
            LoadCollisionModelVer3			    = (LoadCollisionModelVer3_t)		        0x00537CE0;
            CTxdStore_LoadTxd				    = (CTxdStore_LoadTxd_t)				        0x00731DD0;
            CTxdStore_GetTxd				    = (CTxdStore_GetTxd_t)				        0x00408340;
            CTxdStore_RemoveTxd				    = (CTxdStore_RemoveTxd_t)			        0x00731E90;
            CTxdStore_RemoveRef				    = (CTxdStore_RemoveRef_t)			        0x00731A30;
            CTxdStore_AddRef				    = (CTxdStore_AddRef_t)				        0x00731A00; 
            CClothesBuilder_CopyTexture		    = (CClothesBuilder_CopyTexture_t)	        0x005A5730;

            break;
        }    

        // VERSION 1.0 US ADDRESSES
        case VERSION_US_10:
        {
            RwStreamFindChunk					= (RwStreamFindChunk_t)						0x007ED2D0;
            RpClumpStreamRead					= (RpClumpStreamRead_t)						0x0074B420;
            RwErrorGet							= (RwErrorGet_t)							0x00808880;
            RwStreamOpen						= (RwStreamOpen_t)							0x007ECEF0;
            RwStreamClose						= (RwStreamClose_t)							0x007ECE20;
            RpClumpDestroy						= (RpClumpDestroy_t)						0x0074A310;
            RpClumpGetNumAtomics				= (RpClumpGetNumAtomics_t)					0x007498E0;
            RwFrameTranslate					= (RwFrameTranslate_t)						0x007F0E30;
            RpClumpForAllAtomics				= (RpClumpForAllAtomics_t)					0x00749B70;
            RwFrameAddChild						= (RwFrameAddChild_t)						0x007F0B00;
            RpClumpAddAtomic					= (RpClumpAddAtomic_t)						0x0074A490;
            RpAtomicSetFrame					= (RpAtomicSetFrame_t)						0x0074BF20;
            RwTexDictionaryStreamRead		    = (RwTexDictionaryStreamRead_t)				0x00804C30;
            RwTexDictionaryGetCurrent			= (RwTexDictionaryGetCurrent_t)				0x007F3A90;
            RwTexDictionarySetCurrent		    = (RwTexDictionarySetCurrent_t)				0x007F3A70;
            RwTexDictionaryForAllTextures	    = (RwTexDictionaryForAllTextures_t)			0x007F3730;
            RwTexDictionaryAddTexture		    = (RwTexDictionaryAddTexture_t)				0x007F3980;
            RpPrtStdGlobalDataSetStreamEmbedded = (RpPrtStdGlobalDataSetStreamEmbedded_t)	0x0041B350;
            RpClumpRemoveAtomic					= (RpClumpRemoveAtomic_t)					0x0074A4C0;
            RpAtomicClone						= (RpAtomicClone_t)							0x00749E60;
            RwTexDictionaryFindNamedTexture		= (RwTexDictionaryFindNamedTexture_t)		0x007F39F0;
            RwFrameRemoveChild					= (RwFrameRemoveChild_t)					0x007F0CD0;
            RwFrameForAllObjects				= (RwFrameForAllObjects_t)					0x007F1200;
            RpAtomicDestroy						= (RpAtomicDestroy_t)						0x00749DC0;
            RpAtomicSetGeometry					= (RpAtomicSetGeometry_t)					0x00749D40;
            RpWorldAddAtomic					= (RpWorldAddAtomic_t)						0x00750F90;
            RpGeometryCreate					= (RpGeometryCreate_t)						0x0074CA90;
            RpGeometryTriangleSetVertexIndices	= (RpGeometryTriangleSetVertexIndices_t)	0x0074C690;
            RpGeometryUnlock					= (RpGeometryUnlock_t)						0x0074C800;
            RpGeometryLock						= (RpGeometryLock_t)						0x0074C7D0;
            RpAtomicCreate						= (RpAtomicCreate_t)						0x00749C50;
            RwFrameCreate						= (RwFrameCreate_t)							0x007F0410;
            RpGeometryTransform					= (RpGeometryTransform_t)					0x0074BFE0;
            RwFrameSetIdentity					= (RwFrameSetIdentity_t)					0x007F10B0;
            RwMatrixCreate						= (RwMatrixCreate_t)						0x007F2A50;
            RwMatrixTranslate					= (RwMatrixTranslate_t)						0x007F2450;
            RwMatrixScale						= (RwMatrixScale_t)							0x007F22C0;
            RpGeometryTriangleSetMaterial		= (RpGeometryTriangleSetMaterial_t)			0x0074C6C0;
            RpMaterialCreate					= (RpMaterialCreate_t)						0x0074D990;
            RpGeometryDestroy					= (RpGeometryDestroy_t)						0x0074CCC0;
            RpMaterialDestroy					= (RpMaterialDestroy_t)						0x0074DA20;
            RwV3dNormalize						= (RwV3dNormalize_t)						0x007ED9B0;
            RwIm3DTransform						= (RwIm3DTransform_t)						0x007EF450;
            RwIm3DRenderIndexedPrimitive		= (RwIm3DRenderIndexedPrimitive_t)			0x007EF550;
            RwIm3DEnd							= (RwIm3DEnd_t)								0x007EF520;
            RwMatrixInvert						= (RwMatrixInvert_t)						0x007F2070;
            RpWorldAddClump						= (RpWorldAddClump_t)						0x00751300;
            RwFrameScale						= (RwFrameScale_t)							0x007F0ED0;
            RwV3dTransformVector				= (RwV3dTransformVector_t)					0x007EDDC0;
            RpLightCreate						= (RpLightCreate_t)							0x00752110;
            RpClumpAddLight						= (RpClumpAddLight_t)						0x0074A4F0;
            _rwObjectHasFrameSetFrame			= (_rwObjectHasFrameSetFrame_t)				0x00804EF0;
            RpLightSetRadius					= (RpLightSetRadius_t)						0x00751A70;
            RpWorldAddLight						= (RpWorldAddLight_t)						0x00751910;
            RpLightSetColor						= (RpLightSetColor_t)						0x00751A90;
            RwCameraClone						= (RwCameraClone_t)							0x007EF3B0;
            RpClumpClone						= (RpClumpClone_t)							0x00749F70;
            RwTexDictionaryDestroy				= (RwTexDictionaryDestroy_t)				0x007F36A0;
            RwTextureDestroy					= (RwTextureDestroy_t)						0x007F3820;
            RwRasterUnlock						= (RwRasterUnlock_t)						0x007FAEC0;
            RwRasterLock						= (RwRasterLock_t)							0x007FB2D0;
            RwRasterCreate						= (RwRasterCreate_t)						0x007FB230;
            RwTextureCreate						= (RwTextureCreate_t)						0x007F37C0;

            SetTextureDict					    = (SetTextureDict_t)				        0x007319C0;
            LoadClumpFile					    = (LoadClumpFile_t)					        0x005371F0;
            LoadModel						    = (LoadModel_t)						        0x0040C6B0;
            LoadCollisionModel	    		    = (LoadCollisionModel_t)    		        0x00537580;
            LoadCollisionModelVer2			    = (LoadCollisionModelVer2_t)		        0x00537EE0;
            LoadCollisionModelVer3			    = (LoadCollisionModelVer3_t)		        0x00537CE0;
            CTxdStore_LoadTxd				    = (CTxdStore_LoadTxd_t)				        0x00731DD0;
            CTxdStore_GetTxd				    = (CTxdStore_GetTxd_t)				        0x00408340;
            CTxdStore_RemoveTxd				    = (CTxdStore_RemoveTxd_t)			        0x00731E90;
            CTxdStore_RemoveRef				    = (CTxdStore_RemoveRef_t)			        0x00731A30;
            CTxdStore_AddRef				    = (CTxdStore_AddRef_t)				        0x00731A00;
            CClothesBuilder_CopyTexture		    = (CClothesBuilder_CopyTexture_t)	        0x005A5730;

            break;
        }
    }
}


///////////////////////////////////////////////////////////////////////////////



// Adds texture into the TXD of a model, eventually making a copy of each texture first 
void CRenderWareSA::ModelInfoTXDAddTextures ( std::list < RwTexture* >& textures, unsigned short usModelID, bool bMakeCopy, std::list < RwTexture* >* pReplacedTextures, std::list < RwTexture* >* pAddedTextures, bool bAddRef )
{
	// Get the CModelInfo's TXD ID
    unsigned short usTxdId = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;

    // Get the TXD corresponding to this ID
    SetTextureDict ( usTxdId );

    RwTexDictionary* pTXD = CTxdStore_GetTxd ( usTxdId );
    if ( bAddRef )
    {
        if ( !pTXD )
        {
            pGame->GetModelInfo ( usModelID )->Request ( true, true );
            CTxdStore_AddRef ( usTxdId );
            ( (void (__cdecl *)(unsigned short))FUNC_RemoveModel )( usModelID );
            pTXD = CTxdStore_GetTxd ( usTxdId );
        }
        else
        {
            CTxdStore_AddRef ( usTxdId );
        }
    }

	if ( pTXD )
    {
        std::list < RwTexture* >::iterator it;
        for ( it = textures.begin (); it != textures.end (); it++ )
        {
            // Does a texture by this name already exist?
	        RwTexture* pExistingTexture = RwTexDictionaryFindNamedTexture ( pTXD, (*it)->name );
            if ( pExistingTexture )
            {
                // Is it a custom texture?
                if ( ( pAddedTextures && ListContainsNamedTexture ( *pAddedTextures, (*it)->name ) ) ||
                     ( pReplacedTextures && ListContainsNamedTexture ( *pReplacedTextures, (*it)->name ) ) )
                {
                    continue;
                }
                // If not, we can replace it
                RwTexDictionaryRemoveTexture ( pTXD, pExistingTexture );
                if ( pReplacedTextures )
                    pReplacedTextures->push_back ( pExistingTexture );
            }
            else
            {
                if ( pAddedTextures )
                    pAddedTextures->push_back ( *it );
            }

            RwTexture* pTex = *it;

            if ( bMakeCopy )
            {
                // Reuse the given texture's raster
                RwTexture* pNewTex = RwTextureCreate ( pTex->raster );

                // Copy over additional properties
                memcpy ( &pNewTex->name, &pTex->name, RW_TEXTURE_NAME_LENGTH );
                memcpy ( &pNewTex->mask, &pTex->mask, RW_TEXTURE_NAME_LENGTH );
                pNewTex->flags = pTex->flags;
                
                pTex = pNewTex;
            }

            // Add the texture
            RwTexDictionaryAddTexture ( pTXD, pTex );
        }
    }
}

// Removes textures from the TXD of a model, eventually destroying each texture
void CRenderWareSA::ModelInfoTXDRemoveTextures ( std::list < RwTexture* >& textures, unsigned short usModelID, bool bDestroy, bool bKeepRaster, bool bRemoveRef )
{
	// Get the CModelInfo's TXD ID
	unsigned short usTxdId = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;

	// Get the TXD corresponding to this ID
	RwTexDictionary* pTXD = CTxdStore_GetTxd ( usTxdId );

	if ( pTXD )
    {
        std::list < RwTexture* >::iterator it;
        for ( it = textures.begin (); it != textures.end (); it++ )
        {
            // Don't remove the passed texture itself, rather any texture with the same name
	        RwTexture* pTex = RwTexDictionaryFindNamedTexture ( pTXD, (*it)->name );
            if ( pTex )
            {
                if ( bDestroy )
                {
                    // Destroy the texture
                    if ( bKeepRaster )
                        pTex->raster = NULL;
                    RwTextureDestroy ( pTex );
                }
                else
                {
                    // Only remove the texture from the txd
                    RwTexDictionaryRemoveTexture ( pTXD, pTex );
                }
            }
        }
	}

	// Delete the reference we made in ModelInfoTXDAddTextures
    if ( bRemoveRef )
	    CTxdStore_RemoveRef ( usTxdId );
}


// Reads and parses a TXD file specified by a path (szTXD)
RwTexDictionary * CRenderWareSA::ReadTXD ( const char *szTXD )
{
	// open the stream
	RwStream * streamTexture = RwStreamOpen ( STREAM_TYPE_FILENAME, STREAM_MODE_READ, szTXD );

	// check for errors
	if ( streamTexture == NULL )
		return NULL;

	// TXD header id: 0x16
	// find our txd chunk (dff loads textures, so correct loading order is: txd, dff)
	if ( RwStreamFindChunk ( streamTexture, 0x16, NULL, NULL ) == false )
		return NULL;

	// read the texture dictionary from our model (txd)
	RwTexDictionary *pTex = RwTexDictionaryStreamRead ( streamTexture );

	// close the stream
	RwStreamClose ( streamTexture, NULL );

	//return pTex;
	return pTex;
}

// Reads and parses a DFF file specified by a path (szDFF) into a CModelInfo identified by the object id (usModelID)
// usModelID == 0 means no collisions will be loaded (be careful! seems crashy!)
RpClump * CRenderWareSA::ReadDFF ( const char *szDFF, unsigned short usModelID )
{
	// open the stream
	RwStream * streamModel = RwStreamOpen ( STREAM_TYPE_FILENAME, STREAM_MODE_READ, szDFF );

	// get the modelinfo array
	DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;

	// check for errors
	if ( streamModel == NULL )
		return NULL;

	// force a load of the model, if this is a vehicle
	//if ( usModelID != 0 )
	//	pGame->GetModelInfo ( usModelID )->Request ( true, true );

	// DFF header id: 0x10
	// find our dff chunk
	if ( RwStreamFindChunk ( streamModel, 0x10, NULL, NULL ) == false )
		return NULL;

	// rockstar's collision hack: set the global particle emitter to the modelinfo pointer of this model
	//#pragma message(__LOC__ "(IJs) RpPrtStdGlobalDataSetStreamEmbedded R* hack is unstable. Seems to work when forced to packer vehicle id (443).")
	if ( usModelID != 0 )
		RpPrtStdGlobalDataSetStreamEmbedded ( ( void* ) pPool[usModelID] );

	// read the clump with all its extensions
	RpClump * pClump = RpClumpStreamRead ( streamModel );

	// reset collision hack
	if ( usModelID != 0 )
		RpPrtStdGlobalDataSetStreamEmbedded ( NULL );

	// close the stream
	RwStreamClose ( streamModel, NULL );

	//
	/*
	if ( usModelID != 0 ) {
		//
		DWORD dwCreateModelInfo = 0x4C95C0; //0x4C5110
		DWORD dwThis = (DWORD)(pPool[usModelID]);

		__asm {
			mov		ecx, dwThis
			push	pClump
			call	dwCreateModelInfo
		}
		//pGame->GetModelInfo ( usModelID )->Request ( TRUE, TRUE );
	}
	*/

	return pClump;
}

// Replaces a vehicle model
void CRenderWareSA::ReplaceVehicleModel ( RpClump * pNew, unsigned short usModelID )
{
	// get the modelinfo array
	DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;

	DWORD dwFunc = FUNC_LoadVehicleModel;
	DWORD dwThis = (DWORD)(pPool[usModelID]);
	__asm
    {
		mov		ecx, dwThis
		push	pNew
		call	dwFunc
	}
}

// Reads and parses a COL3 file
CColModel * CRenderWareSA::ReadCOL ( const char * szCOLFile )
{
	if ( !szCOLFile )
        return NULL;

	// Read the file
	FILE* pFile = fopen ( szCOLFile, "rb" );
	if ( !pFile )
        return NULL;

	// Create a new CColModel
	CColModelSA* pColModel = new CColModelSA ();

    ColModelFileHeader header;
    while ( fread ( &header, sizeof(ColModelFileHeader), 1, pFile ) )
    {
        // Load the col model
	    if ( header.version[0] == 'C' && header.version[1] == 'O' && header.version[2] == 'L' )
        {
            unsigned char* pModelData = new unsigned char [ header.size - 0x18 ];
            fread ( pModelData, header.size - 0x18, 1, pFile );

            if ( header.version[3] == 'L' )
            {
                LoadCollisionModel ( pModelData, pColModel->GetColModel (), header.name );
            }
            else if ( header.version[3] == '2' )
            {
                LoadCollisionModelVer2 ( pModelData, header.size - 0x18, pColModel->GetColModel (), header.name );
            }
            else if ( header.version[3] == '3' )
            {
	            LoadCollisionModelVer3 ( pModelData, header.size - 0x18, pColModel->GetColModel (), header.name );
            }

            delete[] pModelData;
        }
        else
        {
            break;
        }
    }

    fclose ( pFile );

	// Return the collision model
	return pColModel;
}

// Positions the front seat by reading out the vector from the 'ped_frontseat' atomic in the clump (RpClump*)
// and changing the vector in the CModelInfo class identified by the model id (usModelID)
bool CRenderWareSA::PositionFrontSeat ( RpClump *pClump, unsigned short usModelID )
{
	// get the modelinfo array (+5Ch contains a pointer to vehicle specific dummy data)
	DWORD *pPool = ( DWORD* ) ARRAY_ModelInfo;
	DWORD *pVehicleDummies = ( DWORD* ) ( pPool[usModelID] + 0x5C );

	// read out the 'ped_frontseat' frame
	RwFrame * pPedFrontSeat = RwFrameFindFrame ( RpGetFrame ( pClump ), "ped_frontseat" );
	if ( pPedFrontSeat == NULL )
		return false;

	// in the vehicle specific dummy data, +30h contains the front seat vector
	CVector *vecFrontSeat = ( CVector* ) ( pVehicleDummies + 0x30 );
	vecFrontSeat->fX = pPedFrontSeat->modelling.pos.x;
	vecFrontSeat->fY = pPedFrontSeat->modelling.pos.y;
	vecFrontSeat->fZ = pPedFrontSeat->modelling.pos.z;

	return true;
}

// Loads all atomics from a clump into a container struct and returns the number of atomics it loaded
unsigned int CRenderWareSA::LoadAtomics ( RpClump * pClump, RpAtomicContainer * pAtomics )
{
	// iterate through all atomics in the clump
	SLoadAtomics data = {0};
	data.pReplacements = pAtomics;
	RpClumpForAllAtomics ( pClump, ( void * ) LoadAtomicsCB, &data );
	
	// return the number of atomics that were added to the container
	return data.uiReplacements;
}

// Replaces all atomics for a specific model
typedef struct
{
    unsigned short usTxdID;
    RpClump* pClump;
} SAtomicsReplacer;
RpAtomic* AtomicsReplacer ( RpAtomic* pAtomic, SAtomicsReplacer* pData )
{
    ( (void (*)(RpAtomic*, void*))FUNC_AtomicsReplacer ) ( pAtomic, pData->pClump );
    // The above function adds a reference to the model's TXD. Remove it again.
    CTxdStore_RemoveRef ( pData->usTxdID );
    return pAtomic;
}

void CRenderWareSA::ReplaceAllAtomicsInModel ( RpClump * pSrc, unsigned short usModelID )
{
	// Clone the clump that's to be replaced (FUNC_AtomicsReplacer removes the atomics from the source clump)
	RpClump * pCopy = RpClumpClone ( pSrc );

	// Replace the atomics
    SAtomicsReplacer data;
    data.usTxdID = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelID]->usTextureDictionary;
    data.pClump = pCopy;

	*((DWORD*)DWORD_AtomicsReplacerModelID) = usModelID;
	RpClumpForAllAtomics ( pCopy, AtomicsReplacer, &data );

	// Get rid of the now empty copied clump
	RpClumpDestroy ( pCopy );
}

// Replaces all atomics in a vehicle
void CRenderWareSA::ReplaceAllAtomicsInClump ( RpClump * pDst, RpAtomicContainer * pAtomics, unsigned int uiAtomics )
{
	SReplaceAll data;
	data.pClump = pDst;
	data.pReplacements = pAtomics;
	data.uiReplacements = uiAtomics;
	RpClumpForAllAtomics ( pDst, ReplaceAllCB, &data );
}

// Replaces the wheels in a vehicle
void CRenderWareSA::ReplaceWheels ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szWheel )
{
	// get the clump's frame
	RwFrame * pFrame = RpGetFrame ( pClump );

	SReplaceWheels data;
	data.pClump = pClump;
	data.pReplacements = pAtomics;
	data.uiReplacements = uiAtomics;
	data.szName = szWheel;
	RpClumpForAllAtomics ( pClump, ReplaceWheelsCB, &data );
}

// Repositions an atomic
void CRenderWareSA::RepositionAtomic ( RpClump * pDst, RpClump * pSrc, const char * szName )
{
	RwFrame * pDstFrame = RpGetFrame ( pDst );
	RwFrame * pSrcFrame = RpGetFrame ( pSrc );
	RwFrameCopyMatrix ( RwFrameFindFrame ( pDstFrame, szName ), RwFrameFindFrame ( pSrcFrame, szName ) );
}

// Adds the atomics from a source clump (pSrc) to a destination clump (pDst)
void CRenderWareSA::AddAllAtomics ( RpClump * pDst, RpClump * pSrc )
{
	RpClumpForAllAtomics ( pSrc, AddAllAtomicsCB, pDst );
}

// Replaces dynamic parts of the vehicle (models that have two different versions: 'ok' and 'dam'), such as doors
// szName should be without the part suffix (e.g. 'door_lf' or 'door_rf', and not 'door_lf_dummy')
bool CRenderWareSA::ReplacePartModels ( RpClump * pClump, RpAtomicContainer * pAtomics, unsigned int uiAtomics, const char * szName )
{
	// get the part's dummy name
	char szDummyName[16] = {0};
	_snprintf ( &szDummyName[0], 16, "%s_dummy", szName );

	// get the clump's frame
	RwFrame * pFrame = RpGetFrame ( pClump );

	// get the part's dummy frame
	RwFrame * pPart = RwFrameFindFrame ( pFrame, &szDummyName[0] );
	if ( pPart == NULL )
		return false;

	// now replace all the objects in the frame
	SReplaceParts data = {0};
	data.pReplacements = pAtomics;
	data.uiReplacements = uiAtomics;
	data.szName = szName;
	RwFrameForAllObjects ( pPart, ReplacePartsCB, &data );

	return true;
}

// Replaces a CColModel for a specific object identified by the object id (usModelID)
void CRenderWareSA::ReplaceCollisions ( CColModel* pCol, unsigned short usModelID )
{
	DWORD *pPool = (DWORD *) ARRAY_ModelInfo;
	CColModelSA * pColModel = (CColModelSA*) pCol;
	CModelInfoSA * pModelInfoSA = (CModelInfoSA*)(pGame->GetModelInfo ( usModelID ));

	// Apply some low-level hacks (copies the old col area and sets a flag)
	DWORD pColModelInterface = (DWORD)pColModel->GetColModel ();
	DWORD pOldColModelInterface = *((DWORD *) pPool [ usModelID ] + 20);
	*((BYTE *)( pPool [usModelID ] + 0x13 )) |= 8;
	*((BYTE *)( pColModelInterface + 40 )) = *((BYTE *)( pOldColModelInterface + 40 ));

	// TODO: It seems that on entering the game, when this function is executed, the modelinfo array for this
	// model is still zero, leading to a crash!
	pModelInfoSA->IsLoaded ();
}

// Destroys a DFF instance
void CRenderWareSA::DestroyDFF ( RpClump * pClump )
{
	if ( pClump )
	    RpClumpDestroy ( pClump );
}

// Destroys a TXD instance
void CRenderWareSA::DestroyTXD ( RwTexDictionary * pTXD )
{
	if ( pTXD )
	    RwTexDictionaryDestroy ( pTXD );
}

// Destroys a texture instance
void CRenderWareSA::DestroyTexture ( RwTexture* pTex )
{
    if ( pTex )
        RwTextureDestroy ( pTex );
}

void CRenderWareSA::RwTexDictionaryRemoveTexture ( RwTexDictionary* pTXD, RwTexture* pTex )
{
    if ( pTex->txd != pTXD )
        return;

    pTex->TXDList.next->prev = pTex->TXDList.prev;
    pTex->TXDList.prev->next = pTex->TXDList.next;
    pTex->txd = NULL;
}

short CRenderWareSA::CTxdStore_GetTxdRefcount ( unsigned short usTxdID )
{
    return *(short *)( *(*(DWORD **)0xC8800C) + 0xC*usTxdID + 4 );
}

bool CRenderWareSA::ListContainsNamedTexture ( std::list < RwTexture* >& list, const char* szTexName )
{
    std::list < RwTexture* >::iterator it;
    for ( it = list.begin (); it != list.end (); it++ )
    {
        if ( !stricmp ( szTexName, (*it)->name ) )
            return true;
    }
    return false;
}