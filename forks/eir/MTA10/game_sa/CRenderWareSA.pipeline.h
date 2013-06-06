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

// D3D9 pipeline functions
void __cdecl RwD3D9SetRenderState       ( D3DRENDERSTATETYPE type, DWORD value );
void __cdecl RwD3D9GetRenderState       ( D3DRENDERSTATETYPE type, DWORD& value );
void __cdecl RwD3D9ApplyDeviceStates    ( void );

// MTA extensions
void RwD3D9ForceRenderState             ( D3DRENDERSTATETYPE type, DWORD value );
void RwD3D9FreeRenderState              ( D3DRENDERSTATETYPE type );
void RwD3D9FreeRenderStates             ( void );

// Stack-based anonymous RenderState management
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
    __forceinline RwRenderStateLock( D3DRENDERSTATETYPE type, DWORD value )
    {
        m_type = type;

        // Increase the reference count
        if ( ++_rsLockInfo[type].refCount > 1 )
        {
            // Only have to store the previous RenderState value now
            RwD3D9GetRenderState( type, m_prevValue );
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
    __forceinline ~RwRenderStateLock( void )
    {
        // Free if not referenced anymore
        if ( --_rsLockInfo[m_type].refCount == 0 )
            RwD3D9FreeRenderState( m_type );
        else
            RwD3D9SetRenderState( m_type, m_prevValue );
    }

    D3DRENDERSTATETYPE  m_type;
    DWORD               m_prevValue;
};

template <class allocType, int max>
class StaticAllocator
{
    char mem[sizeof(allocType) * max];
    unsigned int count;

public:
    __forceinline StaticAllocator( void )
    {
        count = 0;
    }
    
    void* Allocate( void )
    {
        return mem + (sizeof(allocType) * count++);
    }
};

void RenderWarePipeline_Init( void );
void RenderWarePipeline_Shutdown( void );

#endif //_RENDERWARE_PIPELINES_