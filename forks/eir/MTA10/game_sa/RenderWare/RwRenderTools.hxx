/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwRenderTools.hxx
*  PURPOSE:     RenderWare device rendering tools
*               Include this file when you want to effectively create
*               RenderWare rendering callbacks.
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_RENDER_TOOLS_
#define _RENDERWARE_RENDER_TOOLS_

#include "../gamesa_renderware.h"

inline IDirect3DDevice9* GetRenderDevice_Native( void )
{
    // Required for callback operations using dummy interfaces.
    // This has been introduced by Core module.
    return *(IDirect3DDevice9**)0x00C97C28;
}

inline IDirect3DDevice9* GetRenderDevice( void )
{
#ifdef OPTIMIZE_DIRECT3D_DEVICE
    return core->GetGraphics()->GetDevice();
#else
    return GetRenderDevice_Native();
#endif //OPTIMIZE_DIRECT3D_DEVICE
}

struct RwD3D9StreamInfo //size: 16 bytes
{
    IDirect3DVertexBuffer9* m_vertexBuf;    // 0
    size_t m_offset;                        // 4
    size_t m_stride;                        // 8
    int unk4;                               // 12
};

struct RwD3D9Streams    //size: 32 bytes
{
    RwD3D9StreamInfo    streams[2];     // 0
};

void __cdecl RwD3D9SetStreams                   ( RwD3D9Streams& streams, int useOffset );

//padlevel: 2
struct RwRenderPass //size: 36 bytes
{
    BYTE                            m_pad[8];               // 0
    RpMaterial*                     m_useMaterial;          // 8
    int                             m_vertexAlpha;          // 12, boolean whether render pass requires alpha blending
    IDirect3DVertexShader9*         m_vertexShader;         // 16
    unsigned int                    m_startVertex;          // 20
    unsigned int                    m_indexedNumVertice;    // 24, used for indexed rendering
    unsigned int                    m_indexedStartIndex;    // 28
    unsigned int                    m_numPrimitives;        // 32
};

//padlevel: 2
struct RwRenderCallbackTraverseImpl //size: 64 bytes (+ m_numPasses * sizeof( RwRenderPass ))
{
    BYTE                            m_pad[4];               // 24, 0
    unsigned int                    m_numPasses;            // 28, 4
    IDirect3DIndexBuffer9*          m_indexBuffer;          // 32, 8
    D3DPRIMITIVETYPE                m_primitiveType;        // 36, 12
    RwD3D9Streams                   m_vertexStreams;        // 40, 16
    int                             m_useOffsets;           // 72, 48
    IDirect3DVertexDeclaration9*    m_vertexDecl;           // 76, 52
    BYTE                            m_pad2[8];              // 80, 56

    // Returns a dynamically allocated render pass from this internal interface.
    // These are appended at the end of the RwRenderCallbackTraverse structure by
    // m_numPasses count.
    inline RwRenderPass& GetRenderPass( unsigned int pass )
    {
        assert( pass < m_numPasses );

        return *( (RwRenderPass*)( this + 1 ) + pass );
    }
};

// padlevel: 1
struct RwRenderCallbackTraverse //size: 24 bytes (+ sizeof( m_impl ))
{
    BYTE                            m_pad[24];              // 0
    RwRenderCallbackTraverseImpl    m_impl;                 // 24
};

void __cdecl RwD3D9SetCurrentPixelShader        ( IDirect3DPixelShader9 *pixelShader );
void __cdecl RwD3D9UnsetPixelShader             ( void );

void __cdecl RwD3D9SetCurrentVertexShader       ( IDirect3DVertexShader9 *vertexShader );
void __cdecl RwD3D9SetCurrentIndexBuffer        ( IDirect3DIndexBuffer9 *indexBuf );
void __cdecl RwD3D9SetCurrentVertexDeclaration  ( IDirect3DVertexDeclaration9 *vertexDecl );

void __cdecl RpD3D9DrawIndexedPrimitive         ( D3DPRIMITIVETYPE primitiveType, INT baseVertexIndex, UINT minVertexIndex, UINT numVertice, UINT startIndex, UINT primCount );
void __cdecl RpD3D9DrawPrimitive                ( D3DPRIMITIVETYPE primitiveType, UINT startVertex, UINT numPrimitives );

void __cdecl RwD3D9EnableClippingIfNeeded       ( RwObject *obj, eRwType renderType );

// Declaration every render callback must have.
typedef void (__cdecl*rxRenderCallback_t)( RwRenderCallbackTraverse *rtinfo, RwObject *renderObject, eRwType renderType, unsigned int renderFlags );

// Helper function to decide whether alpha rendering is required.
inline bool RwD3D9IsAlphaRenderingRequired( unsigned int renderFlags, DWORD& lightValue )
{
    HOOK_RwD3D9GetRenderState( D3DRS_LIGHTING, lightValue );

    return !IS_ANY_FLAG( renderFlags, 0x08 ) && !lightValue;
}

// Helper function to decide whether vertex alpha rendering is required.
inline bool RwD3D9IsVertexAlphaRenderingRequired( RwRenderPass *rtPass, RpMaterial *curMat )
{
    return ( curMat->color.a != 0xFF || rtPass->m_vertexAlpha );
}

inline bool RwD3D9IsVertexAlphaRenderingRequiredEx( RwRenderPass *rtPass, RpMaterial *curMat )
{
    // Remember: RenderWare texture can have no raster.
    return RwD3D9IsVertexAlphaRenderingRequired( rtPass, curMat ) || curMat->texture && RwTextureHasAlpha( curMat->texture );
}

// Helper function to update surface properties.
inline bool RwD3D9UpdateRenderPassSurfaceProperties( RwRenderPass *rtPass, DWORD lightValue, RpMaterial *curMat, unsigned int renderFlags )
{
    if ( lightValue )
    {
        RpD3D9SetSurfaceProperties( curMat->lighting, curMat->color, renderFlags );
        return true;
    }

    return false;
}

// Helper function to disable the effects of the second TSS
inline void RwD3D9DisableSecondTextureStage( void )
{
    // Restore into a safe status.
    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

    RwD3D9SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
    RwD3D9SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
}

// Helper function to draw meshes of current render pass.
inline void RwD3D9DrawRenderPassPrimitive( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
{
    // Assumes that we set index buffer according to rt-rules.
    if ( rtinfo->m_indexBuffer )
        RpD3D9DrawIndexedPrimitive( rtinfo->m_primitiveType, rtPass->m_startVertex, 0, rtPass->m_indexedNumVertice, rtPass->m_indexedStartIndex, rtPass->m_numPrimitives );
    else
        RpD3D9DrawPrimitive( rtinfo->m_primitiveType, rtPass->m_startVertex, rtPass->m_numPrimitives );
}

void RpD3D9RenderLightMeshForPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass );

// Light render manager for rendering light meshes flexibly.
struct lightRenderManager
{
    AINLINE lightRenderManager( void )
    {
        // Cache lighting.
        RpD3D9CacheLighting();
    }

    AINLINE ~lightRenderManager( void )
    {
        // Make sure no light is being used anymore.
        RpD3D9ResetLightStatus();
    }

    AINLINE void OnPrePass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
    {
        // Enable global lighting beforehand.
        // This should optimize things, as we avoid two render passes.
        RpD3D9GlobalLightingPrePass();
    }

    AINLINE void OnPass( RwRenderCallbackTraverseImpl *rtinfo, RwRenderPass *rtPass )
    {
        // Render the light geometry.
        RpD3D9RenderLightMeshForPass( rtinfo, rtPass );
    }
};

enum eRenderCallbackType
{
    RENDER_OPAQUE_ONLY,
    RENDER_OPAQUE_AND_TRANSLUCENT,
    RENDER_TRANSLUCENT
};

inline bool CanProcessPass( bool passRequiresAlpha, eRenderCallbackType renderType )
{
    switch( renderType )
    {
    case RENDER_OPAQUE_ONLY:                return !passRequiresAlpha;
    case RENDER_OPAQUE_AND_TRANSLUCENT:     return true;
    case RENDER_TRANSLUCENT:                return passRequiresAlpha;
    }

    return true;
}

struct MeshRenderManager
{
    template <typename meshRenderCallback_t, typename renderManager>
    AINLINE void Render( RwRenderCallbackTraverseImpl *rtinfo, renderManager& cb, meshRenderCallback_t& meshCB )
    {
        // Check whether we have to use improved alpha blending.
        bool useAlphaFix = !cb.IsProperlyDepthSorted();
        bool renderOpaque = useAlphaFix && cb.CanRenderOpaque();
        bool renderTranslucent = useAlphaFix && cb.CanRenderTranslucent();
        bool renderDepth = useAlphaFix && cb.CanRenderDepth();

        {
            // This way we skip heap allocations
            StaticAllocator <RwRenderStateLock, 4> rsAlloc;
            RwRenderStateLock *alphaRef;

            RwRenderStateLock *alphaFunc;
            RwRenderStateLock *alphaTestEnable;
            RwRenderStateLock *zwriteEnable;

            if ( renderOpaque )
            {
                alphaRef = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHAREF, 0xFF );
                zwriteEnable = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ZWRITEENABLE, true );

                // Optimized pre-pre-pass rendering flags.
                alphaTestEnable = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHATESTENABLE, false );

                // Notify the callback.
                meshCB.OnRenderMesh( cb, rtinfo, RENDER_OPAQUE_ONLY );

                // Destroy optimized pre-pre-pass rendering flags.
                alphaTestEnable->~RwRenderStateLock();

                // Hack: pop static allocator.
                rsAlloc.Pop();
            }

            if ( renderOpaque )
            {
                // Opaque rendering flags.
                alphaFunc = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHAFUNC, D3DCMP_EQUAL );
                alphaTestEnable = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHATESTENABLE, true );
            }

            if ( !useAlphaFix || renderOpaque )
            {
                bool hasCustomAlphaRef = false;

                if ( !useAlphaFix )
                {
                    unsigned int customAlphaRef;

                    if ( RenderMode::GetAlphaClamp( customAlphaRef ) )
                    {
                        alphaRef = new (rsAlloc.Allocate()) RwRenderStateLock( D3DRS_ALPHAREF, customAlphaRef );

                        hasCustomAlphaRef = true;
                    }
                }

                meshCB.OnRenderMesh( cb, rtinfo, ( useAlphaFix ) ? RENDER_TRANSLUCENT : RENDER_OPAQUE_AND_TRANSLUCENT );

                if ( hasCustomAlphaRef )
                {
                    alphaRef->~RwRenderStateLock();

                    rsAlloc.Pop();
                }
            }

            if ( renderOpaque )
            {
                // Destroy opaque states.
                alphaFunc->~RwRenderStateLock();
                alphaTestEnable->~RwRenderStateLock();
            }

            if ( renderOpaque )
            {
                alphaRef->~RwRenderStateLock();
                zwriteEnable->~RwRenderStateLock();
            }
        }

        if ( renderTranslucent )
        {
            // Allocate translucent states.
            RwRenderStateLock alphaRef( D3DRS_ALPHAREF, 0xFF );
            RwRenderStateLock alphaTestEnable( D3DRS_ALPHATESTENABLE, true );
            RwRenderStateLock alphaFunc( D3DRS_ALPHAFUNC, D3DCMP_LESS );
            RwRenderStateLock zwriteEnable( D3DRS_ZWRITEENABLE, false );

            meshCB.OnRenderMesh( cb, rtinfo, RENDER_TRANSLUCENT );
        }

        if ( renderDepth )
        {
            // Render depth to occlude anything behind us.
            RwRenderStateLock zwriteEnable( D3DRS_ZWRITEENABLE, true );
            RwRenderStateLock alphaTestEnable( D3DRS_ALPHATESTENABLE, true );
            RwRenderStateLock alphaFunc( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
            RwRenderStateLock alphaRef( D3DRS_ALPHAREF, cb.GetRenderAlphaClamp() );
            RwRenderStateLock srcBlend( D3DRS_SRCBLEND, D3DBLEND_ZERO );
            RwRenderStateLock dstBlend( D3DRS_DESTBLEND, D3DBLEND_ONE );
            RwRenderStateLock separateAlphaBlend( D3DRS_SEPARATEALPHABLENDENABLE, true );
            RwRenderStateLock srcAlphaBlend( D3DRS_SRCBLENDALPHA, D3DBLEND_ZERO );
            RwRenderStateLock dstAlphaBlend( D3DRS_DESTBLENDALPHA, D3DBLEND_ONE );
            RwRenderStateLock alphaBlendEnable( D3DRS_ALPHABLENDENABLE, true );
            RwRenderStateLock lightState( D3DRS_LIGHTING, false );
            RwRenderStateLock fog( D3DRS_FOGENABLE, false );

            // Do not render any color nor alpha, just the depth.
            RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
            RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_CURRENT );

            RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );

            RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
            RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
            RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

            RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

            for ( unsigned int n = 0; n < rtinfo->m_numPasses; n++ )
            {
                RwRenderPass *rtPass = &rtinfo->GetRenderPass( n );

                // Only do it for meshes that have alpha.
                bool passRequiresAlpha = RwD3D9IsVertexAlphaRenderingRequiredEx( rtPass, rtPass->m_useMaterial );

                if ( !passRequiresAlpha )
                    continue;

                RpMaterial *useMat = rtPass->m_useMaterial;

                RwRenderStateLock tfactor( D3DRS_TEXTUREFACTOR, useMat->color.ToD3DColor() );

                RwD3D9SetTexture( useMat->texture, 0 );

                // Render depth.
                RwD3D9DrawRenderPassPrimitive( rtinfo, rtPass );
            }
        }
    }
};

// Special alpha sorting exports.
bool AlphaSort_IsEnabled( void );
bool AlphaSort_CanRenderOpaquePrimitives( void );
bool AlphaSort_CanRenderTranslucentPrimitives( void );
bool AlphaSort_CanRenderDepthLayer( void );

// Generic mesh render manager.
struct GenericMeshRenderCallback
{
    AINLINE GenericMeshRenderCallback( lightRenderManager& lightMan )
        : m_lightMan( lightMan )
    {
        return;
    }

    template <typename renderManager>
    AINLINE void OnRenderMesh( renderManager& cb, RwRenderCallbackTraverseImpl *rtinfo, eRenderCallbackType renderType )
    {
        // Draw rendering passes.
        for ( unsigned int n = 0; n < rtinfo->m_numPasses; n++ )
        {
            RwRenderPass *rtPass = &rtinfo->GetRenderPass( n );

            if ( !CanProcessPass( RwD3D9IsVertexAlphaRenderingRequiredEx( rtPass, rtPass->m_useMaterial ), renderType ) )
                continue;

            m_lightMan.OnPrePass( rtinfo, rtPass );

            // Notify the callback template.
            cb.OnRenderPass( rtPass );

            // Update vertex shader status.
            RwD3D9SetCurrentVertexShader( rtPass->m_vertexShader );

            // Draw the primitive.
            RwD3D9DrawRenderPassPrimitive( rtinfo, rtPass );

            m_lightMan.OnPass( rtinfo, rtPass );
        }
    }

    lightRenderManager& m_lightMan;
};

// Generic render pass loop.
template <typename callbackType>
__forceinline void _RenderVideoDataGeneric( RwRenderCallbackTraverseImpl *rtinfo, callbackType& cb )
{
    // Set up the managers.
    lightRenderManager lightMan;

    GenericMeshRenderCallback genericCB( lightMan );
    MeshRenderManager renderMan;

    renderMan.Render( rtinfo, cb, genericCB );
}

// Helper function to set textured render states.
inline void RwD3D9SetTexturedRenderStates( bool hasTexture )
{
    if ( hasTexture )
    {
        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    }
    else
    {
        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    }
}

#endif //_RENDERWARE_RENDER_TOOLS_