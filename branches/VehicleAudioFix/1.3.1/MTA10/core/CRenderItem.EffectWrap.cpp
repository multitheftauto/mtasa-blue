/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CRenderItem.EffectWrap.cpp
*  PURPOSE:
*  DEVELOPERS:  zidiot
*
*****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectCloner.h"

enum EStateGroup
{
    STATE_GROUP_RENDER,
    STATE_GROUP_STAGE,
    STATE_GROUP_SAMPLER,
    STATE_GROUP_MATERIAL,
    STATE_GROUP_TRANSFORM,
    STATE_GROUP_TEXTURE,
    STATE_GROUP_LIGHT,
    STATE_GROUP_LIGHT_ENABLED,
    STATE_GROUP_DEVICE_CAPS,
    STATE_GROUP_VERTEX_DECL,
};
DECLARE_ENUM( EStateGroup )

IMPLEMENT_ENUM_BEGIN( EStateGroup )
    ADD_ENUM( STATE_GROUP_RENDER,       "renderState" )
    ADD_ENUM( STATE_GROUP_STAGE,        "stageState" )
    ADD_ENUM( STATE_GROUP_SAMPLER,      "samplerState" )
    ADD_ENUM( STATE_GROUP_MATERIAL,     "materialState" )
    ADD_ENUM( STATE_GROUP_TRANSFORM,    "transformState" )
    ADD_ENUM( STATE_GROUP_TEXTURE,      "textureState" )
    ADD_ENUM( STATE_GROUP_LIGHT,        "lightState" )
    ADD_ENUM( STATE_GROUP_LIGHT_ENABLED,"lightEnabledState" )
    ADD_ENUM( STATE_GROUP_DEVICE_CAPS,  "deviceCaps" )
    ADD_ENUM( STATE_GROUP_VERTEX_DECL,  "vertexDeclState" )
IMPLEMENT_ENUM_END( "state-group" )


enum EReadableAsType
{
    TYPE_INT,               // int
    TYPE_D3DCOLOR,          // Can be read as float4 or DWORD color
    TYPE_IFLOAT,            // Can be read as float or as int on float memory
    TYPE_D3DCOLORVALUE,     // float4
    TYPE_FLOAT,             // float
    TYPE_VECTOR3,           // float3
    TYPE_MATRIX,            // float4x4
    TYPE_TEXTURE,
};

 
namespace RegMap
{
    // Vector is 3 floats
    // Color is 4 floats
    // Matrix is 16 floats
    enum ETranslateType
    {
        Int2Int,
        Int2Color,
        Int2Float,
        Float2Float,
        Color2Color,
        Vector2Vector,
        Matrix2Matrix,
        Texture2Texture,
    };
}


// Map effect parameter to something
struct STypeMapping
{
    EReadableAsType         readableAsType;
    D3DXPARAMETER_CLASS     ParamMatchClass;
    D3DXPARAMETER_TYPE      ParamMatchType;
    RegMap::ETranslateType  OutType;
    int                     OutSize;
};

STypeMapping TypeMappingList[] = {
        { TYPE_INT,             D3DXPC_SCALAR,      D3DXPT_INT,         RegMap::Int2Int,            1 },
        { TYPE_D3DCOLOR,        D3DXPC_SCALAR,      D3DXPT_INT,         RegMap::Int2Int,            1 },    // Can be read as float4 or DWORD color
        { TYPE_D3DCOLOR,        D3DXPC_VECTOR,      D3DXPT_FLOAT,       RegMap::Int2Color,          4 },
        { TYPE_IFLOAT,          D3DXPC_SCALAR,      D3DXPT_FLOAT,       RegMap::Int2Float,          1 },    // Can be read as float or as int on float memory
        { TYPE_IFLOAT,          D3DXPC_SCALAR,      D3DXPT_INT,         RegMap::Int2Int,            1 },    // Can be read as float or as int on float memory
        { TYPE_FLOAT,           D3DXPC_SCALAR,      D3DXPT_FLOAT,       RegMap::Float2Float,        1 },
        { TYPE_D3DCOLORVALUE,   D3DXPC_VECTOR,      D3DXPT_FLOAT,       RegMap::Color2Color,        4 },
        { TYPE_VECTOR3,         D3DXPC_VECTOR,      D3DXPT_FLOAT,       RegMap::Vector2Vector,      3 },
        { TYPE_MATRIX,          D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT,       RegMap::Matrix2Matrix,      16 },
        { TYPE_TEXTURE,         D3DXPC_OBJECT,      D3DXPT_TEXTURE,     RegMap::Texture2Texture,    1 },
        { TYPE_TEXTURE,         D3DXPC_OBJECT,      D3DXPT_TEXTURE1D,   RegMap::Texture2Texture,    1 },
        { TYPE_TEXTURE,         D3DXPC_OBJECT,      D3DXPT_TEXTURE2D,   RegMap::Texture2Texture,    1 },
        { TYPE_TEXTURE,         D3DXPC_OBJECT,      D3DXPT_TEXTURE3D,   RegMap::Texture2Texture,    1 },
        { TYPE_TEXTURE,         D3DXPC_OBJECT,      D3DXPT_TEXTURECUBE, RegMap::Texture2Texture,    1 },
    };


struct SStateVar
{
    EStateGroup             stateGroup;
    D3DXHANDLE              hHandle;
    int                     iStage;
    int                     iRegister;
    RegMap::ETranslateType  iType;
    int                     iSize;
};


// Info about one D3D register
struct SRegisterInfo
{
    int                 iOffset;
    const char*         szGroupName;
    const char*         szRegName;
    EReadableAsType     eReadableAsType;
};


//
// Big list of D3D registers
//

#define MEMBER_OFFSET( cls, member ) \
    ( (ulong)&((cls*)NULL)->member )

#define ADD_REGISTER(type,name) \
    { MEMBER_OFFSET( USING_STRUCT, name ), USING_LIST, #name, type },


    SRegisterInfo BigRegisterInfoList [] = {

#define USING_STRUCT CProxyDirect3DDevice9::SD3DRenderState
#define USING_LIST "RenderState"
                ADD_REGISTER( TYPE_INT, ZENABLE )                         //  = 7,    /* D3DZBUFFERTYPE (or TRUE/FALSE for legacy) */
                ADD_REGISTER( TYPE_INT, FILLMODE )                        //  = 8,    /* D3DFILLMODE */
                ADD_REGISTER( TYPE_INT, SHADEMODE )                       //  = 9,    /* D3DSHADEMODE */
                ADD_REGISTER( TYPE_INT, ZWRITEENABLE )                    //  = 14,   /* TRUE to enable z writes */
                ADD_REGISTER( TYPE_INT, ALPHATESTENABLE )                 //  = 15,   /* TRUE to enable alpha tests */
                ADD_REGISTER( TYPE_INT, LASTPIXEL )                       //  = 16,   /* TRUE for last-pixel on lines */
                ADD_REGISTER( TYPE_INT, SRCBLEND )                        //  = 19,   /* D3DBLEND */
                ADD_REGISTER( TYPE_INT, DESTBLEND )                       //  = 20,   /* D3DBLEND */
                ADD_REGISTER( TYPE_INT, CULLMODE )                        //  = 22,   /* D3DCULL */
                ADD_REGISTER( TYPE_INT, ZFUNC )                           //  = 23,   /* D3DCMPFUNC */
                ADD_REGISTER( TYPE_INT, ALPHAREF )                        //  = 24,   /* D3DFIXED */
                ADD_REGISTER( TYPE_INT, ALPHAFUNC )                       //  = 25,   /* D3DCMPFUNC */
                ADD_REGISTER( TYPE_INT, DITHERENABLE )                    //  = 26,   /* TRUE to enable dithering */
                ADD_REGISTER( TYPE_INT, ALPHABLENDENABLE )                //  = 27,   /* TRUE to enable alpha blending */
                ADD_REGISTER( TYPE_INT, FOGENABLE )                       //  = 28,   /* TRUE to enable fog blending */
                ADD_REGISTER( TYPE_INT, SPECULARENABLE )                  //  = 29,   /* TRUE to enable specular */
                ADD_REGISTER( TYPE_D3DCOLOR, FOGCOLOR )                   //  = 34,   /* D3DCOLOR */
                ADD_REGISTER( TYPE_INT, FOGTABLEMODE )                    //  = 35,   /* D3DFOGMODE */
                ADD_REGISTER( TYPE_IFLOAT, FOGSTART )                     //  = 36,   /* Fog start (for both vertex and pixel fog) */
                ADD_REGISTER( TYPE_IFLOAT, FOGEND )                       //  = 37,   /* Fog end      */
                ADD_REGISTER( TYPE_IFLOAT, FOGDENSITY )                   //  = 38,   /* Fog density  */
                ADD_REGISTER( TYPE_INT, RANGEFOGENABLE )                  //  = 48,   /* Enables range-based fog */
                ADD_REGISTER( TYPE_INT, STENCILENABLE )                   //  = 52,   /* BOOL enable/disable stenciling */
                ADD_REGISTER( TYPE_INT, STENCILFAIL )                     //  = 53,   /* D3DSTENCILOP to do if stencil test fails */
                ADD_REGISTER( TYPE_INT, STENCILZFAIL )                    //  = 54,   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
                ADD_REGISTER( TYPE_INT, STENCILPASS )                     //  = 55,   /* D3DSTENCILOP to do if both stencil and Z tests pass */
                ADD_REGISTER( TYPE_INT, STENCILFUNC )                     //  = 56,   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
                ADD_REGISTER( TYPE_INT, STENCILREF )                      //  = 57,   /* Reference value used in stencil test */
                ADD_REGISTER( TYPE_INT, STENCILMASK )                     //  = 58,   /* Mask value used in stencil test */
                ADD_REGISTER( TYPE_INT, STENCILWRITEMASK )                //  = 59,   /* Write mask applied to values written to stencil buffer */
                ADD_REGISTER( TYPE_D3DCOLOR, TEXTUREFACTOR )              //  = 60,   /* D3DCOLOR used for multi-texture blend */
                ADD_REGISTER( TYPE_INT, WRAP0 )                           //  = 128,  /* wrap for 1st texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP1 )                           //  = 129,  /* wrap for 2nd texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP2 )                           //  = 130,  /* wrap for 3rd texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP3 )                           //  = 131,  /* wrap for 4th texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP4 )                           //  = 132,  /* wrap for 5th texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP5 )                           //  = 133,  /* wrap for 6th texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP6 )                           //  = 134,  /* wrap for 7th texture coord. set */
                ADD_REGISTER( TYPE_INT, WRAP7 )                           //  = 135,  /* wrap for 8th texture coord. set */
                ADD_REGISTER( TYPE_INT, CLIPPING )                        //  = 136,
                ADD_REGISTER( TYPE_INT, LIGHTING )                        //  = 137,
                ADD_REGISTER( TYPE_D3DCOLOR, AMBIENT )                    //  = 139,
                ADD_REGISTER( TYPE_INT, FOGVERTEXMODE )                   //  = 140,
                ADD_REGISTER( TYPE_INT, COLORVERTEX )                     //  = 141,
                ADD_REGISTER( TYPE_INT, LOCALVIEWER )                     //  = 142,
                ADD_REGISTER( TYPE_INT, NORMALIZENORMALS )                //  = 143,
                ADD_REGISTER( TYPE_INT, DIFFUSEMATERIALSOURCE )           //  = 145,
                ADD_REGISTER( TYPE_INT, SPECULARMATERIALSOURCE )          //  = 146,
                ADD_REGISTER( TYPE_INT, AMBIENTMATERIALSOURCE )           //  = 147,
                ADD_REGISTER( TYPE_INT, EMISSIVEMATERIALSOURCE )          //  = 148,
                ADD_REGISTER( TYPE_INT, VERTEXBLEND )                     //  = 151,
                ADD_REGISTER( TYPE_INT, CLIPPLANEENABLE )                 //  = 152,
                ADD_REGISTER( TYPE_IFLOAT, POINTSIZE )                    //  = 154,   /* float point size */
                ADD_REGISTER( TYPE_IFLOAT, POINTSIZE_MIN )                //  = 155,   /* float point size min threshold */
                ADD_REGISTER( TYPE_INT, POINTSPRITEENABLE )               //  = 156,   /* BOOL point texture coord control */
                ADD_REGISTER( TYPE_INT, POINTSCALEENABLE )                //  = 157,   /* BOOL point size scale enable */
                ADD_REGISTER( TYPE_IFLOAT, POINTSCALE_A )                 //  = 158,   /* float point attenuation A value */
                ADD_REGISTER( TYPE_IFLOAT, POINTSCALE_B )                 //  = 159,   /* float point attenuation B value */
                ADD_REGISTER( TYPE_IFLOAT, POINTSCALE_C )                 //  = 160,   /* float point attenuation C value */
                ADD_REGISTER( TYPE_INT, MULTISAMPLEANTIALIAS )            //  = 161,  // BOOL - set to do FSAA with multisample buffer
                ADD_REGISTER( TYPE_INT, MULTISAMPLEMASK )                 //  = 162,  // DWORD - )        //per-sample enable/disable
                ADD_REGISTER( TYPE_INT, PATCHEDGESTYLE )                  //  = 163,  // Sets whether patch edges will use float style tessellation
                ADD_REGISTER( TYPE_INT, DEBUGMONITORTOKEN )               //  = 165,  // DEBUG ONLY - token to debug monitor
                ADD_REGISTER( TYPE_IFLOAT, POINTSIZE_MAX )                //  = 166,   /* float point size max threshold */
                ADD_REGISTER( TYPE_INT,  INDEXEDVERTEXBLENDENABLE )       //  = 167,
                ADD_REGISTER( TYPE_INT,  COLORWRITEENABLE )               //  = 168,  // per-channel write enable
                ADD_REGISTER( TYPE_IFLOAT, TWEENFACTOR )                  //  = 170,   // float tween factor
                ADD_REGISTER( TYPE_INT,  BLENDOP )                        //  = 171,   // D3DBLENDOP setting
                ADD_REGISTER( TYPE_INT,  POSITIONDEGREE )                 //  = 172,   // NPatch position interpolation degree. D3DDEGREE_LINEAR or D3DDEGREE_CUBIC (default)
                ADD_REGISTER( TYPE_INT,  NORMALDEGREE )                   //  = 173,   // NPatch normal interpolation degree. D3DDEGREE_LINEAR (default) or D3DDEGREE_QUADRATIC
                ADD_REGISTER( TYPE_INT,  SCISSORTESTENABLE )              //  = 174,
                ADD_REGISTER( TYPE_INT,  SLOPESCALEDEPTHBIAS )            //  = 175,
                ADD_REGISTER( TYPE_INT,  ANTIALIASEDLINEENABLE )          //  = 176,
                ADD_REGISTER( TYPE_INT,  MINTESSELLATIONLEVEL )           //  = 178,
                ADD_REGISTER( TYPE_INT,  MAXTESSELLATIONLEVEL )           //  = 179,
                ADD_REGISTER( TYPE_INT,  ADAPTIVETESS_X )                 //  = 180,
                ADD_REGISTER( TYPE_INT,  ADAPTIVETESS_Y )                 //  = 181,
                ADD_REGISTER( TYPE_INT,  ADAPTIVETESS_Z )                 //  = 182,
                ADD_REGISTER( TYPE_INT,  ADAPTIVETESS_W )                 //  = 183,
                ADD_REGISTER( TYPE_INT,  ENABLEADAPTIVETESSELLATION )     //  = 184,
                ADD_REGISTER( TYPE_INT,  TWOSIDEDSTENCILMODE )            //  = 185,   /* BOOL enable/disable 2 sided stenciling */
                ADD_REGISTER( TYPE_INT,  CCW_STENCILFAIL )                //  = 186,   /* D3DSTENCILOP to do if ccw stencil test fails */
                ADD_REGISTER( TYPE_INT,  CCW_STENCILZFAIL )               //  = 187,   /* D3DSTENCILOP to do if ccw stencil test passes and Z test fails */
                ADD_REGISTER( TYPE_INT,  CCW_STENCILPASS )                //  = 188,   /* D3DSTENCILOP to do if both ccw stencil and Z tests pass */
                ADD_REGISTER( TYPE_INT,  CCW_STENCILFUNC )                //  = 189,   /* D3DCMPFUNC fn.  ccw Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
                ADD_REGISTER( TYPE_INT,  COLORWRITEENABLE1 )              //  = 190,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                ADD_REGISTER( TYPE_INT,  COLORWRITEENABLE2 )              //  = 191,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                ADD_REGISTER( TYPE_INT,  COLORWRITEENABLE3 )              //  = 192,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                ADD_REGISTER( TYPE_D3DCOLOR, BLENDFACTOR )                //  = 193,   /* D3DCOLOR used for a constant blend factor during alpha blending for devices that support D3DPBLENDCAPS_BLENDFACTOR */
                ADD_REGISTER( TYPE_INT,  SRGBWRITEENABLE )                //  = 194,   /* Enable rendertarget writes to be DE-linearized to SRGB (for formats that expose D3DUSAGE_QUERY_SRGBWRITE) */
                ADD_REGISTER( TYPE_INT,  DEPTHBIAS )                      //  = 195,
                ADD_REGISTER( TYPE_INT,  WRAP8 )                          //  = 198,   /* Additional wrap states for vs_3_0+ attributes with D3DDECLUSAGE_TEXCOORD */
                ADD_REGISTER( TYPE_INT,  WRAP9 )                          //  = 199,
                ADD_REGISTER( TYPE_INT,  WRAP10 )                         //  = 200,
                ADD_REGISTER( TYPE_INT,  WRAP11 )                         //  = 201,
                ADD_REGISTER( TYPE_INT,  WRAP12 )                         //  = 202,
                ADD_REGISTER( TYPE_INT,  WRAP13 )                         //  = 203,
                ADD_REGISTER( TYPE_INT,  WRAP14 )                         //  = 204,
                ADD_REGISTER( TYPE_INT,  WRAP15 )                         //  = 205,
                ADD_REGISTER( TYPE_INT,  SEPARATEALPHABLENDENABLE )       //  = 206,  /* TRUE to enable a separate blending function for the alpha channel */
                ADD_REGISTER( TYPE_INT,  SRCBLENDALPHA )                  //  = 207,  /* SRC blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE )        //is TRUE */
                ADD_REGISTER( TYPE_INT,  DESTBLENDALPHA )                 //  = 208,  /* DST blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE )        //is TRUE */
                ADD_REGISTER( TYPE_INT,  BLENDOPALPHA )                   //  = 209,  /* Blending operation for the alpha channel when DWORD SEPARATEDESTALPHAENABLE )        //is TRUE */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DStageState
#define USING_LIST "StageState"
                ADD_REGISTER( TYPE_INT, COLOROP )                //  =  1, /* D3DTEXTUREOP - per-stage blending controls for color channels */
                ADD_REGISTER( TYPE_INT, COLORARG1 )              //  =  2, /* D3DTA_* (texture arg) */
                ADD_REGISTER( TYPE_INT, COLORARG2 )              //  =  3, /* D3DTA_* (texture arg) */
                ADD_REGISTER( TYPE_INT, ALPHAOP )                //  =  4, /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
                ADD_REGISTER( TYPE_INT, ALPHAARG1 )              //  =  5, /* D3DTA_* (texture arg) */
                ADD_REGISTER( TYPE_INT, ALPHAARG2 )              //  =  6, /* D3DTA_* (texture arg) */
                ADD_REGISTER( TYPE_INT, BUMPENVMAT00 )           //  =  7, /* float (bump mapping matrix) */
                ADD_REGISTER( TYPE_INT, BUMPENVMAT01 )           //  =  8, /* float (bump mapping matrix) */
                ADD_REGISTER( TYPE_INT, BUMPENVMAT10 )           //  =  9, /* float (bump mapping matrix) */
                ADD_REGISTER( TYPE_INT, BUMPENVMAT11 )           //  = 10, /* float (bump mapping matrix) */
                ADD_REGISTER( TYPE_INT, TEXCOORDINDEX )          //  = 11, /* identifies which set of texture coordinates index this texture */
                ADD_REGISTER( TYPE_INT, BUMPENVLSCALE )          //  = 22, /* float scale for bump map luminance */
                ADD_REGISTER( TYPE_INT, BUMPENVLOFFSET )         //  = 23, /* float offset for bump map luminance */
                ADD_REGISTER( TYPE_INT, TEXTURETRANSFORMFLAGS )  // = 24, /* D3DTEXTURETRANSFORMFLAGS controls texture transform */
                ADD_REGISTER( TYPE_INT, COLORARG0 )              //  = 26, /* D3DTA_* third arg for triadic ops */
                ADD_REGISTER( TYPE_INT, ALPHAARG0 )              //  = 27, /* D3DTA_* third arg for triadic ops */
                ADD_REGISTER( TYPE_INT, RESULTARG )              //  = 28, /* D3DTA_* arg for result (CURRENT or TEMP) */
                ADD_REGISTER( TYPE_INT, CONSTANT )               //  = 32, /* Per-stage constant D3DTA_CONSTANT */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DSamplerState
#define USING_LIST "SamplerState"
                ADD_REGISTER( TYPE_INT, ADDRESSU )            //  = 1,  /* D3DTEXTUREADDRESS for U coordinate */
                ADD_REGISTER( TYPE_INT, ADDRESSV )            //  = 2,  /* D3DTEXTUREADDRESS for V coordinate */
                ADD_REGISTER( TYPE_INT, ADDRESSW )            //  = 3,  /* D3DTEXTUREADDRESS for W coordinate */
                ADD_REGISTER( TYPE_D3DCOLOR, BORDERCOLOR )    //  = 4,  /* D3DCOLOR */
                ADD_REGISTER( TYPE_INT, MAGFILTER )           //  = 5,  /* D3DTEXTUREFILTER filter to use for magnification */
                ADD_REGISTER( TYPE_INT, MINFILTER )           //  = 6,  /* D3DTEXTUREFILTER filter to use for minification */
                ADD_REGISTER( TYPE_INT, MIPFILTER )           //  = 7,  /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
                ADD_REGISTER( TYPE_INT, MIPMAPLODBIAS )       //  = 8,  /* float Mipmap LOD bias */
                ADD_REGISTER( TYPE_INT, MAXMIPLEVEL )         //  = 9,  /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
                ADD_REGISTER( TYPE_INT, MAXANISOTROPY )       //  = 10, /* DWORD maximum anisotropy */
                ADD_REGISTER( TYPE_INT, SRGBTEXTURE )         //  = 11, /* Default = 0 (which means Gamma 1.0, no correction required.) else correct for Gamma = 2.2 */
                ADD_REGISTER( TYPE_INT, ELEMENTINDEX )        //  = 12, /* When multi-element texture is assigned to sampler, this indicates which element index to use.  Default = 0.  */
                ADD_REGISTER( TYPE_INT, DMAPOFFSET )          //  = 13, /* Offset in vertices in the pre-sampled displacement map. Only valid for D3DDMAPSAMPLER sampler  */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DMATERIAL9
#define USING_LIST "MaterialState"
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Diffuse )        /* Diffuse color RGBA */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Ambient )        /* Ambient color RGB */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Specular )       /* Specular 'shininess' */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Emissive )       /* Emissive color RGB */
                ADD_REGISTER( TYPE_FLOAT,         Power )          /* Sharpness if specular highlight */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DTransformState
#define USING_LIST "TransformState"
                ADD_REGISTER( TYPE_MATRIX, VIEW )             //  = 2,
                ADD_REGISTER( TYPE_MATRIX, PROJECTION )       //  = 3,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE0 )         //  = 16,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE1 )         //  = 17,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE2 )         //  = 18,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE3 )         //  = 19,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE4 )         //  = 20,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE5 )         //  = 21,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE6 )         //  = 22,
                ADD_REGISTER( TYPE_MATRIX, TEXTURE7 )         //  = 23,
                ADD_REGISTER( TYPE_MATRIX, WORLD )            //  = 256,
                ADD_REGISTER( TYPE_MATRIX, WORLD1 )           //  = 257,
                ADD_REGISTER( TYPE_MATRIX, WORLD2 )           //  = 258,
                ADD_REGISTER( TYPE_MATRIX, WORLD3 )           //  = 259,
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DLIGHT9
#define USING_LIST "LightState"
                ADD_REGISTER( TYPE_INT,           Type )             /* Type of light source */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Diffuse )          /* Diffuse color of light */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Specular )         /* Specular color of light */
                ADD_REGISTER( TYPE_D3DCOLORVALUE, Ambient )          /* Ambient color of light */
                ADD_REGISTER( TYPE_VECTOR3,       Position )         /* Position in world space */
                ADD_REGISTER( TYPE_VECTOR3,       Direction )        /* Direction in world space */
                ADD_REGISTER( TYPE_FLOAT,         Range )            /* Cutoff range */
                ADD_REGISTER( TYPE_FLOAT,         Falloff )          /* Falloff */
                ADD_REGISTER( TYPE_FLOAT,         Attenuation0 )     /* Constant attenuation */
                ADD_REGISTER( TYPE_FLOAT,         Attenuation1 )     /* Linear attenuation */
                ADD_REGISTER( TYPE_FLOAT,         Attenuation2 )     /* Quadratic attenuation */
                ADD_REGISTER( TYPE_FLOAT,         Theta )            /* Inner angle of spotlight cone */
                ADD_REGISTER( TYPE_FLOAT,         Phi )              /* Outer angle of spotlight cone */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DLightEnableState
#define USING_LIST "LightEnableState"
                ADD_REGISTER( TYPE_INT,           Enable )
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DTextureState
#define USING_LIST "TextureState"
                ADD_REGISTER( TYPE_TEXTURE,      Texture )
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DCAPS9
#define USING_LIST "DeviceCaps"
                ADD_REGISTER( TYPE_INT, DeviceType ) 
                ADD_REGISTER( TYPE_INT, AdapterOrdinal ) 
                ADD_REGISTER( TYPE_INT, Caps )                        /* Caps from DX7 Draw */
                ADD_REGISTER( TYPE_INT, Caps2 ) 
                ADD_REGISTER( TYPE_INT, Caps3 ) 
                ADD_REGISTER( TYPE_INT, PresentationIntervals ) 
                ADD_REGISTER( TYPE_INT, CursorCaps )                  /* Cursor Caps */
                ADD_REGISTER( TYPE_INT, DevCaps )                     /* 3D Device Caps */
                ADD_REGISTER( TYPE_INT, PrimitiveMiscCaps ) 
                ADD_REGISTER( TYPE_INT, RasterCaps ) 
                ADD_REGISTER( TYPE_INT, ZCmpCaps ) 
                ADD_REGISTER( TYPE_INT, SrcBlendCaps ) 
                ADD_REGISTER( TYPE_INT, DestBlendCaps ) 
                ADD_REGISTER( TYPE_INT, AlphaCmpCaps ) 
                ADD_REGISTER( TYPE_INT, ShadeCaps ) 
                ADD_REGISTER( TYPE_INT, TextureCaps ) 
                ADD_REGISTER( TYPE_INT, TextureFilterCaps )           // D3DPTFILTERCAPS for IDirect3DTexture9's
                ADD_REGISTER( TYPE_INT, CubeTextureFilterCaps )       // D3DPTFILTERCAPS for IDirect3DCubeTexture9's
                ADD_REGISTER( TYPE_INT, VolumeTextureFilterCaps )     // D3DPTFILTERCAPS for IDirect3DVolumeTexture9's
                ADD_REGISTER( TYPE_INT, TextureAddressCaps )          // D3DPTADDRESSCAPS for IDirect3DTexture9's
                ADD_REGISTER( TYPE_INT, VolumeTextureAddressCaps )    // D3DPTADDRESSCAPS for IDirect3DVolumeTexture9's
                ADD_REGISTER( TYPE_INT, LineCaps )                    // D3DLINECAPS
                ADD_REGISTER( TYPE_INT, MaxTextureWidth ) 
                ADD_REGISTER( TYPE_INT, MaxTextureHeight ) 
                ADD_REGISTER( TYPE_INT, MaxVolumeExtent ) 
                ADD_REGISTER( TYPE_INT, MaxTextureRepeat ) 
                ADD_REGISTER( TYPE_INT, MaxTextureAspectRatio ) 
                ADD_REGISTER( TYPE_INT, MaxAnisotropy ) 
                ADD_REGISTER( TYPE_FLOAT, MaxVertexW ) 
                ADD_REGISTER( TYPE_FLOAT, GuardBandLeft ) 
                ADD_REGISTER( TYPE_FLOAT, GuardBandTop ) 
                ADD_REGISTER( TYPE_FLOAT, GuardBandRight ) 
                ADD_REGISTER( TYPE_FLOAT, GuardBandBottom ) 
                ADD_REGISTER( TYPE_FLOAT, ExtentsAdjust ) 
                ADD_REGISTER( TYPE_INT, StencilCaps ) 
                ADD_REGISTER( TYPE_INT, FVFCaps ) 
                ADD_REGISTER( TYPE_INT, TextureOpCaps ) 
                ADD_REGISTER( TYPE_INT, MaxTextureBlendStages ) 
                ADD_REGISTER( TYPE_INT, MaxSimultaneousTextures ) 
                ADD_REGISTER( TYPE_INT, VertexProcessingCaps ) 
                ADD_REGISTER( TYPE_INT, MaxActiveLights ) 
                ADD_REGISTER( TYPE_INT, MaxUserClipPlanes ) 
                ADD_REGISTER( TYPE_INT, MaxVertexBlendMatrices ) 
                ADD_REGISTER( TYPE_INT, MaxVertexBlendMatrixIndex ) 
                ADD_REGISTER( TYPE_FLOAT, MaxPointSize ) 
                ADD_REGISTER( TYPE_INT, MaxPrimitiveCount )           // max number of primitives per DrawPrimitive call
                ADD_REGISTER( TYPE_INT, MaxVertexIndex ) 
                ADD_REGISTER( TYPE_INT, MaxStreams ) 
                ADD_REGISTER( TYPE_INT, MaxStreamStride )             // max stride for SetStreamSource
                ADD_REGISTER( TYPE_INT, VertexShaderVersion ) 
                ADD_REGISTER( TYPE_INT, MaxVertexShaderConst )        // number of vertex shader constant registers
                ADD_REGISTER( TYPE_INT, PixelShaderVersion ) 
                ADD_REGISTER( TYPE_FLOAT, PixelShader1xMaxValue )       // max value storable in registers of ps.1.x shaders
                ADD_REGISTER( TYPE_INT, DevCaps2 )                    // Here are the DX9 specific ones
                ADD_REGISTER( TYPE_FLOAT, MaxNpatchTessellationLevel ) 
                ADD_REGISTER( TYPE_INT, Reserved5 ) 
                ADD_REGISTER( TYPE_INT, MasterAdapterOrdinal )         // ordinal of master adaptor for adapter group
                ADD_REGISTER( TYPE_INT, AdapterOrdinalInGroup )        // ordinal inside the adapter group
                ADD_REGISTER( TYPE_INT, NumberOfAdaptersInGroup )      // number of adapters in this adapter group (only if master)
                ADD_REGISTER( TYPE_INT, DeclTypes)                    // Data types, supported in vertex declarations
                ADD_REGISTER( TYPE_INT, NumSimultaneousRTs )          // Will be at least 1
                ADD_REGISTER( TYPE_INT, StretchRectFilterCaps )       // Filter caps supported by StretchRect
                ADD_REGISTER( TYPE_INT, VS20Caps.Caps )
                ADD_REGISTER( TYPE_INT, VS20Caps.DynamicFlowControlDepth )
                ADD_REGISTER( TYPE_INT, VS20Caps.NumTemps )
                ADD_REGISTER( TYPE_INT, VS20Caps.StaticFlowControlDepth )
                ADD_REGISTER( TYPE_INT, PS20Caps.Caps )
                ADD_REGISTER( TYPE_INT, PS20Caps.DynamicFlowControlDepth )
                ADD_REGISTER( TYPE_INT, PS20Caps.NumTemps )
                ADD_REGISTER( TYPE_INT, PS20Caps.StaticFlowControlDepth )
                ADD_REGISTER( TYPE_INT, PS20Caps.NumInstructionSlots )
                ADD_REGISTER( TYPE_INT, VertexTextureFilterCaps )     // D3DPTFILTERCAPS for IDirect3DTexture9's for texture, used in vertex shaders
                ADD_REGISTER( TYPE_INT, MaxVShaderInstructionsExecuted )  // maximum number of vertex shader instructions that can be executed
                ADD_REGISTER( TYPE_INT, MaxPShaderInstructionsExecuted )  // maximum number of pixel shader instructions that can be executed
                ADD_REGISTER( TYPE_INT, MaxVertexShader30InstructionSlots )  
                ADD_REGISTER( TYPE_INT, MaxPixelShader30InstructionSlots ) 
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DVertexDeclState
#define USING_LIST "VertexDeclState"
                ADD_REGISTER( TYPE_INT, Position )                  // Does each vertex have these components ?
                ADD_REGISTER( TYPE_INT, PositionT )
                ADD_REGISTER( TYPE_INT, Normal )
                ADD_REGISTER( TYPE_INT, Color0 )
                ADD_REGISTER( TYPE_INT, Color1 )
                ADD_REGISTER( TYPE_INT, TexCoord0 )
                ADD_REGISTER( TYPE_INT, TexCoord1 )
#undef  USING_STRUCT
#undef  USING_LIST

            };


////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl
//
class CEffectWrapImpl : public CEffectWrap
{
public:
    ZERO_ON_NEW

    virtual void    PostConstruct           ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    virtual void    PreDestruct             ( void );
    virtual bool    IsValid                 ( void );
    virtual void    OnLostDevice            ( void );
    virtual void    OnResetDevice           ( void );
    void            CreateUnderlyingData    ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug );
    void            ReleaseUnderlyingData   ( void );
    void            ApplyCommonHandles      ( void );
    void            ApplyMappedHandles      ( void );
    void            ReadParameterHandles    ( void );

    static void             InitMaps                        ( void );
    const SRegisterInfo*    GetRegisterInfo                 ( EStateGroup stateGroup, const SString& strName );
    const STypeMapping*     MatchTypeMapping                ( EReadableAsType readableAsType, const D3DXPARAMETER_DESC& desc );
    void                    AddStateMappedParameter         ( EStateGroup stateGroup, const SStateVar& var );
    bool                    TryMappingParameterToRegister   ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc );
    bool                    AddStandardParameter            ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc );
    bool                    TryParseSpecialParameter        ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc );

    static std::map < SString, SRegisterInfo > ms_RegisterInfoMap;

    SString                 m_strWarnings;
    bool                    m_bVerboseWarnings;
    bool                    m_bSkipUnusedParameters;
    std::set < D3DXHANDLE >  m_ReferencedParameterMap;

    std::vector < SStateVar > renderStateVarList;
    std::vector < SStateVar > stageStateVarList;
    std::vector < SStateVar > samplerStateVarList;
    std::vector < SStateVar > materialStateVarList;
    std::vector < SStateVar > transformStateVarList;
    std::vector < SStateVar > textureStateVarList;
    std::vector < SStateVar > lightStateVarList;
    std::vector < SStateVar > lightEnabledStateVarList;
    std::vector < SStateVar > deviceCapsVarList;
    std::vector < SStateVar > vertexDeclStateVarList;
};

std::map < SString, SRegisterInfo > CEffectWrapImpl::ms_RegisterInfoMap;


CEffectWrap* NewEffectWrap ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    CEffectWrapImpl* m_pEffectWrap;
    m_pEffectWrap = new CEffectWrapImpl ();
    m_pEffectWrap->PostConstruct ( pManager, strFilename, strRootPath, strOutStatus, bDebug );
    return m_pEffectWrap;
}



////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::PostConstruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::PostConstruct ( CRenderItemManager* pManager, const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    Super::PostConstruct ( pManager );

    m_strName = ExtractFilename ( strFilename );

    // Initialise static data if needed
    InitMaps ();

    // Initial creation of d3d data
    CreateUnderlyingData ( strFilename, strRootPath, strOutStatus, bDebug );
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::PreDestruct
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::PreDestruct ( void )
{
    ReleaseUnderlyingData ();
    Super::PreDestruct ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::IsValid
//
// Check underlying data is present
//
////////////////////////////////////////////////////////////////
bool CEffectWrapImpl::IsValid ( void )
{
    return m_pD3DEffect != NULL;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::OnLostDevice
//
// Release device stuff
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::OnLostDevice ( void )
{
    m_pD3DEffect->OnLostDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::OnResetDevice
//
// Recreate device stuff
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::OnResetDevice ( void )
{
    m_pD3DEffect->OnResetDevice ();
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::CreateUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::CreateUnderlyingData ( const SString& strFilename, const SString& strRootPath, SString& strOutStatus, bool bDebug )
{
    assert ( !m_pD3DEffect );

    // Optimization
    m_uiSaveStateFlags = D3DXFX_DONOTSAVESHADERSTATE;     // D3DXFX_DONOTSAVE(SHADER|SAMPLER)STATE

    // Fetch compiled D3DEffect
    m_pD3DEffect = m_pManager->GetEffectCloner ()->CreateD3DEffect ( strFilename, strRootPath, strOutStatus, m_bUsesVertexShader, m_bUsesDepthBuffer, bDebug );

    if ( !m_pD3DEffect )
        return;

    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc ( &EffectDesc );
    D3DXHANDLE hTechnique = m_pD3DEffect->GetCurrentTechnique ();

    // Make a map of referenced parameter handles
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = m_pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC ParameterDesc;
        m_pD3DEffect->GetParameterDesc( hParameter, &ParameterDesc );

        if ( m_pD3DEffect->IsParameterUsed ( hParameter, hTechnique ) )
            MapInsert ( m_ReferencedParameterMap, hParameter );
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::ReleaseUnderlyingData
//
//
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::ReleaseUnderlyingData ( void )
{
    if ( m_pD3DEffect )
    {
        m_pManager->GetEffectCloner ()->ReleaseD3DEffect ( m_pD3DEffect );
        m_pD3DEffect = NULL;
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::InitMaps
//
// Initialize data maps
// Static function
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::InitMaps ( void )
{
    if ( !ms_RegisterInfoMap.empty () )
        return;

    // ms_RegisterInfoMap is used to speed up access to data in BigRegisterInfoList
    for ( uint i = 0 ; i < NUMELMS( BigRegisterInfoList ) ; i++ )
    {
        const SRegisterInfo& info = BigRegisterInfoList[i];
        SString strKey ( "%s.%s", info.szRegName, info.szGroupName );
        MapSet ( ms_RegisterInfoMap, strKey.ToUpper (), info );
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::ApplyCommonHandles
//
// Set parameters for MTA known names
// Called before render
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::ApplyCommonHandles ( void )
{
    CEffectWrap& m_CommonHandles = *this;

    LPDIRECT3DDEVICE9 pDevice;
    m_pD3DEffect->GetDevice ( &pDevice );

    D3DXMATRIX matWorld, matView, matProjection;
    pDevice->GetTransform ( D3DTS_WORLD, &matWorld );
    pDevice->GetTransform ( D3DTS_VIEW, &matView );
    pDevice->GetTransform ( D3DTS_PROJECTION, &matProjection );

    // Set matrices
    if ( m_CommonHandles.hWorld )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hWorld, &matWorld );

    if ( m_CommonHandles.hView )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hView, &matView );

    if ( m_CommonHandles.hProjection )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hProjection, &matProjection );

    if ( m_CommonHandles.hWorldView )
    {
        D3DXMATRIX matWorldView = matWorld * matView;
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hWorldView, &matWorldView );
    }

    if ( m_CommonHandles.hWorldViewProj )
    {
        D3DXMATRIX matWorldViewProj = matWorld * matView * matProjection;
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hWorldViewProj, &matWorldViewProj );
    }

    if ( m_CommonHandles.hViewProj )
    {
        D3DXMATRIX matViewProj = matView * matProjection;
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hViewProj, &matViewProj );
    }

    D3DXMATRIX matViewInv;
    D3DXMATRIX* pmatViewInv = NULL;
    if ( m_CommonHandles.hViewInv )
    {
        pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hViewInv, &matViewInv );
    }

    if ( m_CommonHandles.hWorldInvTr )
    {
        D3DXMATRIX matWorldTr;
        D3DXMatrixTranspose ( &matWorldTr, &matWorld );
        D3DXMATRIX matWorldInvTr;
        D3DXMatrixInverse ( &matWorldInvTr, NULL, &matWorldTr );
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hWorldInvTr, &matWorldInvTr );
    }

    if ( m_CommonHandles.hViewInvTr )
    {
        D3DXMATRIX matViewTr;
        D3DXMatrixTranspose ( &matViewTr, &matView );
        D3DXMATRIX matViewInvTr;
        D3DXMatrixInverse ( &matViewInvTr, NULL, &matViewTr );
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hViewInvTr, &matViewInvTr );
    }

    // Set camera position
    if ( m_CommonHandles.hCamPos )
    {
        if ( !pmatViewInv )
            pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );

        D3DXVECTOR3 vecCamPos ( matViewInv.m[3][0], matViewInv.m[3][1], matViewInv.m[3][2] );
        m_pD3DEffect->SetFloatArray ( m_CommonHandles.hCamPos, (float*)&vecCamPos, 3 );
    }

    // Set camera direction
    if ( m_CommonHandles.hCamDir )
    {
        if ( !pmatViewInv )
            pmatViewInv = D3DXMatrixInverse ( &matViewInv, NULL, &matView );

        D3DXVECTOR3 vecCamDir ( matViewInv.m[2][0], matViewInv.m[2][1], matViewInv.m[2][2] );
        m_pD3DEffect->SetFloatArray ( m_CommonHandles.hCamDir, (float*)&vecCamDir, 3 );
    }

    // Set time
    if ( m_CommonHandles.hTime )
        m_pD3DEffect->SetFloat ( m_CommonHandles.hTime, GetTickCount64_ () * 0.001f );


    // Set light
    if ( m_CommonHandles.hLightAmbient || m_CommonHandles.hLightDiffuse || m_CommonHandles.hLightSpecular || m_CommonHandles.hLightDirection )
    {
        D3DXCOLOR totalAmbient ( 0, 0, 0, 0 );
        float fStrongestDiffuseScore = -1;
        D3DXCOLOR strongestDiffuse ( 0, 0, 0, 0 );
        D3DXCOLOR strongestSpecular ( 0, 0, 0, 0 );
        D3DXVECTOR3 strongestDirection ( 0, 0, -1 );
        for ( uint i = 0 ; i < 4 ; i++ )
        {
            BOOL bEnabled;
            if ( SUCCEEDED( pDevice->GetLightEnable ( i, &bEnabled ) ) && bEnabled )
            {
                D3DLIGHT9 D3DLight;
                pDevice->GetLight ( i, &D3DLight );
                if ( D3DLight.Type == D3DLIGHT_DIRECTIONAL )
                {
                    totalAmbient += D3DLight.Ambient;
                    float fScore = D3DLight.Diffuse.r + D3DLight.Diffuse.g + D3DLight.Diffuse.b;
                    if ( fScore > fStrongestDiffuseScore )
                    {
                        fStrongestDiffuseScore = fScore;
                        strongestDiffuse = D3DLight.Diffuse;
                        strongestSpecular = D3DLight.Specular;
                        strongestDirection = D3DLight.Direction;
                    }
                }
            }
        }

        if ( m_CommonHandles.hLightAmbient )
        {
            totalAmbient.a = Min ( totalAmbient.a, 1.0f );
            totalAmbient.r = Min ( totalAmbient.r, 1.0f );
            totalAmbient.g = Min ( totalAmbient.g, 1.0f );
            totalAmbient.b = Min ( totalAmbient.b, 1.0f );
            m_pD3DEffect->SetFloatArray ( m_CommonHandles.hLightAmbient, (float*)&totalAmbient, 4 );
        }

        if ( m_CommonHandles.hLightDiffuse )
            m_pD3DEffect->SetFloatArray ( m_CommonHandles.hLightDiffuse, (float*)&strongestDiffuse, 4 );

        if ( m_CommonHandles.hLightSpecular )
            m_pD3DEffect->SetFloatArray ( m_CommonHandles.hLightSpecular, (float*)&strongestSpecular, 4 );

        if ( m_CommonHandles.hLightDirection )
            m_pD3DEffect->SetFloatArray ( m_CommonHandles.hLightDirection, (float*)&strongestDirection, 4 );
    }

    if ( m_CommonHandles.hDepthBuffer )
        m_pD3DEffect->SetTexture ( m_CommonHandles.hDepthBuffer, g_pDeviceState->MainSceneState.DepthBuffer );

    if ( m_CommonHandles.hViewMainScene )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hViewMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.VIEW );

    if ( m_CommonHandles.hWorldMainScene )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hWorldMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.WORLD );

    if ( m_CommonHandles.hProjectionMainScene )
        m_pD3DEffect->SetMatrix ( m_CommonHandles.hProjectionMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.PROJECTION );
};


void BOUNDS_CHECK ( const void* ptr, int ptrsize, const void* bufstart, int bufsize )
{
    // ptr should not be before bufstart
    assert ( ptr >= bufstart );
    // ptr+ptrsize should not be after bufstart+bufsize
    assert ( (uchar*)ptr + ptrsize <= (uchar*)bufstart + bufsize );
}

////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::ApplyMappedHandles
//
// Set parameters which are tied to D3D registers
// Called before render
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::ApplyMappedHandles ( void )
{
	//////////////////////////////////////////
    //
    // RenderState
    //
    for ( uint i = 0 ; i < renderStateVarList.size () ; i++ )
    {
        const SStateVar& var = renderStateVarList[i];
        const DWORD* pdwValue = &g_pDeviceState->RenderState.Raw [ var.iRegister ];

        if ( var.iType == RegMap::Int2Int )
        {
            m_pD3DEffect->SetInt ( var.hHandle, *pdwValue );
        }
        else
        if ( var.iType == RegMap::Int2Float )
        {
            m_pD3DEffect->SetFloat ( var.hHandle, *(float*)pdwValue );
        }
        else
        if ( var.iType == RegMap::Int2Color )
        {
            D3DXCOLOR colorValue ( *pdwValue );
            m_pD3DEffect->SetFloatArray ( var.hHandle, (float*)&colorValue, 4 );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->RenderState, sizeof ( g_pDeviceState->RenderState ) );
    }

	//////////////////////////////////////////
    //
    // StageState
    //
    for ( uint i = 0 ; i < stageStateVarList.size () ; i++ )
    {
        const SStateVar& var = stageStateVarList[i];
        const DWORD* pdwValue = &g_pDeviceState->StageState[ var.iStage ].Raw [ var.iRegister ];
        if ( var.iType == RegMap::Int2Int )
        {
            m_pD3DEffect->SetInt ( var.hHandle, *pdwValue );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->StageState, sizeof ( g_pDeviceState->StageState ) );
    }

	//////////////////////////////////////////
    //
    // SamplerState
    //
    for ( uint i = 0 ; i < samplerStateVarList.size () ; i++ )
    {
        const SStateVar& var = samplerStateVarList[i];
        const DWORD* pdwValue = &g_pDeviceState->SamplerState[ var.iStage ].Raw [ var.iRegister ];

        if ( var.iType == RegMap::Int2Int )
        {
            m_pD3DEffect->SetIntArray ( var.hHandle, (const INT*)pdwValue, 1 );
        }
        else
        if ( var.iType == RegMap::Int2Color )
        {
            D3DXCOLOR colorValue ( *pdwValue );
            m_pD3DEffect->SetFloatArray ( var.hHandle, (float*)&colorValue, 4 );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->SamplerState, sizeof ( g_pDeviceState->SamplerState ) );
    }

	//////////////////////////////////////////
    //
    // Material
    //
    for ( uint i = 0 ; i < materialStateVarList.size () ; i++ )
    {
        const SStateVar& var = materialStateVarList[i];
        const float* pfValue = &((const float*)&g_pDeviceState->Material)[ var.iRegister ];
        if ( var.iType == RegMap::Color2Color )
        {
            assert ( var.iSize == 4 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, pfValue, var.iSize );
        }
        else
        if ( var.iType == RegMap::Float2Float )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, pfValue, var.iSize );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pfValue, sizeof ( *pfValue ), &g_pDeviceState->Material, sizeof ( g_pDeviceState->Material ) );
    }

	//////////////////////////////////////////
    //
    // TransformState
    //
    for ( uint i = 0 ; i < transformStateVarList.size () ; i++ )
    {
        const SStateVar& var = transformStateVarList[i];
        const D3DXMATRIX* pMatrix = &((const D3DXMATRIX*)&g_pDeviceState->TransformState)[ var.iRegister / 16 ];
        if ( var.iType == RegMap::Matrix2Matrix )
        {
            m_pD3DEffect->SetMatrix ( var.hHandle, pMatrix );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pMatrix, sizeof ( *pMatrix ), &g_pDeviceState->TransformState, sizeof ( g_pDeviceState->TransformState ) );
    }

	//////////////////////////////////////////
    //
    // TextureState
    //
    for ( uint i = 0 ; i < textureStateVarList.size () ; i++ )
    {
        const SStateVar& var = textureStateVarList[i];
        assert ( var.iRegister == 0 );
        IDirect3DBaseTexture9* const* ppTexture = &g_pDeviceState->TextureState[ var.iStage ].Texture;
        if ( var.iType == RegMap::Texture2Texture )
        {
            m_pD3DEffect->SetTexture ( var.hHandle, CDirect3DEvents9::GetRealTexture ( *ppTexture ) );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( ppTexture, sizeof ( *ppTexture ), &g_pDeviceState->TextureState, sizeof ( g_pDeviceState->TextureState ) );
    }

	//////////////////////////////////////////
    //
    // Lights
    //
    for ( uint i = 0 ; i < lightStateVarList.size () ; i++ )
    {
        const SStateVar& var = lightStateVarList[i];
        const float* pfValue = &( (const float*)&g_pDeviceState->Lights[ var.iStage ] )[ var.iRegister ];

        if ( var.iType == RegMap::Int2Int )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetIntArray ( var.hHandle, (int*)pfValue, var.iSize );
        }
        else
        if ( var.iType == RegMap::Color2Color )
        {
            assert ( var.iSize == 4 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, pfValue, var.iSize );
        }
        else
        if ( var.iType == RegMap::Vector2Vector )
        {
            assert ( var.iSize == 3 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, pfValue, var.iSize );
        }
        else
        if ( var.iType == RegMap::Float2Float )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, pfValue, var.iSize );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pfValue, sizeof ( *pfValue ), &g_pDeviceState->Lights, sizeof ( g_pDeviceState->Lights ) );
   }

	//////////////////////////////////////////
    //
    // LightEnableState
    //
    for ( uint i = 0 ; i < lightEnabledStateVarList.size () ; i++ )
    {
        const SStateVar& var = lightEnabledStateVarList[i];
        assert ( var.iRegister == 0 );
        const DWORD* pdwValue = &g_pDeviceState->LightEnableState[ var.iStage ].Enable;
        if ( var.iType == RegMap::Int2Int )
        {
            m_pD3DEffect->SetInt ( var.hHandle, *pdwValue );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->LightEnableState, sizeof ( g_pDeviceState->LightEnableState ) );
    }

	//////////////////////////////////////////
    //
    // DeviceCaps
    //
    for ( uint i = 0 ; i < deviceCapsVarList.size () ; i++ )
    {
        const SStateVar& var = deviceCapsVarList[i];
        const DWORD* pdwValue = &((const DWORD*)&g_pDeviceState->DeviceCaps)[ var.iRegister ];
        if ( var.iType == RegMap::Int2Int )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetInt ( var.hHandle, *pdwValue );
        }
        else
        if ( var.iType == RegMap::Float2Float )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetFloatArray ( var.hHandle, (float*)pdwValue, var.iSize );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->DeviceCaps, sizeof ( g_pDeviceState->DeviceCaps ) );
    }

    //////////////////////////////////////////
    //
    // VertexDeclState
    //
    for ( uint i = 0 ; i < vertexDeclStateVarList.size () ; i++ )
    {
        const SStateVar& var = vertexDeclStateVarList[i];
        const DWORD* pdwValue = &((const DWORD*)&g_pDeviceState->VertexDeclState)[ var.iRegister ];
        if ( var.iType == RegMap::Int2Int )
        {
            assert ( var.iSize == 1 );
            m_pD3DEffect->SetInt ( var.hHandle, *pdwValue );
        }
        else
            assert ( 0 );

        BOUNDS_CHECK( pdwValue, sizeof ( *pdwValue ), &g_pDeviceState->VertexDeclState, sizeof ( g_pDeviceState->VertexDeclState ) );
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::ReadParameterHandles
//
// Read global parameter names
// Called during parse stage
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::ReadParameterHandles ( void )
{
    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc( &EffectDesc );

    // Search for custom flags first
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = m_pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC ParameterDesc;
        m_pD3DEffect->GetParameterDesc( hParameter, &ParameterDesc );

        // See if this parameter is 'special'
        if ( TryParseSpecialParameter ( hParameter, ParameterDesc ) )
            continue;
    }

    // Add parameter handles
    for ( uint i = 0 ; i < EffectDesc.Parameters ; i++ )
    {
        D3DXHANDLE hParameter = m_pD3DEffect->GetParameter ( NULL, i );
        if ( !hParameter )
            break;
        D3DXPARAMETER_DESC ParameterDesc;
        m_pD3DEffect->GetParameterDesc( hParameter, &ParameterDesc );

        // See if this parameter is 'special'
        if ( TryParseSpecialParameter ( hParameter, ParameterDesc ) )
            continue;

        // Check if parameter is used
        if ( m_bSkipUnusedParameters && !MapContains ( m_ReferencedParameterMap, hParameter ) )
                continue;

        // See if this parameter wants to be mapped to a D3D register
        if ( TryMappingParameterToRegister ( hParameter, ParameterDesc ) )
            continue;

        // Otherwise, assume its standard parameter
        if ( AddStandardParameter ( hParameter, ParameterDesc ) )
            continue;
    }
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::TryParseSpecialParameter
//
// See if this parameter is 'special'
//
////////////////////////////////////////////////////////////////
bool CEffectWrapImpl::TryParseSpecialParameter ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc )
{
    // Use semantic if it exists
    SString strName = ParameterDesc.Semantic ? ParameterDesc.Semantic : ParameterDesc.Name;

    if ( strName.CompareI ( "CUSTOMFLAGS" ) )
    {
        // For each annotation
        for ( uint i = 0 ; i < ParameterDesc.Annotations ; i++ )
        {
            // Get name and value
            D3DXHANDLE hAnnotation = m_pD3DEffect->GetAnnotation( hParameter, i );
            D3DXPARAMETER_DESC AnnotDesc;
            m_pD3DEffect->GetParameterDesc ( hAnnotation, &AnnotDesc );
            LPCSTR szAnnotValue;
            m_pD3DEffect->GetString ( hAnnotation, &szAnnotValue );
            const SString strAnnotName = AnnotDesc.Name;
            const SString strAnnotValue = szAnnotValue;

            if ( strAnnotName == "createNormals" )
            {
                if ( strAnnotValue == "yes" )
                    m_bRequiresNormals = true;
                else
                if ( strAnnotValue == "no" )
                    m_bRequiresNormals = false;
                else
                    m_strWarnings += SString ( "Unknown value for createNormals '%s'\n", *strAnnotValue );
            }
            else
            if ( strAnnotName == "skipUnusedParameters" )
            {
                if ( strAnnotValue == "yes" )
                    m_bSkipUnusedParameters = true;
                else
                if ( strAnnotValue == "no" )
                    m_bSkipUnusedParameters = false;
                else
                    m_strWarnings += SString ( "Unknown value for skipUnusedParameters '%s'\n", *strAnnotValue );
            }
            else
            {
                m_strWarnings += SString ( "Unknown custom flag '%s'\n", *strAnnotName );
            }
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::AddStandardParameter
//
// Global parameter not tied to a D3D register
// Called during parse stage
//
////////////////////////////////////////////////////////////////
bool CEffectWrapImpl::AddStandardParameter ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc )
{
    // Use semantic if it exists
    SString strName = ParameterDesc.Semantic ? ParameterDesc.Semantic : ParameterDesc.Name;
    // Add to correct lookup map
    if ( ParameterDesc.Type == D3DXPT_TEXTURE )
    {
        // Keep handle to first texture to get size of shader
        if ( !m_hFirstTexture )
            m_hFirstTexture = hParameter;
        MapSet ( m_texureHandleMap, strName.ToUpper (), hParameter );
    }
    else
        MapSet ( m_valueHandleMap, strName.ToUpper (), hParameter );

    return true;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::TryMappingParameterToRegister
//
// See if this global parameter is tied to a D3D register
//
////////////////////////////////////////////////////////////////
bool CEffectWrapImpl::TryMappingParameterToRegister ( D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc )
{
    // For each annotation
    for ( uint i = 0 ; i < ParameterDesc.Annotations ; i++ )
    {
        // Get name and value
        D3DXHANDLE hAnnotation = m_pD3DEffect->GetAnnotation( hParameter, i );
        D3DXPARAMETER_DESC AnnotDesc;
        m_pD3DEffect->GetParameterDesc ( hAnnotation, &AnnotDesc );
        LPCSTR szAnnotValue;
        m_pD3DEffect->GetString ( hAnnotation, &szAnnotValue );
        const SString strAnnotName = AnnotDesc.Name;

        // Find group match from name
        EStateGroup stateGroup;
        if ( !StringToEnum ( strAnnotName, stateGroup ) )
        {
            if ( m_bVerboseWarnings )
                m_strWarnings += SString ( "Unknown annotation '%s' for parameter '%s'\n", *strAnnotName, ParameterDesc.Name );
            continue;
        }

        // Extract prepended stage number, if any
        int iStage = atoi ( szAnnotValue );
        SString strName = SStringX ( szAnnotValue ).SplitRight( ",", NULL, -1 );

        // Find D3D register line for this group+name
        const SRegisterInfo* pRegsiterInfo = GetRegisterInfo ( stateGroup, strName );
        if ( !pRegsiterInfo )
        {
            m_strWarnings += SString ( "No match for annotation '%s' '%s' on parameter '%s'\n", *strAnnotName, szAnnotValue, ParameterDesc.Name );
            continue;
        }

        // See if types can be mapped
        const STypeMapping* pTypeMapping = MatchTypeMapping ( pRegsiterInfo->eReadableAsType, ParameterDesc );

        if ( !pTypeMapping )
        {
            m_strWarnings += SString ( "Type mismatch for annotation '%s' '%s' on parameter '%s'\n", *strAnnotName, szAnnotValue, ParameterDesc.Name );
            continue;
        }

        SStateVar var;
        var.stateGroup = stateGroup;
        var.hHandle = hParameter;
        var.iStage = iStage;
        var.iRegister = pRegsiterInfo->iOffset / 4;
        var.iType = pTypeMapping->OutType;
        var.iSize = pTypeMapping->OutSize;
        AddStateMappedParameter ( stateGroup, var );
        return true;      // We have a weiner
    }

    return false;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::GetRegisterInfo
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
const SRegisterInfo* CEffectWrapImpl::GetRegisterInfo ( EStateGroup stateGroup, const SString& strName )
{
    SString strKey ( "%s.%s", *strName, *EnumToString ( stateGroup ) );
    const SRegisterInfo* pInfo = MapFind ( ms_RegisterInfoMap, strKey.ToUpper () );
    return pInfo;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::MatchTypeMapping
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
const STypeMapping* CEffectWrapImpl::MatchTypeMapping ( EReadableAsType readableAsType, const D3DXPARAMETER_DESC& desc )
{
    // For each type mapping
    for ( uint i = 0 ; i < NUMELMS( TypeMappingList ) ; i++ )
    {
        const STypeMapping& mapping = TypeMappingList[i];

        // Find a match
        if ( mapping.readableAsType == readableAsType )
        {
            // Check it will work
            if ( mapping.ParamMatchClass == desc.Class && mapping.ParamMatchType == desc.Type )
            {
                return &mapping;
            }
        }    
    }
    return NULL;
}


////////////////////////////////////////////////////////////////
//
// CEffectWrapImpl::AddStateMappedParameter
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
void CEffectWrapImpl::AddStateMappedParameter ( EStateGroup stateGroup, const SStateVar& var )
{
    switch ( stateGroup )
    {
        case STATE_GROUP_RENDER:        renderStateVarList.push_back ( var ); break;
        case STATE_GROUP_STAGE:         stageStateVarList.push_back ( var ); break;
        case STATE_GROUP_SAMPLER:       samplerStateVarList.push_back ( var ); break;
        case STATE_GROUP_MATERIAL:      materialStateVarList.push_back ( var ); break;
        case STATE_GROUP_TRANSFORM:     transformStateVarList.push_back ( var ); break;
        case STATE_GROUP_TEXTURE:       textureStateVarList.push_back ( var ); break;
        case STATE_GROUP_LIGHT:         lightStateVarList.push_back ( var ); break;
        case STATE_GROUP_LIGHT_ENABLED: lightEnabledStateVarList.push_back ( var ); break;
        case STATE_GROUP_DEVICE_CAPS:   deviceCapsVarList.push_back ( var ); break;
        case STATE_GROUP_VERTEX_DECL:   vertexDeclStateVarList.push_back ( var ); break;
    }
}
