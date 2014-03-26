/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CRenderWareSA.pipeline.h
*  PURPOSE:     RenderWare pipeline (OS implementation) management
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _RENDERWARE_PIPELINES_
#define _RENDERWARE_PIPELINES_

// D3D9 pipeline functions (use HOOK_* instead)
void __cdecl RwD3D9SetRenderState       ( D3DRENDERSTATETYPE type, DWORD value );
void __cdecl RwD3D9GetRenderState       ( D3DRENDERSTATETYPE type, DWORD& value );
void __cdecl RwD3D9ApplyDeviceStates    ( void );
void __cdecl RwD3D9ValidateDeviceStates ( void );

void __cdecl RwD3D9SetSamplerState      ( DWORD samplerId, D3DSAMPLERSTATETYPE stateType, DWORD value );

void __cdecl RwD3D9SetTextureStageState ( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD value );
void __cdecl RwD3D9GetTextureStageState ( DWORD stageId, D3DTEXTURESTAGESTATETYPE stateType, DWORD& value );

// GTA:SA native functions for compatibility when reversing.
void __cdecl HOOK_RwD3D9SetRenderState  ( D3DRENDERSTATETYPE type, DWORD value );
void __cdecl HOOK_RwD3D9GetRenderState  ( D3DRENDERSTATETYPE type, DWORD& value );

// MTA extensions
void RwD3D9ForceRenderState             ( D3DRENDERSTATETYPE type, DWORD value );
void RwD3D9FreeRenderState              ( D3DRENDERSTATETYPE type );
void RwD3D9FreeRenderStates             ( void );

void RwD3D9InitializeCurrentStates      ( void );

typedef CPool <CEnvMapMaterialSA, 16000> CEnvMapMaterialPool;
typedef CPool <CEnvMapAtomicSA, 4000> CEnvMapAtomicPool;
typedef CPool <CSpecMapMaterialSA, 16000> CSpecMapMaterialPool;

namespace RenderWare
{
    // Nasty pools which limit rendering.
    inline CEnvMapMaterialPool*&    GetEnvMapMaterialPool   ( void )        { return *(CEnvMapMaterialPool**)0x00C02D28; }
    inline CEnvMapAtomicPool*&      GetEnvMapAtomicPool     ( void )        { return *(CEnvMapAtomicPool**)0x00C02D2C; }
    inline CSpecMapMaterialPool*&   GetSpecMapMaterialPool  ( void )        { return *(CSpecMapMaterialPool**)0x00C02D30; }

    // Global rendering utilities.
    D3DLIGHT9&  GetSpecialVehicleLight( void );
};

// Stack-based anonymous RenderState management
//todo: fix this (removed inlined RS changes from native code)
struct RwRenderStateLock
{
    struct _rsLockDesc
    {
        _rsLockDesc( void )
        {
            refCount = 0;
        }

        unsigned int refCount;
    };

    static _rsLockDesc _rsLockInfo[210];

    /*=========================================================
        RwRenderStateLock::constructor (MTA extension)

        Purpose:
            Sets a RenderState for RenderWare drawing operations
            and locks them from changing by the GTA:SA engine
            until the reference count reaches zero.
    =========================================================*/
    AINLINE RwRenderStateLock( D3DRENDERSTATETYPE type, DWORD value )
    {
        m_type = type;

        // Increase the reference count
        if ( ++_rsLockInfo[type].refCount > 1 )
        {
            // Only have to store the previous RenderState value now
            HOOK_RwD3D9GetRenderState( type, m_prevValue );
        }

        // Apply our RenderState and save the original GTA:SA one.
        RwD3D9ForceRenderState( type, value );
    }

    /*=========================================================
        RwRenderStateLock::destructor (MTA extension)

        Purpose:
            Restore the status of said RenderState to the one previous
            to creation of this lock. If reference count reaches
            zero, 
    =========================================================*/
    AINLINE ~RwRenderStateLock( void )
    {
        // Free if not referenced anymore
        if ( --_rsLockInfo[m_type].refCount == 0 )
            RwD3D9FreeRenderState( m_type );
        else
            RwD3D9ForceRenderState( m_type, m_prevValue );
    }

    D3DRENDERSTATETYPE  m_type;
    DWORD               m_prevValue;
};

// Structure for fast stack based struct/class allocation.
template <class allocType, int max>
class StaticAllocator
{
    char mem[sizeof(allocType) * max];
    unsigned int count;

public:
    AINLINE StaticAllocator( void )
    {
        count = 0;
    }
    
    AINLINE void* Allocate( void )
    {
        assume( count < max );

        void *outMem = mem + (sizeof(allocType) * count++);

        assume( outMem != NULL );

        return outMem;
    }

    // Hack function; only use if performance critical code is used.
    AINLINE void Pop( void )
    {
        count--;
    }
};

void RenderWarePipeline_Init( void );
void RenderWarePipeline_Shutdown( void );

#endif //_RENDERWARE_PIPELINES_