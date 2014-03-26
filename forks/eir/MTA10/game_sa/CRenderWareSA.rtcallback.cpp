/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.rtcallback.cpp
*  PURPOSE:     Conglomeration of all known GTA:SA render callbacks
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RenderWare/RwRenderTools.hxx"

// todo: reverse these render passes.
typedef void (__cdecl*RpMatFXD3D9AtomicMatFXEnvRender_t)( RwRenderCallbackTraverseImpl*, RwRenderPass*, unsigned int, bool, RwTexture*, RwTexture* );
typedef void (__cdecl*RpMatFXD3D9AtomicMatFXBumpMapRender_t)( RwRenderCallbackTraverseImpl*, RwRenderPass*, unsigned int, RwTexture*, RwTexture*, RwTexture* );

static RW_JUSTDEF( RpMatFXD3D9AtomicMatFXEnvRender );
static RW_JUSTDEF( RpMatFXD3D9AtomicMatFXBumpMapRender );

/*=========================================================
    _GenericGamePreTexturedRenderPass

    Arguments:
        rtinfo - RenderWare video data structure
        rtPass - current render pass
    Purpose:
        Renders video data without caring about textures. Used
        by GTA:SA as an adaption to RenderWare's internal
        rendering pipeline.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005DA640
=========================================================*/
inline void _GenericGamePreTexturedRenderPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
{
    RwD3D9UnsetPixelShader();
    RwD3D9SetCurrentVertexShader( rtPass->m_vertexShader );

    RwD3D9DrawRenderPassPrimitive( rtinfo, rtPass );
}

/*=========================================================
    _GenericGamePreTexturedRenderPass

    Arguments:
        rtinfo - RenderWare video data structure
        rtPass - current render pass
        renderFlags - rendering flags passed to rt callback
        useTexture - texture to use for this rendering pass
    Purpose:
        Applies the texture into the pipeline (if required) and
        renders the video data as described in
        _GenericGamePreTexturedRenderPass.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005DA6A0
=========================================================*/
inline void _GenericGameTexturedRenderPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RwTexture *useTexture )
{
    bool needTexture = IS_ANY_FLAG( renderFlags, 0x84 );

    RwD3D9SetTexture( ( needTexture ) ? useTexture : NULL, 0 );
    RwD3D9SetTexturedRenderStates( needTexture );

    _GenericGamePreTexturedRenderPass( rtinfo, rtPass );
}

/*=========================================================
    _SpecialFXDefaultRenderPass

    Arguments:
        rtinfo - RenderWare video data structure
        rtPass - current render pass
        renderFlags - rendering flags passed to rt callback
        useTexture - texture to use for this rendering pass
    Purpose:
        Renders a mesh whose material has special effects
        attached to it. This callback is called directly
        when no special effect is active at the material.
    Binary offsets:
        (1.0 US): 0x00812AC0
        (1.0 EU): 0x00812B00
=========================================================*/
inline D3DMATRIX& GetEffectMatrix( void )
{
    return *(D3DMATRIX*)0x008E29C8;
}

inline void _SetupFXTextureMatrix( unsigned int stageIndex, RwMatrix *fxMatrix )
{
    if ( !IS_ANY_FLAG( fxMatrix->a, 0x20000 ) )
    {
        // Set transformation flags to include the matrix.
        RwD3D9SetTextureStageState( stageIndex, D3DTSS_TEXTURETRANSFORMFLAGS, 0x2 );

        D3DMATRIX& effectMat = GetEffectMatrix();
        effectMat.m[0][0] = fxMatrix->vRight.fX;
        effectMat.m[0][1] = fxMatrix->vRight.fY;

        effectMat.m[1][0] = fxMatrix->vFront.fX;
        effectMat.m[1][1] = fxMatrix->vFront.fY;

        effectMat.m[2][0] = fxMatrix->vPos.fX;
        effectMat.m[2][1] = fxMatrix->vPos.fY;

        RwD3D9SetTransform( (D3DTRANSFORMSTATETYPE)( D3DTS_TEXTURE0 + stageIndex ), &effectMat );
    }
}

inline void _SpecialFXDefaultRenderPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RwTexture *useTexture )
{
    CSpecialFXMatSA *usedSpecialFX = NULL;

    bool needTexture = IS_ANY_FLAG( renderFlags, 0x84 );

    RwD3D9SetTexture( ( needTexture ) ? useTexture : NULL, 0 );

    if ( needTexture )
    {
        CSpecialFXMatSA *specialFX = rtPass->m_useMaterial->specialFX;

        if ( specialFX && specialFX->effectType == 5 )
        {
            usedSpecialFX = specialFX;

            _SetupFXTextureMatrix( 0, specialFX->effect5.fxMatrix );
        }
    }

    RwD3D9SetTexturedRenderStates( needTexture );

    _GenericGamePreTexturedRenderPass( rtinfo, rtPass );

    // Clean up transformation flags.
    if ( usedSpecialFX )
    {
        RwD3D9SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
    }
}

/*=========================================================
    _SpecialFXDualBlendRenderPass
    (RpMatFXD3D9AtomicMatFXDualPassRender)

    Arguments:
        rtinfo - RenderWare video data structure
        rtPass - current render pass
        renderFlags - rendering flags passed to rt callback
        useTexture - texture to use for this rendering pass
        blendTexture - texture to use for special effects
    Purpose:
        Renders a mesh whose material has special effects
        attached to it.
    Binary offsets:
        (1.0 US): 0x00812D40
        (1.0 EU): 0x00812D80
=========================================================*/
void __cdecl RwD3D9RasterConvertToNonPalettized( RwRaster *raster )
{
    ((void (__cdecl*)( RwRaster* ))0x004CD250)( raster );
}

inline unsigned int GetNumTextureStages( void )
{
    return *(unsigned int*)0x00C9ABBC;
}

inline void _SpecialFXDualBlendRenderPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RwTexture *useTexture, RwTexture *blendTexture )
{
    if ( !blendTexture || !blendTexture->raster )
    {
        _SpecialFXDefaultRenderPass( rtinfo, rtPass, renderFlags, useTexture );
        return;
    }

    matFXBlendData *blendData = NULL;
    CSpecialFXMatSA *useSpecialFX = NULL;

    {
        CSpecialFXMatSA *eff = rtPass->m_useMaterial->specialFX;

        if ( eff->effectType != 6 )
        {
            blendData = &eff->effect4;
        }
        else
        {
            blendData = &eff->effect6.fxBlend;
            useSpecialFX = eff;
        }
    }

    unsigned int unkBoolean = true;

    bool needTexture = IS_ANY_FLAG( renderFlags, 0x84 );

    RwD3D9SetTexture( ( needTexture ) ? useTexture : NULL, 0 );
    RwD3D9SetTexturedRenderStates( needTexture );

    if ( useSpecialFX )
    {
        _SetupFXTextureMatrix( 0, useSpecialFX->effect6.fxMatrix );
    }

    if ( useTexture )
    {
        RwRaster *useRaster = useTexture->raster;
        RwRaster *effRaster = blendTexture->raster;

        // The_GTA: made sure the textures have rasters. Seriously, textures may have _no raster_.
        if ( useRaster && effRaster )
        {
            if ( IS_ANY_FLAG( useRaster->format, 0x20 ) && IS_ANY_FLAG( effRaster->format, 0x20 ) && useTexture != blendTexture )
            {
                RwD3D9RasterConvertToNonPalettized( effRaster );
            }
        }
    }

    if ( *(unsigned int*)0x00C9ABAC )
    {
        bool setupTextureMatrixSecondStage = false;

        if ( blendData->srcBlend == 9 && blendData->dstBlend == 1 || blendData->srcBlend == 1 && blendData->dstBlend == 3 )
        {
            if ( useTexture )
            {
                RwD3D9SetTexture( blendTexture, 1 );

                unsigned int texStages = GetNumTextureStages();

                if ( texStages == 3 )
                {
                    // Stage 1
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                    
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
                }

                // Stage 2
                RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                if ( texStages == 3 )
                {
                    // Stage 3
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
                }
            }
            else
            {
                RwD3D9SetTexture( blendTexture, 0 );

                RwD3D9SetTexturedRenderStates( true );
            }

            unkBoolean = false;
            setupTextureMatrixSecondStage = true;
        }
        else if ( blendData->srcBlend == 5 && blendData->dstBlend == 6 )
        {
            if ( useTexture )
            {
                if ( GetNumTextureStages() >= 3 )
                {
                    RwD3D9SetTexture( blendTexture, 1 );

                    // Stage 1
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

                    // Stage 2
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_BLENDTEXTUREALPHA );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                    // Stage 3
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                    unkBoolean = false;
                    setupTextureMatrixSecondStage = true;
                }
            }
            else
            {
                HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, 0xFFFFFFFF );

                RwD3D9SetTexture( blendTexture, 0 );

                // Stage 1
                RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                // Is this an actual bug? Yes it is! (1.0 US): 0x0081316B
                RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                // Stage 2
                RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                unkBoolean = false;
                setupTextureMatrixSecondStage = true;
            }
        }
        else if ( blendData->srcBlend == 9 && blendData->dstBlend == 3 )
        {
            if ( *(unsigned int*)0x00C9ABB4 )
            {
                if ( useTexture )
                {
                    RwD3D9SetTexture( blendTexture, 1 );

                    // Stage 2
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                    if ( GetNumTextureStages() == 3 )
                    {
                        // Stage 1
                        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
                        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

                        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
                        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );

                        // Stage 2
                        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                        // Stage 3
                        RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
                        RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                        RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

                        RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                        RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                        RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );
                    }
                    else
                    {
                        // Stage 2
                        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
                    }
                }
                else
                {
                    RwD3D9SetTexture( blendTexture, 0 );

                    // Stage 1
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TFACTOR );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
                }

                // Check for any errors.
                if ( *(unsigned int*)0x008E29C0 )
                {
                    DWORD numPasses = 0;

                    RwD3D9ApplyDeviceStates();

                    HRESULT validateResult = GetRenderDevice()->ValidateDevice( &numPasses );

                    if ( validateResult < 0 || numPasses != 1 )
                    {
                        *(unsigned int*)0x00C9ABB4 = 0;

                        // Reset the first stage texture.
                        RwD3D9SetTexture( useTexture, 0 );

                        // Return to default rendering method.
                        RwD3D9SetTexturedRenderStates( true );

                        // Disable second and third stage.
                        RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
                        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

                        RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
                        RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

                        // Remove texture link from second stage.
                        RwD3D9SetTexture( NULL, 1 );
                    }
                    else
                        unkBoolean = false;

                    *(unsigned int*)0x008E29C0 = false;

                    if ( !unkBoolean )
                        setupTextureMatrixSecondStage = true;
                }
                else
                {
                    unkBoolean = false;
                    setupTextureMatrixSecondStage = true;
                }
            }
        }
        else if ( blendData->srcBlend == 2 && blendData->dstBlend == 2 )
        {
            if ( useTexture )
            {
                if ( *(unsigned int*)0x00C9ABB0 )
                {
                    RwD3D9SetTexture( blendTexture, 1 );

                    // Stage 2
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG0, D3DTA_CURRENT );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                    unkBoolean = false;
                    setupTextureMatrixSecondStage = true;
                }
            }
            else
            {
                RwD3D9SetTexture( blendTexture, 0 );

                // Stage 1
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

                // Stage 2
                RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_ADD );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                unkBoolean = false;
                setupTextureMatrixSecondStage = true;
            }
        }
        else if ( blendData->srcBlend == 1 && blendData->dstBlend == 5 )
        {
            if ( useTexture )
            {
                RwD3D9SetTexture( blendTexture, 1 );

                if ( GetNumTextureStages() == 3 )
                {
                    // Stage 1
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
                }

                // Stage 2
                RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_ALPHAREPLICATE | D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                if ( GetNumTextureStages() == 3 )
                {
                    // Stage 3
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_COLORARG2, D3DTA_CURRENT );

                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
                }

                unkBoolean = false;
                setupTextureMatrixSecondStage = true;
            }
            else
            {
                RwD3D9SetTexture( blendTexture, 0 );

                // Stage 1
                RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_ALPHAREPLICATE | D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

                unkBoolean = false;
                setupTextureMatrixSecondStage = true;
            }
        }

        if ( setupTextureMatrixSecondStage )
        {
            if ( IS_ANY_FLAG( renderFlags, 0x80 ) )
            {
                RwD3D9SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
            }

            if ( useSpecialFX )
            {
                RwMatrix *fxMatrix = useSpecialFX->effect6.fxMatrix2;

                if ( fxMatrix )
                    _SetupFXTextureMatrix( 1, fxMatrix );
            }
        }
    }

    _GenericGamePreTexturedRenderPass( rtinfo, rtPass );

    if ( unkBoolean )
    {
        RwD3D9SetTexture( blendTexture, 0 );

        if ( useSpecialFX )
        {
            RwMatrix *fxMatrix = useSpecialFX->effect6.fxMatrix2;

            if ( fxMatrix )
                _SetupFXTextureMatrix( 0, fxMatrix );
        }

        if ( !RwD3D9RenderStateIsVertexAlphaEnabled() )
        {
            RwD3D9RenderStateSetVertexAlphaEnabled( true );
        }

        if ( blendData->srcBlend == 9 && blendData->dstBlend == 1 ||
             blendData->srcBlend == 1 && blendData->dstBlend == 3 ||
             blendData->srcBlend == 9 && blendData->dstBlend == 3 )
        {
            HOOK_RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, false );
        }

        RwInterface *rwInterface = RenderWare::GetInterface();

        int unkStatus1, unkStatus2;

        rwInterface->m_getDeviceCommand( (eRwDeviceCmd)10, unkStatus1 );
        rwInterface->m_getDeviceCommand( (eRwDeviceCmd)11, unkStatus2 );

        RwD3D9SetSrcBlend( blendData->srcBlend );
        RwD3D9SetDstBlend( blendData->dstBlend );

        DWORD zwriteStatus;
        HOOK_RwD3D9GetRenderState( D3DRS_ZWRITEENABLE, zwriteStatus );
        HOOK_RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, false );

        DWORD fogEnableStatus;
        HOOK_RwD3D9GetRenderState( D3DRS_FOGENABLE, fogEnableStatus );

        DWORD fogColor;

        if ( fogEnableStatus )
        {
            HOOK_RwD3D9GetRenderState( D3DRS_FOGCOLOR, fogColor );

            if ( blendData->dstBlend == 2 )
            {
                HOOK_RwD3D9SetRenderState( D3DRS_FOGCOLOR, 0 );
            }
            else if ( blendData->srcBlend == 9 || blendData->dstBlend == 3 )
            {
                HOOK_RwD3D9SetRenderState( D3DRS_FOGCOLOR, 0xFFFFFFFF );
            }
        }

        if ( IS_ANY_FLAG( renderFlags, 0x80 ) )
        {
            RwD3D9SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 1 );
        }

        RwD3D9DrawRenderPassPrimitive( rtinfo, rtPass );

        HOOK_RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, zwriteStatus );

        if ( fogEnableStatus )
        {
            HOOK_RwD3D9SetRenderState( D3DRS_FOGCOLOR, fogColor );
        }

        if ( IS_ANY_FLAG( renderFlags, 0x80 ) )
        {
            RwD3D9SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        }

        RwD3D9RenderStateSetVertexAlphaEnabled( false );

        rwInterface->m_deviceCommand( (eRwDeviceCmd)10, unkStatus1 );
        rwInterface->m_deviceCommand( (eRwDeviceCmd)11, unkStatus2 );
    }
    else
    {
        // Stage 1
        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

        RwD3D9SetTexture( NULL, 1 );

        // Stage 3
        RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
        RwD3D9SetTextureStageState( 2, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

        if ( IS_ANY_FLAG( renderFlags, 0x80 ) )
        {
            // Stage 2
            RwD3D9SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        }
    }

    if ( useSpecialFX )
    {
        RwD3D9SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
        RwD3D9SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, 1 );
    }
}

static bool enableEnvMapRendering = true;

void RenderCallbacks::SetEnvMapRenderingEnabled( bool enabled )
{
    enableEnvMapRendering = enabled;
}

bool RenderCallbacks::IsEnvMapRenderingEnabled( void )
{
    return enableEnvMapRendering;
}

/*=========================================================
    GameRenderGeneric

    Arguments:
        rtnative - RenderWare video data pointer
        renderObject - RenderWare object to be rendered
        renderType - type of the RenderWare object
        renderFlags - flags passed to the rendering system
    Purpose:
        Rendering callback used to draw buildings and objects in
        the world. This is an adapted rewrite of RenderWare's
        default rendering callback.
=========================================================*/
static bool _renderEnableCallbacks = true;

static inline bool IsRenderingEnabled( void )
{
    return _renderEnableCallbacks;
}

void RenderCallbacks::SetRenderingEnabled( bool enabled )
{
    _renderEnableCallbacks = enabled;
}

bool RenderCallbacks::IsRenderingEnabled( void )
{
    return _renderEnableCallbacks;
}

// Alpha sorting API parameters and functions.
static bool _renderIsAlphaSortingEnabled = false;
static bool _renderDoOpaquePrimitives = false;
static bool _renderDoTranslucentPrimitives = false;
static bool _renderDoDepthLayer = false;

bool AlphaSort_IsEnabled( void )
{
    return _renderIsAlphaSortingEnabled;
}

bool AlphaSort_CanRenderOpaquePrimitives( void )
{
    return _renderDoOpaquePrimitives;
}

bool AlphaSort_CanRenderTranslucentPrimitives( void )
{
    return _renderDoTranslucentPrimitives;
}

bool AlphaSort_CanRenderDepthLayer( void )
{
    return _renderDoDepthLayer;
}

void RenderCallbacks::SetAlphaSortingEnabled( bool enabled )
{
    _renderIsAlphaSortingEnabled = enabled;
}

bool RenderCallbacks::IsAlphaSortingEnabled( void )
{
    return _renderIsAlphaSortingEnabled;
}

void RenderCallbacks::SetAlphaSortingParams( bool doOpaque, bool doTranslucent, bool doDepth )
{
    _renderDoOpaquePrimitives = doOpaque;
    _renderDoTranslucentPrimitives = doTranslucent;
    _renderDoDepthLayer = doDepth;
}

void RenderCallbacks::GetAlphaSortingParams( bool& doOpaque, bool& doTranslucent, bool& doDepth )
{
    doOpaque = _renderDoOpaquePrimitives;
    doTranslucent = _renderDoTranslucentPrimitives;
    doDepth = _renderDoDepthLayer;
}

template <typename callbackType>
__forceinline void GameRenderPassGeneric( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, bool requireAlpha, DWORD lightingEnabled, unsigned int renderFlags, bool enableAlpha, lightRenderManager& lightMan, callbackType& cb )
{
    lightMan.OnPrePass( rtinfo, rtPass );

    // Notify the generic callback.
    RpMaterial *curMat = rtPass->m_useMaterial;

    cb.OnRenderPass( rtPass, curMat );

    RwD3D9RenderStateSetVertexAlphaEnabled( requireAlpha );

    if ( !enableAlpha )
    {
        cb.OnRenderSurfacePrepare( rtPass, lightingEnabled, curMat, renderFlags );

        cb.OnTexturedPass( rtinfo, rtPass, renderFlags, curMat );
    }
    else
    {
        cb.OnQuickPass( rtinfo, rtPass );
    }

    cb.OnPostRenderPass();

    lightMan.OnPass( rtinfo, rtPass );
}

struct GameMeshRenderCallback
{
    // Light manager.
    lightRenderManager& m_lightMan;
    DWORD& m_lightingEnabled;
    unsigned int& m_renderFlags;
    bool& m_enableAlpha;

    AINLINE GameMeshRenderCallback( lightRenderManager& lightMan, DWORD& lightingEnabled, unsigned int& renderFlags, bool& enableAlpha )
        : m_lightMan( lightMan ), m_lightingEnabled( lightingEnabled ), m_renderFlags( renderFlags ), m_enableAlpha( enableAlpha )
    {
        return;
    }

    template <typename renderManager>
    AINLINE void OnRenderMesh( renderManager& cb, RwRenderCallbackTraverseImpl *rtinfo, eRenderCallbackType renderType )
    {
        for ( unsigned int n = 0; n < rtinfo->m_numPasses; n++ )
        {
            RwRenderPass *rtPass = &rtinfo->GetRenderPass( n );

            bool passRequiresAlpha = RwD3D9IsVertexAlphaRenderingRequiredEx( rtPass, rtPass->m_useMaterial );

            // Check whether we can process this pass.
            // Depends on whether this mesh allows alpha and the current render type.
            if ( !CanProcessPass( passRequiresAlpha, renderType ) )
                continue;

            GameRenderPassGeneric( rtinfo, rtPass, passRequiresAlpha, m_lightingEnabled, m_renderFlags, m_enableAlpha, m_lightMan, cb );
        }
    }
};

template <typename callbackType>
__forceinline void __cdecl GameRenderGeneric( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags, callbackType& cb )
{
    if ( !IsRenderingEnabled() )
        return;

    RwRenderCallbackTraverseImpl *rtinfo = &rtnative->m_impl;

    DWORD lightingEnabled = FALSE;

    RwD3D9EnableClippingIfNeeded( renderObject, renderType );

    bool enableAlpha = RwD3D9IsAlphaRenderingRequired( renderFlags, lightingEnabled );

    if ( enableAlpha )
    {
        RwD3D9SetTexture( NULL, 0 );

        HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );

        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
    }

    if ( IDirect3DIndexBuffer9 *indexBuf = rtinfo->m_indexBuffer )
        RwD3D9SetCurrentIndexBuffer( indexBuf );

    RwD3D9SetStreams( rtinfo->m_vertexStreams, rtinfo->m_useOffsets );

    RwD3D9SetCurrentVertexDeclaration( rtinfo->m_vertexDecl );

    cb.OnRenderPrepare( lightingEnabled );

    // Do the rendering logic.
    {
        // Set up the rendering managers.
        lightRenderManager lightMan;

        MeshRenderManager meshRenderMan;
        GameMeshRenderCallback myRenderCB( lightMan, lightingEnabled, renderFlags, enableAlpha );

        meshRenderMan.Render( rtinfo, cb, myRenderCB );
    }

    // Notify the render manager that we quit.
    cb.OnRenderFinish();
}

__forceinline void RwD3D9ResetCommonColorChannels( void )
{
    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
}

__forceinline unsigned char GetColorComponent( int colorMod )
{
    if ( colorMod <= 0 )
        return 0xFF;

    return (unsigned char)colorMod;
}

template <typename reflectiveManager, typename renderObjType>
__forceinline void RenderReflectiveEnvMap( renderObjType *renderObj, CEnvMapMaterialSA *envMapMat, reflectiveManager& reflectMan )
{
    // Crashfix (imported from MP_SA)
    if ( enableEnvMapRendering && envMapMat )
    {
        RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)2, 1 );

        // Notify the reflective manager.
        reflectMan.OnMaterialEnvMapRender( renderObj, envMapMat );

        RwD3D9SetTexture( envMapMat->envTexture, 1 );

        // Calculate the brightness of the environment map texture.
        int colorMod = (int)( (double)envMapMat->envMapBrightness / 255.0f * reflectMan.GetColorMultiplier() * 254.0f );

        unsigned char colorComponent = GetColorComponent( colorMod );

        D3DCOLOR envMapColor = 0xFFFFFF00;
        envMapColor = ( envMapColor | colorComponent ) << 8;
        envMapColor = ( envMapColor | colorComponent ) << 8;
        envMapColor = ( envMapColor | colorComponent );

        HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, envMapColor );

        RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD );
        RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG0, D3DTA_CURRENT );
        RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );

        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

        RwD3D9SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0x10001 );
        RwD3D9SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, reflectMan.GetTextureTransformFlags() );

        RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
    }
}

static inline bool UseDualWorldRendering( void )
{
    // If true, improved alpha blending is used.
    return true;
}

template <typename reflectiveManager, typename renderObjType>
struct ReflectiveGeneralRenderManager
{
    __forceinline ReflectiveGeneralRenderManager( renderObjType*& obj ) : renderObject( obj )
    {
    }

    __forceinline void OnRenderPrepare( DWORD lightValue )
    {
        return;
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass, RpMaterial *curMat )
    {
        RwD3D9ResetCommonColorChannels();

        if ( IS_ANY_FLAG( curMat->shaderFlags, 0x01 ) )
        {
            RenderReflectiveEnvMap( renderObject, curMat->envMapMat, m_reflectMan );
        }
    }

    __forceinline bool OnRenderSurfacePrepare( RwRenderPass *rtPass, DWORD lightValue, RpMaterial *curMat, unsigned int renderFlags )
    {
        return RwD3D9UpdateRenderPassSurfaceProperties( rtPass, lightValue, curMat, renderFlags );
    }

    __forceinline void OnTexturedPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RpMaterial *useMat )
    {
        _GenericGameTexturedRenderPass( rtinfo, rtPass, renderFlags, useMat->texture );
    }

    __forceinline void OnQuickPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
    {
        _GenericGamePreTexturedRenderPass( rtinfo, rtPass );
    }

    __forceinline void OnPostRenderPass( void )
    {
        return;
    }

    __forceinline void OnRenderFinish( void )
    {
        RwD3D9DisableSecondTextureStage();
    }

    __forceinline bool IsProperlyDepthSorted( void )
    {
        // Since this template is used for any world model, we assume by default that they
        // are not properly depth sorted, as seen on the GTA:SA trees. We ask the mod whether
        // it wants to have these instances alpha fixed.
        // todo: since this process is expensive, it'd be cool if we somehow knew which
        // models are totally opaque using a geometry flag.
        return !AlphaSort_IsEnabled();
    }

    __forceinline bool CanRenderOpaque( void )
    {
        return AlphaSort_CanRenderOpaquePrimitives();
    }

    __forceinline bool CanRenderTranslucent( void )
    {
        return AlphaSort_CanRenderTranslucentPrimitives();
    }

    __forceinline bool CanRenderDepth( void )
    {
        return AlphaSort_CanRenderDepthLayer();
    }

    __forceinline unsigned int GetRenderAlphaClamp( void )
    {
        // todo: allow the user to define this value.
        return 100;
    }

    reflectiveManager m_reflectMan;
    renderObjType*& renderObject;
};

inline void ConstructTextureReflectiveIdentity( D3DMATRIX& matrix, CEnvMapMaterialSA *envMapMat )
{
    matrix.m[0][0] = (float)envMapMat->envMod / 8;
    matrix.m[1][1] = (float)envMapMat->envMod2 / 8;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
}

struct SunReflectManager
{
    __forceinline void OnMaterialEnvMapRender( RwObject *renderObj, CEnvMapMaterialSA *envMapMat )
    {
        // The_GTA: These matrices do not appear to be used.
        // Rockstar Games has probably not finished this functionality.
        ConstructTextureReflectiveIdentity( *(D3DMATRIX*)0x00C02C28, envMapMat );
    }

    inline float GetColorMultiplier( void )
    {
        return 1.0f;
    }

    inline unsigned int GetTextureTransformFlags( void )
    {
        return 0;
    }
};

// Binary offsets: (1.0 US and 1.0 EU): 0x005D6480
void __cdecl HOOK_ReflectiveRenderCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
    typedef ReflectiveGeneralRenderManager <SunReflectManager, RwObject> reflectMan;

    GameRenderGeneric( rtnative, renderObject, renderType, renderFlags, reflectMan( renderObject ) );
}

struct SpecialReflectManager
{
    __forceinline void OnMaterialEnvMapRender( RwObject *renderObj, CEnvMapMaterialSA *envMapMat )
    {
        ConstructTextureReflectiveIdentity( *(D3DMATRIX*)0x00C02C70, envMapMat );
    }

    inline float GetColorMultiplier( void )
    {
        return 1.0f;
    }

    inline unsigned int GetTextureTransformFlags( void )
    {
        return 0;
    }
};

// Binary offsets: (1.0 US and 1.0 EU): 0x005D77D0
void __cdecl HOOK_SpecialObjectRenderCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
    typedef ReflectiveGeneralRenderManager <SpecialReflectManager, RwObject> reflectMan;

    GameRenderGeneric( rtnative, renderObject, renderType, renderFlags, reflectMan( renderObject ) );
}

inline float modulo_transform( float coord, float cap )
{
    return -( ( coord - (float)(int)( coord / cap ) * cap ) / cap );
}

inline RwFrame* GetAtomicReflectionRootFrame( RpAtomic *atomic )
{
    if ( RpClump *clump = atomic->clump )
        return clump->parent;
    
    return atomic->parent;
}

//todo: set proper exception routines for fibers.

#define ENV_MAP_UNPACK_FLOAT( x )   ( (float)(x) / 8 * 50 )

// Binary offsets: (1.0 US and 1.0 EU): 0x005D84C0
int __fastcall CalculateVehicleReflectiveMapParams( CEnvMapMaterialSA *envMapMat, RpAtomic *renderObj, float paramOut[] )
{
    float x_trans = ENV_MAP_UNPACK_FLOAT( envMapMat->envMod3 );
    float y_trans = ENV_MAP_UNPACK_FLOAT( envMapMat->envMod4 );

    const RwMatrix& reflectMatrix = GetAtomicReflectionRootFrame( renderObj )->GetLTM();

    paramOut[0] = modulo_transform( reflectMatrix.vPos.fX, x_trans );
    paramOut[1] = modulo_transform( reflectMatrix.vPos.fY, y_trans );
    return true;
}

// Whatever that math is supposed to mean: it aint mattering now.
inline float reflect_calculate( float x, float y, float x_trans, float y_trans )
{
    float x_trans_mod = 1 / x_trans;
    float y_trans_mod = 1 / y_trans;

    int int_x_trans_mult = (int)( x_trans_mod * x );
    int int_y_trans_mult = (int)( y_trans_mod * y );

    float x_calcVal = fabs( ( (float)int_x_trans_mult * x_trans - x ) * x_trans_mod );

    if ( int_x_trans_mult % 2 != 0 )
        x_calcVal = 1 - x_calcVal;

    float y_calcVal = fabs( ( (float)int_y_trans_mult * y_trans - y ) * y_trans_mod );

    if ( int_y_trans_mult % 2 != 0 )
        y_calcVal = 1 - y_calcVal;

    return x_calcVal + y_calcVal;
}

inline float VectorAngle( CVector left, CVector right )
{
    left.Normalize();
    right.Normalize();

    return left.DotProduct( right );
}

int __fastcall CalculateVehicleReflectiveMapParamsAtomic( CEnvMapMaterialSA *envMapMat, CEnvMapAtomicSA *envMapAtom, RpAtomic *renderObj, float paramOut[] )
{
    const RwMatrix& reflectMatrix = GetAtomicReflectionRootFrame( renderObj )->GetLTM();

    float x_trans = ENV_MAP_UNPACK_FLOAT( envMapMat->envMod3 );
    float y_trans = ENV_MAP_UNPACK_FLOAT( envMapMat->envMod4 );

    unsigned short frame = RenderWare::GetInterface()->m_renderScanCode;

    float unkFloat = envMapAtom->m_unk1;

    if ( envMapMat->updateFrame != frame )
    {
        envMapMat->updateFrame = frame;

        float addedCalcVals1 = reflect_calculate( reflectMatrix.vPos.fX, reflectMatrix.vPos.fY, x_trans, y_trans );
        float addedCalcVals2 = reflect_calculate( envMapAtom->m_xMod, envMapAtom->m_yMod, x_trans, y_trans );

        CVector progr(
            reflectMatrix.vPos.fX - envMapAtom->m_xMod,
            reflectMatrix.vPos.fY - envMapAtom->m_yMod,
            0
        );

        float reflectEndVal = fabs( addedCalcVals1 - addedCalcVals2 );

        if ( progr.LengthSquared() > 0.0f && VectorAngle( progr, reflectMatrix.vFront ) < 0 )
        {
            unkFloat -= reflectEndVal;
            
            if ( unkFloat < 0.0f )
                unkFloat += 1.0f;
        }
        else
        {
            unkFloat += reflectEndVal;

            if ( unkFloat >= 1.0f )
                unkFloat -= 1.0f;
        }

        // Update the environment atomic structure.
        envMapAtom->m_unk1 = unkFloat;
        envMapAtom->m_xMod = reflectMatrix.vPos.fX;
        envMapAtom->m_yMod = reflectMatrix.vPos.fY;
    }

    float newY = Clamp( 0.0f, reflectMatrix.vUp.fX + reflectMatrix.vUp.fY, 0.1f );

    if ( reflectMatrix.vUp.fZ < 0.0f )
        newY = 1.0f - newY;

    paramOut[0] = -unkFloat;
    paramOut[1] = newY;
    return true;
}

struct VehAtomicReflectManager
{
    __forceinline VehAtomicReflectManager( float& specular ) : m_specular( specular )
    {
    }

    __forceinline void OnMaterialEnvMapRender( RpAtomic *renderObj, CEnvMapMaterialSA *envMapMat )
    {
        float reflectiveParams[2] = { 0, 0 };

        D3DMATRIX& vehMapReflectMat = *(D3DMATRIX*)0x00C02D78;

        ConstructTextureReflectiveIdentity( vehMapReflectMat, envMapMat );

        CalculateVehicleReflectiveMapParams( envMapMat, renderObj, reflectiveParams );

        vehMapReflectMat.m[2][0] = reflectiveParams[0];
        vehMapReflectMat.m[2][1] = reflectiveParams[1];

        RwD3D9SetTransform( D3DTS_TEXTURE1, &vehMapReflectMat );
    }

    inline float GetColorMultiplier( void )
    {
        return m_specular;
    }

    inline unsigned int GetTextureTransformFlags( void )
    {
        return 0x103;
    }

    float& m_specular;
};

// Vehicle rendering parameters.
static bool _vehicleRenderIsAlphaSortingEnabled = false;
static bool _vehicleRenderDoOpaquePrimitives = false;
static bool _vehicleRenderDoTranslucentPrimitives = false;
static bool _vehicleRenderDoDepthLayer = false;
static unsigned int _vehicleRenderAlphaClamp = 0;

void RenderCallbacks::SetVehicleAlphaSortingEnabled( bool enabled )
{
    _vehicleRenderIsAlphaSortingEnabled = enabled;
}

bool RenderCallbacks::IsVehicleAlphaSortingEnabled( void )
{
    return _vehicleRenderIsAlphaSortingEnabled;
}

void RenderCallbacks::SetVehicleAlphaSortingParams( bool doOpaque, bool doTranslucent, bool doDepth )
{
    _vehicleRenderDoOpaquePrimitives = doOpaque;
    _vehicleRenderDoTranslucentPrimitives = doTranslucent;
    _vehicleRenderDoDepthLayer = doDepth;
}

void RenderCallbacks::GetVehicleAlphaSortingParams( bool& doOpaque, bool& doTranslucent, bool& doDepth )
{
    doOpaque = _vehicleRenderDoOpaquePrimitives;
    doTranslucent = _vehicleRenderDoTranslucentPrimitives;
    doDepth = _vehicleRenderDoDepthLayer;
}

void RenderCallbacks::SetVehicleAlphaClamp( unsigned int clamp )
{
    _vehicleRenderAlphaClamp = clamp;
}

struct ReflectiveVehicleRenderManager
{
    __forceinline ReflectiveVehicleRenderManager( RpAtomic*& atom ) : m_atomic( atom )
    {
        m_unk = *(float*)0x008D12D0 * 1.85f;
        m_unk2 = IS_ANY_FLAG( atom->componentFlags, 0x6000 );
        m_geomFlags = atom->geometry->flags;
    }

    __forceinline void OnRenderPrepare( DWORD _lightValue )
    {
        lightValue = _lightValue;
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass, RpMaterial *curMat )
    {
        CEnvMapMaterialSA *envMapMat = curMat->envMapMat;

        // Decide which special effects to apply.
        bool specialEffect1 = IS_ANY_FLAG( curMat->shaderFlags, 0x01 ) && !m_unk2;
        bool specialEffect2 = IS_ANY_FLAG( curMat->shaderFlags, 0x02 ) && !m_unk2 && IS_ANY_FLAG( m_geomFlags, 0x80 );

        // For specular lighting, we have to calculate things.
        specularFloat1 = 0.0f;
        specularFloat2 = 0.0f;

        // todo: insert crashfix here.
        CSpecMapMaterialSA *specMapMat = curMat->specMapMat;

        // Decide whether specular lighting should be applied.
        bool doSpecularTransform = false;

        if ( g_effectManager->GetEffectQuality() >= 2 )
        {
            m_unk3 = IS_ANY_FLAG( curMat->shaderFlags, 0x04 );

            doSpecularTransform = ( m_unk3 && lightValue );
        }
        else
        {
            m_unk3 = ( IS_ANY_FLAG( curMat->shaderFlags, 0x04 ) && !m_unk2 );

            doSpecularTransform = m_unk3;
        }

        if ( doSpecularTransform )
        {
            // Enable the special vehicle light.
            RpD3D9SetLight( 7, RenderWare::GetSpecialVehicleLight() );
            RpD3D9EnableLight( 7, true );

            specularFloat1 = std::min( m_unk * specMapMat->specular * 2, 1.0f );
            specularFloat2 = specMapMat->specular * 100.0f;
        }

        HOOK_RwD3D9SetRenderState( D3DRS_SPECULARENABLE, doSpecularTransform );

        if ( doSpecularTransform )
        {
            HOOK_RwD3D9SetRenderState( D3DRS_LOCALVIEWER, 0 );
            HOOK_RwD3D9SetRenderState( D3DRS_SPECULARMATERIALSOURCE, 0 );
        }

        RwD3D9ResetCommonColorChannels();

        if ( envMapMat )
        {
            if ( specialEffect1 )
            {
                // Render the reflection as seen on upgraded vehicle parts.
                RenderReflectiveEnvMap( m_atomic, envMapMat, VehAtomicReflectManager( m_unk ) );
            }

            if ( specialEffect2 && enableEnvMapRendering )
            {
                // Render the reflection as seen on all vehicle chasis.
                RenderWare::GetInterface()->m_deviceCommand( (eRwDeviceCmd)2, 1 );

                float reflectParams[2] = { 0, 0 };

                CEnvMapAtomicSA*& envMapAtom = m_atomic->envMap;

                if ( !envMapAtom )
                {
                    envMapAtom = new CEnvMapAtomicSA( NULL, NULL, NULL );
                }

                if ( envMapAtom )
                    CalculateVehicleReflectiveMapParamsAtomic( envMapMat, envMapAtom, m_atomic, reflectParams );

                D3DMATRIX& specEffMat = *(D3DMATRIX*)0x00C02D38;

                specEffMat.m[0][0] = 1.0f;
                specEffMat.m[1][1] = 1.0f;
                specEffMat.m[2][2] = 1.0f;
                specEffMat.m[3][3] = 1.0f;

                specEffMat.m[2][0] = reflectParams[0];
                specEffMat.m[2][1] = reflectParams[1];

                RwD3D9SetTransform( D3DTS_TEXTURE1, &specEffMat );

                RwD3D9SetTexture( envMapMat->envTexture, 1 );

                unsigned char specularComponent = GetColorComponent( (int)( m_unk * 24.0f ) );

                DWORD colorValue = ( 0x00FFFFFF | ( specularComponent << 24 ) );

                HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, colorValue );

                RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
                RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );

                RwD3D9SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
                RwD3D9SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, 0x02 );

                RwD3D9SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );
            }
        }
    }

    __forceinline bool OnRenderSurfacePrepare( RwRenderPass *rtPass, DWORD lightValue, RpMaterial *curMat, unsigned int renderFlags )
    {
        if ( lightValue )
        {
            RwColor surfColor = curMat->color;

            unsigned int colorValue = surfColor & 0x00FFFFFF;

            bool isColorBlack = false;

            // Some color combinations are supposed to display black.
            // No idea why. We should ask Rockstar!
            if ( colorValue > 0xAF00FF )
            {
                isColorBlack = ( colorValue == 0x00C8FF00 || colorValue == 0x00FF00FF || colorValue == 0x00FFFF00 );
            }
            else if ( colorValue == 0x00AF00FF )
                isColorBlack = true;
            else if ( colorValue > 0x0000FF3C )
            {
                isColorBlack = ( colorValue == 0x0000FFB9 );
            }
            else if ( colorValue == 0x0000FF3C )
                isColorBlack = true;
            else
            {
                isColorBlack = ( colorValue == 0x00003CFF || colorValue == 0x0000AFFF );
            }

            if ( isColorBlack )
            {
                surfColor.r = 0;
                surfColor.g = 0;
                surfColor.b = 0;
            }

            // Reverse this at some point.
            // It is not required for now.
            ((void (__cdecl*)( RpMaterialLighting& matLight, RwColor& matColor, unsigned int renderFlags, float f1, float f2 ))0x005DA790)
                ( curMat->lighting, surfColor, renderFlags, specularFloat1, specularFloat2 );
        }

        return true;
    }

    __forceinline void OnTexturedPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RpMaterial *useMat )
    {
        _GenericGameTexturedRenderPass( rtinfo, rtPass, renderFlags, useMat->texture );
    }

    __forceinline void OnQuickPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
    {
        _GenericGamePreTexturedRenderPass( rtinfo, rtPass );
    }

    __forceinline void OnPostRenderPass( void )
    {
        if ( m_unk3 && lightValue )
        {
            HOOK_RwD3D9SetRenderState( D3DRS_SPECULARENABLE, false );

            RpD3D9EnableLight( 7, false );
        }
    }

    __forceinline void OnRenderFinish( void )
    {
        // Normalize surface settings (?)
        RpD3D9SetSurfaceProperties( *(RpMaterialLighting*)0x008D131C, *(RwColor*)0x008D1328, 0x20 );

        // Restore some rendering settings.
        RwD3D9DisableSecondTextureStage();
    }

    __forceinline bool IsProperlyDepthSorted( void )
    {
        // In the GTA:SA engine, the model creator is responsible for the proper depth sorting of the model.
        // This means that he places the model atomics in a way that the ones closest to the camera have to
        // be rendered first. Hence the implementation does not have to render these models complex under
        // the hood.
        return !RenderCallbacks::IsVehicleAlphaSortingEnabled();
    }

    __forceinline bool CanRenderOpaque( void )
    {
        return _vehicleRenderDoOpaquePrimitives;
    }

    __forceinline bool CanRenderTranslucent( void )
    {
        return _vehicleRenderDoTranslucentPrimitives;
    }

    __forceinline bool CanRenderDepth( void )
    {
        return _vehicleRenderDoDepthLayer;
    }

    __forceinline unsigned int GetRenderAlphaClamp( void )
    {
        return _vehicleRenderAlphaClamp;
    }

    RpAtomic*& m_atomic;
    float m_unk;
    bool m_unk2;
    bool m_unk3;
    unsigned int m_geomFlags;
    DWORD lightValue;
    float specularFloat1;
    float specularFloat2;
};

// This gotta be the vehicle atomic render routine; it uses component flags.
// Binary offsets: (1.0 US and 1.0 EU): 0x005D9900
void __cdecl HOOK_VehicleAtomicRenderCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
    //todo.
    // note: this callback crashes if anything else than atomics are fed into it.
    // since atomics are the generally used meshes, this should not happen, under normal circumstances.

    assume( renderType == RW_ATOMIC );

    GameRenderGeneric( rtnative, renderObject, renderType, renderFlags, ReflectiveVehicleRenderManager( (RpAtomic*&)renderObject ) );
}

struct FXAtomicRenderManager
{
    __forceinline FXAtomicRenderManager( void )
    {
        return;
    }

    __forceinline void OnRenderPrepare( DWORD _lightValue )
    {
        return;
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass, RpMaterial *curMat )
    {
        return;
    }

    __forceinline bool OnRenderSurfacePrepare( RwRenderPass *rtPass, DWORD lightValue, RpMaterial *curMat, unsigned int renderFlags )
    {
        return RwD3D9UpdateRenderPassSurfaceProperties( rtPass, lightValue, curMat, renderFlags );
    }

    __forceinline void OnTexturedPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass, unsigned int renderFlags, RpMaterial *useMat )
    {
        CSpecialFXMatSA *specialFX = useMat->specialFX;

        unsigned int effectType = 0;

        if ( specialFX )
        {
            effectType = specialFX->effectType;
        }

        switch( effectType )
        {
        // todo: reverse the remainder.
        case 1:
            RpMatFXD3D9AtomicMatFXBumpMapRender( rtinfo, rtPass, renderFlags, useMat->texture, specialFX->bumpMapEffect.bumpTexture, NULL );
            break;
        case 2:
            RpMatFXD3D9AtomicMatFXEnvRender( rtinfo, rtPass, renderFlags, false, useMat->texture, specialFX->bumpMapEffect.bumpTexture );
            break;
        case 3:
            RpMatFXD3D9AtomicMatFXBumpMapRender( rtinfo, rtPass, renderFlags, useMat->texture, specialFX->bumpMapEffect.bumpTexture, specialFX->bumpMapEffect.bumpTexture2 );
            break;
        case 4:
            _SpecialFXDualBlendRenderPass( rtinfo, rtPass, renderFlags, useMat->texture, specialFX->effect4.fxTexture );
            break;
        case 6:
            _SpecialFXDualBlendRenderPass( rtinfo, rtPass, renderFlags, useMat->texture, specialFX->effect6.fxBlend.fxTexture );
            break;
        default:
            _SpecialFXDefaultRenderPass( rtinfo, rtPass, renderFlags, useMat->texture );
            break;
        }
    }

    __forceinline void OnQuickPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
    {
        _GenericGamePreTexturedRenderPass( rtinfo, rtPass );
    }

    __forceinline void OnPostRenderPass( void )
    {
        return;
    }

    __forceinline void OnRenderFinish( void )
    {
        return;
    }

    __forceinline bool IsProperlyDepthSorted( void )
    {
        // Since this template is used for any world model, we assume by default that they
        // are not properly depth sorted, as seen on the GTA:SA trees. We ask the mod whether
        // it wants to have these instances alpha fixed.
        // todo: since this process is expensive, it'd be cool if we somehow knew which
        // models are totally opaque using a geometry flag.
        return !AlphaSort_IsEnabled();
    }

    __forceinline bool CanRenderOpaque( void )
    {
        return AlphaSort_CanRenderOpaquePrimitives();
    }

    __forceinline bool CanRenderTranslucent( void )
    {
        return AlphaSort_CanRenderTranslucentPrimitives();
    }

    __forceinline bool CanRenderDepth( void )
    {
        return AlphaSort_CanRenderDepthLayer();
    }

    __forceinline unsigned int GetRenderAlphaClamp( void )
    {
        // todo: allow the user to define this value.
        return 100;
    }
};

void __cdecl HOOK_FXAtomicRenderCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
#if 1
    GameRenderGeneric( rtnative, renderObject, renderType, renderFlags, FXAtomicRenderManager() );
#else
    ((void (__cdecl*)( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags))0x00815C20)( rtnative, renderObject, renderType, renderFlags );
#endif
}

void RenderCallbacks_Init( void )
{
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        MemPut( 0x00815F1F, HOOK_FXAtomicRenderCallback );
        MemPut( 0x00815FA5, HOOK_FXAtomicRenderCallback );

        // Initialize pointers.
        RW_JUSTSET( RpMatFXD3D9AtomicMatFXEnvRender, 0x00813A50 );
        RW_JUSTSET( RpMatFXD3D9AtomicMatFXBumpMapRender, 0x008144A0 );
        break;
    case VERSION_US_10:
        MemPut( 0x00815EDF, HOOK_FXAtomicRenderCallback );
        MemPut( 0x00815F65, HOOK_FXAtomicRenderCallback );

        // Initialize pointers.
        RW_JUSTSET( RpMatFXD3D9AtomicMatFXEnvRender, 0x00813A10 );
        RW_JUSTSET( RpMatFXD3D9AtomicMatFXBumpMapRender, 0x00814460 );
        break;
    }

    // Hook shared routines.
    MemPut( 0x005D67F4, HOOK_ReflectiveRenderCallback ),
    MemPut( 0x005D7B0B, HOOK_SpecialObjectRenderCallback );
    MemPut( 0x005D9FE4, HOOK_VehicleAtomicRenderCallback );
}

void RenderCallbacks_Shutdown( void )
{
}

void RenderCallbacks_Reset( void )
{
    RenderCallbacks::SetRenderingEnabled( true );

    RenderCallbacks::SetEnvMapRenderingEnabled( true );

    RenderCallbacks::SetAlphaSortingEnabled( false );
    RenderCallbacks::SetAlphaSortingParams( true, true, true );

    RenderCallbacks::SetVehicleAlphaSortingEnabled( false );
    RenderCallbacks::SetVehicleAlphaSortingParams( true, true, true );
}