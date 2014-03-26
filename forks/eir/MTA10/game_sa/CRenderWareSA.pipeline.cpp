/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.pipeline.cpp
*  PURPOSE:     RenderWare pipeline (OS implementation) management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "RenderWare/RwRenderTools.hxx"
#include "RenderWare/RwInternals.h"

RwRenderStateLock::_rsLockDesc RwRenderStateLock::_rsLockInfo[210];

/*=========================================================
    RwD3D9SetRenderState

    Arguments:
        type - D3D9 RenderState id
        value - integer with the new RenderState value
    Purpose:
        Sets a RenderState to the RenderWare D3D9 pipeline
        system and applies it during next atomic render.
    Binary offsets:
        (1.0 US): 0x007FC2D0
        (1.0 EU): 0x007FC310
=========================================================*/
struct _renderStateDesc
{
    deviceStateValue value;
    int addedToQuery;
};

inline _renderStateDesc& GetNativeRenderStateDesc( D3DRENDERSTATETYPE type )
{
    return ((_renderStateDesc*)0x00C991D0)[type];
}

void __cdecl RwD3D9SetRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    // The reason we use the native table is because a lot of calls to
    // RwD3D9SetRenderState have been inlined by the compiler. I realized
    // that the render state locks are flawed until we reverse all such
    // inlined calls.
    // * RenderWare appears to have a native inlined routine for setting render
    //   states that always assumes they need updating (1.0 US): 0x007FE0D0
    _renderStateDesc& desc = GetNativeRenderStateDesc( type );

    if ( desc.value == value )
        return;

    desc.value = value;

    if ( !desc.addedToQuery )
    {
        desc.addedToQuery = 1;

        ((D3DRENDERSTATETYPE*)0x00C98B40)[ (*(unsigned int*)0x00C9A5EC)++ ] = type;
    }
}

/*=========================================================
    RwD3D9ForceRenderState (MTA extension)

    Arguments:
        type - D3D9 RenderState id
        value - integer with the new RenderState value
    Purpose:
        Applies a RenderState and ignores further RenderState
        assignments until that RenderState is not required
        by MTA anymore.
=========================================================*/
struct _internalRenderStateDesc
{
    _internalRenderStateDesc( void )
    {
        value = 0;
        isForced = false;
    }

    deviceStateValue value;         // native GTA:SA renderstate
    deviceStateValue forceValue;    // value to be forced into the rendering device
    bool isForced;                  // boolean whether MTA wants only its renderstate active
};
static _internalRenderStateDesc _intRenderStates[210];

void RwD3D9ForceRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    _internalRenderStateDesc& desc = _intRenderStates[type];

    // Notify the system that we force a RenderState
    if ( !desc.isForced )
    {
        desc.isForced = true;
        RwD3D9GetRenderState( type, desc.value );
    }

    // Store the forced renderstate so we ensure sync with native code.
    desc.forceValue = value;

    // Set the MTA RenderState
    RwD3D9SetRenderState( type, value );
}

/*=========================================================
    HOOK_RwD3D9SetRenderState

    Arguments:
        type - D3D9 RenderState id
        value - integer with the new RenderState value
    Purpose:
        Called by the engine to set a RenderWare D3D9 pipeline
        RenderState. MTA RenderStates can take priority over
        GTA:SA ones, so they should be delayed if there is a
        conflict.
    Binary offsets:
        (1.0 US): 0x007FC2D0
        (1.0 EU): 0x007FC310
=========================================================*/
void __cdecl HOOK_RwD3D9SetRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    // Actual bugfix.
    __asm push edx

    _internalRenderStateDesc& desc = _intRenderStates[type];

    // Check whether MTA wants to force a RenderState
    if ( desc.isForced )
    {
        // Store the internal value
        desc.value = value;
    }
    else    // Otherwise we just set it
        RwD3D9SetRenderState( type, value );

    __asm pop edx
}

/*=========================================================
    RwD3D9GetRenderState

    Arguments:
        type - D3D9 RenderState id
    Purpose:
        Retrieves the current RenderWare D3D9 pipeline
        RenderState.
    Binary offsets:
        (1.0 US): 0x007FC320
        (1.0 EU): 0x007FC360
    Note:
        When dealing with API that tries to adapt to proprietory
        APIs like Direct3D, we better leave the usage as close to
        the original as possible. I had the idea to let this
        function return a DWORD, but decided against it since I
        realized that it did not match the GTA:SA representation.
=========================================================*/
void __cdecl RwD3D9GetRenderState( D3DRENDERSTATETYPE type, DWORD& value )
{
    value = GetNativeRenderStateDesc( type ).value;
}

/*=========================================================
    RwD3D9SetTextureStageState

    Arguments:
        stageId - index of the Direct3D 9 TSS
        stateType - type index of the TSS
        value - the new value to the TSS
    Purpose:
        Updates a texture stage state of the main Direct3D 9
        device. The entries are cached, so that unnecessary
        accesses are not made.
    Binary offsets:
        (1.0 US): 0x007FC340
        (1.0 EU): 0x007FC380
    Note:
        When dealing with API that tries to adapt to proprietory
        APIs like Direct3D, we better leave the usage as close to
        the original as possible. I had the idea to let this
        function return a DWORD, but decided against it since I
        realized that it did not match the GTA:SA representation.
=========================================================*/
struct _texStageStateQuery
{
    DWORD id;
    D3DTEXTURESTAGESTATETYPE type;
};

struct _texStageStateDesc
{
    deviceStateValue value;
    int isQueried;
};

static _texStageStateDesc _texStateStages[8][33];

inline _texStageStateDesc& GetNativeTextureStageStateDesc( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType )
{
    return _texStateStages[stageId][stateType];
}

void __cdecl RwD3D9SetTextureStageState( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD value )
{
    _texStageStateDesc& stateInfo = GetNativeTextureStageStateDesc( stageId, stateType );

    if ( stateInfo.value == value )
        return;

    stateInfo.value = value;

    // Make sure the update it notified to the device.
    if ( !stateInfo.isQueried )
    {
        stateInfo.isQueried = true;

        // Queue the request to update our TSS to the device.
        _texStageStateQuery& queryItem = ((_texStageStateQuery*)0x00C99C80)[ (*(unsigned int*)0x00C9A5F0)++ ];
        queryItem.id = stageId;
        queryItem.type = stateType;
    }
}

/*=========================================================
    RwD3D9GetTextureStageState

    Arguments:
        stageId - index of the Direct3D 9 TSS
        stateType - type index of the TSS
        value - pointer to the value where the TSS value
                should be written to
    Purpose:
        Retrieves the current Direct3D 9 device texture stage
        state. The value may not be written to the device yet
        but is pending for a push on RwD3D9ApplyDeviceStates.
    Binary offsets:
        (1.0 US): 0x007FC3A0
        (1.0 EU): 0x007FC3E0
    Note:
        When dealing with API that tries to adapt to proprietory
        APIs like Direct3D, we better leave the usage as close to
        the original as possible. I had the idea to let this
        function return a DWORD, but decided against it since I
        realized that it did not match the GTA:SA representation.
=========================================================*/
void __cdecl RwD3D9GetTextureStageState( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD& value )
{
    value = GetNativeTextureStageStateDesc( stageId, stateType ).value;
}

/*=========================================================
    HOOK_RwD3D9GetRenderState

    Arguments:
        type - D3D9 RenderState id
    Purpose:
        Retrieves the current RenderWare D3D9 pipeline
        RenderState (for GTA:SA).
    Binary offsets:
        (1.0 US): 0x007FC320
        (1.0 EU): 0x007FC360
=========================================================*/
void __cdecl HOOK_RwD3D9GetRenderState( D3DRENDERSTATETYPE type, DWORD& value )
{
    // Actual bugfix.
    __asm push edx

    _internalRenderStateDesc& desc = _intRenderStates[type];

    if ( desc.isForced )
        value = desc.forceValue;
    else
        RwD3D9GetRenderState( type, value );

    __asm pop edx
}

/*=========================================================
    RwD3D9FreeRenderState (MTA extension)

    Arguments:
        type - D3D9 RenderState id
    Purpose:
        Removes the forced assignment of a RenderWare D3D9
        pipeline RenderState by MTA and sets the GTA:SA
        RenderState value.
=========================================================*/
void RwD3D9FreeRenderState( D3DRENDERSTATETYPE type )
{
    _internalRenderStateDesc& desc = _intRenderStates[type];

    if ( !desc.isForced )
        return;

    // Notify we are not forcing a value anymore
    desc.isForced = false;

    // If a rogue renderstate change has happened, we cannot revert back.
    // Reverting back would screw the integrity of renderstates.
    // (rogue changes should be avoided at all cost, but happen often
    // since we do not control the whole GTA:SA codebase yet).
    DWORD curState = 0;

    RwD3D9GetRenderState( type, curState );

    if ( curState == desc.forceValue )
    {
        // Revert to the GTA:SA version
        RwD3D9SetRenderState( type, desc.value );
    }
}

/*=========================================================
    RwD3D9FreeRenderStates (MTA extension)

    Purpose:
        Works through the internal queue of GTA:SA assigned
        RenderStates and reverts them to their internal
        values.
=========================================================*/
void RwD3D9FreeRenderStates( void )
{
    for ( unsigned int n = 0; n < 210; n++ )
    {
        D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)n;
        _internalRenderStateDesc& desc = _intRenderStates[type];

        // Revert the value (if forced)
        RwD3D9FreeRenderState( type );
    }

    RwD3D9ApplyDeviceStates();
}

/*=========================================================
    RwD3D9ApplyDeviceStates

    Purpose:
        Applies the queued RenderWare D3D9 RenderStates to
        the device.
    Binary offsets:
        (1.0 US): 0x007FC200
        (1.0 EU): 0x007FC240
=========================================================*/
static deviceStateValue currentTextureStageStateValues[8][33];

inline deviceStateValue& GetNativeCurrentRenderState( D3DRENDERSTATETYPE type )
{
    return ((deviceStateValue*)0x00C98E88)[type];
}

inline deviceStateValue& GetNativeCurrentTextureStageState( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType )
{
    return currentTextureStageStateValues[stageId][stateType];
}

void __cdecl RwD3D9ApplyDeviceStates( void )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice();
    
    // Apply RenderStates
    unsigned int renderStates = *(unsigned int*)0x00C9A5EC;

    for ( unsigned int n = 0; n < renderStates; n++ )
    {
        D3DRENDERSTATETYPE type = ((D3DRENDERSTATETYPE*)0x00C98B40)[n];
        _renderStateDesc& desc = GetNativeRenderStateDesc( type );
        
        DWORD newState = desc.value;
        deviceStateValue& currentState = GetNativeCurrentRenderState( type );

        // We processed this entry
        desc.addedToQuery = false;

        if ( currentState != newState )
        {
            // Check whether native code conflicts with our implementation here.
            // If so, fix the render state representation.
            _internalRenderStateDesc& intInfo = _intRenderStates[type];

            if ( intInfo.isForced && intInfo.forceValue != newState )
            {
                desc.value = intInfo.forceValue;
                intInfo.value = newState;
            }
            else
            {
                // RS change is legit, apply it.
                renderDevice->SetRenderState( type, newState );

                currentState = newState;
            }
        }
    }

    // Processed, so zero out
    *(unsigned int*)0x00C9A5EC = 0;

    unsigned int textureStates = *(unsigned int*)0x00C9A5F0;

    // Apply texture stage states
    for ( unsigned int n = 0; n < textureStates; n++ )
    {
        _texStageStateQuery& item = ((_texStageStateQuery*)0x00C99C80)[n];
        _texStageStateDesc& desc = GetNativeTextureStageStateDesc( item.id, item.type );

        desc.isQueried = false;

        DWORD newState = desc.value;
        deviceStateValue& currentState = GetNativeCurrentTextureStageState( item.id, item.type );

        if ( newState != currentState )
        {
            // Update the texture stage
            renderDevice->SetTextureStageState( item.id, item.type, newState );

            currentState = newState;
        }
    }

    // No more texture states to process.
    *(unsigned int*)0x00C9A5F0 = 0;
}

/*=========================================================
    RwD3D9ValidateDeviceStates

    Purpose:
        Makes sure that the RenderWare states match the states
        that they are meant to have. This function should be
        used to debug MTA and GTA:SA renderstate interactions.

        Define DEBUG_DEVICE_STATE_INTEGRITY to use breakpoint
        on device state integrity violation. These violations
        are caused by MTA and have to be debugged.
=========================================================*/
void __cdecl RwD3D9ValidateDeviceStates( void )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    // Validate renderstates.
    for ( unsigned int n = 0; n < 210; n++ )
    {
        D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)n;

        deviceStateValue& currentState = GetNativeCurrentRenderState( type );

        DWORD realCurrent = 0xFFFFFFFF;
        if ( renderDevice->GetRenderState( type, &realCurrent ) == D3D_OK )
        {
            if ( currentState != realCurrent )
            {
#ifdef DEBUG_DEVICE_STATE_INTEGRITY
                __asm int 3
#endif //DEBUG_DEVICE_STATE_INTEGRITY

                currentState = realCurrent;
            }
        }
    }

    // Validate texture stage states.
    for ( unsigned int n = 0; n < MAX_SAMPLERS; n++ )
    {
        for ( unsigned int stageType = 0; stageType < 33; stageType++ )
        {
            D3DTEXTURESTAGESTATETYPE type = (D3DTEXTURESTAGESTATETYPE)stageType;

            deviceStateValue& currentState = GetNativeCurrentTextureStageState( n, type );

            DWORD realCurrent = 0xFFFFFFFF;
            if ( renderDevice->GetTextureStageState( n, type, &realCurrent ) == D3D_OK )
            {
                if ( currentState != realCurrent )
                {
#ifdef DEBUG_DEVICE_STATE_INTEGRITY
                    __asm int 3
#endif //DEBUG_DEVICE_STATE_INTEGRITY

                    currentState = realCurrent;
                }
            }
        }
    }
}

/*=========================================================
    RwD3D9SetSamplerState

    Arguments:
        samplerId - index of the sampler to access
        stateType - type identifier of the parameter to update
        value - parameter to pass to the sampler
    Purpose:
        Updates a sampler parameter of the specified sampler.
        The change is directly updated to the GPU if it is
        changed.
    Binary offsets:
        (1.0 US): 0x007FC3C0
        (1.0 EU): 0x007FC400
=========================================================*/
typedef deviceStateValue samplerStates[14];

inline deviceStateValue& GetNativeCurrentSamplerStateValue( DWORD samplerId, D3DSAMPLERSTATETYPE stateType )
{
    assume( samplerId < MAX_SAMPLERS );

    return ((samplerStates*)0x00C98938)[samplerId][stateType];
}

void __cdecl RwD3D9SetSamplerState( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD value )
{
    deviceStateValue& currentState = GetNativeCurrentSamplerStateValue( samplerId, stateType );

    if ( currentState != value )
    {
        currentState = value;

        GetRenderDevice()->SetSamplerState( samplerId, stateType, value );
    }
}

/*=========================================================
    RwD3D9GetSamplerState

    Arguments:
        samplerId - index of the sampler to access
        stateType - type identifier of the parameter to update
        value - ptr to save the current parameter at
    Purpose:
        Retrieves the current sampler parameter from the
        specified sampler. The implementation assumes that its
        sampler states are active, so it returns data from its
        native table.
    Binary offsets:
        (1.0 US): 0x007FC400
        (1.0 EU): 0x007FC440
=========================================================*/
void __cdecl RwD3D9GetSamplerState( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD& valueOut )
{
    valueOut = GetNativeCurrentSamplerStateValue( samplerId, stateType );
}

/*=========================================================
    RwD3D9SetMaterial

    Arguments:
        material - device material data pointer
    Purpose:
        Sets the current device material for rendering. The
        implementation currently only supports one material.
    Binary offsets:
        (1.0 US): 0x007FC430
        (1.0 EU): 0x007FC470
=========================================================*/
inline D3DMATERIAL9& GetNativeCurrentMaterial( void )
{
    return *(D3DMATERIAL9*)0x00C98AF8;
}

void __cdecl RwD3D9SetMaterial( const D3DMATERIAL9& material )
{
    D3DMATERIAL9& currentMaterial = GetNativeCurrentMaterial();
    
    if ( memcmp( &currentMaterial, &material, sizeof( D3DMATERIAL9 ) ) != 0 )
    {
        currentMaterial = material;

        *(unsigned int*)0x00C9A5F4 = 0;

        GetRenderDevice()->SetMaterial( &material );
    }
}

/*=========================================================
    RwD3D9SetStreams

    Arguments:
        streamInfo - descriptor of stream to set as current
        useOffset - boolean whether to use offseting into
                    the stream buffers
    Purpose:
        Updates the Direct3D 9 device vertex stream buffers.
        This function caches the current stream buffers so that
        unnecessary updates to the GPU are avoided.
    Binary offsets:
        (1.0 US): 0x007FA090
        (1.0 EU): 0x007FA0D0
=========================================================*/
inline RwD3D9Streams& GetStreamsInfo( unsigned int index )
{
    assert( index < 2 );

    return ((RwD3D9Streams*)0x00C97BD8)[index];
}

void __cdecl RwD3D9SetStreams( RwD3D9Streams& streams, int useOffset )
{
    IDirect3DDevice9 *renderDevice = GetRenderDevice_Native();

    RwD3D9Streams& currentStreams = GetStreamsInfo( 0 );

    // Loop through all stream interfaces (there can be maximally two)
    for ( unsigned int n = 0; n < 2; n++ )
    {
        RwD3D9StreamInfo& info = streams.streams[n];
        RwD3D9StreamInfo& current = currentStreams.streams[n];

        // The caller decides whether we use byte offsets in this stream.
        size_t streamOffset = ( useOffset ) ? info.m_offset : 0;

        // Decide whether the stream interface needs updating at all.
        if ( IDirect3DVertexBuffer9 *vertexBuf = info.m_vertexBuf )
        {
            if ( current.m_vertexBuf != vertexBuf ||
                 current.m_offset != streamOffset ||
                 current.m_stride != info.m_stride )
            {
                current.m_vertexBuf = vertexBuf;
                current.m_offset = streamOffset;
                current.m_stride = info.m_stride;

                renderDevice->SetStreamSource( n, vertexBuf, streamOffset, info.m_stride );
            }
        }
        else
        {
            // Clear the vertex stream if it has been unloaded in the given streams info.
            if ( current.m_vertexBuf )
            {
                current.m_vertexBuf = NULL;
                current.m_offset = 0;
                current.m_stride = 0;

                renderDevice->SetStreamSource( n, NULL, 0, 0 );
            }
        }
    }
}

/*=========================================================
    RwD3D9SetCurrentPixelShader

    Arguments:
        pixelShader - device fragment shader pointer
    Purpose:
        Sets the active fragment shader that is used for
        rendering video data.
    Binary offsets:
        (1.0 US): 0x007F9FF0
        (1.0 EU): 0x007FA030
=========================================================*/
inline IDirect3DPixelShader9*& GetCurrentPixelShader( void )
{
    return *(IDirect3DPixelShader9**)0x008E244C;
}

void __cdecl RwD3D9SetCurrentPixelShader( IDirect3DPixelShader9 *pixelShader )
{
    IDirect3DPixelShader9*& currentPixelShader = GetCurrentPixelShader();

    if ( pixelShader != currentPixelShader )
    {
        bool success = GetRenderDevice_Native()->SetPixelShader( pixelShader ) >= 0;

        currentPixelShader = ( success ) ? pixelShader : (IDirect3DPixelShader9*)-1;
    }
}

inline void __cdecl RwD3D9UnsetPixelShader( void )
{
    IDirect3DPixelShader9*& currentPixelShader = GetCurrentPixelShader();

    if ( currentPixelShader )
    {
        currentPixelShader = NULL;

        GetRenderDevice_Native()->SetPixelShader( NULL );
    }
}

/*=========================================================
    RwD3D9SetCurrentVertexShader

    Arguments:
        vertexShader - device vertex shader pointer
    Purpose:
        Sets the active vertex shader that is used for
        blending video data.
    Binary offsets:
        (1.0 US): 0x007F9FB0
        (1.0 EU): 0x007F9FF0
=========================================================*/
inline IDirect3DVertexShader9*& GetCurrentVertexShader( void )
{
    return *(IDirect3DVertexShader9**)0x008E2448;
}

inline void __cdecl RwD3D9SetCurrentVertexShader( IDirect3DVertexShader9 *vertexShader )
{
    IDirect3DVertexShader9*& currentVertexShader = GetCurrentVertexShader();

    if ( currentVertexShader != vertexShader )
    {
        bool success = GetRenderDevice_Native()->SetVertexShader( vertexShader ) >= 0;

        currentVertexShader = ( success ) ? vertexShader : (IDirect3DVertexShader9*)-1;
    }
}

/*=========================================================
    RwD3D9SetCurrentIndexBuffer

    Arguments:
        indexBuf - device index buffer pointer
    Purpose:
        Sets the index buffer that holds pointers to vertice
        data inside of the vertex streams.
    Binary offsets:
        (1.0 US): 0x007FA1C0
        (1.0 EU): 0x007FA200
=========================================================*/
inline IDirect3DIndexBuffer9*& GetCurrentIndexBuffer( void )
{
    return *(IDirect3DIndexBuffer9**)0x008E2450;
}

inline void __cdecl RwD3D9SetCurrentIndexBuffer( IDirect3DIndexBuffer9 *indexBuf )
{
    IDirect3DIndexBuffer9*& curIndexBuf = GetCurrentIndexBuffer();

    if ( curIndexBuf != indexBuf )
    {
        curIndexBuf = indexBuf;

        GetRenderDevice_Native()->SetIndices( indexBuf );
    }
}

/*=========================================================
    RwD3D9SetCurrentVertexDeclaration

    Arguments:
        vertexDecl - device vertex declaration pointer
    Purpose:
        Sets the active device vertex declaration. It specifies
        how the vertex data is formated.
    Binary offsets:
        (1.0 US): 0x007F9F70
        (1.0 EU): 0x007F9FB0
=========================================================*/
inline IDirect3DVertexDeclaration9*& GetCurrentVertexDeclaration( void )
{
    return *(IDirect3DVertexDeclaration9**)0x008E2444;
}

inline DWORD& GetCurrentFixedVertexFunction( void )
{
    return *(DWORD*)0x008E2440;
}

inline void RwD3D9SetCurrentVertexDeclaration( IDirect3DVertexDeclaration9 *vertexDecl )
{
    IDirect3DVertexDeclaration9*& curVertexDecl = GetCurrentVertexDeclaration();

    if ( curVertexDecl != vertexDecl )
    {
        GetCurrentFixedVertexFunction() = -1;

        bool success = GetRenderDevice_Native()->SetVertexDeclaration( vertexDecl ) >= 0;

        curVertexDecl = ( success ) ? vertexDecl : (IDirect3DVertexDeclaration9*)-1;
    }
}

inline void __cdecl RpD3D9DrawIndexedPrimitive( D3DPRIMITIVETYPE primitiveType, INT baseVertexIndex, UINT minVertexIndex, UINT numVertice, UINT startIndex, UINT primCount )
{
    // Update the render and texture states before drawing.
    RwD3D9ApplyDeviceStates();

    GetRenderDevice_Native()->DrawIndexedPrimitive( primitiveType, baseVertexIndex, minVertexIndex, numVertice, startIndex, primCount );
}

inline void __cdecl RpD3D9DrawPrimitive( D3DPRIMITIVETYPE primitiveType, UINT startVertex, UINT numPrimitives )
{
    // Update the render and texture states before drawing.
    RwD3D9ApplyDeviceStates();

    GetRenderDevice_Native()->DrawPrimitive( primitiveType, startVertex, numPrimitives );
}

/*=========================================================
    RwD3D9EnableClippingIfNeeded

    Arguments:
        renderObject - the RenderWare object to clip
        renderType - type of the RenderWare object
    Purpose:
        Decides whether the given RenderWare object can be clipped.
        Polygons are clipped when they are outside of the screen.
        A simple frustum check decides this.
    Binary offsets:
        (1.0 US): 0x007F9F70
        (1.0 EU): 0x007F9FB0
=========================================================*/
inline void RwD3D9EnableClippingIfNeeded( RwObject *renderObject, eRwType renderType )
{
    bool isObjectVisible = false;
    RwCamera *currentCam = RenderWare::GetInterface()->m_renderCam;

    if ( renderType == RW_ATOMIC )
    {
        isObjectVisible = RwD3D9CameraIsSphereFullyInsideFrustum( currentCam, ((RpAtomic*)renderObject)->GetWorldBoundingSphere() );
    }
    else
    {
        // Unknown type rendering!
        isObjectVisible = RwD3D9CameraIsBBoxFullyInsideFrustum( currentCam, ((CVector*) ((char*)renderObject + 96) ) );
    }

    // Clip polygons if the object is not visible.
    // todo: fix this; it bugs for old GPUs!
    //HOOK_RwD3D9SetRenderState( D3DRS_CLIPPING, !isObjectVisible );
    HOOK_RwD3D9SetRenderState( D3DRS_CLIPPING, true );
}

/*=========================================================
    HOOK_DefaultAtomicRenderingCallback

    Purpose:
        Sets up the atomic pipeline, prepares the device information
        and initializes the material, atomic and specular material
        custom pools.
    Binary offsets:
        (1.0 US): 0x00756DF0
        (1.0 EU): 0x00756E40
=========================================================*/
struct GenericVideoPassRender
{
    __forceinline GenericVideoPassRender( unsigned int& _renderFlags ) : renderFlags( _renderFlags )
    {
        // Store some render states.
        HOOK_RwD3D9GetRenderState( D3DRS_DITHERENABLE, ditheringEnabled );
        HOOK_RwD3D9GetRenderState( D3DRS_SHADEMODE, shadeMode );

        // Change render states.
        RwD3D9RenderStateSetVertexAlphaEnabled( false );

        HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );
        HOOK_RwD3D9SetRenderState( D3DRS_DITHERENABLE, FALSE );
        HOOK_RwD3D9SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

        RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

        RwD3D9SetTexture( 0, NULL );

        // Keep track of alpha modulation status.
        hasAlphaModulation = false;
    }

    __forceinline ~GenericVideoPassRender( void )
    {
        // Restore some render states.
        HOOK_RwD3D9SetRenderState( D3DRS_SHADEMODE, shadeMode );
        HOOK_RwD3D9SetRenderState( D3DRS_DITHERENABLE, ditheringEnabled );
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass )
    {
        RpMaterial *curMat = rtPass->m_useMaterial;

        if ( IS_FLAG( renderFlags, 0x84 ) && RwTextureHasAlpha( curMat->texture ) )
        {
            RwD3D9SetTexture( curMat->texture, 0 );

            if ( !hasAlphaModulation )
            {
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );

                hasAlphaModulation = true;
            }
        }
        else
        {
            if ( hasAlphaModulation )
            {
                RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );

                hasAlphaModulation = false;

                RwD3D9SetTexture( NULL, 0 );
            }
        }
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

    unsigned int& renderFlags;
    DWORD ditheringEnabled;
    DWORD shadeMode;
    bool hasAlphaModulation;
};

struct AlphaTexturedVideoPassRender
{
    __forceinline AlphaTexturedVideoPassRender( unsigned int& _renderFlags, DWORD& _lightValue ) : renderFlags( _renderFlags ), lightValue( _lightValue )
    {
        // Store current vertex alpha status.
        hasVertexAlpha = RwD3D9RenderStateIsVertexAlphaEnabled() ? true : false;

        // Store the current state flags
        // These are used for optimization purposes.
        // The initial value should be a flag that is not used and invalid.
        curStateFlags = 0x80000000;
    }

    __forceinline void TerminateSecondStage( void )
    {
        if ( curStateFlags == 0x03 )
        {
            RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
            RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
        }
    }

    __forceinline ~AlphaTexturedVideoPassRender( void )
    {
        TerminateSecondStage();
    }

    __forceinline void OnRenderPass( RwRenderPass *rtPass )
    {
        RpMaterial *curMat = rtPass->m_useMaterial;

        // Update device vertex alpha status.
        // Did Rockstar even know how to fully utilize RenderWare?
        bool needsVertexAlpha = RwD3D9IsVertexAlphaRenderingRequiredEx( rtPass, curMat );

        if ( needsVertexAlpha != hasVertexAlpha )
        {
            hasVertexAlpha = needsVertexAlpha;

            RwD3D9RenderStateSetVertexAlphaEnabled( needsVertexAlpha );
        }

        // Update surface properties.
        unsigned int stateFlags = 0;

        if ( !RwD3D9UpdateRenderPassSurfaceProperties( rtPass, lightValue, curMat, renderFlags ) && IS_ANY_FLAG( renderFlags, 0x40 ) && curMat->color != 0xFFFFFFFF )
        {
            stateFlags |= 0x02;

            HOOK_RwD3D9SetRenderState( D3DRS_TEXTUREFACTOR, curMat->color.ToD3DColor() );
        }

        // Update texture status.
        RwTexture *matTex = curMat->texture;

        if ( matTex && IS_ANY_FLAG( renderFlags, 0x84 ) )
        {
            RwD3D9SetTexture( matTex, 0 );

            stateFlags |= 0x01;
        }
        else
            RwD3D9SetTexture( NULL, 0 );

        // Change render states depending on the current status that we set above.
        if ( stateFlags != curStateFlags )
        {
            bool requireSecondStage = false;

            if ( IS_ANY_FLAG( stateFlags, 0x01 ) )
            {
                if ( !IS_ANY_FLAG( curStateFlags, 0x01 ) )
                {
                    RwD3D9SetTexturedRenderStates( true );
                }

                if ( IS_ANY_FLAG( stateFlags, 0x02 ) )
                {
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_CURRENT );
                    RwD3D9SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT );
                    RwD3D9SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );

                    requireSecondStage = true;
                }
            }
            else
            {
                if ( IS_ANY_FLAG( stateFlags, 0x02 ) )
                {
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );

                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
                    RwD3D9SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
                }
                else
                {
                    RwD3D9SetTexturedRenderStates( false );
                }
            }

            if ( !requireSecondStage )
                TerminateSecondStage();

            // We changed state.
            curStateFlags = stateFlags;
        }
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

    unsigned int& renderFlags;
    DWORD& lightValue;
    bool hasVertexAlpha;
    unsigned int curStateFlags;
};

// todo: is there even a "default" rendering callback?
// after all, every pipeline is stand-alone.
void __cdecl HOOK_DefaultAtomicRenderingCallback( RwRenderCallbackTraverse *rtnative, RwObject *renderObject, eRwType renderType, unsigned int renderFlags )
{
    if ( !RenderCallbacks::IsRenderingEnabled() )
        return;

    RwRenderCallbackTraverseImpl *rtinfo = &rtnative->m_impl;

    // Make sure we are not rendering using any pixel shader.
    RwD3D9UnsetPixelShader();

    // Set clipping status.
    RwD3D9EnableClippingIfNeeded( renderObject, renderType );

    // Update the current index buffer (only if set)
    if ( IDirect3DIndexBuffer9 *indexBuf = rtinfo->m_indexBuffer )
        RwD3D9SetCurrentIndexBuffer( indexBuf );

    // Update the vertex streams.
    RwD3D9SetStreams( rtinfo->m_vertexStreams, rtinfo->m_useOffsets );

    // Set the vertex type.
    RwD3D9SetCurrentVertexDeclaration( rtinfo->m_vertexDecl );

    // Do we want vertex alpha?
    // Kind of an important flag here.
    DWORD lightValue;

    bool enableAlpha = RwD3D9IsAlphaRenderingRequired( renderFlags, lightValue );

    if ( enableAlpha )
    {
        _RenderVideoDataGeneric( rtinfo, GenericVideoPassRender( renderFlags ) );
    }
    else
    {
        _RenderVideoDataGeneric( rtinfo, AlphaTexturedVideoPassRender( renderFlags, lightValue ) );
    }
}

inline RwColor& GetCurrentSurfaceColor( void )
{
    return *(RwColor*)0x00C9A5F8;
}

void RwD3D9InitializeCurrentStates( void )
{
    // The_GTA: fixed current-ness of device states by actually querying the device.
    IDirect3DDevice9 *renderDevice = GetRenderDevice();

    // Set up the current renderstates.
    for ( unsigned int n = 0; n < 210; n++ )
    {
        D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)n;

        deviceStateValue& currentRenderState = GetNativeCurrentRenderState( type );

        renderDevice->GetRenderState( type, &currentRenderState.value );
    }

    // Initialize the renderstates maintenance array based on the current render states.
    for ( unsigned int n = 0; n < 210; n++ )
    {
        D3DRENDERSTATETYPE type = (D3DRENDERSTATETYPE)n;

        _renderStateDesc& rsDesc = GetNativeRenderStateDesc( type );

        rsDesc.value = GetNativeCurrentRenderState( type );
        rsDesc.addedToQuery = false;
    }

    // Reset the renderstate query.
    *(unsigned int*)0x00C9A5EC = 0;

    // Set up the current TSS values.
    for ( unsigned int stageId = 0; stageId < MAX_SAMPLERS; stageId++ )
    {
        for ( unsigned int stateId = 0; stateId < 33; stateId++ )
        {
            D3DTEXTURESTAGESTATETYPE type = (D3DTEXTURESTAGESTATETYPE)stateId;

            deviceStateValue& currentValue = GetNativeCurrentTextureStageState( stageId, type );

            renderDevice->GetTextureStageState( stageId, type, &currentValue.value );
        }
    }

    // Initialize the TSS maintenance array based on the current texture stage states.
    for ( unsigned int stageId = 0; stageId < MAX_SAMPLERS; stageId++ )
    {
        for ( unsigned int stateId = 0; stateId < 33; stateId++ )
        {
            D3DTEXTURESTAGESTATETYPE type = (D3DTEXTURESTAGESTATETYPE)stateId;

            _texStageStateDesc& tssDesc = GetNativeTextureStageStateDesc( stageId, type );

            tssDesc.value = GetNativeCurrentTextureStageState( stageId, type );
            tssDesc.isQueried = false;
        }
    }

    // Reset TSS query.
    *(unsigned int*)0x00C9A5F0 = 0;

    // Set up the sampler states array.
    for ( unsigned int samplerIndex = 0; samplerIndex < MAX_SAMPLERS; samplerIndex++ )
    {
        for ( unsigned int stateType = 0; stateType < 14; stateType++ )
        {
            D3DSAMPLERSTATETYPE type = (D3DSAMPLERSTATETYPE)stateType;

            deviceStateValue& samplerValue = GetNativeCurrentSamplerStateValue( samplerIndex, type );

            renderDevice->GetSamplerState( samplerIndex, type, &samplerValue.value );
        }
    }

    // Initialize the current material.
    renderDevice->GetMaterial( &GetNativeCurrentMaterial() );

    // Initialize surface environment.
    *(float*)0x00C9A600 = 0.0f;
    *(float*)0x00C9A5D8 = 0.0f;
    *(float*)0x00C9A5DC = 0.0f;
    *(float*)0x00C9A5E0 = 0.0f;

    *(unsigned int*)0x00C9A5F4 = 0;
    GetCurrentSurfaceColor() = 0x00000000;
}

/*=========================================================
    HOOK_InitDeviceSystem

    Purpose:
        Sets up the atomic pipeline, prepares the device information
        and initializes the material, atomic and specular material
        custom pools.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005DA020
=========================================================*/
D3DLIGHT9& RenderWare::GetSpecialVehicleLight( void )
{
    return *(D3DLIGHT9*)0x00C02CB0;
}

static int __cdecl HOOK_InitDeviceSystem( void )
{
    RwPipeline *defPipeline;

    // Pah, might analyze those chunks way later :P
    // I am here because of the pools!
    __asm
    {
        // Initialize pipelines
        mov eax,0x005D9F80
        call eax
        mov defPipeline,eax
    }

    if ( !defPipeline )
        return 0;

    *(RwPipeline**)0x00C02D24 = defPipeline;

    int iReturn;    // did you know that "iRet" is a reserved value in VC++ ASM?

    __asm
    {
        // Initialize device information
        mov eax,0x005D8980
        call eax
        mov iReturn,eax
    }

    if ( !iReturn )
        return 0;

    // Initialize the special vehicle light interface.
    memset( &RenderWare::GetSpecialVehicleLight(), 0, sizeof(D3DLIGHT9) );

    // Finally initialize the pools!
    RenderWare::GetEnvMapMaterialPool() = new CEnvMapMaterialPool;
    RenderWare::GetEnvMapAtomicPool() = new CEnvMapAtomicPool;
    RenderWare::GetSpecMapMaterialPool() = new CSpecMapMaterialPool;
    return 1;
}

void RenderWarePipeline_Init( void )
{
    // Depending on game version
    switch( pGame->GetGameVersion() )
    {
    case VERSION_EU_10:
        HookInstall( 0x007FC310, (DWORD)HOOK_RwD3D9SetRenderState, 5 );
        HookInstall( 0x007FC360, (DWORD)HOOK_RwD3D9GetRenderState, 5 );
        HookInstall( 0x007FC380, (DWORD)RwD3D9SetTextureStageState, 5 );
        HookInstall( 0x007FC3E0, (DWORD)RwD3D9GetTextureStageState, 5 );
        HookInstall( 0x007FC240, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        HookInstall( 0x00756E40, (DWORD)HOOK_DefaultAtomicRenderingCallback, 5 );
        HookInstall( 0x007FCB00, (DWORD)RwD3D9InitializeDeviceStates, 5 );
        HookInstall( 0x007FD140, (DWORD)RwD3D9ResetDeviceStates, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FC2D0, (DWORD)HOOK_RwD3D9SetRenderState, 5 );
        HookInstall( 0x007FC320, (DWORD)HOOK_RwD3D9GetRenderState, 5 );
        HookInstall( 0x007FC340, (DWORD)RwD3D9SetTextureStageState, 5 );
        HookInstall( 0x007FC3A0, (DWORD)RwD3D9GetTextureStageState, 5 );
        HookInstall( 0x007FC200, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        HookInstall( 0x00756DF0, (DWORD)HOOK_DefaultAtomicRenderingCallback, 5 );
        HookInstall( 0x007FCAC0, (DWORD)RwD3D9InitializeDeviceStates, 5 );
        HookInstall( 0x007FD100, (DWORD)RwD3D9ResetDeviceStates, 5 );
        break;
    }

    HookInstall( 0x005DA020, (DWORD)HOOK_InitDeviceSystem, 5 );
}

void RenderWarePipeline_Shutdown( void )
{
}