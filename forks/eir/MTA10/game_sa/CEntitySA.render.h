/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CEntitySA.render.h
*  PURPOSE:     Header file for entity render framework
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _ENTITY_RENDER_
#define _ENTITY_RENDER_

// Common utility for grouped z-layered rendering
template <typename entryType>
class CRenderChainInterface
{
public:
    typedef entryType depthLevel;

    class renderChain
    {
    public:
        depthLevel              m_entry;
        renderChain*            next;
        renderChain*            prev;
    };

    /*=========================================================
        CRenderChainInterface::constructor

        Arguments:
            max - maximum number of rendering entries available
        Purpose:
            Constructs this rendering chain by allocating the maximum
            number of entries which this chain can hold.
        Note:
            Multiple constructors have been inlined into
            HOOK_InitRenderChains. This sorted list is used in
            CStreaming::Init.
    =========================================================*/
    __forceinline CRenderChainInterface( unsigned int max )
    {
        // Allocate all entries
        m_stack = new renderChain[max];

        // Initialize the list
        m_root.prev = &m_rootLast;
        m_rootLast.next = &m_root;
        m_renderStack.prev = &m_renderLast;
        m_renderLast.next = &m_renderStack;

        // Setup height properties
        m_rootLast.m_entry.InitLast();
        m_root.m_entry.InitFirst();

        while ( max-- )
        {
            renderChain& iter = m_stack[max];

            LIST_APPEND( m_renderStack, iter );
        }
    }

    /*=========================================================
        CRenderChainInterface::destructor

        Purpose:
            Deallocates the memory used for this rendering list.
    =========================================================*/
    __forceinline ~CRenderChainInterface( void )
    {
        delete [] m_stack;
    }

    /*=========================================================
        CRenderChainInterface::AllocateNew (MTA extension)

        Purpose:
            Allocates a new heap node into the system. Due to limitations,
            we cannot free this memory. It will be floating around
            and during the lifetime of this class never be wasted.
        Note:
            The_GTA: I made this to fix streaming issues.
    =========================================================*/
    void AllocateNew( void )
    {
        renderChain *newItem = new renderChain;

        if ( !newItem )
            return;

        LIST_APPEND( m_renderStack, *newItem );
    }

    /*=========================================================
        CRenderChainInterface::PushRender

        Arguments:
            level - the description of the new entry
        Purpose:
            Queries to render the specified userdata using a given
            callback.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00733910 (atomicRenderChain)
        Note:
            Apparently the render chains have a pre-allocated amount of
            possible render instances. We should investigate, how this limit
            affects the performance and quality of gameplay.
    =========================================================*/
    __forceinline renderChain* AllocateItem( void )
    {
        renderChain *item;

        return ( ( item = m_renderStack.prev ) == &m_renderLast ) ? NULL : item;
    }

    __forceinline renderChain* PushRenderFirst( depthLevel *level )
    {
        renderChain *progr = AllocateItem();

        if ( !progr )
            return NULL;

        progr->m_entry = *level;
        level->InitFirst();

        LIST_REMOVE( *progr );
        LIST_APPEND( m_root, *progr );
        return progr;
    }

    __forceinline renderChain* PushRenderLast( depthLevel *level )
    {
        renderChain *progr = AllocateItem();

        if ( !progr )
            return NULL;

        progr->m_entry = *level;
        level->InitLast();

        LIST_REMOVE( *progr );
        LIST_INSERT( m_rootLast, *progr );
        return progr;
    }

    renderChain* PushRender( depthLevel *level )
    {
        renderChain *iter = m_root.prev;

        // Scan until we find an appropriate slot
        for ( ; iter != &m_rootLast && iter->m_entry < *level; 
            iter = iter->prev );

        renderChain *progr = AllocateItem();

        if ( !progr )
            return NULL;

        // Update render details
        progr->m_entry = *level;

        LIST_REMOVE( *progr );
        
        iter = iter->next;

        LIST_APPEND( *iter, *progr );
        return progr;
    }

    /*=========================================================
        CRenderChainInterface::RemoveItem (inlined)

        Arguments:
            entry - node in this chain interface
        Purpose:
            Removes the given node from this chain interface.
    =========================================================*/
    inline void RemoveItem( renderChain *entry )
    {
        LIST_REMOVE( *entry );
        LIST_APPEND( m_renderStack, *entry );
    }

    /*=========================================================
        CRenderChainInterface::GetFirstUsed (MTA extension)

        Purpose:
            Returns the first allocated renderChain node in this
            sorted container. Returns NULL if there is none
            allocated.
    =========================================================*/
    inline renderChain* GetFirstUsed( void )
    {
        renderChain *iter = m_rootLast.next;

        return iter == &m_root ? NULL : iter;
    }

    /*=========================================================
        CRenderChainInterface::Execute

        Purpose:
            Executes all items in their order.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00733E90 (vehicleRenderChain)
    =========================================================*/
    void Execute( void )
    {
        for ( renderChain *iter = m_rootLast.next; iter != &m_root; iter = iter->next )
        {
            iter->m_entry.Execute();
        }
    }

    /*=========================================================
        CRenderChainInterface::ExecuteReverse

        Purpose:
            Executes all items in the reverse order
    =========================================================*/
    void ExecuteReverse( void )
    {
        for ( renderChain *iter = m_root.prev; iter != &m_rootLast; iter = iter->prev )
        {
            iter->m_entry.Execute();
        }
    }

    /*=========================================================
        CRenderChainInterface::ExecuteCustom

        Purpose:
            Executes all items in forward order using a template
            object.
    =========================================================*/
    template <typename callbackType>
    inline bool ExecuteCustom( callbackType& cb )
    {
        for ( renderChain *iter = m_rootLast.next, *nextIter = iter->next; iter != &m_root; iter = nextIter, nextIter = iter->next )
        {
            if ( cb.OnEntry( iter->m_entry ) == false )
                return true;
        }
        return false;
    }

    template <typename callbackType>
    inline bool ExecuteCustomReverse( callbackType& cb )
    {
        for ( renderChain *iter = m_root.prev; iter != &m_rootLast; iter = iter->prev )
        {
            if ( cb.OnEntry( iter->m_entry ) == false )
                return true;
        }
        return false;
    }

    /*=========================================================
        CRenderChainInterface::Clear

        Purpose:
            Resets all ordered items
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00733F40 (vehicleRenderChain)
    =========================================================*/
    void Clear( void )
    {
        while ( m_rootLast.next != &m_root )
        {
            RemoveItem( m_rootLast.next );
        }
    }

    /*=========================================================
        CRenderChainInterface::CountActive (MTA extension)

        Purpose:
            Returns the number of sorted list entries that are
            in use by this render chain.
    =========================================================*/
    unsigned int CountActive( void ) const
    {
        unsigned int count = 0;

        for ( renderChain *iter = m_rootLast.next; iter != &m_root; iter = iter->next )
            count++;
        
        return count;
    }

    /*=========================================================
        CRenderChainInterface::CountFree (MTA extension)

        Purpose:
            Returns the number of sorted list entries that can
            be allocated. Returning a non-zero value means that
            you can push entries into this chain.
    =========================================================*/
    unsigned int CountFree( void ) const
    {
        unsigned int count = 0;

        for ( renderChain *iter = m_renderLast.next; iter != &m_renderStack; iter = iter->next )
            count++;

        return count;
    }

    renderChain                 m_root;             // 0
    renderChain                 m_rootLast;         // 20
    renderChain                 m_renderStack;      // 40
    renderChain                 m_renderLast;       // 60
    renderChain*                m_stack;            // 80
};

// Used by vehicles and such.
struct atomicRenderInfo
{
    void InitFirst( void )
    {
        distance = 0;
    }

    void InitLast( void )
    {
        distance = 100000000;
    }

    bool operator <( const atomicRenderInfo& right )
    {
        return distance < right.distance;
    }

    RpAtomic    *atomic;
    void        (__cdecl*callback)( RpAtomic *atom );
    float       distance;

    inline void Execute( void )
    {
        callback( atomic );
    }
};
typedef CRenderChainInterface <atomicRenderInfo> atomicRenderChain_t;

// Entity chain information
struct entityRenderInfo
{
    void InitFirst( void )
    {
        distance = 0;
    }

    void InitLast( void )
    {
        distance = 100000000;
    }

    bool operator <( const entityRenderInfo& right )
    {
        return distance < right.distance;
    }

    typedef void (__cdecl*callback_t)( CEntitySAInterface *intf, float dist );

    CEntitySAInterface  *entity;
    callback_t          callback;
    float               distance;
    bool                isReferenced;       // MTA extension

    inline void Execute( void )
    {
        callback( entity, distance );
    }
};
typedef CRenderChainInterface <entityRenderInfo> entityRenderChain_t;

// Ped information
struct pedRenderInfo
{
    void InitFirst( void )
    {
    }

    void InitLast( void )
    {
    }

    bool operator <( const pedRenderInfo& right )
    {
        return false;
    }

    class CPedSAInterface   *ped;
    
    inline void Execute( void );
};
typedef CRenderChainInterface <pedRenderInfo> pedRenderChain_t;

enum eRenderType : unsigned int
{
    ENTITY_RENDER_CROSS_ZONES,
    ENTITY_RENDER_DEFAULT,
    ENTITY_RENDER_CONTROVERIAL,
    ENTITY_RENDER_REQUEST_MODEL
};

namespace Entity
{
    void    SetPreRenderCallback                        ( gameEntityPreRenderCallback_t callback );
    void    SetRenderCallback                           ( gameEntityRenderCallback_t callback );
    void    SetRenderUnderwaterCallback                 ( gameEntityRenderUnderwaterCallback_t callback );
    void    SetRenderPostProcessCallback                ( gameEntityPostProcessCallback_t callback );

    // Rendering mode functions.
    void                SetWorldRenderMode              ( eWorldRenderMode mode );
    eWorldRenderMode    GetWorldRenderMode              ( void );

    CGame::entityList_t GetEntitiesInRenderQueue        ( void );
};

// Namespace of definitions that should not be directly exported.
namespace EntityRender
{
    void __cdecl        DefaultRenderEntityHandler      ( CEntitySAInterface *entity, float camDistance );

    // Render chain management.
    extern atomicRenderChain_t boatRenderChain;
    extern entityRenderChain_t defaultEntityRenderChain;
    extern entityRenderChain_t underwaterEntityRenderChain;
    extern entityRenderChain_t alphaEntityRenderChain;

    inline entityRenderChain_t& GetDefaultEntityRenderChain     ( void )        { return defaultEntityRenderChain; }
    inline entityRenderChain_t& GetUnderwaterEntityRenderChain  ( void )        { return underwaterEntityRenderChain; }
    inline entityRenderChain_t& GetAlphaEntityRenderChain       ( void )        { return alphaEntityRenderChain; }
    inline atomicRenderChain_t& GetBoatRenderChain              ( void )        { return boatRenderChain; }

    // Important utilities.
    bool                DoBlockModelVisionLoading( void );
};

// Include sub modules.
#include "CEntitySA.rendersetup.h"
#include "CEntitySA.renderutils.h"

// Function exports.
bool CanVehicleRenderNatively( void );
void EntityRender_Global( CEntitySAInterface *entity, unsigned int renderAlpha );

void EntityRender_Init( void );
void EntityRender_Shutdown( void );
void EntityRender_Reset( void );

#endif //_ENTITY_RENDER_