/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        CRenderItem.EffectParameters.cpp
 *  PURPOSE:     Handles and settings for a compiled effect
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRenderItem.EffectParameters.h"

#include <cctype>
#include <cstdlib>
#include <limits>

IMPLEMENT_ENUM_BEGIN(EStateGroup)
ADD_ENUM(STATE_GROUP_RENDER, "renderState")
ADD_ENUM(STATE_GROUP_STAGE, "stageState")
ADD_ENUM(STATE_GROUP_SAMPLER, "samplerState")
ADD_ENUM(STATE_GROUP_MATERIAL, "materialState")
ADD_ENUM(STATE_GROUP_TRANSFORM, "transformState")
ADD_ENUM(STATE_GROUP_TEXTURE, "textureState")
ADD_ENUM(STATE_GROUP_LIGHT, "lightState")
ADD_ENUM(STATE_GROUP_LIGHT_ENABLE, "lightEnableState")
ADD_ENUM(STATE_GROUP_DEVICE_CAPS, "deviceCaps")
ADD_ENUM(STATE_GROUP_VERTEX_DECL, "vertexDeclState")
IMPLEMENT_ENUM_END("state-group")

const STypeMapping TypeMappingList[] = {
    {TYPE_INT, D3DXPC_SCALAR, D3DXPT_INT, RegMap::Int2Int, 1},
    {TYPE_D3DCOLOR, D3DXPC_SCALAR, D3DXPT_INT, RegMap::Int2Int, 1},            // Can be read as float4 or DWORD color
    {TYPE_D3DCOLOR, D3DXPC_VECTOR, D3DXPT_FLOAT, RegMap::Int2Color, 4},
    {TYPE_IFLOAT, D3DXPC_SCALAR, D3DXPT_FLOAT, RegMap::Int2Float, 1},            // Can be read as float or as int on float memory
    {TYPE_IFLOAT, D3DXPC_SCALAR, D3DXPT_INT, RegMap::Int2Int, 1},                // Can be read as float or as int on float memory
    {TYPE_FLOAT, D3DXPC_SCALAR, D3DXPT_FLOAT, RegMap::Float2Float, 1},
    {TYPE_D3DCOLORVALUE, D3DXPC_VECTOR, D3DXPT_FLOAT, RegMap::Color2Color, 4},
    {TYPE_VECTOR3, D3DXPC_VECTOR, D3DXPT_FLOAT, RegMap::Vector2Vector, 3},
    {TYPE_MATRIX, D3DXPC_MATRIX_ROWS, D3DXPT_FLOAT, RegMap::Matrix2Matrix, 16},
    {TYPE_TEXTURE, D3DXPC_OBJECT, D3DXPT_TEXTURE, RegMap::Texture2Texture, 1},
    {TYPE_TEXTURE, D3DXPC_OBJECT, D3DXPT_TEXTURE1D, RegMap::Texture2Texture, 1},
    {TYPE_TEXTURE, D3DXPC_OBJECT, D3DXPT_TEXTURE2D, RegMap::Texture2Texture, 1},
    {TYPE_TEXTURE, D3DXPC_OBJECT, D3DXPT_TEXTURE3D, RegMap::Texture2Texture, 1},
    {TYPE_TEXTURE, D3DXPC_OBJECT, D3DXPT_TEXTURECUBE, RegMap::Texture2Texture, 1},
};

//
// Big list of D3D registers
//

#define ADD_REGISTER(type,name) \
    {offsetof(USING_STRUCT, name), USING_LIST, #name, type}

const SRegisterInfo BigRegisterInfoList[] = {

#define USING_STRUCT CProxyDirect3DDevice9::SD3DRenderState
#define USING_LIST "RenderState"
    ADD_REGISTER(TYPE_INT, ZENABLE),                             //  = 7,    /* D3DZBUFFERTYPE (or TRUE/FALSE for legacy) */
    ADD_REGISTER(TYPE_INT, FILLMODE),                            //  = 8,    /* D3DFILLMODE */
    ADD_REGISTER(TYPE_INT, SHADEMODE),                           //  = 9,    /* D3DSHADEMODE */
    ADD_REGISTER(TYPE_INT, ZWRITEENABLE),                        //  = 14,   /* TRUE to enable z writes */
    ADD_REGISTER(TYPE_INT, ALPHATESTENABLE),                     //  = 15,   /* TRUE to enable alpha tests */
    ADD_REGISTER(TYPE_INT, LASTPIXEL),                           //  = 16,   /* TRUE for last-pixel on lines */
    ADD_REGISTER(TYPE_INT, SRCBLEND),                            //  = 19,   /* D3DBLEND */
    ADD_REGISTER(TYPE_INT, DESTBLEND),                           //  = 20,   /* D3DBLEND */
    ADD_REGISTER(TYPE_INT, CULLMODE),                            //  = 22,   /* D3DCULL */
    ADD_REGISTER(TYPE_INT, ZFUNC),                               //  = 23,   /* D3DCMPFUNC */
    ADD_REGISTER(TYPE_INT, ALPHAREF),                            //  = 24,   /* D3DFIXED */
    ADD_REGISTER(TYPE_INT, ALPHAFUNC),                           //  = 25,   /* D3DCMPFUNC */
    ADD_REGISTER(TYPE_INT, DITHERENABLE),                        //  = 26,   /* TRUE to enable dithering */
    ADD_REGISTER(TYPE_INT, ALPHABLENDENABLE),                    //  = 27,   /* TRUE to enable alpha blending */
    ADD_REGISTER(TYPE_INT, FOGENABLE),                           //  = 28,   /* TRUE to enable fog blending */
    ADD_REGISTER(TYPE_INT, SPECULARENABLE),                      //  = 29,   /* TRUE to enable specular */
    ADD_REGISTER(TYPE_D3DCOLOR, FOGCOLOR),                       //  = 34,   /* D3DCOLOR */
    ADD_REGISTER(TYPE_INT, FOGTABLEMODE),                        //  = 35,   /* D3DFOGMODE */
    ADD_REGISTER(TYPE_IFLOAT, FOGSTART),                         //  = 36,   /* Fog start (for both vertex and pixel fog) */
    ADD_REGISTER(TYPE_IFLOAT, FOGEND),                           //  = 37,   /* Fog end      */
    ADD_REGISTER(TYPE_IFLOAT, FOGDENSITY),                       //  = 38,   /* Fog density  */
    ADD_REGISTER(TYPE_INT, RANGEFOGENABLE),                      //  = 48,   /* Enables range-based fog */
    ADD_REGISTER(TYPE_INT, STENCILENABLE),                       //  = 52,   /* BOOL enable/disable stenciling */
    ADD_REGISTER(TYPE_INT, STENCILFAIL),                         //  = 53,   /* D3DSTENCILOP to do if stencil test fails */
    ADD_REGISTER(TYPE_INT, STENCILZFAIL),                        //  = 54,   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
    ADD_REGISTER(TYPE_INT, STENCILPASS),                         //  = 55,   /* D3DSTENCILOP to do if both stencil and Z tests pass */
    ADD_REGISTER(TYPE_INT, STENCILFUNC),                         //  = 56,   /* D3DCMPFUNC fn.   */
    ADD_REGISTER(TYPE_INT, STENCILREF),                          //  = 57,   /* Reference value used in stencil test */
    ADD_REGISTER(TYPE_INT, STENCILMASK),                         //  = 58,   /* Mask value used in stencil test */
    ADD_REGISTER(TYPE_INT, STENCILWRITEMASK),                    //  = 59,   /* Write mask applied to values written to stencil buffer */
    ADD_REGISTER(TYPE_D3DCOLOR, TEXTUREFACTOR),                  //  = 60,   /* D3DCOLOR used for multi-texture blend */
    ADD_REGISTER(TYPE_INT, WRAP0),                               //  = 128,  /* wrap for 1st texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP1),                               //  = 129,  /* wrap for 2nd texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP2),                               //  = 130,  /* wrap for 3rd texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP3),                               //  = 131,  /* wrap for 4th texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP4),                               //  = 132,  /* wrap for 5th texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP5),                               //  = 133,  /* wrap for 6th texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP6),                               //  = 134,  /* wrap for 7th texture coord. set */
    ADD_REGISTER(TYPE_INT, WRAP7),                               //  = 135,  /* wrap for 8th texture coord. set */
    ADD_REGISTER(TYPE_INT, CLIPPING),                            //  = 136,
    ADD_REGISTER(TYPE_INT, LIGHTING),                            //  = 137,
    ADD_REGISTER(TYPE_D3DCOLOR, AMBIENT),                        //  = 139,
    ADD_REGISTER(TYPE_INT, FOGVERTEXMODE),                       //  = 140,
    ADD_REGISTER(TYPE_INT, COLORVERTEX),                         //  = 141,
    ADD_REGISTER(TYPE_INT, LOCALVIEWER),                         //  = 142,
    ADD_REGISTER(TYPE_INT, NORMALIZENORMALS),                    //  = 143,
    ADD_REGISTER(TYPE_INT, DIFFUSEMATERIALSOURCE),               //  = 145,
    ADD_REGISTER(TYPE_INT, SPECULARMATERIALSOURCE),              //  = 146,
    ADD_REGISTER(TYPE_INT, AMBIENTMATERIALSOURCE),               //  = 147,
    ADD_REGISTER(TYPE_INT, EMISSIVEMATERIALSOURCE),              //  = 148,
    ADD_REGISTER(TYPE_INT, VERTEXBLEND),                         //  = 151,
    ADD_REGISTER(TYPE_INT, CLIPPLANEENABLE),                     //  = 152,
    ADD_REGISTER(TYPE_IFLOAT, POINTSIZE),                        //  = 154,   /* float point size */
    ADD_REGISTER(TYPE_IFLOAT, POINTSIZE_MIN),                    //  = 155,   /* float point size min threshold */
    ADD_REGISTER(TYPE_INT, POINTSPRITEENABLE),                   //  = 156,   /* BOOL point texture coord control */
    ADD_REGISTER(TYPE_INT, POINTSCALEENABLE),                    //  = 157,   /* BOOL point size scale enable */
    ADD_REGISTER(TYPE_IFLOAT, POINTSCALE_A),                     //  = 158,   /* float point attenuation A value */
    ADD_REGISTER(TYPE_IFLOAT, POINTSCALE_B),                     //  = 159,   /* float point attenuation B value */
    ADD_REGISTER(TYPE_IFLOAT, POINTSCALE_C),                     //  = 160,   /* float point attenuation C value */
    ADD_REGISTER(TYPE_INT, MULTISAMPLEANTIALIAS),                //  = 161,  // BOOL - set to do FSAA with multisample buffer
    ADD_REGISTER(TYPE_INT, MULTISAMPLEMASK),                     //  = 162,  // DWORD - )        //per-sample enable/disable
    ADD_REGISTER(TYPE_INT, PATCHEDGESTYLE),                      //  = 163,  // Sets whether patch edges will use float style tessellation
    ADD_REGISTER(TYPE_INT, DEBUGMONITORTOKEN),                   //  = 165,  // DEBUG ONLY - token to debug monitor
    ADD_REGISTER(TYPE_IFLOAT, POINTSIZE_MAX),                    //  = 166,   /* float point size max threshold */
    ADD_REGISTER(TYPE_INT, INDEXEDVERTEXBLENDENABLE),            //  = 167,
    ADD_REGISTER(TYPE_INT, COLORWRITEENABLE),                    //  = 168,  // per-channel write enable
    ADD_REGISTER(TYPE_IFLOAT, TWEENFACTOR),                      //  = 170,   // float tween factor
    ADD_REGISTER(TYPE_INT, BLENDOP),                             //  = 171,   // D3DBLENDOP setting
    ADD_REGISTER(TYPE_INT, POSITIONDEGREE),               //  = 172,   // NPatch position interpolation degree. D3DDEGREE_LINEAR or D3DDEGREE_CUBIC (default)
    ADD_REGISTER(TYPE_INT, NORMALDEGREE),                 //  = 173,   aa NPatch normal interpolation degree. D3DDEGREE_LINEAR (default) or D3DDEGREE_QUADRATIC
    ADD_REGISTER(TYPE_INT, SCISSORTESTENABLE),            //  = 174
    ADD_REGISTER(TYPE_INT, SLOPESCALEDEPTHBIAS),                   //  = 175,
    ADD_REGISTER(TYPE_INT, ANTIALIASEDLINEENABLE),                 //  = 176,
    ADD_REGISTER(TYPE_INT, MINTESSELLATIONLEVEL),                  //  = 178,
    ADD_REGISTER(TYPE_INT, MAXTESSELLATIONLEVEL),                  //  = 179,
    ADD_REGISTER(TYPE_INT, ADAPTIVETESS_X),                        //  = 180,
    ADD_REGISTER(TYPE_INT, ADAPTIVETESS_Y),                        //  = 181,
    ADD_REGISTER(TYPE_INT, ADAPTIVETESS_Z),                        //  = 182,
    ADD_REGISTER(TYPE_INT, ADAPTIVETESS_W),                        //  = 183,
    ADD_REGISTER(TYPE_INT, ENABLEADAPTIVETESSELLATION),            //  = 184,
    ADD_REGISTER(TYPE_INT, TWOSIDEDSTENCILMODE),                   //  = 185,   /* BOOL enable/disable 2 sided stenciling */
    ADD_REGISTER(TYPE_INT, CCW_STENCILFAIL),                       //  = 186,   /* D3DSTENCILOP to do if ccw stencil test fails */
    ADD_REGISTER(TYPE_INT, CCW_STENCILZFAIL),                      //  = 187,   /* D3DSTENCILOP to do if ccw stencil test passes and Z test fails */
    ADD_REGISTER(TYPE_INT, CCW_STENCILPASS),                       //  = 188,   /* D3DSTENCILOP to do if both ccw stencil and Z tests pass */
    ADD_REGISTER(TYPE_INT, CCW_STENCILFUNC),                       //  = 189,   /* D3DCMPFUNC fn.  */
    ADD_REGISTER(TYPE_INT, COLORWRITEENABLE1),                     //  = 190,   /* Additional ColorWriteEnables */
    ADD_REGISTER(TYPE_INT, COLORWRITEENABLE2),                     //  = 191,   /* Additional ColorWriteEnables */
    ADD_REGISTER(TYPE_INT, COLORWRITEENABLE3),                     //  = 192,   /* Additional ColorWriteEnables */
    ADD_REGISTER(TYPE_D3DCOLOR, BLENDFACTOR),                      //  = 193,   /* D3DCOLOR used for a constant blend factor during alpha blending*/
    ADD_REGISTER(TYPE_INT, SRGBWRITEENABLE),                       //  = 194,   /* Enable rendertarget writes to be DE-linearized to SRGB */
    ADD_REGISTER(TYPE_INT, DEPTHBIAS),                             //  = 195,
    ADD_REGISTER(TYPE_INT, WRAP8),                                 //  = 198,   /* Additional wrap states for vs_3_0+ attributes with D3DDECLUSAGE_TEXCOORD */
    ADD_REGISTER(TYPE_INT, WRAP9),                                 //  = 199,
    ADD_REGISTER(TYPE_INT, WRAP10),                                //  = 200,
    ADD_REGISTER(TYPE_INT, WRAP11),                                //  = 201,
    ADD_REGISTER(TYPE_INT, WRAP12),                                //  = 202,
    ADD_REGISTER(TYPE_INT, WRAP13),                                //  = 203,
    ADD_REGISTER(TYPE_INT, WRAP14),                                //  = 204,
    ADD_REGISTER(TYPE_INT, WRAP15),                                //  = 205,
    ADD_REGISTER(TYPE_INT, SEPARATEALPHABLENDENABLE),              //  = 206,  /* TRUE to enable a separate blending function for the alpha channel */
    ADD_REGISTER(TYPE_INT, SRCBLENDALPHA),             //  = 207,  /* SRC blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE )  */
    ADD_REGISTER(TYPE_INT, DESTBLENDALPHA),            //  = 208,  /* DST blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE ) */
    ADD_REGISTER(TYPE_INT, BLENDOPALPHA),              //  = 209,  /* Blending operation for the alpha channel when DWORD SEPARATEDESTALPHAENABLE ) */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DStageState
#define USING_LIST "StageState"
    ADD_REGISTER(TYPE_INT, COLOROP),                          //  =  1, /* D3DTEXTUREOP - per-stage blending controls for color channels */
    ADD_REGISTER(TYPE_INT, COLORARG1),                        //  =  2, /* D3DTA_* (texture arg) */
    ADD_REGISTER(TYPE_INT, COLORARG2),                        //  =  3, /* D3DTA_* (texture arg) */
    ADD_REGISTER(TYPE_INT, ALPHAOP),                          //  =  4, /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
    ADD_REGISTER(TYPE_INT, ALPHAARG1),                        //  =  5, /* D3DTA_* (texture arg) */
    ADD_REGISTER(TYPE_INT, ALPHAARG2),                        //  =  6, /* D3DTA_* (texture arg) */
    ADD_REGISTER(TYPE_INT, BUMPENVMAT00),                     //  =  7, /* float (bump mapping matrix) */
    ADD_REGISTER(TYPE_INT, BUMPENVMAT01),                     //  =  8, /* float (bump mapping matrix) */
    ADD_REGISTER(TYPE_INT, BUMPENVMAT10),                     //  =  9, /* float (bump mapping matrix) */
    ADD_REGISTER(TYPE_INT, BUMPENVMAT11),                     //  = 10, /* float (bump mapping matrix) */
    ADD_REGISTER(TYPE_INT, TEXCOORDINDEX),                    //  = 11, /* identifies which set of texture coordinates index this texture */
    ADD_REGISTER(TYPE_INT, BUMPENVLSCALE),                    //  = 22, /* float scale for bump map luminance */
    ADD_REGISTER(TYPE_INT, BUMPENVLOFFSET),                   //  = 23, /* float offset for bump map luminance */
    ADD_REGISTER(TYPE_INT, TEXTURETRANSFORMFLAGS),            // = 24, /* D3DTEXTURETRANSFORMFLAGS controls texture transform */
    ADD_REGISTER(TYPE_INT, COLORARG0),                        //  = 26, /* D3DTA_* third arg for triadic ops */
    ADD_REGISTER(TYPE_INT, ALPHAARG0),                        //  = 27, /* D3DTA_* third arg for triadic ops */
    ADD_REGISTER(TYPE_INT, RESULTARG),                        //  = 28, /* D3DTA_* arg for result (CURRENT or TEMP) */
    ADD_REGISTER(TYPE_INT, CONSTANT),                         //  = 32, /* Per-stage constant D3DTA_CONSTANT */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DSamplerState
#define USING_LIST "SamplerState"
    ADD_REGISTER(TYPE_INT, ADDRESSU),                    //  = 1,  /* D3DTEXTUREADDRESS for U coordinate */
    ADD_REGISTER(TYPE_INT, ADDRESSV),                    //  = 2,  /* D3DTEXTUREADDRESS for V coordinate */
    ADD_REGISTER(TYPE_INT, ADDRESSW),                    //  = 3,  /* D3DTEXTUREADDRESS for W coordinate */
    ADD_REGISTER(TYPE_D3DCOLOR, BORDERCOLOR),            //  = 4,  /* D3DCOLOR */
    ADD_REGISTER(TYPE_INT, MAGFILTER),                   //  = 5,  /* D3DTEXTUREFILTER filter to use for magnification */
    ADD_REGISTER(TYPE_INT, MINFILTER),                   //  = 6,  /* D3DTEXTUREFILTER filter to use for minification */
    ADD_REGISTER(TYPE_INT, MIPFILTER),                   //  = 7,  /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
    ADD_REGISTER(TYPE_INT, MIPMAPLODBIAS),               //  = 8,  /* float Mipmap LOD bias */
    ADD_REGISTER(TYPE_INT, MAXMIPLEVEL),                 //  = 9,  /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
    ADD_REGISTER(TYPE_INT, MAXANISOTROPY),               //  = 10, /* DWORD maximum anisotropy */
    ADD_REGISTER(TYPE_INT, SRGBTEXTURE),             //  = 11, /* Default = 0 (which means Gamma 1.0, no correction required.) else correct for Gamma = 2.2 */
    ADD_REGISTER(TYPE_INT, ELEMENTINDEX),            //  = 12, When multi-element texture is assigned to sampler, this indicates which element index to use.
    ADD_REGISTER(TYPE_INT, DMAPOFFSET),              //  = 13, /* Offset in vertices in the pre-sampled displacement map. */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DMATERIAL9
#define USING_LIST "MaterialState"
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Diffuse),  /* Diffuse color RGBA */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Ambient),  /* Ambient color RGB */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Specular), /* Specular 'shininess' */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Emissive), /* Emissive color RGB */
    ADD_REGISTER(TYPE_FLOAT, Power),            /* Sharpness if specular highlight */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DTransformState
#define USING_LIST "TransformState"
    ADD_REGISTER(TYPE_MATRIX, VIEW),                  //  = 2,
    ADD_REGISTER(TYPE_MATRIX, PROJECTION),            //  = 3,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE0),              //  = 16,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE1),              //  = 17,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE2),              //  = 18,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE3),              //  = 19,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE4),              //  = 20,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE5),              //  = 21,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE6),              //  = 22,
    ADD_REGISTER(TYPE_MATRIX, TEXTURE7),              //  = 23,
    ADD_REGISTER(TYPE_MATRIX, WORLD),                 //  = 256,
    ADD_REGISTER(TYPE_MATRIX, WORLD1),                //  = 257,
    ADD_REGISTER(TYPE_MATRIX, WORLD2),                //  = 258,
    ADD_REGISTER(TYPE_MATRIX, WORLD3),                //  = 259,
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DLIGHT9
#define USING_LIST "LightState"
    ADD_REGISTER(TYPE_INT, Type),               /* Type of light source */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Diffuse),  /* Diffuse color of light */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Specular), /* Specular color of light */
    ADD_REGISTER(TYPE_D3DCOLORVALUE, Ambient),  /* Ambient color of light */
    ADD_REGISTER(TYPE_VECTOR3, Position),       /* Position in world space */
    ADD_REGISTER(TYPE_VECTOR3, Direction),      /* Direction in world space */
    ADD_REGISTER(TYPE_FLOAT, Range),            /* Cutoff range */
    ADD_REGISTER(TYPE_FLOAT, Falloff),          /* Falloff */
    ADD_REGISTER(TYPE_FLOAT, Attenuation0),     /* Constant attenuation */
    ADD_REGISTER(TYPE_FLOAT, Attenuation1),     /* Linear attenuation */
    ADD_REGISTER(TYPE_FLOAT, Attenuation2),     /* Quadratic attenuation */
    ADD_REGISTER(TYPE_FLOAT, Theta),            /* Inner angle of spotlight cone */
    ADD_REGISTER(TYPE_FLOAT, Phi),              /* Outer angle of spotlight cone */
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DLightEnableState
#define USING_LIST "LightEnableState"
    ADD_REGISTER(TYPE_INT, Enable),
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DTextureState
#define USING_LIST "TextureState"
    ADD_REGISTER(TYPE_TEXTURE, Texture),
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT D3DCAPS9
#define USING_LIST "DeviceCaps"
    ADD_REGISTER(TYPE_INT, DeviceType),
    ADD_REGISTER(TYPE_INT, AdapterOrdinal),
    ADD_REGISTER(TYPE_INT, Caps), /* Caps from DX7 Draw */
    ADD_REGISTER(TYPE_INT, Caps2),
    ADD_REGISTER(TYPE_INT, Caps3),
    ADD_REGISTER(TYPE_INT, PresentationIntervals),
    ADD_REGISTER(TYPE_INT, CursorCaps), /* Cursor Caps */
    ADD_REGISTER(TYPE_INT, DevCaps),    /* 3D Device Caps */
    ADD_REGISTER(TYPE_INT, PrimitiveMiscCaps),
    ADD_REGISTER(TYPE_INT, RasterCaps),
    ADD_REGISTER(TYPE_INT, ZCmpCaps),
    ADD_REGISTER(TYPE_INT, SrcBlendCaps),
    ADD_REGISTER(TYPE_INT, DestBlendCaps),
    ADD_REGISTER(TYPE_INT, AlphaCmpCaps),
    ADD_REGISTER(TYPE_INT, ShadeCaps),
    ADD_REGISTER(TYPE_INT, TextureCaps),
    ADD_REGISTER(TYPE_INT, TextureFilterCaps),                   // D3DPTFILTERCAPS for IDirect3DTexture9's
    ADD_REGISTER(TYPE_INT, CubeTextureFilterCaps),               // D3DPTFILTERCAPS for IDirect3DCubeTexture9's
    ADD_REGISTER(TYPE_INT, VolumeTextureFilterCaps),             // D3DPTFILTERCAPS for IDirect3DVolumeTexture9's
    ADD_REGISTER(TYPE_INT, TextureAddressCaps),                  // D3DPTADDRESSCAPS for IDirect3DTexture9's
    ADD_REGISTER(TYPE_INT, VolumeTextureAddressCaps),            // D3DPTADDRESSCAPS for IDirect3DVolumeTexture9's
    ADD_REGISTER(TYPE_INT, LineCaps),                            // D3DLINECAPS
    ADD_REGISTER(TYPE_INT, MaxTextureWidth),
    ADD_REGISTER(TYPE_INT, MaxTextureHeight),
    ADD_REGISTER(TYPE_INT, MaxVolumeExtent),
    ADD_REGISTER(TYPE_INT, MaxTextureRepeat),
    ADD_REGISTER(TYPE_INT, MaxTextureAspectRatio),
    ADD_REGISTER(TYPE_INT, MaxAnisotropy),
    ADD_REGISTER(TYPE_FLOAT, MaxVertexW),
    ADD_REGISTER(TYPE_FLOAT, GuardBandLeft),
    ADD_REGISTER(TYPE_FLOAT, GuardBandTop),
    ADD_REGISTER(TYPE_FLOAT, GuardBandRight),
    ADD_REGISTER(TYPE_FLOAT, GuardBandBottom),
    ADD_REGISTER(TYPE_FLOAT, ExtentsAdjust),
    ADD_REGISTER(TYPE_INT, StencilCaps),
    ADD_REGISTER(TYPE_INT, FVFCaps),
    ADD_REGISTER(TYPE_INT, TextureOpCaps),
    ADD_REGISTER(TYPE_INT, MaxTextureBlendStages),
    ADD_REGISTER(TYPE_INT, MaxSimultaneousTextures),
    ADD_REGISTER(TYPE_INT, VertexProcessingCaps),
    ADD_REGISTER(TYPE_INT, MaxActiveLights),
    ADD_REGISTER(TYPE_INT, MaxUserClipPlanes),
    ADD_REGISTER(TYPE_INT, MaxVertexBlendMatrices),
    ADD_REGISTER(TYPE_INT, MaxVertexBlendMatrixIndex),
    ADD_REGISTER(TYPE_FLOAT, MaxPointSize),
    ADD_REGISTER(TYPE_INT, MaxPrimitiveCount),            // max number of primitives per DrawPrimitive call
    ADD_REGISTER(TYPE_INT, MaxVertexIndex),
    ADD_REGISTER(TYPE_INT, MaxStreams),
    ADD_REGISTER(TYPE_INT, MaxStreamStride),            // max stride for SetStreamSource
    ADD_REGISTER(TYPE_INT, VertexShaderVersion),
    ADD_REGISTER(TYPE_INT, MaxVertexShaderConst),            // number of vertex shader constant registers
    ADD_REGISTER(TYPE_INT, PixelShaderVersion),
    ADD_REGISTER(TYPE_FLOAT, PixelShader1xMaxValue),            // max value storable in registers of ps.1.x shaders
    ADD_REGISTER(TYPE_INT, DevCaps2),                           // Here are the DX9 specific ones
    ADD_REGISTER(TYPE_FLOAT, MaxNpatchTessellationLevel),
    ADD_REGISTER(TYPE_INT, Reserved5),
    ADD_REGISTER(TYPE_INT, MasterAdapterOrdinal),               // ordinal of master adaptor for adapter group
    ADD_REGISTER(TYPE_INT, AdapterOrdinalInGroup),              // ordinal inside the adapter group
    ADD_REGISTER(TYPE_INT, NumberOfAdaptersInGroup),            // number of adapters in this adapter group (only if master)
    ADD_REGISTER(TYPE_INT, DeclTypes),                          // Data types, supported in vertex declarations
    ADD_REGISTER(TYPE_INT, NumSimultaneousRTs),                 // Will be at least 1
    ADD_REGISTER(TYPE_INT, StretchRectFilterCaps),              // Filter caps supported by StretchRect
    ADD_REGISTER(TYPE_INT, VS20Caps.Caps),
    ADD_REGISTER(TYPE_INT, VS20Caps.DynamicFlowControlDepth),
    ADD_REGISTER(TYPE_INT, VS20Caps.NumTemps),
    ADD_REGISTER(TYPE_INT, VS20Caps.StaticFlowControlDepth),
    ADD_REGISTER(TYPE_INT, PS20Caps.Caps),
    ADD_REGISTER(TYPE_INT, PS20Caps.DynamicFlowControlDepth),
    ADD_REGISTER(TYPE_INT, PS20Caps.NumTemps),
    ADD_REGISTER(TYPE_INT, PS20Caps.StaticFlowControlDepth),
    ADD_REGISTER(TYPE_INT, PS20Caps.NumInstructionSlots),
    ADD_REGISTER(TYPE_INT, VertexTextureFilterCaps),                   // D3DPTFILTERCAPS for IDirect3DTexture9's for texture, used in vertex shaders
    ADD_REGISTER(TYPE_INT, MaxVShaderInstructionsExecuted),            // maximum number of vertex shader instructions that can be executed
    ADD_REGISTER(TYPE_INT, MaxPShaderInstructionsExecuted),            // maximum number of pixel shader instructions that can be executed
    ADD_REGISTER(TYPE_INT, MaxVertexShader30InstructionSlots),
    ADD_REGISTER(TYPE_INT, MaxPixelShader30InstructionSlots),
#undef  USING_STRUCT
#undef  USING_LIST

#define USING_STRUCT CProxyDirect3DDevice9::SD3DVertexDeclState
#define USING_LIST "VertexDeclState"
    ADD_REGISTER(TYPE_INT, Position),            // Does each vertex have these components ?
    ADD_REGISTER(TYPE_INT, PositionT),
    ADD_REGISTER(TYPE_INT, Normal),
    ADD_REGISTER(TYPE_INT, Color0),
    ADD_REGISTER(TYPE_INT, Color1),
    ADD_REGISTER(TYPE_INT, TexCoord0),
    ADD_REGISTER(TYPE_INT, TexCoord1),
#undef  USING_STRUCT
#undef  USING_LIST
};

////////////////////////////////////////////////////////////////
//
// CEffectParameters::Begin
//
// Wrap for ID3DEffect::Begin
// Ensures secondary render targets are set if required
//
////////////////////////////////////////////////////////////////
HRESULT CEffectParameters::Begin(UINT* pPasses, DWORD Flags, bool bWorldRender)
{
    if (!m_pD3DEffect)
        return D3DERR_INVALIDCALL;

    if (m_bUsesDepthBuffer && !bWorldRender)
    {
        // Ensure readable depth buffer is ready to be read
        CGraphics::GetSingleton().GetRenderItemManager()->SaveReadableDepthBuffer();
    }

    LPDIRECT3DDEVICE9 pDevice = nullptr;
    m_pD3DEffect->GetDevice(&pDevice);

    bool bCanBindRenderTargets = (pDevice != nullptr);
    if (pDevice)
    {
        const HRESULT hrCooperativeLevel = pDevice->TestCooperativeLevel();
        if (hrCooperativeLevel != D3D_OK)
        {
            bCanBindRenderTargets = false;
            if (hrCooperativeLevel != D3DERR_DEVICELOST && hrCooperativeLevel != D3DERR_DEVICENOTRESET)
                WriteDebugEvent(SString("CEffectParameters::Begin: unexpected cooperative level %08x", hrCooperativeLevel));
        }
    }

    if (bCanBindRenderTargets)
    {
        for (uint i = 0; i < m_SecondaryRenderTargetList.size(); i++)
        {
            D3DXHANDLE             hTexture = m_SecondaryRenderTargetList[i];
            IDirect3DBaseTexture9* pD3DTexture = nullptr;
            HRESULT                hr = m_pD3DEffect->GetTexture(hTexture, &pD3DTexture);
            if (SUCCEEDED(hr) && pD3DTexture)
            {
                if (pD3DTexture->GetType() == D3DRTYPE_TEXTURE)
                {
                    IDirect3DSurface9* pD3DSurface = nullptr;
                    HRESULT            hrSurface = ((IDirect3DTexture9*)pD3DTexture)->GetSurfaceLevel(0, &pD3DSurface);
                    if (hrSurface == D3D_OK && pD3DSurface)
                    {
                        pDevice->SetRenderTarget(i + 1, pD3DSurface);
                        SAFE_RELEASE(pD3DSurface);
                    }
                }
            }
            SAFE_RELEASE(pD3DTexture);
        }
    }

    SAFE_RELEASE(pDevice);
    return m_pD3DEffect->Begin(pPasses, Flags);
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::End
//
// Wrap for ID3DEffect::End
// Ensures secondary render targets are unset
//
////////////////////////////////////////////////////////////////
HRESULT CEffectParameters::End(bool bDeviceOperational)
{
    if (!m_pD3DEffect)
        return D3D_OK;

    HRESULT hResult = m_pD3DEffect->End();

    if (!m_SecondaryRenderTargetList.empty())
    {
        LPDIRECT3DDEVICE9 pDevice = nullptr;
        m_pD3DEffect->GetDevice(&pDevice);

        if (pDevice)
        {
            bool bCanTouchDevice = bDeviceOperational;
            if (!bCanTouchDevice)
            {
                const HRESULT hrCooperativeLevel = pDevice->TestCooperativeLevel();
                bCanTouchDevice = (hrCooperativeLevel == D3D_OK);
            }

            if (bCanTouchDevice)
            {
                for (uint i = 0; i < m_SecondaryRenderTargetList.size(); i++)
                    pDevice->SetRenderTarget(i + 1, nullptr);
            }

            SAFE_RELEASE(pDevice);
        }
    }

    return hResult;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::ApplyCommonHandles
//
// Set parameters for MTA known names
// Called before render
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::ApplyCommonHandles()
{
    if (!m_bUsesCommonHandles)
        return false;

    if (!m_pD3DEffect)
        return false;

    assert(g_pDeviceState);

    LPDIRECT3DDEVICE9 pDevice = nullptr;
    if (FAILED(m_pD3DEffect->GetDevice(&pDevice)) || !pDevice)
        return false;

    D3DXMATRIX matWorld, matView, matProjection;
    pDevice->GetTransform(D3DTS_WORLD, &matWorld);
    pDevice->GetTransform(D3DTS_VIEW, &matView);
    pDevice->GetTransform(D3DTS_PROJECTION, &matProjection);

    // Set matrices
    if (m_CommonHandles.hWorld)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hWorld, &matWorld);

    if (m_CommonHandles.hView)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hView, &matView);

    if (m_CommonHandles.hProjection)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hProjection, &matProjection);

    if (m_CommonHandles.hWorldView)
    {
        D3DXMATRIX matWorldView = matWorld * matView;
        m_pD3DEffect->SetMatrix(m_CommonHandles.hWorldView, &matWorldView);
    }

    if (m_CommonHandles.hWorldViewProj)
    {
        D3DXMATRIX matWorldViewProj = matWorld * matView * matProjection;
        m_pD3DEffect->SetMatrix(m_CommonHandles.hWorldViewProj, &matWorldViewProj);
    }

    if (m_CommonHandles.hViewProj)
    {
        D3DXMATRIX matViewProj = matView * matProjection;
        m_pD3DEffect->SetMatrix(m_CommonHandles.hViewProj, &matViewProj);
    }

    D3DXMATRIX  matViewInv;
    D3DXMATRIX* pmatViewInv = NULL;
    if (m_CommonHandles.hViewInv)
    {
        pmatViewInv = D3DXMatrixInverse(&matViewInv, NULL, &matView);
        m_pD3DEffect->SetMatrix(m_CommonHandles.hViewInv, &matViewInv);
    }

    if (m_CommonHandles.hWorldInvTr)
    {
        D3DXMATRIX matWorldTr;
        D3DXMatrixTranspose(&matWorldTr, &matWorld);
        D3DXMATRIX matWorldInvTr;
        D3DXMatrixInverse(&matWorldInvTr, NULL, &matWorldTr);
        m_pD3DEffect->SetMatrix(m_CommonHandles.hWorldInvTr, &matWorldInvTr);
    }

    if (m_CommonHandles.hViewInvTr)
    {
        D3DXMATRIX matViewTr;
        D3DXMatrixTranspose(&matViewTr, &matView);
        D3DXMATRIX matViewInvTr;
        D3DXMatrixInverse(&matViewInvTr, NULL, &matViewTr);
        m_pD3DEffect->SetMatrix(m_CommonHandles.hViewInvTr, &matViewInvTr);
    }

    // Set camera position
    if (m_CommonHandles.hCamPos)
    {
        if (!pmatViewInv)
            pmatViewInv = D3DXMatrixInverse(&matViewInv, NULL, &matView);

        D3DXVECTOR3 vecCamPos(matViewInv.m[3][0], matViewInv.m[3][1], matViewInv.m[3][2]);
        m_pD3DEffect->SetFloatArray(m_CommonHandles.hCamPos, (float*)&vecCamPos, 3);
    }

    // Set camera direction
    if (m_CommonHandles.hCamDir)
    {
        if (!pmatViewInv)
            pmatViewInv = D3DXMatrixInverse(&matViewInv, NULL, &matView);

        D3DXVECTOR3 vecCamDir(matViewInv.m[2][0], matViewInv.m[2][1], matViewInv.m[2][2]);
        m_pD3DEffect->SetFloatArray(m_CommonHandles.hCamDir, (float*)&vecCamDir, 3);
    }

    // Set time
    if (m_CommonHandles.hTime)
        m_pD3DEffect->SetFloat(m_CommonHandles.hTime, GetTickCount64_() * 0.001f);

    // Set light
    if (m_CommonHandles.hLightAmbient || m_CommonHandles.hLightDiffuse || m_CommonHandles.hLightSpecular || m_CommonHandles.hLightDirection)
    {
        D3DXCOLOR   totalAmbient(0, 0, 0, 0);
        float       fStrongestDiffuseScore = -1;
        D3DXCOLOR   strongestDiffuse(0, 0, 0, 0);
        D3DXCOLOR   strongestSpecular(0, 0, 0, 0);
        D3DXVECTOR3 strongestDirection(0, 0, -1);
        for (uint i = 0; i < 4; i++)
        {
            BOOL bEnabled;
            if (SUCCEEDED(pDevice->GetLightEnable(i, &bEnabled)) && bEnabled)
            {
                D3DLIGHT9 D3DLight;
                pDevice->GetLight(i, &D3DLight);
                if (D3DLight.Type == D3DLIGHT_DIRECTIONAL)
                {
                    totalAmbient += D3DLight.Ambient;
                    float fScore = D3DLight.Diffuse.r + D3DLight.Diffuse.g + D3DLight.Diffuse.b;
                    if (fScore > fStrongestDiffuseScore)
                    {
                        fStrongestDiffuseScore = fScore;
                        strongestDiffuse = D3DLight.Diffuse;
                        strongestSpecular = D3DLight.Specular;
                        strongestDirection = D3DLight.Direction;
                    }
                }
            }
        }

        if (m_CommonHandles.hLightAmbient)
        {
            totalAmbient.a = std::min(totalAmbient.a, 1.0f);
            totalAmbient.r = std::min(totalAmbient.r, 1.0f);
            totalAmbient.g = std::min(totalAmbient.g, 1.0f);
            totalAmbient.b = std::min(totalAmbient.b, 1.0f);
            m_pD3DEffect->SetFloatArray(m_CommonHandles.hLightAmbient, (float*)&totalAmbient, 4);
        }

        if (m_CommonHandles.hLightDiffuse)
            m_pD3DEffect->SetFloatArray(m_CommonHandles.hLightDiffuse, (float*)&strongestDiffuse, 4);

        if (m_CommonHandles.hLightSpecular)
            m_pD3DEffect->SetFloatArray(m_CommonHandles.hLightSpecular, (float*)&strongestSpecular, 4);

        if (m_CommonHandles.hLightDirection)
            m_pD3DEffect->SetFloatArray(m_CommonHandles.hLightDirection, (float*)&strongestDirection, 4);
    }

    if (m_CommonHandles.hDepthBuffer)
        m_pD3DEffect->SetTexture(m_CommonHandles.hDepthBuffer, g_pDeviceState->MainSceneState.DepthBuffer);

    if (m_CommonHandles.hViewMainScene)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hViewMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.VIEW);

    if (m_CommonHandles.hWorldMainScene)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hWorldMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.WORLD);

    if (m_CommonHandles.hProjectionMainScene)
        m_pD3DEffect->SetMatrix(m_CommonHandles.hProjectionMainScene, (D3DXMATRIX*)&g_pDeviceState->MainSceneState.TransformState.PROJECTION);

    SAFE_RELEASE(pDevice);
    return true;
};

void BOUNDS_CHECK(const void* ptr, int ptrsize, const void* bufstart, int bufsize)
{
    // ptr should not be before bufstart
    assert(ptr >= bufstart);
    // ptr+ptrsize should not be after bufstart+bufsize
    assert((uchar*)ptr + ptrsize <= (uchar*)bufstart + bufsize);
}

static SString TrimAsciiWhitespace(const SString& value)
{
    size_t start = 0;
    size_t end = value.length();

    while (start < end && std::isspace(static_cast<unsigned char>(value[start])))
        ++start;

    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
        --end;

    return value.substr(start, end - start);
}

static bool DoesStateGroupUseStageIndex(EStateGroup stateGroup)
{
    switch (stateGroup)
    {
        case STATE_GROUP_STAGE:
        case STATE_GROUP_SAMPLER:
        case STATE_GROUP_TEXTURE:
        case STATE_GROUP_LIGHT:
        case STATE_GROUP_LIGHT_ENABLE:
            return true;
        default:
            return false;
    }
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::ApplyMappedHandles
//
// Set parameters which are tied to D3D registers
// Called before render
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::ApplyMappedHandles()
{
    if (!m_bUsesMappedHandles)
        return false;

    if (!m_pD3DEffect)
        return false;

    assert(g_pDeviceState);

    //////////////////////////////////////////
    //
    // RenderState
    //
    for (const SStateVar& var : m_renderStateVarList)
    {
        const DWORD* pdwValue = &g_pDeviceState->RenderState.Raw[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            m_pD3DEffect->SetInt(var.hHandle, *pdwValue);
        }
        else if (var.iType == RegMap::Int2Float)
        {
            m_pD3DEffect->SetFloat(var.hHandle, *(float*)pdwValue);
        }
        else if (var.iType == RegMap::Int2Color)
        {
            D3DXCOLOR colorValue(*pdwValue);
            m_pD3DEffect->SetFloatArray(var.hHandle, (float*)&colorValue, 4);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->RenderState, sizeof(g_pDeviceState->RenderState));
    }

    //////////////////////////////////////////
    //
    // StageState
    //
    for (const SStateVar& var : m_stageStateVarList)
    {
        const DWORD* pdwValue = &g_pDeviceState->StageState[var.iStage].Raw[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            m_pD3DEffect->SetInt(var.hHandle, *pdwValue);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->StageState, sizeof(g_pDeviceState->StageState));
    }

    //////////////////////////////////////////
    //
    // SamplerState
    //
    for (const SStateVar& var : m_samplerStateVarList)
    {
        const DWORD* pdwValue = &g_pDeviceState->SamplerState[var.iStage].Raw[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            m_pD3DEffect->SetIntArray(var.hHandle, (const INT*)pdwValue, 1);
        }
        else if (var.iType == RegMap::Int2Color)
        {
            D3DXCOLOR colorValue(*pdwValue);
            m_pD3DEffect->SetFloatArray(var.hHandle, (float*)&colorValue, 4);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->SamplerState, sizeof(g_pDeviceState->SamplerState));
    }

    //////////////////////////////////////////
    //
    // Material
    //
    for (const SStateVar& var : m_materialStateVarList)
    {
        const float* pfValue = &((const float*)&g_pDeviceState->Material)[var.iRegister];
        if (var.iType == RegMap::Color2Color)
        {
            assert(var.iSize == 4);
            m_pD3DEffect->SetFloatArray(var.hHandle, pfValue, var.iSize);
        }
        else if (var.iType == RegMap::Float2Float)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetFloatArray(var.hHandle, pfValue, var.iSize);
        }
        else
            assert(0);

        BOUNDS_CHECK(pfValue, sizeof(*pfValue), &g_pDeviceState->Material, sizeof(g_pDeviceState->Material));
    }

    //////////////////////////////////////////
    //
    // TransformState
    //
    for (const SStateVar& var : m_transformStateVarList)
    {
        const D3DXMATRIX* pMatrix = &((const D3DXMATRIX*)&g_pDeviceState->TransformState)[var.iRegister / 16];
        if (var.iType == RegMap::Matrix2Matrix)
        {
            m_pD3DEffect->SetMatrix(var.hHandle, pMatrix);
        }
        else
            assert(0);

        BOUNDS_CHECK(pMatrix, sizeof(*pMatrix), &g_pDeviceState->TransformState, sizeof(g_pDeviceState->TransformState));
    }

    //////////////////////////////////////////
    //
    // TextureState
    //
    for (const SStateVar& var : m_textureStateVarList)
    {
        assert(var.iRegister == 0);
        IDirect3DBaseTexture9* const* ppTexture = &g_pDeviceState->TextureState[var.iStage].Texture;
        if (var.iType == RegMap::Texture2Texture)
        {
            m_pD3DEffect->SetTexture(var.hHandle, CDirect3DEvents9::GetRealTexture(*ppTexture));
        }
        else
            assert(0);

        BOUNDS_CHECK(ppTexture, sizeof(*ppTexture), &g_pDeviceState->TextureState, sizeof(g_pDeviceState->TextureState));
    }

    //////////////////////////////////////////
    //
    // Lights
    //
    for (const SStateVar& var : m_lightStateVarList)
    {
        const float* pfValue = &((const float*)&g_pDeviceState->Lights[var.iStage])[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetIntArray(var.hHandle, (int*)pfValue, var.iSize);
        }
        else if (var.iType == RegMap::Color2Color)
        {
            assert(var.iSize == 4);
            m_pD3DEffect->SetFloatArray(var.hHandle, pfValue, var.iSize);
        }
        else if (var.iType == RegMap::Vector2Vector)
        {
            assert(var.iSize == 3);
            m_pD3DEffect->SetFloatArray(var.hHandle, pfValue, var.iSize);
        }
        else if (var.iType == RegMap::Float2Float)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetFloatArray(var.hHandle, pfValue, var.iSize);
        }
        else
            assert(0);

        BOUNDS_CHECK(pfValue, sizeof(*pfValue), &g_pDeviceState->Lights, sizeof(g_pDeviceState->Lights));
    }

    //////////////////////////////////////////
    //
    // LightEnableState
    //
    for (const SStateVar& var : m_lightEnableStateVarList)
    {
        assert(var.iRegister == 0);
        const DWORD* pdwValue = &g_pDeviceState->LightEnableState[var.iStage].Enable;
        if (var.iType == RegMap::Int2Int)
        {
            m_pD3DEffect->SetInt(var.hHandle, *pdwValue);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->LightEnableState, sizeof(g_pDeviceState->LightEnableState));
    }

    //////////////////////////////////////////
    //
    // DeviceCaps
    //
    for (const SStateVar& var : m_deviceCapsVarList)
    {
        const DWORD* pdwValue = &((const DWORD*)&g_pDeviceState->DeviceCaps)[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetInt(var.hHandle, *pdwValue);
        }
        else if (var.iType == RegMap::Float2Float)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetFloatArray(var.hHandle, (float*)pdwValue, var.iSize);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->DeviceCaps, sizeof(g_pDeviceState->DeviceCaps));
    }

    //////////////////////////////////////////
    //
    // VertexDeclState
    //
    for (const SStateVar& var : m_vertexDeclStateVarList)
    {
        const DWORD* pdwValue = &((const DWORD*)&g_pDeviceState->VertexDeclState)[var.iRegister];
        if (var.iType == RegMap::Int2Int)
        {
            assert(var.iSize == 1);
            m_pD3DEffect->SetInt(var.hHandle, *pdwValue);
        }
        else
            assert(0);

        BOUNDS_CHECK(pdwValue, sizeof(*pdwValue), &g_pDeviceState->VertexDeclState, sizeof(g_pDeviceState->VertexDeclState));
    }

    return true;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::NotifyModifiedParameter
//
// Keep track of parameters set by any shader instance
//
////////////////////////////////////////////////////////////////
void CEffectParameters::NotifyModifiedParameter(D3DXHANDLE hParameter)
{
    if (!MapContains(m_ModifiedParametersList, hParameter))
    {
        MapInsert(m_ModifiedParametersList, hParameter);
        m_uiModifiedParametersRevision++;
    }
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::GetModifiedParameters
//
// Get list of parameters set by any shader instance
//
////////////////////////////////////////////////////////////////
const std::set<D3DXHANDLE>& CEffectParameters::GetModifiedParameters()
{
    return m_ModifiedParametersList;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::RestoreParametersDefaultValue
//
// Set default value for each supplied parameter
//
////////////////////////////////////////////////////////////////
void CEffectParameters::RestoreParametersDefaultValue(const std::vector<D3DXHANDLE>& parameterList)
{
    if (!m_pD3DEffect)
        return;

    for (auto hParameter : parameterList)
    {
        std::vector<char>* pBuffer = MapFind(m_defaultValues, hParameter);
        if (pBuffer)
            m_pD3DEffect->SetValue(hParameter, pBuffer->data(), pBuffer->size());
    }
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::ReadParameterHandles
//
// Read global parameter names
// Called during parse stage
//
////////////////////////////////////////////////////////////////
void CEffectParameters::ReadParameterHandles()
{
    if (!m_pD3DEffect)
        return;

    D3DXEFFECT_DESC EffectDesc;
    m_pD3DEffect->GetDesc(&EffectDesc);

    // Search for custom flags first
    for (uint i = 0; i < EffectDesc.Parameters; i++)
    {
        D3DXHANDLE         hParameter = m_pD3DEffect->GetParameter(NULL, i);
        D3DXPARAMETER_DESC ParameterDesc;
        if (FAILED(m_pD3DEffect->GetParameterDesc(hParameter, &ParameterDesc)))
            continue;

        // See if this parameter is 'special'
        if (TryParseSpecialParameter(hParameter, ParameterDesc))
            continue;
    }

    // Add parameter handles
    for (uint i = 0; i < EffectDesc.Parameters; i++)
    {
        D3DXHANDLE         hParameter = m_pD3DEffect->GetParameter(NULL, i);
        D3DXPARAMETER_DESC ParameterDesc;
        if (FAILED(m_pD3DEffect->GetParameterDesc(hParameter, &ParameterDesc)))
            continue;

        // Check if parameter is used
        if (m_bSkipUnusedParameters && !m_pD3DEffect->IsParameterUsed(hParameter, m_pD3DEffect->GetCurrentTechnique()) &&
            !IsSecondaryRenderTarget(hParameter, ParameterDesc))
            continue;

        // See if this parameter wants to be mapped to a D3D register
        if (TryMappingParameterToRegister(hParameter, ParameterDesc))
            continue;

        // Otherwise, assume its standard parameter
        if (AddStandardParameter(hParameter, ParameterDesc))
            continue;
    }

    // Finally find common handles that are automatically set by MTA before each render
    ReadCommonHandles();
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::GetAnnotationNameAndValue
//
// Get parameter annotation by index
//
////////////////////////////////////////////////////////////////
SString CEffectParameters::GetAnnotationNameAndValue(D3DXHANDLE hParameter, uint uiIndex, SString& strOutValue)
{
    if (!m_pD3DEffect)
        return "";

    D3DXHANDLE         hAnnotation = m_pD3DEffect->GetAnnotation(hParameter, uiIndex);
    if (!hAnnotation)
        return "";

    D3DXPARAMETER_DESC AnnotDesc;
    if (FAILED(m_pD3DEffect->GetParameterDesc(hAnnotation, &AnnotDesc)) || !AnnotDesc.Name)
        return "";

    LPCSTR szAnnotValue = nullptr;
    if (SUCCEEDED(m_pD3DEffect->GetString(hAnnotation, &szAnnotValue)) && szAnnotValue)
        strOutValue = szAnnotValue;

    return AnnotDesc.Name;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::TryParseSpecialParameter
//
// See if this parameter is 'special'
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::TryParseSpecialParameter(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc)
{
    if (!m_pD3DEffect)
        return false;

    // Use semantic if it exists
    SString strName = ParameterDesc.Semantic ? ParameterDesc.Semantic : ParameterDesc.Name;
    if (strName.CompareI("CUSTOMFLAGS"))
    {
        for (uint i = 0; i < ParameterDesc.Annotations; i++)
        {
            SString strAnnotValue, strAnnotName = GetAnnotationNameAndValue(hParameter, i, strAnnotValue);
            if (strAnnotName == "createNormals")
            {
                m_bRequiresNormals = (strAnnotValue == "yes");
            }
            else if (strAnnotName == "skipUnusedParameters")
            {
                m_bSkipUnusedParameters = (strAnnotValue == "yes");
            }
        }
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::IsSecondaryRenderTarget
//
// Return true if paramter is flagged for use as a secondary render target texture
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::IsSecondaryRenderTarget(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc)
{
    if (ParameterDesc.Type == D3DXPT_TEXTURE)
    {
        for (uint i = 0; i < ParameterDesc.Annotations; i++)
        {
            SString strAnnotValue, strAnnotName = GetAnnotationNameAndValue(hParameter, i, strAnnotValue);
            if (strAnnotName == "renderTarget")
            {
                return strAnnotValue == "yes";
            }
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::AddStandardParameter
//
// Global parameter not tied to a D3D register
// Called during parse stage
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::AddStandardParameter(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc)
{
    if (!m_pD3DEffect)
        return false;

    // Use semantic if it exists
    SString strName = ParameterDesc.Semantic ? ParameterDesc.Semantic : ParameterDesc.Name;
    // Add to correct lookup map
    if (ParameterDesc.Type == D3DXPT_TEXTURE)
    {
        if (IsSecondaryRenderTarget(hParameter, ParameterDesc))
        {
            m_SecondaryRenderTargetList.push_back(hParameter);
        }
        else if (!m_hFirstTexture)
        {
            // Keep handle to first non render target texture to get size of shader
            m_hFirstTexture = hParameter;
        }
        MapSet(m_textureHandleMap, strName.ToUpper(), hParameter);
    }
    else
        MapSet(m_valueHandleMap, strName.ToUpper(), hParameter);

    // Save default value in case we need to restore later
    if (ParameterDesc.Bytes != 0)
    {
        std::vector<char> buffer;
        buffer.resize(ParameterDesc.Bytes);
        if (SUCCEEDED(m_pD3DEffect->GetValue(hParameter, buffer.data(), buffer.size())))
        {
            MapSet(m_defaultValues, hParameter, buffer);
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::TryMappingParameterToRegister
//
// See if this global parameter is tied to a D3D register
//
////////////////////////////////////////////////////////////////
bool CEffectParameters::TryMappingParameterToRegister(D3DXHANDLE hParameter, const D3DXPARAMETER_DESC& ParameterDesc)
{
    // For each annotation
    for (uint i = 0; i < ParameterDesc.Annotations; i++)
    {
        // Get name and value
        SString strAnnotValue, strAnnotName = GetAnnotationNameAndValue(hParameter, i, strAnnotValue);

        // Find group match from name
        EStateGroup stateGroup;
        if (!StringToEnum(strAnnotName, stateGroup))
        {
            continue;
        }

        // Extract prepended stage number, if any
        SString strStagePart;
        SString strName;
        const bool bHasStagePart = strAnnotValue.Split(",", &strStagePart, &strName, -1);
        if (!bHasStagePart)
            strName = strAnnotValue;

        strName = TrimAsciiWhitespace(strName);
        strStagePart = TrimAsciiWhitespace(strStagePart);

        if (strName.empty())
            continue;

        int iStage = 0;
        const bool bGroupUsesStage = DoesStateGroupUseStageIndex(stateGroup);
        if (bHasStagePart)
        {
            if (!bGroupUsesStage)
            {
                // Stage index supplied where it is not expected
                continue;
            }

            if (strStagePart.empty())
                continue;

            char* pParseEnd = nullptr;
            const long parsedStage = strtol(strStagePart.c_str(), &pParseEnd, 10);
            if (!pParseEnd || *pParseEnd != '\0')
                continue;

            if (parsedStage < 0 || parsedStage > std::numeric_limits<int>::max())
                continue;

            iStage = static_cast<int>(parsedStage);
        }
        else if (!bGroupUsesStage)
        {
            iStage = 0;
        }

        if (bGroupUsesStage)
        {
            const int kMaxFixedStageCount = 8;
            if (iStage < 0 || iStage >= kMaxFixedStageCount)
                continue;
        }
        else if (bHasStagePart)
        {
            // Already handled above, but keep guard for readability
            continue;
        }

        // Find D3D register line for this group+name
        const SRegisterInfo* pRegsiterInfo = GetRegisterInfo(stateGroup, strName);
        if (!pRegsiterInfo)
        {
            continue;
        }

        // See if types can be mapped
        const STypeMapping* pTypeMapping = MatchTypeMapping(pRegsiterInfo->eReadableAsType, ParameterDesc);
        if (!pTypeMapping)
        {
            continue;
        }

        SStateVar var;
        var.stateGroup = stateGroup;
        var.hHandle = hParameter;
        var.iStage = iStage;
        var.iRegister = pRegsiterInfo->iOffset / 4;
        var.iType = pTypeMapping->OutType;
        var.iSize = pTypeMapping->OutSize;
        AddStateMappedParameter(stateGroup, var);
        m_bUsesMappedHandles = true;
        return true;            // We have a weiner
    }

    return false;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::GetRegisterInfo
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
const SRegisterInfo* CEffectParameters::GetRegisterInfo(EStateGroup stateGroup, const SString& strName)
{
    // registerInfoMap is used to speed up access to data in BigRegisterInfoList
    static std::map<SString, SRegisterInfo> registerInfoMap;
    if (registerInfoMap.empty())
    {
        for (uint i = 0; i < NUMELMS(BigRegisterInfoList); i++)
        {
            const SRegisterInfo& info = BigRegisterInfoList[i];
            SString              strKey("%s.%s", info.szRegName, info.szGroupName);
            MapSet(registerInfoMap, strKey.ToUpper(), info);
        }
    }

    SString              strKey("%s.%s", *strName, *EnumToString(stateGroup));
    const SRegisterInfo* pInfo = MapFind(registerInfoMap, strKey.ToUpper());
    return pInfo;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::MatchTypeMapping
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
const STypeMapping* CEffectParameters::MatchTypeMapping(EReadableAsType readableAsType, const D3DXPARAMETER_DESC& desc)
{
    // For each type mapping
    for (uint i = 0; i < NUMELMS(TypeMappingList); i++)
    {
        const STypeMapping& mapping = TypeMappingList[i];

        // Find a match
        if (mapping.readableAsType == readableAsType)
        {
            // Check it will work
            if (mapping.ParamMatchClass == desc.Class && mapping.ParamMatchType == desc.Type)
            {
                return &mapping;
            }
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::AddStateMappedParameter
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
void CEffectParameters::AddStateMappedParameter(EStateGroup stateGroup, const SStateVar& var)
{
    switch (stateGroup)
    {
        case STATE_GROUP_RENDER:
            m_renderStateVarList.push_back(var);
            break;
        case STATE_GROUP_STAGE:
            m_stageStateVarList.push_back(var);
            break;
        case STATE_GROUP_SAMPLER:
            m_samplerStateVarList.push_back(var);
            break;
        case STATE_GROUP_MATERIAL:
            m_materialStateVarList.push_back(var);
            break;
        case STATE_GROUP_TRANSFORM:
            m_transformStateVarList.push_back(var);
            break;
        case STATE_GROUP_TEXTURE:
            m_textureStateVarList.push_back(var);
            break;
        case STATE_GROUP_LIGHT:
            m_lightStateVarList.push_back(var);
            break;
        case STATE_GROUP_LIGHT_ENABLE:
            m_lightEnableStateVarList.push_back(var);
            break;
        case STATE_GROUP_DEVICE_CAPS:
            m_deviceCapsVarList.push_back(var);
            break;
        case STATE_GROUP_VERTEX_DECL:
            m_vertexDeclStateVarList.push_back(var);
            break;
    }
}

////////////////////////////////////////////////////////////////
//
// CEffectParameters::ReadCommonHandles
//
// Called during parse stage
//
////////////////////////////////////////////////////////////////
void CEffectParameters::ReadCommonHandles()
{
    struct
    {
        D3DXHANDLE& hHandle;
        const char* szName;
    } handleNames[] = {
        {m_CommonHandles.hWorld, "WORLD"},
        {m_CommonHandles.hView, "VIEW"},
        {m_CommonHandles.hProjection, "PROJECTION"},
        {m_CommonHandles.hWorldView, "WORLDVIEW"},
        {m_CommonHandles.hWorldViewProj, "WORLDVIEWPROJECTION"},
        {m_CommonHandles.hViewProj, "VIEWPROJECTION"},
        {m_CommonHandles.hViewInv, "VIEWINVERSE"},
        {m_CommonHandles.hWorldInvTr, "WORLDINVERSETRANSPOSE"},
        {m_CommonHandles.hViewInvTr, "VIEWINVERSETRANSPOSE"},
        {m_CommonHandles.hCamPos, "CAMERAPOSITION"},
        {m_CommonHandles.hCamDir, "CAMERADIRECTION"},
        {m_CommonHandles.hTime, "TIME"},
        {m_CommonHandles.hLightAmbient, "LIGHTAMBIENT"},
        {m_CommonHandles.hLightDiffuse, "LIGHTDIFFUSE"},
        {m_CommonHandles.hLightSpecular, "LIGHTSPECULAR"},
        {m_CommonHandles.hLightDirection, "LIGHTDIRECTION"},
        {m_CommonHandles.hDepthBuffer, "DEPTHBUFFER"},
        {m_CommonHandles.hViewMainScene, "VIEW_MAIN_SCENE"},
        {m_CommonHandles.hWorldMainScene, "WORLD_MAIN_SCENE"},
        {m_CommonHandles.hProjectionMainScene, "PROJECTION_MAIN_SCENE"},
    };

    for (uint h = 0; h < NUMELMS(handleNames); h++)
    {
        D3DXHANDLE* phHandle = MapFind(m_valueHandleMap, handleNames[h].szName);
        if (!phHandle)
            phHandle = MapFind(m_textureHandleMap, handleNames[h].szName);
        if (phHandle)
        {
            handleNames[h].hHandle = *phHandle;
            m_bUsesCommonHandles = true;
        }
    }
}
