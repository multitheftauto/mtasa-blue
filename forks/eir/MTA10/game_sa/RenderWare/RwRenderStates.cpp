/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/RwRenderStates.cpp
*  PURPOSE:     RenderWare renderstate management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RwRenderTools.hxx"
#include "RwInternals.h"

inline float& GetCurrentFogDensity( void )                  { return *(float*)0x00C9A4FC; }
inline unsigned int& GetCurrentFogEnable( void )            { return *(unsigned int*)0x00C9A4F4; }
inline unsigned int& GetCurrentFogTableIndex( void )        { return *(unsigned int*)0x00C9A4F8; }
inline unsigned int& GetCurrentFogColor( void )             { return *(unsigned int*)0x00C9A500; }
inline unsigned int& GetCurrentShadeMode( void )            { return *(unsigned int*)0x00C9A504; }
inline unsigned int& GetCurrentZWriteEnable( void )         { return *(unsigned int*)0x00C9A4C0; }
inline unsigned int& GetCurrentDepthFunctionEnable( void )  { return *(unsigned int*)0x00C9A4C4; }
inline unsigned int& GetCurrentStencilEnable( void )        { return *(unsigned int*)0x00C9A4C8; }
inline unsigned int& GetCurrentStencilFail( void )          { return *(unsigned int*)0x00C9A4CC; }
inline unsigned int& GetCurrentStencilZFail( void )         { return *(unsigned int*)0x00C9A4D0; }
inline unsigned int& GetCurrentStencilPass( void )          { return *(unsigned int*)0x00C9A4D4; }
inline unsigned int& GetCurrentStencilFunc( void )          { return *(unsigned int*)0x00C9A4D8; }
inline unsigned int& GetCurrentStencilRef( void )           { return *(unsigned int*)0x00C9A4DC; }
inline unsigned int& GetCurrentStencilMask( void )          { return *(unsigned int*)0x00C9A4E0; }
inline unsigned int& GetCurrentStencilWriteMask( void )     { return *(unsigned int*)0x00C9A4E4; }
inline unsigned int& GetCurrentAlphaBlendEnable( void )     { return *(unsigned int*)0x00C9A4E8; }
inline unsigned int& GetCurrentCullMode( void )             { return *(unsigned int*)0x00C9A4F0; }
inline unsigned int& GetCurrentSrcBlend( void )             { return *(unsigned int*)0x00C9A5C8; }
inline unsigned int& GetCurrentDstBlend( void )             { return *(unsigned int*)0x00C9A5CC; }
inline unsigned int& GetCurrentAlphaRefFunc( void )         { return *(unsigned int*)0x00C9A5D0; }
inline unsigned int& GetCurrentAlphaTestEnable( void )      { return *(unsigned int*)0x00C9A5D4; }

static const D3DFOGMODE _fogTableModes[] =
{
    D3DFOG_NONE,
    D3DFOG_LINEAR,
    D3DFOG_EXP,
    D3DFOG_EXP2
};

inline int RwD3D9SetFogColor( unsigned int color )
{
    HOOK_RwD3D9SetRenderState( D3DRS_FOGCOLOR, color );

    GetCurrentFogColor() = color;
    return 1;
}

inline int RwD3D9ResetFogColor( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_FOGCOLOR, GetCurrentFogColor() );
    return 1;
}

static const D3DSHADEMODE _shadeModeTable[] =
{
    (D3DSHADEMODE)0,
    D3DSHADE_FLAT,
    D3DSHADE_GOURAUD
};

inline int RwD3D9SetShadeMode( unsigned int mode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_SHADEMODE, _shadeModeTable[mode] );

    GetCurrentShadeMode() = mode;
    return 1;
}

inline int RwD3D9ResetShadeMode( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_SHADEMODE, _shadeModeTable[GetCurrentShadeMode()] );
    return 1;
}

inline int RwD3D9SetZWriteEnable( unsigned int enable )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, enable );

    GetCurrentZWriteEnable() = enable;
    return 1;
}

inline int RwD3D9ResetZWriteEnable( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ZWRITEENABLE, GetCurrentZWriteEnable() );
    return 1;
}

static inline void _RwD3D9UpdateDepthFunctionEnable( unsigned int enable )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ZFUNC, ( enable ) ? D3DCMP_LESSEQUAL : D3DCMP_ALWAYS );
}

inline int RwD3D9SetDepthFunctionEnable( unsigned int enable )
{
    _RwD3D9UpdateDepthFunctionEnable( enable );

    GetCurrentDepthFunctionEnable() = enable;
    return 1;
}

inline int RwD3D9ResetDepthFunctionEnable( void )
{
    _RwD3D9UpdateDepthFunctionEnable( GetCurrentDepthFunctionEnable() );
    return 1;
}

inline int RwD3D9SetStencilEnable( unsigned int enable )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILENABLE, enable );

    GetCurrentStencilEnable() = enable;
    return 1;
}

inline int RwD3D9ResetStencilEnable( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILENABLE, GetCurrentStencilEnable() );
    return 1;
}

static const D3DSTENCILOP _stencilOpTable[] =
{
    (D3DSTENCILOP)0,
    D3DSTENCILOP_KEEP,
    D3DSTENCILOP_ZERO,
    D3DSTENCILOP_REPLACE,
    D3DSTENCILOP_REPLACE,
    D3DSTENCILOP_INCRSAT,
    D3DSTENCILOP_DECRSAT,
    D3DSTENCILOP_INVERT,
    D3DSTENCILOP_INCR,
    D3DSTENCILOP_DECR
};

inline int RwD3D9SetStencilFail( unsigned int opMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILFAIL, _stencilOpTable[opMode] );

    GetCurrentStencilFail() = opMode;
    return 1;
}

inline int RwD3D9ResetStencilFail( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILFAIL, _stencilOpTable[GetCurrentStencilFail()] );
    return 1;
}

inline int RwD3D9SetStencilZFail( unsigned int opMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILZFAIL, _stencilOpTable[opMode] );

    GetCurrentStencilZFail() = opMode;
    return 1;
}

inline int RwD3D9ResetStencilZFail( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILZFAIL, _stencilOpTable[GetCurrentStencilZFail()] );
    return 1;
}

inline int RwD3D9SetStencilPass( unsigned int opMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILPASS, _stencilOpTable[opMode] );

    GetCurrentStencilPass() = opMode;
    return 1;
}

inline int RwD3D9ResetStencilPass( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILPASS, _stencilOpTable[GetCurrentStencilPass()] );
    return 1;
}

static const D3DCMPFUNC _cmpOpTable[] =
{
    (D3DCMPFUNC)0,
    D3DCMP_NEVER,
    D3DCMP_LESS,
    D3DCMP_EQUAL,
    D3DCMP_LESSEQUAL,
    D3DCMP_GREATER,
    D3DCMP_NOTEQUAL,
    D3DCMP_GREATEREQUAL,
    D3DCMP_ALWAYS
};

inline int RwD3D9SetStencilFunc( unsigned int cmpOp )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILFUNC, _cmpOpTable[cmpOp] );

    GetCurrentStencilFunc() = cmpOp;
    return 1;
}

inline int RwD3D9ResetStencilFunc( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILFUNC, _cmpOpTable[GetCurrentStencilFunc()] );
    return 1;
}

inline int RwD3D9SetStencilRef( unsigned int ref )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILREF, ref );

    GetCurrentStencilRef() = ref;
    return 1;
}

inline int RwD3D9ResetStencilRef( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILREF, GetCurrentStencilRef() );
    return 1;
}

inline int RwD3D9SetStencilMask( unsigned int mask )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILMASK, mask );

    GetCurrentStencilMask() = mask;
    return 1;
}

inline int RwD3D9ResetStencilMask( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILMASK, GetCurrentStencilMask() );
    return 1;
}

inline int RwD3D9SetStencilWriteMask( unsigned int mask )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILWRITEMASK, mask );

    GetCurrentStencilWriteMask() = mask;
    return 1;
}

inline int RwD3D9ResetStencilWriteMask( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_STENCILWRITEMASK, GetCurrentStencilWriteMask() );
    return 1;
}

struct _samplerFilterMode
{
    D3DTEXTUREFILTERTYPE filterType;
    D3DTEXTUREFILTERTYPE mipFilterType;
};

static const _samplerFilterMode _samplerFilterModes[] =
{
    { D3DTEXF_NONE, D3DTEXF_NONE },
    { D3DTEXF_POINT, D3DTEXF_NONE },
    { D3DTEXF_LINEAR, D3DTEXF_NONE },
    { D3DTEXF_NONE, D3DTEXF_POINT },
    { D3DTEXF_POINT, D3DTEXF_POINT },
    { D3DTEXF_LINEAR, D3DTEXF_POINT },
    { D3DTEXF_NONE, D3DTEXF_LINEAR },
    { D3DTEXF_POINT, D3DTEXF_LINEAR },
    { D3DTEXF_LINEAR, D3DTEXF_LINEAR },
    { D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR }
};

static inline void _RwD3D9RasterStageUpdateFilterMode( unsigned int stageIdx, const _samplerFilterMode& mode )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MINFILTER, mode.filterType );
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MAGFILTER, mode.filterType );

    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MIPFILTER, mode.mipFilterType );
}

inline int RwD3D9RasterStageSetFilterMode( unsigned int stageIdx, unsigned int filterMode )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    const _samplerFilterMode& mode = _samplerFilterModes[filterMode];

    _RwD3D9RasterStageUpdateFilterMode( stageIdx, mode );

    rasterStage.filterType = filterMode;
    return 1;
}

inline int RwD3D9RasterStageResetFilterMode( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    const _samplerFilterMode& mode = _samplerFilterModes[rasterStage.filterType];

    _RwD3D9RasterStageUpdateFilterMode( stageIdx, mode );
    return 1;
}

static const D3DTEXTUREADDRESS _textureAddressModes[] =
{
    (D3DTEXTUREADDRESS)0,
    D3DTADDRESS_WRAP,
    D3DTADDRESS_MIRROR,
    D3DTADDRESS_CLAMP,
    D3DTADDRESS_BORDER
};

static inline void _RwD3D9RasterStageUpdateAddressModeU( unsigned int stageIdx, unsigned int modeIdx )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_ADDRESSU, _textureAddressModes[modeIdx] );
}

inline int RwD3D9RasterStageSetAddressModeU( unsigned int stageIdx, unsigned int modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeU( stageIdx, modeIdx );

    rasterStage.u_addressMode = modeIdx;
    return 1;
}

inline int RwD3D9RasterStageResetAddressModeU( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeU( stageIdx, rasterStage.u_addressMode );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateAddressModeV( unsigned int stageIdx, unsigned int modeIdx )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_ADDRESSV, _textureAddressModes[modeIdx] );
}

inline int RwD3D9RasterStageSetAddressModeV( unsigned int stageIdx, unsigned int modeIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeV( stageIdx, modeIdx );

    rasterStage.v_addressMode = modeIdx;
    return 1;
}

inline int RwD3D9RasterStageResetAddressModeV( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateAddressModeV( stageIdx, rasterStage.v_addressMode );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateBorderColor( unsigned int stageIdx, unsigned int color )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_BORDERCOLOR, color );
}

inline int RwD3D9RasterStageSetBorderColor( unsigned int stageIdx, unsigned int color )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateBorderColor( stageIdx, color );

    rasterStage.borderColor = color;
    return 1;
}

inline int RwD3D9RasterStageResetBorderColor( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateBorderColor( stageIdx, rasterStage.borderColor );
    return 1;
}

static inline void _RwD3D9RasterStageUpdateMaxAnisotropy( unsigned int stageIdx, unsigned int maxAnisotropy )
{
    RwD3D9SetSamplerState( stageIdx, D3DSAMP_MAXANISOTROPY, maxAnisotropy );
}

inline int RwD3D9RasterStageSetMaxAnisotropy( unsigned int stageIdx, unsigned int maxAnisotropy )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateMaxAnisotropy( stageIdx, maxAnisotropy );

    rasterStage.maxAnisotropy = maxAnisotropy;
    return 1;
}

inline int RwD3D9RasterStageResetMaxAnisotropy( unsigned int stageIdx )
{
    d3d9RasterStage& rasterStage = GetRasterStageInfo( stageIdx );

    _RwD3D9RasterStageUpdateMaxAnisotropy( stageIdx, rasterStage.maxAnisotropy );
    return 1;
}

static const D3DBLEND blendModes[] =
{
    (D3DBLEND)0,
    D3DBLEND_ZERO,
    D3DBLEND_ONE,
    D3DBLEND_SRCCOLOR,
    D3DBLEND_INVSRCCOLOR,
    D3DBLEND_SRCALPHA,
    D3DBLEND_INVSRCALPHA,
    D3DBLEND_DESTALPHA,
    D3DBLEND_INVDESTALPHA,
    D3DBLEND_DESTCOLOR,
    D3DBLEND_INVDESTCOLOR,
    D3DBLEND_SRCALPHASAT
};

inline int RwD3D9ResetSrcBlend( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_SRCBLEND, blendModes[GetCurrentSrcBlend()] );
    return 1;
}

inline int _RwD3D9SetSrcBlend( unsigned int blendMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_SRCBLEND, blendModes[blendMode] );

    GetCurrentSrcBlend() = blendMode;
    return 1;
}

int RwD3D9SetSrcBlend( unsigned int blendMode )
{
    if ( GetCurrentSrcBlend() != blendMode )
        _RwD3D9SetSrcBlend( blendMode );

    return 1;
}

inline int RwD3D9ResetDstBlend( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_DESTBLEND, blendModes[GetCurrentDstBlend()] );
    return 1;
}

inline int _RwD3D9SetDstBlend( unsigned int blendMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_DESTBLEND, blendModes[blendMode] );

    GetCurrentDstBlend() = blendMode;
    return 1;
}

int RwD3D9SetDstBlend( unsigned int blendMode )
{
    if ( GetCurrentDstBlend() != blendMode )
        _RwD3D9SetDstBlend( blendMode );

    return 1;
}

inline int RwD3D9ResetAlphaFunc( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAFUNC, _cmpOpTable[GetCurrentAlphaRefFunc()] );
    return 1;
}

inline int RwD3D9SetAlphaFunc( unsigned int cmpOp )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAFUNC, _cmpOpTable[cmpOp] );

    GetCurrentAlphaRefFunc() = cmpOp;
    return 1;
}

inline void _RwD3D9UpdateAlphaEnable( unsigned int blendEnable, unsigned int testEnable )
{
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHABLENDENABLE, blendEnable );
    HOOK_RwD3D9SetRenderState( D3DRS_ALPHATESTENABLE, ( blendEnable && testEnable ) );
}

inline int RwD3D9SetAlphaEnable( unsigned int blendEnable, unsigned int testEnable )
{
    _RwD3D9UpdateAlphaEnable( blendEnable, testEnable );

    GetCurrentAlphaBlendEnable() = blendEnable;
    GetCurrentAlphaTestEnable() = testEnable;
    return 1;
}

inline int RwD3D9ResetAlphaEnable( void )
{
    _RwD3D9UpdateAlphaEnable( GetCurrentAlphaBlendEnable(), GetCurrentAlphaTestEnable() );
    return 1;
}

static const D3DCULL cullModes[] =
{
    (D3DCULL)0,
    D3DCULL_NONE,
    D3DCULL_CW,
    D3DCULL_CCW
};

inline int RwD3D9SetCullMode( unsigned int cullMode )
{
    HOOK_RwD3D9SetRenderState( D3DRS_CULLMODE, cullModes[cullMode] );

    GetCurrentCullMode() = cullMode;
    return 1;
}

inline int RwD3D9ResetCullMode( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_CULLMODE, cullModes[GetCurrentCullMode()] );
    return 1;
}

inline int RwD3D9SetFogEnable( unsigned int enable )
{
    HOOK_RwD3D9SetRenderState( D3DRS_FOGENABLE, enable );

    GetCurrentFogEnable() = enable;
    return 1;
}

inline int RwD3D9ResetFogEnable( void )
{
    HOOK_RwD3D9SetRenderState( D3DRS_FOGENABLE, GetCurrentFogEnable() );
    return 1;
}

/*=========================================================
    RwD3D9_RwSetRenderState

    Arguments:
        deviceType - type of the device state to change
        value - parameter to pass to the device
    Purpose:
        Changes device states of the RenderWare rendering
        context. This is the function that shall be used by
        game code.
    Binary offsets:
        (1.0 US): 0x007FE420
        (1.0 EU): 0x007FE460
=========================================================*/
void __cdecl RwD3D9_RwSetRenderState( eRwDeviceCmd deviceType, unsigned int value )
{
    switch( deviceType )
    {
    case RWSTATE_FOGENABLE:
        if ( value && !GetCurrentFogEnable() && !IS_ANY_FLAG( *(unsigned int*)0x00C9BF20, 0x180 ) )
            return;

        if ( value != GetCurrentFogEnable() )
            RwD3D9SetFogEnable( value );

        break;
    case RWSTATE_FOGCOLOR:
        if ( value != GetCurrentFogColor() )
            RwD3D9SetFogColor( value );

        break;

    }
}

/*=========================================================
    RwD3D9InitializeDeviceStates

    Purpose:
        Sets up the RenderWare Direct3D 9 device state
        environment. After this, the RenderWare API is ready
        for usage.
    Binary offsets:
        (1.0 US): 0x007FCAC0
        (1.0 EU): 0x007FCB00
=========================================================*/
inline void RwD3D9SetupRenderingTransformation( void )
{
    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

    HOOK_RwD3D9SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL );
    HOOK_RwD3D9SetRenderState( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
    HOOK_RwD3D9SetRenderState( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
    
    HOOK_RwD3D9SetRenderState( D3DRS_DITHERENABLE, IS_ANY_FLAG( *(unsigned int*)0x00C9BF24, 0x01 ) );
    HOOK_RwD3D9SetRenderState( D3DRS_SPECULARENABLE, false );
    HOOK_RwD3D9SetRenderState( D3DRS_LOCALVIEWER, false );
    HOOK_RwD3D9SetRenderState( D3DRS_AMBIENT, 0xFFFFFFFF );
    HOOK_RwD3D9SetRenderState( D3DRS_NORMALIZENORMALS, false );
}

void __cdecl RwD3D9InitializeDeviceStates( void )
{
    // Set current device states.
    RwD3D9InitializeCurrentStates();

    GetCurrentFogTableIndex() = 1;
    GetCurrentFogDensity() = 1.0f;

    // Set states.
    RwD3D9SetFogEnable( false );

    unsigned int startupFlags = *(unsigned int*)0x00C9BF24;

    if ( IS_ANY_FLAG( startupFlags, 0x100100 ) )
    {
        D3DFOGMODE fogMode = _fogTableModes[GetCurrentFogTableIndex()];

        HOOK_RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, fogMode );
    }
    else
    {
        HOOK_RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    }

    HOOK_RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE, _fogTableModes[GetCurrentFogTableIndex()] );
    RwD3D9SetFogColor( 0x00000000 );
    RwD3D9SetShadeMode( 2 );

    RwD3D9SetDepthFunctionEnable( true );
    RwD3D9SetZWriteEnable( true );
    HOOK_RwD3D9SetRenderState( D3DRS_ZENABLE, true );

    RwD3D9SetStencilEnable( false );
    RwD3D9SetStencilFail( D3DSTENCILOP_KEEP );
    RwD3D9SetStencilZFail( D3DSTENCILOP_KEEP );
    RwD3D9SetStencilPass( D3DSTENCILOP_KEEP );
    RwD3D9SetStencilFunc( D3DCMP_ALWAYS );
    RwD3D9SetStencilRef( 0 );
    RwD3D9SetStencilMask( 0xFFFFFFFF );
    RwD3D9SetStencilWriteMask( 0xFFFFFFFF );

    // Initialize the texture stage states and sampler states.
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    for ( unsigned int stage = 0; stage < MAX_SAMPLERS; stage++ )
    {
        d3d9RasterStage& rasterStage = GetRasterStageInfo( stage );

        // Make sure there is no texture attached.
        renderDevice->SetTexture( stage, NULL );

        rasterStage.raster = NULL;

        unsigned int samplerStartupFlags = *(unsigned int*)0x00C9BF40;

        bool makeSamplerActive = IS_ANY_FLAG( samplerStartupFlags, 0x200 );

        RwD3D9RasterStageSetFilterMode( stage, ( makeSamplerActive ) ? 2 : 0 );
        RwD3D9RasterStageSetAddressModeU( stage, D3DTADDRESS_WRAP );
        RwD3D9RasterStageSetAddressModeV( stage, D3DTADDRESS_WRAP );
        RwD3D9RasterStageSetBorderColor( stage, 0xFF000000 );
        RwD3D9RasterStageSetMaxAnisotropy( stage, 1 );

        if ( stage != 0 )
        {
            RwD3D9SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    _RwD3D9SetSrcBlend( D3DBLEND_SRCALPHA );
    _RwD3D9SetDstBlend( D3DBLEND_INVSRCALPHA );

    RwD3D9SetAlphaFunc( D3DCMP_GREATER );

    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAREF, 0 );
    RwD3D9SetAlphaEnable( false, true );

    *(RwRaster**)0x00C9A4EC = NULL;

    HOOK_RwD3D9SetRenderState( D3DRS_LIGHTING, false );
    
    RwD3D9SetCullMode( D3DCULL_CW );

    RwD3D9SetupRenderingTransformation();

    RwD3D9ApplyDeviceStates();
}

/*=========================================================
    RwD3D9ResetDeviceStates

    Purpose:
        Resets the device states as if the device was reset
        to the beginning.
    Binary offsets:
        (1.0 US): 0x007FD100
        (1.0 EU): 0x007FD140
=========================================================*/
void __cdecl RwD3D9ResetDeviceStates( void )
{
    DWORD previousFogStart;
    DWORD previousFogEnd;
    DWORD previousAlphaRef;

    HOOK_RwD3D9GetRenderState( D3DRS_FOGSTART, previousFogStart );
    HOOK_RwD3D9GetRenderState( D3DRS_FOGEND, previousFogEnd );
    HOOK_RwD3D9GetRenderState( D3DRS_ALPHAREF, previousAlphaRef );

    // Get the device states to the beginning.
    RwD3D9InitializeCurrentStates();

    HOOK_RwD3D9SetRenderState( D3DRS_FOGDENSITY, F2DW( GetCurrentFogDensity() ) );
    HOOK_RwD3D9SetRenderState( D3DRS_FOGENABLE, GetCurrentFogEnable() );

    unsigned int startupFlags = *(unsigned int*)0x00C9BF24;

    if ( IS_ANY_FLAG( startupFlags, 0x100100 ) )
    {
        D3DFOGMODE fogMode = _fogTableModes[GetCurrentFogTableIndex()];

        HOOK_RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, fogMode );
    }
    else
    {
        HOOK_RwD3D9SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_NONE );
    }

    HOOK_RwD3D9SetRenderState( D3DRS_FOGVERTEXMODE, _fogTableModes[GetCurrentFogTableIndex()] );
    HOOK_RwD3D9SetRenderState( D3DRS_FOGSTART, previousFogStart );
    HOOK_RwD3D9SetRenderState( D3DRS_FOGEND, previousFogEnd );
    RwD3D9ResetFogColor();
    
    RwD3D9ResetShadeMode();

    RwD3D9ResetDepthFunctionEnable();
    RwD3D9SetZWriteEnable( true );
    HOOK_RwD3D9SetRenderState( D3DRS_ZENABLE, true );

    RwD3D9ResetStencilEnable();
    RwD3D9ResetStencilFail();
    RwD3D9ResetStencilZFail();
    RwD3D9ResetStencilPass();
    RwD3D9ResetStencilFunc();
    RwD3D9ResetStencilRef();
    RwD3D9ResetStencilMask();
    RwD3D9ResetStencilWriteMask();

    // Initialize the texture stage states and sampler states.
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    for ( unsigned int stage = 0; stage < MAX_SAMPLERS; stage++ )
    {
        d3d9RasterStage& rasterStage = GetRasterStageInfo( stage );

        // Make sure there is no texture attached.
        renderDevice->SetTexture( stage, NULL );

        rasterStage.raster = NULL;

        RwD3D9RasterStageResetFilterMode( stage );
        RwD3D9RasterStageResetAddressModeU( stage );
        RwD3D9RasterStageResetAddressModeV( stage );
        RwD3D9RasterStageResetBorderColor( stage );
        RwD3D9RasterStageResetMaxAnisotropy( stage );

        if ( stage != 0 )
        {
            RwD3D9SetTextureStageState( stage, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    // MTA extension: make sure we reset the light states.
    RpD3D9LightingOnDeviceReset();

    RwD3D9ResetSrcBlend();
    RwD3D9ResetDstBlend();

    RwD3D9ResetAlphaFunc();

    HOOK_RwD3D9SetRenderState( D3DRS_ALPHAREF, previousAlphaRef );
    RwD3D9ResetAlphaEnable();

    *(RwRaster**)0x00C9A4EC = NULL;

    HOOK_RwD3D9SetRenderState( D3DRS_LIGHTING, false );
    
    RwD3D9ResetCullMode();

    RwD3D9SetupRenderingTransformation();

    RwD3D9ApplyDeviceStates();
}