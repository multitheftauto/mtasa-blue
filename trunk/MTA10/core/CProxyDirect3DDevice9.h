/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CProxyDirect3DDevice8.h
*  PURPOSE:     Header file for Direct3D 9 device proxy class
*  DEVELOPERS:  Derek Abdine <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CPROXYDIRECT3DDEVICE9_H
#define __CPROXYDIRECT3DDEVICE9_H

#include <d3d9.h>
#include "CDirect3DData.h"

interface CProxyDirect3DDevice9 : public IDirect3DDevice9
{
    // Proxy constructor and destructor.
    CProxyDirect3DDevice9 ( IDirect3DDevice9 * pDevice  );
   ~CProxyDirect3DDevice9 ( );

    /*** IUnknown methods ***/
    virtual HRESULT __stdcall    QueryInterface                 ( REFIID riid, void** ppvObj );
    virtual ULONG   __stdcall    AddRef                         ( VOID );
    virtual ULONG   __stdcall    Release                        ( VOID );

    /*** IDirect3DDevice9 methods ***/
    virtual HRESULT __stdcall    TestCooperativeLevel           ( VOID );
    virtual UINT    __stdcall    GetAvailableTextureMem         ( VOID );
    virtual HRESULT __stdcall    EvictManagedResources          ( VOID );
    virtual HRESULT __stdcall    GetDirect3D                    ( IDirect3D9** ppD3D9 );
    virtual HRESULT __stdcall    GetDeviceCaps                  ( D3DCAPS9* pCaps );
    virtual HRESULT __stdcall    GetDisplayMode                 ( UINT iSwapChain,D3DDISPLAYMODE* pMode );
    virtual HRESULT __stdcall    GetCreationParameters          ( D3DDEVICE_CREATION_PARAMETERS *pParameters );
    virtual HRESULT __stdcall    SetCursorProperties            ( UINT XHotSpot,UINT YHotSpot,IDirect3DSurface9* pCursorBitmap );
    virtual void    __stdcall    SetCursorPosition              ( int X,int Y,DWORD Flags );
    virtual BOOL    __stdcall    ShowCursor                     ( BOOL bShow );
    virtual HRESULT __stdcall    CreateAdditionalSwapChain      ( D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DSwapChain9** pSwapChain );
    virtual HRESULT __stdcall    GetSwapChain                   ( UINT iSwapChain,IDirect3DSwapChain9** pSwapChain );
    virtual UINT    __stdcall    GetNumberOfSwapChains          ( VOID );
    virtual HRESULT __stdcall    Reset                          ( D3DPRESENT_PARAMETERS* pPresentationParameters );
    virtual HRESULT __stdcall    Present                        ( CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion );
    virtual HRESULT __stdcall    GetBackBuffer                  ( UINT iSwapChain,UINT iBackBuffer,D3DBACKBUFFER_TYPE Type,IDirect3DSurface9** ppBackBuffer );
    virtual HRESULT __stdcall    GetRasterStatus                ( UINT iSwapChain,D3DRASTER_STATUS* pRasterStatus );
    virtual HRESULT __stdcall    SetDialogBoxMode               ( BOOL bEnableDialogs );
    virtual VOID    __stdcall    SetGammaRamp                   ( UINT iSwapChain,DWORD Flags,CONST D3DGAMMARAMP* pRamp );
    virtual VOID    __stdcall    GetGammaRamp                   ( UINT iSwapChain,D3DGAMMARAMP* pRamp );
    virtual HRESULT __stdcall    CreateTexture                  ( UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DTexture9** ppTexture,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateVolumeTexture            ( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DVolumeTexture9** ppVolumeTexture,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateCubeTexture              ( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DCubeTexture9** ppCubeTexture,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateVertexBuffer             ( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool,IDirect3DVertexBuffer9** ppVertexBuffer,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateIndexBuffer              ( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool,IDirect3DIndexBuffer9** ppIndexBuffer,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateRenderTarget             ( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    CreateDepthStencilSurface      ( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    UpdateSurface                  ( IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestinationSurface,CONST POINT* pDestPoint );
    virtual HRESULT __stdcall    UpdateTexture                  ( IDirect3DBaseTexture9* pSourceTexture,IDirect3DBaseTexture9* pDestinationTexture );
    virtual HRESULT __stdcall    GetRenderTargetData            ( IDirect3DSurface9* pRenderTarget,IDirect3DSurface9* pDestSurface );
    virtual HRESULT __stdcall    GetFrontBufferData             ( UINT iSwapChain,IDirect3DSurface9* pDestSurface );
    virtual HRESULT __stdcall    StretchRect                    ( IDirect3DSurface9* pSourceSurface,CONST RECT* pSourceRect,IDirect3DSurface9* pDestSurface,CONST RECT* pDestRect,D3DTEXTUREFILTERTYPE Filter );
    virtual HRESULT __stdcall    ColorFill                      ( IDirect3DSurface9* pSurface,CONST RECT* pRect,D3DCOLOR color );
    virtual HRESULT __stdcall    CreateOffscreenPlainSurface    ( UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle );
    virtual HRESULT __stdcall    SetRenderTarget                ( DWORD RenderTargetIndex,IDirect3DSurface9* pRenderTarget );
    virtual HRESULT __stdcall    GetRenderTarget                ( DWORD RenderTargetIndex,IDirect3DSurface9** ppRenderTarget );
    virtual HRESULT __stdcall    SetDepthStencilSurface         ( IDirect3DSurface9* pNewZStencil );
    virtual HRESULT __stdcall    GetDepthStencilSurface         ( IDirect3DSurface9** ppZStencilSurface );
    virtual HRESULT __stdcall    BeginScene                     ( VOID );
    virtual HRESULT __stdcall    EndScene                       ( VOID );
    virtual HRESULT __stdcall    Clear                          ( DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil );
    virtual HRESULT __stdcall    SetTransform                   ( D3DTRANSFORMSTATETYPE State,CONST D3DMATRIX* pMatrix );
    virtual HRESULT __stdcall    GetTransform                   ( D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix );
    virtual HRESULT __stdcall    MultiplyTransform              ( D3DTRANSFORMSTATETYPE,CONST D3DMATRIX* );
    virtual HRESULT __stdcall    SetViewport                    ( CONST D3DVIEWPORT9* pViewport );
    virtual HRESULT __stdcall    GetViewport                    ( D3DVIEWPORT9* pViewport );
    virtual HRESULT __stdcall    SetMaterial                    ( CONST D3DMATERIAL9* pMaterial );
    virtual HRESULT __stdcall    GetMaterial                    ( D3DMATERIAL9* pMaterial );
    virtual HRESULT __stdcall    SetLight                       ( DWORD Index,CONST D3DLIGHT9* );
    virtual HRESULT __stdcall    GetLight                       ( DWORD Index,D3DLIGHT9* );
    virtual HRESULT __stdcall    LightEnable                    ( DWORD Index,BOOL Enable );
    virtual HRESULT __stdcall    GetLightEnable                 ( DWORD Index,BOOL* pEnable );
    virtual HRESULT __stdcall    SetClipPlane                   ( DWORD Index,CONST float* pPlane );
    virtual HRESULT __stdcall    GetClipPlane                   ( DWORD Index,float* pPlane );
    virtual HRESULT __stdcall    SetRenderState                 ( D3DRENDERSTATETYPE State,DWORD Value );
    virtual HRESULT __stdcall    GetRenderState                 ( D3DRENDERSTATETYPE State,DWORD* pValue );
    virtual HRESULT __stdcall    CreateStateBlock               ( D3DSTATEBLOCKTYPE Type,IDirect3DStateBlock9** ppSB );
    virtual HRESULT __stdcall    BeginStateBlock                ( VOID );
    virtual HRESULT __stdcall    EndStateBlock                  ( IDirect3DStateBlock9** ppSB );
    virtual HRESULT __stdcall    SetClipStatus                  ( CONST D3DCLIPSTATUS9* pClipStatus );
    virtual HRESULT __stdcall    GetClipStatus                  ( D3DCLIPSTATUS9* pClipStatus );
    virtual HRESULT __stdcall    GetTexture                     ( DWORD Stage,IDirect3DBaseTexture9** ppTexture );
    virtual HRESULT __stdcall    SetTexture                     ( DWORD Stage,IDirect3DBaseTexture9* pTexture );
    virtual HRESULT __stdcall    GetTextureStageState           ( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD* pValue );
    virtual HRESULT __stdcall    SetTextureStageState           ( DWORD Stage,D3DTEXTURESTAGESTATETYPE Type,DWORD Value );
    virtual HRESULT __stdcall    GetSamplerState                ( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD* pValue );
    virtual HRESULT __stdcall    SetSamplerState                ( DWORD Sampler,D3DSAMPLERSTATETYPE Type,DWORD Value );
    virtual HRESULT __stdcall    ValidateDevice                 ( DWORD* pNumPasses );
    virtual HRESULT __stdcall    SetPaletteEntries              ( UINT PaletteNumber,CONST PALETTEENTRY* pEntries );
    virtual HRESULT __stdcall    GetPaletteEntries              ( UINT PaletteNumber,PALETTEENTRY* pEntries );
    virtual HRESULT __stdcall    SetCurrentTexturePalette       ( UINT PaletteNumber );
    virtual HRESULT __stdcall    GetCurrentTexturePalette       ( UINT *PaletteNumber );
    virtual HRESULT __stdcall    SetScissorRect                 ( CONST RECT* pRect );
    virtual HRESULT __stdcall    GetScissorRect                 ( RECT* pRect );
    virtual HRESULT __stdcall    SetSoftwareVertexProcessing    ( BOOL bSoftware );
    virtual BOOL    __stdcall    GetSoftwareVertexProcessing    ( VOID );
    virtual HRESULT __stdcall    SetNPatchMode                  ( float nSegments );
    virtual FLOAT   __stdcall    GetNPatchMode                  ( VOID );
    virtual HRESULT __stdcall    DrawPrimitive                  ( D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount );
    virtual HRESULT __stdcall    DrawIndexedPrimitive           ( D3DPRIMITIVETYPE,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    virtual HRESULT __stdcall    DrawPrimitiveUP                ( D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride );
    virtual HRESULT __stdcall    DrawIndexedPrimitiveUP         ( D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertices,UINT PrimitiveCount,CONST void* pIndexData,D3DFORMAT IndexDataFormat,CONST void* pVertexStreamZeroData,UINT VertexStreamZeroStride );
    virtual HRESULT __stdcall    ProcessVertices                ( UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,IDirect3DVertexBuffer9* pDestBuffer,IDirect3DVertexDeclaration9* pVertexDecl,DWORD Flags );
    virtual HRESULT __stdcall    CreateVertexDeclaration        ( CONST D3DVERTEXELEMENT9* pVertexElements,IDirect3DVertexDeclaration9** ppDecl );
    virtual HRESULT __stdcall    SetVertexDeclaration           ( IDirect3DVertexDeclaration9* pDecl );
    virtual HRESULT __stdcall    GetVertexDeclaration           ( IDirect3DVertexDeclaration9** ppDecl );
    virtual HRESULT __stdcall    SetFVF                         ( DWORD FVF );
    virtual HRESULT __stdcall    GetFVF                         ( DWORD* pFVF );
    virtual HRESULT __stdcall    CreateVertexShader             ( CONST DWORD* pFunction,IDirect3DVertexShader9** ppShader );
    virtual HRESULT __stdcall    SetVertexShader                ( IDirect3DVertexShader9* pShader );
    virtual HRESULT __stdcall    GetVertexShader                ( IDirect3DVertexShader9** ppShader );
    virtual HRESULT __stdcall    SetVertexShaderConstantF       ( UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount );
    virtual HRESULT __stdcall    GetVertexShaderConstantF       ( UINT StartRegister,float* pConstantData,UINT Vector4fCount );
    virtual HRESULT __stdcall    SetVertexShaderConstantI       ( UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount );
    virtual HRESULT __stdcall    GetVertexShaderConstantI       ( UINT StartRegister,int* pConstantData,UINT Vector4iCount );
    virtual HRESULT __stdcall    SetVertexShaderConstantB       ( UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount );
    virtual HRESULT __stdcall    GetVertexShaderConstantB       ( UINT StartRegister,BOOL* pConstantData,UINT BoolCount );
    virtual HRESULT __stdcall    SetStreamSource                ( UINT StreamNumber,IDirect3DVertexBuffer9* pStreamData,UINT OffsetInBytes,UINT Stride );
    virtual HRESULT __stdcall    GetStreamSource                ( UINT StreamNumber,IDirect3DVertexBuffer9** ppStreamData,UINT* pOffsetInBytes,UINT* pStride );
    virtual HRESULT __stdcall    SetStreamSourceFreq            ( UINT StreamNumber,UINT Setting );
    virtual HRESULT __stdcall    GetStreamSourceFreq            ( UINT StreamNumber,UINT* pSetting );
    virtual HRESULT __stdcall    SetIndices                     ( IDirect3DIndexBuffer9* pIndexData );
    virtual HRESULT __stdcall    GetIndices                     ( IDirect3DIndexBuffer9** ppIndexData );
    virtual HRESULT __stdcall    CreatePixelShader              ( CONST DWORD* pFunction,IDirect3DPixelShader9** ppShader );
    virtual HRESULT __stdcall    SetPixelShader                 ( IDirect3DPixelShader9* pShader );
    virtual HRESULT __stdcall    GetPixelShader                 ( IDirect3DPixelShader9** ppShader );
    virtual HRESULT __stdcall    SetPixelShaderConstantF        ( UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount );
    virtual HRESULT __stdcall    GetPixelShaderConstantF        ( UINT StartRegister,float* pConstantData,UINT Vector4fCount );
    virtual HRESULT __stdcall    SetPixelShaderConstantI        ( UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount );
    virtual HRESULT __stdcall    GetPixelShaderConstantI        ( UINT StartRegister,int* pConstantData,UINT Vector4iCount );
    virtual HRESULT __stdcall    SetPixelShaderConstantB        ( UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount );
    virtual HRESULT __stdcall    GetPixelShaderConstantB        ( UINT StartRegister,BOOL* pConstantData,UINT BoolCount );
    virtual HRESULT __stdcall    DrawRectPatch                  ( UINT Handle,CONST float* pNumSegs,CONST D3DRECTPATCH_INFO* pRectPatchInfo );
    virtual HRESULT __stdcall    DrawTriPatch                   ( UINT Handle,CONST float* pNumSegs,CONST D3DTRIPATCH_INFO* pTriPatchInfo );
    virtual HRESULT __stdcall    DeletePatch                    ( UINT Handle );
    virtual HRESULT __stdcall    CreateQuery                    ( D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery );

private:

    IDirect3DDevice9 *          m_pDevice;
    DWORD                       m_dwRefCount;
    CDirect3DData *             m_pData;

public:

    //
    // Capture device state
    //

    const static int D3DRS_MAX = 210;
    const static int D3DTSS_MAX = 33;
    const static int D3DSAMP_MAX = 14;
	const static int D3DTS_MAX = 260;

    #define DPAD(f,t)   DWORD       pad##f##_##t[(t)-(f)+1]
	#define MPAD(f,t)   D3DMATRIX   pad##f##_##t[(t)-(f)+1]

    struct SD3DRenderState
    {
        union {
            DWORD Raw[D3DRS_MAX];
            struct {
                DPAD(0,6);
                D3DZBUFFERTYPE  ZENABLE;                        //  = 7,    /* D3DZBUFFERTYPE (or TRUE/FALSE for legacy) */
                D3DFILLMODE     FILLMODE;                       //  = 8,    /* D3DFILLMODE */
                D3DSHADEMODE    SHADEMODE;                      //  = 9,    /* D3DSHADEMODE */
                DPAD(10,13);
                DWORD           ZWRITEENABLE;                   //  = 14,   /* TRUE to enable z writes */
                DWORD           ALPHATESTENABLE;                //  = 15,   /* TRUE to enable alpha tests */
                DWORD           LASTPIXEL;                      //  = 16,   /* TRUE for last-pixel on lines */
                DPAD(17,18);
                D3DBLEND        SRCBLEND;                       //  = 19,   /* D3DBLEND */
                D3DBLEND        DESTBLEND;                      //  = 20,   /* D3DBLEND */
                DPAD(21,21);
                D3DCULL         CULLMODE;                       //  = 22,   /* D3DCULL */
                D3DCMPFUNC      ZFUNC;                          //  = 23,   /* D3DCMPFUNC */
                DWORD           ALPHAREF;                       //  = 24,   /* D3DFIXED */
                D3DCMPFUNC      ALPHAFUNC;                      //  = 25,   /* D3DCMPFUNC */
                DWORD           DITHERENABLE;                   //  = 26,   /* TRUE to enable dithering */
                DWORD           ALPHABLENDENABLE;               //  = 27,   /* TRUE to enable alpha blending */
                DWORD           FOGENABLE;                      //  = 28,   /* TRUE to enable fog blending */
                DWORD           SPECULARENABLE;                 //  = 29,   /* TRUE to enable specular */
                DPAD(30,33);
                D3DCOLOR        FOGCOLOR;                       //  = 34,   /* D3DCOLOR */
                D3DFOGMODE      FOGTABLEMODE;                   //  = 35,   /* D3DFOGMODE */
                DWORD           FOGSTART;                       //  = 36,   /* Fog start (for both vertex and pixel fog) */
                DWORD           FOGEND;                         //  = 37,   /* Fog end      */
                DWORD           FOGDENSITY;                     //  = 38,   /* Fog density  */
                DPAD(39,47);
                DWORD           RANGEFOGENABLE;                 //  = 48,   /* Enables range-based fog */
                DPAD(49,51);
                DWORD           STENCILENABLE;                  //  = 52,   /* BOOL enable/disable stenciling */
                D3DSTENCILOP    STENCILFAIL;                    //  = 53,   /* D3DSTENCILOP to do if stencil test fails */
                D3DSTENCILOP    STENCILZFAIL;                   //  = 54,   /* D3DSTENCILOP to do if stencil test passes and Z test fails */
                D3DSTENCILOP    STENCILPASS;                    //  = 55,   /* D3DSTENCILOP to do if both stencil and Z tests pass */
                D3DCMPFUNC      STENCILFUNC;                    //  = 56,   /* D3DCMPFUNC fn.  Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
                DWORD           STENCILREF;                     //  = 57,   /* Reference value used in stencil test */
                DWORD           STENCILMASK;                    //  = 58,   /* Mask value used in stencil test */
                DWORD           STENCILWRITEMASK;               //  = 59,   /* Write mask applied to values written to stencil buffer */
                D3DCOLOR        TEXTUREFACTOR;                  //  = 60,   /* D3DCOLOR used for multi-texture blend */
                DPAD(61,127);
                DWORD           WRAP0;                          //  = 128,  /* wrap for 1st texture coord. set */
                DWORD           WRAP1;                          //  = 129,  /* wrap for 2nd texture coord. set */
                DWORD           WRAP2;                          //  = 130,  /* wrap for 3rd texture coord. set */
                DWORD           WRAP3;                          //  = 131,  /* wrap for 4th texture coord. set */
                DWORD           WRAP4;                          //  = 132,  /* wrap for 5th texture coord. set */
                DWORD           WRAP5;                          //  = 133,  /* wrap for 6th texture coord. set */
                DWORD           WRAP6;                          //  = 134,  /* wrap for 7th texture coord. set */
                DWORD           WRAP7;                          //  = 135,  /* wrap for 8th texture coord. set */
                DWORD           CLIPPING;                       //  = 136,
                DWORD           LIGHTING;                       //  = 137,
                DPAD(138,138);
                D3DCOLOR        AMBIENT;                        //  = 139,
                DWORD           FOGVERTEXMODE;                  //  = 140,
                DWORD           COLORVERTEX;                    //  = 141,
                DWORD           LOCALVIEWER;                    //  = 142,
                DWORD           NORMALIZENORMALS;               //  = 143,
                DPAD(144,144);
                DWORD           DIFFUSEMATERIALSOURCE;          //  = 145,
                DWORD           SPECULARMATERIALSOURCE;         //  = 146,
                DWORD           AMBIENTMATERIALSOURCE;          //  = 147,
                DWORD           EMISSIVEMATERIALSOURCE;         //  = 148,
                DPAD(149,150);
                DWORD           VERTEXBLEND;                    //  = 151,
                DWORD           CLIPPLANEENABLE;                //  = 152,
                DPAD(153,153);
                DWORD           POINTSIZE;                      //  = 154,   /* float point size */
                DWORD           POINTSIZE_MIN;                  //  = 155,   /* float point size min threshold */
                DWORD           POINTSPRITEENABLE;              //  = 156,   /* BOOL point texture coord control */
                DWORD           POINTSCALEENABLE;               //  = 157,   /* BOOL point size scale enable */
                DWORD           POINTSCALE_A;                   //  = 158,   /* float point attenuation A value */
                DWORD           POINTSCALE_B;                   //  = 159,   /* float point attenuation B value */
                DWORD           POINTSCALE_C;                   //  = 160,   /* float point attenuation C value */
                DWORD           MULTISAMPLEANTIALIAS;           //  = 161,  // BOOL - set to do FSAA with multisample buffer
                DWORD           MULTISAMPLEMASK;                //  = 162,  // DWORD -;        //per-sample enable/disable
                DWORD           PATCHEDGESTYLE;                 //  = 163,  // Sets whether patch edges will use float style tessellation
                DPAD(164,164);
                DWORD           DEBUGMONITORTOKEN;              //  = 165,  // DEBUG ONLY - token to debug monitor
                DWORD           POINTSIZE_MAX;                  //  = 166,   /* float point size max threshold */
                DWORD           INDEXEDVERTEXBLENDENABLE;       //  = 167,
                DWORD           COLORWRITEENABLE;               //  = 168,  // per-channel write enable
                DPAD(169,169);
                DWORD           TWEENFACTOR;                    //  = 170,   // float tween factor
                D3DBLENDOP      BLENDOP;                        //  = 171,   // D3DBLENDOP setting
                D3DDEGREETYPE   POSITIONDEGREE;                 //  = 172,   // NPatch position interpolation degree. D3DDEGREE_LINEAR or D3DDEGREE_CUBIC (default)
                D3DDEGREETYPE   NORMALDEGREE;                   //  = 173,   // NPatch normal interpolation degree. D3DDEGREE_LINEAR (default) or D3DDEGREE_QUADRATIC
                DWORD           SCISSORTESTENABLE;              //  = 174,
                DWORD           SLOPESCALEDEPTHBIAS;            //  = 175,
                DWORD           ANTIALIASEDLINEENABLE;          //  = 176,
                DPAD(177,177);
                DWORD           MINTESSELLATIONLEVEL;           //  = 178,
                DWORD           MAXTESSELLATIONLEVEL;           //  = 179,
                DWORD           ADAPTIVETESS_X;                 //  = 180,
                DWORD           ADAPTIVETESS_Y;                 //  = 181,
                DWORD           ADAPTIVETESS_Z;                 //  = 182,
                DWORD           ADAPTIVETESS_W;                 //  = 183,
                DWORD           ENABLEADAPTIVETESSELLATION;     //  = 184,
                DWORD           TWOSIDEDSTENCILMODE;            //  = 185,   /* BOOL enable/disable 2 sided stenciling */
                D3DSTENCILOP    CCW_STENCILFAIL;                //  = 186,   /* D3DSTENCILOP to do if ccw stencil test fails */
                D3DSTENCILOP    CCW_STENCILZFAIL;               //  = 187,   /* D3DSTENCILOP to do if ccw stencil test passes and Z test fails */
                D3DSTENCILOP    CCW_STENCILPASS;                //  = 188,   /* D3DSTENCILOP to do if both ccw stencil and Z tests pass */
                D3DCMPFUNC      CCW_STENCILFUNC;                //  = 189,   /* D3DCMPFUNC fn.  ccw Stencil Test passes if ((ref & mask) stencilfn (stencil & mask)) is true */
                DWORD           COLORWRITEENABLE1;              //  = 190,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                DWORD           COLORWRITEENABLE2;              //  = 191,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                DWORD           COLORWRITEENABLE3;              //  = 192,   /* Additional ColorWriteEnables for the devices that support D3DPMISCCAPS_INDEPENDENTWRITEMASKS */
                D3DCOLOR        BLENDFACTOR;                    //  = 193,   /* D3DCOLOR used for a constant blend factor during alpha blending for devices that support D3DPBLENDCAPS_BLENDFACTOR */
                DWORD           SRGBWRITEENABLE;                //  = 194,   /* Enable rendertarget writes to be DE-linearized to SRGB (for formats that expose D3DUSAGE_QUERY_SRGBWRITE) */
                DWORD           DEPTHBIAS;                      //  = 195,
                DPAD(196,197);
                DWORD           WRAP8;                          //  = 198,   /* Additional wrap states for vs_3_0+ attributes with D3DDECLUSAGE_TEXCOORD */
                DWORD           WRAP9;                          //  = 199,
                DWORD           WRAP10;                         //  = 200,
                DWORD           WRAP11;                         //  = 201,
                DWORD           WRAP12;                         //  = 202,
                DWORD           WRAP13;                         //  = 203,
                DWORD           WRAP14;                         //  = 204,
                DWORD           WRAP15;                         //  = 205,
                DWORD           SEPARATEALPHABLENDENABLE;       //  = 206,  /* TRUE to enable a separate blending function for the alpha channel */
                DWORD           SRCBLENDALPHA;                  //  = 207,  /* SRC blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE;        //is TRUE */
                DWORD           DESTBLENDALPHA;                 //  = 208,  /* DST blend factor for the alpha channel when DWORD SEPARATEDESTALPHAENABLE;        //is TRUE */
                DWORD           BLENDOPALPHA;                   //  = 209,  /* Blending operation for the alpha channel when DWORD SEPARATEDESTALPHAENABLE;        //is TRUE */
            };
        };
    };

    struct SD3DStageState
    {
        union {
            DWORD Raw[D3DTSS_MAX];
            struct {
                DPAD(0,0);
                D3DTEXTUREOP    COLOROP;                        //  =  1, /* D3DTEXTUREOP - per-stage blending controls for color channels */
                DWORD           COLORARG1;                      //  =  2, /* D3DTA_* (texture arg) */
                DWORD           COLORARG2;                      //  =  3, /* D3DTA_* (texture arg) */
                D3DTEXTUREOP    ALPHAOP;                        //  =  4, /* D3DTEXTUREOP - per-stage blending controls for alpha channel */
                DWORD           ALPHAARG1;                      //  =  5, /* D3DTA_* (texture arg) */
                DWORD           ALPHAARG2;                      //  =  6, /* D3DTA_* (texture arg) */
                DWORD           BUMPENVMAT00;                   //  =  7, /* float (bump mapping matrix) */
                DWORD           BUMPENVMAT01;                   //  =  8, /* float (bump mapping matrix) */
                DWORD           BUMPENVMAT10;                   //  =  9, /* float (bump mapping matrix) */
                DWORD           BUMPENVMAT11;                   //  = 10, /* float (bump mapping matrix) */
                DWORD           TEXCOORDINDEX;                  //  = 11, /* identifies which set of texture coordinates index this texture */
                DPAD(12,21);
                DWORD           BUMPENVLSCALE;                  //  = 22, /* float scale for bump map luminance */
                DWORD           BUMPENVLOFFSET;                 //  = 23, /* float offset for bump map luminance */
                D3DTEXTURETRANSFORMFLAGS TEXTURETRANSFORMFLAGS; // = 24, /* D3DTEXTURETRANSFORMFLAGS controls texture transform */
                DPAD(25,25);
                DWORD            COLORARG0;                     //  = 26, /* D3DTA_* third arg for triadic ops */
                DWORD            ALPHAARG0;                     //  = 27, /* D3DTA_* third arg for triadic ops */
                DWORD            RESULTARG;                     //  = 28, /* D3DTA_* arg for result (CURRENT or TEMP) */
                DPAD(29,31);
                DWORD            CONSTANT;                      //  = 32, /* Per-stage constant D3DTA_CONSTANT */
            };
        };
    };

    struct SD3DSamplerState
    {
        union {
            DWORD Raw[D3DSAMP_MAX];
            struct {
                DPAD(0,0);
                D3DTEXTUREADDRESS       ADDRESSU;           //  = 1,  /* D3DTEXTUREADDRESS for U coordinate */
                D3DTEXTUREADDRESS       ADDRESSV;           //  = 2,  /* D3DTEXTUREADDRESS for V coordinate */
                D3DTEXTUREADDRESS       ADDRESSW;           //  = 3,  /* D3DTEXTUREADDRESS for W coordinate */
                D3DCOLOR                BORDERCOLOR;        //  = 4,  /* D3DCOLOR */
                D3DTEXTUREFILTERTYPE    MAGFILTER;          //  = 5,  /* D3DTEXTUREFILTER filter to use for magnification */
                D3DTEXTUREFILTERTYPE    MINFILTER;          //  = 6,  /* D3DTEXTUREFILTER filter to use for minification */
                D3DTEXTUREFILTERTYPE    MIPFILTER;          //  = 7,  /* D3DTEXTUREFILTER filter to use between mipmaps during minification */
                DWORD                   MIPMAPLODBIAS;      //  = 8,  /* float Mipmap LOD bias */
                DWORD                   MAXMIPLEVEL;        //  = 9,  /* DWORD 0..(n-1) LOD index of largest map to use (0 == largest) */
                DWORD                   MAXANISOTROPY;      //  = 10, /* DWORD maximum anisotropy */
                DWORD                   SRGBTEXTURE;        //  = 11, /* Default = 0 (which means Gamma 1.0,
                                                            //       no correction required.) else correct for
                                                            //       Gamma = 2.2 */
                DWORD                   ELEMENTINDEX;       //  = 12, /* When multi-element texture is assigned to sampler, this
                                                            //        indicates which element index to use.  Default = 0.  */
                DWORD                   DMAPOFFSET;         //  = 13, /* Offset in vertices in the pre-sampled displacement map.
                                                            //        Only valid for D3DDMAPSAMPLER sampler  */
            };
        };
    };

    struct SD3DTransformState
    {
        union {
            D3DMATRIX Raw[D3DTS_MAX];
            struct {
                MPAD(0,1);
                D3DMATRIX   VIEW;                    //  = 2,
                D3DMATRIX   PROJECTION;              //  = 3,
                MPAD(4,15);
                D3DMATRIX   TEXTURE0;                //  = 16,
                D3DMATRIX   TEXTURE1;                //  = 17,
                D3DMATRIX   TEXTURE2;                //  = 18,
                D3DMATRIX   TEXTURE3;                //  = 19,
                D3DMATRIX   TEXTURE4;                //  = 20,
                D3DMATRIX   TEXTURE5;                //  = 21,
                D3DMATRIX   TEXTURE6;                //  = 22,
                D3DMATRIX   TEXTURE7;                //  = 23,
                MPAD(24,255);
                D3DMATRIX   WORLD;                   //  = 256,
                D3DMATRIX   WORLD1;                  //  = 257,
                D3DMATRIX   WORLD2;                  //  = 258,
                D3DMATRIX   WORLD3;                  //  = 259,
            };
        };
    };

    struct SD3DTextureState
    {
        IDirect3DBaseTexture9* Texture;
    };

    struct SD3DLightEnableState
    {
        DWORD Enable;
    };

    struct SD3DVertexDeclState
    {
        SD3DVertexDeclState ( void )
        {
            ZERO_POD_STRUCT( this );
        }

        bool bUsesStreamAtIndex[2];
        DWORD Position;
        DWORD PositionT;
        DWORD Normal;
        DWORD Color0;
        DWORD Color1;
        DWORD TexCoord0;
        DWORD TexCoord1;
    };

    struct SAdapterState
    {
        SFixedString < 128 > Name;
        int InstalledMemoryKB;
        int MaxAnisotropicSetting;
    };

    struct SCallState
    {
        enum eD3DCallType
        {
            NONE,
            DRAW_PRIMITIVE,
            DRAW_INDEXED_PRIMITIVE,
        };
        eD3DCallType callType;
        uint uiNumArgs;
        int args [ 10 ];
        SFixedString < 32 > strShaderName;
        bool bShaderRequiresNormals;
    };

    struct SResourceMemory
    {
        int iCurrentCount;
        int iCurrentBytes;
        int iCreatedCount;
        int iCreatedBytes;
        int iDestroyedCount;
        int iDestroyedBytes;
        int iLockedCount;
    };

    struct SMemoryState
    {
        SResourceMemory StaticVertexBuffer;
        SResourceMemory DynamicVertexBuffer;
        SResourceMemory StaticIndexBuffer;
        SResourceMemory DynamicIndexBuffer;
        SResourceMemory StaticTexture;
        SResourceMemory DynamicTexture;
    };

    struct SStreamSourceState
    {
        IDirect3DVertexBuffer9*     StreamData;
        UINT                        StreamOffset;
        UINT                        StreamStride;
    };

    struct SD3DDeviceState
    {
        SD3DDeviceState ()
        {
            ZERO_POD_STRUCT( this );
        }

        SD3DRenderState                 RenderState;
        SD3DStageState                  StageState[8];
        SD3DSamplerState                SamplerState[8];
        SD3DTransformState              TransformState;
        SD3DTextureState                TextureState[8];
        SD3DLightEnableState            LightEnableState[8];

        IDirect3DVertexDeclaration9*    VertexDeclaration;
        IDirect3DVertexShader9*         VertexShader;
        IDirect3DPixelShader9*          PixelShader;
        IDirect3DIndexBuffer9*          IndexBufferData;
        D3DLIGHT9                       Lights[8];
        D3DMATERIAL9                    Material;
        D3DCAPS9                        DeviceCaps;
        SD3DVertexDeclState             VertexDeclState;
        SAdapterState                   AdapterState;
        SMemoryState                    MemoryState;
        SCallState                      CallState;
        SStreamSourceState              VertexStreams[16];
    };

    SD3DDeviceState     DeviceState;
    std::map < IDirect3DVertexDeclaration9*, SD3DVertexDeclState > m_VertexDeclMap;

    // Debugging
    void                SetCallType     ( SCallState::eD3DCallType callType, uint uiNumArgs = 0, ... );
};

extern CProxyDirect3DDevice9* g_pProxyDevice;
extern CProxyDirect3DDevice9::SD3DDeviceState* g_pDeviceState;


#endif
