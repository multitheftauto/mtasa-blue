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

// Nasty pools which limit rendering.
CEnvMapMaterialPool **ppEnvMapMaterialPool = (CEnvMapMaterialPool**)0x00C02D28;
CEnvMapAtomicPool **ppEnvMapAtomicPool = (CEnvMapAtomicPool**)0x00C02D2C;
CSpecMapMaterialPool **ppSpecMapMaterialPool = (CSpecMapMaterialPool**)0x00C02D30;

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
    DWORD value;
    int addedToQuery;
};

void __cdecl RwD3D9SetRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
    _renderStateDesc& desc = ((_renderStateDesc*)0x00C991D0)[type];

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

    DWORD value;
    bool isForced;
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
static void __cdecl HOOK_RwD3D9SetRenderState( D3DRENDERSTATETYPE type, DWORD value )
{
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
    value = ((_renderStateDesc*)0x00C991D0)[type].value;
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
static void __cdecl HOOK_RwD3D9GetRenderState( D3DRENDERSTATETYPE type, DWORD& value )
{
    __asm push edx

    _internalRenderStateDesc& desc = _intRenderStates[type];

    if ( desc.isForced )
        value = desc.value;
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

    // Revert to the GTA:SA version
    RwD3D9SetRenderState( type, desc.value );
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
struct _texStageStateQuery
{
    DWORD id;
    D3DTEXTURESTAGESTATETYPE type;
};

struct _texStageStateDesc
{
    DWORD value;
    int isQueried;
};

void __cdecl RwD3D9ApplyDeviceStates( void )
{
    IDirect3DDevice9 *renderDevice = g_pCore->GetGraphics()->GetDevice();
    
    // Apply RenderStates
    unsigned int renderStates = *(unsigned int*)0x00C9A5EC;

    for ( unsigned int n = 0; n < renderStates; n++ )
    {
        D3DRENDERSTATETYPE type = ((D3DRENDERSTATETYPE*)0x00C98B40)[n];
        _renderStateDesc& desc = ((_renderStateDesc*)0x00C991D0)[type];
        
        DWORD newState = desc.value;
        DWORD& currentState = ((DWORD*)0x00C98E88)[type];

        // We processed this entry
        desc.addedToQuery = false;

        if ( currentState != newState )
        {
            renderDevice->SetRenderState( type, newState );

            currentState = newState;
        }
    }

    // Processed, so zero out
    *(unsigned int*)0x00C9A5EC = 0;

    unsigned int textureStates = *(unsigned int*)0x00C9A5F0;

    // Apply texture stage states
    for ( unsigned int n = 0; n < textureStates; n++ )
    {
        _texStageStateQuery& item = ((_texStageStateQuery*)0x00C99C80)[n];
        unsigned int idx = item.id * 32 + item.id + item.type;
        
        _texStageStateDesc& desc = ((_texStageStateDesc*)0x00C980F8)[idx];

        desc.isQueried = false;

        DWORD newState = desc.value;
        DWORD& currentState = ((DWORD*)0x00C99860)[idx];

        if ( newState != currentState )
        {
            // Update the texture stage
            renderDevice->SetTextureStageState( item.id, item.type, newState );

            currentState = newState;
        }
    }

    *(unsigned int*)0x00C9A5F0 = 0;
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

    // Set some NULL pointers
    memset( (void*)0x00C02CB0, 0, sizeof(void*) * 26 );

    // Finally initialize the pools!
    *ppEnvMapMaterialPool = new CEnvMapMaterialPool;
    *ppEnvMapAtomicPool = new CEnvMapAtomicPool;
    *ppSpecMapMaterialPool = new CSpecMapMaterialPool;
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
        HookInstall( 0x007FC240, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        break;
    case VERSION_US_10:
        HookInstall( 0x007FC2D0, (DWORD)HOOK_RwD3D9SetRenderState, 5 );
        HookInstall( 0x007FC320, (DWORD)HOOK_RwD3D9GetRenderState, 5 );
        HookInstall( 0x007FC200, (DWORD)RwD3D9ApplyDeviceStates, 5 );
        break;
    }

    HookInstall( 0x005DA020, (DWORD)HOOK_InitDeviceSystem, 5 );
}

void RenderWarePipeline_Shutdown( void )
{
}