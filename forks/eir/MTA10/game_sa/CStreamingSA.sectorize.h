/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.sectorize.h
*  PURPOSE:     Template for the sectorization of bounding instances
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_SECTORIZE_
#define _STREAMING_SECTORIZE_

// todo: not belonging in here.
// Put here because it first appeared to me in this context.
#define MAX_MISSION_ENTITIES    75

struct missionEntity_t
{
    unsigned char   type;
    unsigned int    poolId : 8;     // 4
    unsigned int    poolIndex : 24; // 5
};

inline missionEntity_t& GetMissionEntity( unsigned int index )
{
    return *((missionEntity_t*)0x00A90850 + index);
}

// Another section that probably does not belong in here.
inline void __cdecl renderTimerPause( void )
{
    ((void (__cdecl*)( void ))0x005619D0)();
}

inline void __cdecl renderTimerResume( void )
{
    ((void (__cdecl*)( void ))0x00561A00)();
}

template <typename sectorType, unsigned int numSectors, typename managerType>
struct Sectorizer
{
    typedef CPool <sectorType, numSectors> pool_t;
    typedef CQuadTreeNodeSAInterface <sectorType> quadTreeNode_t;

    /*
        The_GTA: since this template is very abstract, I would like to explain why I created it.
        R* Games does indeed use this template. I am amazed myself.

        This template is used for IPL and collision sectors, both of which are managed by
        the streaming system. Previously I have created random function prototypes that
        matched the purpose. Those prototypes have had a distinct layout that kept re-appearing
        (see RegisterCollision and RegisterIPLFile in previous revisions, now
        managerType::RegisterInstance). It took me till the investigation on CStreaming::Update
        to realize that huge chunks of logic had exactly the same functionality for IPL and
        collision sectors.

        So, to decrease code repetition redundancy, I reconstruct the R* games sectorization
        system. I call it "Sectorizer" because both IPL and collision structures are sectors
        which contain things, and the large matter of things shall be managed here dynamically.

        Maintaining templates avoids issues. If we instead maintained copy-cat collision and
        IPL code sections, we may oversee logic here and there. A template logic mitigates this
        risk by having shared code.

        Every sectorizer appears to be a static class (or a global class item that has been
        optimized to a namespace). Problems may exist (for now) regarding machine code
        equivalence of sectorizer methods to their original GTA:SA representations.
    */

    /*=========================================================
        Sectorizer::Init

        Purpose:
            Initializes the sectorizer interface. This means that
            the item pool and the quad tree node is loaded.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00405EC0 (IPL)
            (1.0 US and 1.0 EU): 0x004113F0 (COL)
    =========================================================*/
    void Init( void )
    {
        // Generic world bounds interface.
        // This is where every sectorizer should operate.
        CBounds2D worldBounds( -WORLD_BOUNDS, WORLD_BOUNDS, WORLD_BOUNDS, -WORLD_BOUNDS );

        // Initialize position dependant loading.
        m_doPositionLoad = false;

        // If the pool has not been created yet, do it now.
        if ( !m_pool )
            m_pool = new pool_t;

        // In every environment we need a generic instance that is used to reference
        // non-dynamically loaded entries. Hence id 0 is a dummy id.
        // We skip id 0 when we loop through stream-able sectors.
        RegisterInstance( "generic" );

        // Create the quad tree that is used to assign sectors to
        // elements. Quad tree systems improve performance.
        m_quadTree = new quadTreeNode_t( worldBounds, 3 );

        // Initialize the management environment.
        m_sectorMan.Init();
    }

    /*=========================================================
        Sectorizer::Shutdown

        Purpose:
            Frees all data used by the sectorizer.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00405FA0 (IPL)
            (1.0 US and 1.0 EU): 0x004114D0 (COL)
    =========================================================*/
    struct SectorDestructionLoop
    {
        __forceinline SectorDestructionLoop( Sectorizer *sectorizer ) : m_sectorizer( sectorizer )
        {
        }

        void __forceinline OnEntry( sectorType *entry, unsigned int id )
        {
            m_sectorizer->UnregisterInstance( id );
        }

        Sectorizer *m_sectorizer;
    };

    void Shutdown( void )
    {
        if ( m_sectorMan.ShouldUnloadNonPersistentSectors() )
        {
            UnloadNonPersistentSectors();
        }

        // Loop through all allocated sectors and unregister them.
        // This time, also deallocate the "generic" dummy item (index 0).
        {
            SectorDestructionLoop loop( this );

            m_pool->ForAllActiveEntries( loop, 0 );
        }

        if ( m_pool )
        {
            // Finally, delete the pool.
            delete m_pool;
        }

        m_pool = NULL;

        // Shutdown the management environment.
        m_sectorMan.Shutdown();

        if ( m_quadTree )
        {
            // Destroy our quad tree node.
            delete m_quadTree;
        }

        m_quadTree = NULL;
    }

    /*=========================================================
        Sectorizer::UnloadNonPersistentSectors

        Purpose:
            Called at shutdown if desired by the management env.
            Unloads every sector that is not marked as persisent.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00405720 (IPL)
    =========================================================*/
    struct SectorUnloadNonPersistent
    {
        void __forceinline OnEntry( sectorType *sector, unsigned int index )
        {
            modelId_t model = managerType::GetModelForPoolEntity( index );
            CModelLoadInfoSA& info = Streaming::GetModelLoadInfo( model );

            if ( !IS_ANY_FLAG( info.m_flags, 0x06 ) )
                Streaming::FreeModel( model );
        }
    };

    void UnloadNonPersistentSectors( void )
    {
        SectorUnloadNonPersistent loop;

        m_pool->ForAllActiveEntries( loop, 1 );
    }

    /*=========================================================
        Sectorizer::RegisterInstance

        Arguments:
            name - string of the element name
        Purpose:
            Allocates an instance in the sector pool and returns
            its index. The name is given to the constructor so
            that it can properly initialize the object.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00405AC0 (IPL)
            (1.0 US and 1.0 EU): 0x00411140 (COL)
    =========================================================*/
    unsigned int RegisterInstance( const char *name )
    {
        sectorType *element = new sectorType( name );

        return m_pool->GetIndex( element );
    }

    /*=========================================================
        Sectorizer::UnregisterInstance

        Arguments:
            index - pool index of the instance
        Purpose:
            Deallocates the given sector instance from this
            environment. All data used by this instance is deleted.
            All references to it are removed.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00405B60 (IPL)
            (1.0 US and 1.0 EU): 0x00411330 (COL)
    =========================================================*/
    void UnregisterInstance( unsigned int index )
    {
        sectorType *sector = m_pool->Get( index );

        if ( sector->IsLoaded() )
        {
            // Let the manager instance deallocate all referencing data
            // from the game.
            UnloadSector( index );
        }

        // Remove all references to the sector.
        m_quadTree->RemoveItemFromHierarchy( sector );

        // Deallocate the instance.
        m_pool->Free( index );
    }

    /*=========================================================
        Sectorizer::LoadSector

        Arguments:
            index - pool index of the instance
        Purpose:
            Allocates sector data from a resource buffer. If loaded
            successfully, the sector is marked as loaded.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00406080 (IPL)
            (1.0 US and 1.0 EU): 0x004106D0 (COL)
    =========================================================*/
    bool LoadSector( unsigned int index, const char *buf, size_t bufSize )
    {
        sectorType *sector = m_pool->Get( index );

        return m_sectorMan.LoadInstance( sector, index, buf, bufSize );
    }

    /*=========================================================
        Sectorizer::UnloadSector

        Arguments:
            index - pool index of the instance
        Purpose:
            Deallocates all resources used by the given sector.
            The sector is then marked as unloaded.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00404B20 (IPL)
            (1.0 US and 1.0 EU): 0x00410730 (COL)
    =========================================================*/
    void UnloadSector( unsigned int index )
    {
        sectorType *sector = m_pool->Get( index );

        m_sectorMan.UnloadInstance( sector, index );
    }

    /*=========================================================
        Sectorizer::FlagSectorsByPosition

        Arguments:
            pos - position to load sectors around
            areaId - the interior to load in
        Purpose:
            Loops through all active sectors and flags
            the ones active at said position for loading.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x00404700 (IPL)
            (1.0 US and 1.0 EU): 0x004104E0 (COL)
    =========================================================*/
    inline unsigned int GetCurrentArea( void )
    {
        if ( CPlayerPedSAInterface *ped = GetPlayerPed( -1 ) )
            return ped->m_areaCode;

        return *(unsigned int*)VAR_currArea;
    }

    void FlagSectorsByPosition( const CVector& pos, unsigned int areaId )
    {
        unsigned int currArea = ( areaId == 0xFFFFFFFF ) ? GetCurrentArea() : areaId;

        quadTreeNode_t::intersectCallback_t cbIntersect = m_sectorMan.ObtainSectorFlagCallback( pos, currArea );

        m_quadTree->ForEachIntersection( pos, cbIntersect );
    }

    /*=========================================================
        Sectorizer::SetLoadPosition

        Arguments:
            pos - position to load sectors around
        Purpose:
            Sets the loading position that is used to request
            IPL sectors. By doing that, the IPL sector does another
            marking by position.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x004045B0 (IPL)
            (1.0 US and 1.0 EU): 0x004103A0 (COL)
    =========================================================*/
    void SetLoadPosition( const CVector& pos )
    {
        m_posLoadVector = pos;
        m_doPositionLoad = true;
    }

    /*=========================================================
        Sectorizer::StreamSectors

        Arguments:
            requestPos - position to request around
            calcVelocity - weird position modificator
        Purpose:
            Manages all present sector instances. Sectors that
            are pending to be loaded are requested. Unused IPL
            sectors are unloaded. Futher management is done by
            the managerType instance.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00405170 (IPL)
            (1.0 US and 1.0 EU): 0x00410860 (COL)
    =========================================================*/
    inline void FlagSectorsForEntity( CPhysicalSAInterface *entity )
    {
        if ( !entity )
            return;

        if ( IS_ANY_FLAG( entity->physicalFlags, 0x60 ) )
            return;

        const CVector& pos = entity->Placeable.GetPosition();

        m_quadTree->ForEachIntersection( pos, m_sectorMan.ObtainDefaultSectorFlagCallback( pos, entity->m_areaCode ) );
    }

    struct ManageSector
    {
        inline void OnEntry( sectorType *sector, unsigned int id )
        {
            if ( sector->IsStreamingDisabled() )
                return;

            if ( sector->IsSectorFlagged() )
            {
                if ( !sector->IsLoaded() )
                    Streaming::RequestModel( managerType::GetModelForPoolEntity( id ), 0x18 );

                sector->FlagSector( false );
            }
            else
            {
                if ( sector->IsLoaded() )
                {
                    Streaming::FreeModel( managerType::GetModelForPoolEntity( id ) );
                    
                    sector->OnStreamOut();
                }
            }
        }
    };

    void StreamSectors( CVector requestPos, bool calcVelocity )
    {
        // If sector management is disabled, return.
        if ( *(bool*)0x009654B0 )
            return;

        if ( GetPlayerPed( -1 ) && calcVelocity )
        {
            CVehicleSAInterface *vehicle = GetPlayerVehicle( -1, false );

            if ( vehicle )
            {
                m_sectorMan.AddVelocityFeedback( requestPos, vehicle->m_vecLinearVelocity );
            }
        }

        FlagSectorsByPosition( requestPos, 0xFFFFFFFF );

        if ( m_doPositionLoad )
            FlagSectorsByPosition( m_posLoadVector, *(unsigned int*)VAR_currArea );

        // Manage sectors for all mission entities, too.
        for ( unsigned int n = 0; n < MAX_MISSION_ENTITIES; n++ )
        {
            missionEntity_t& info = GetMissionEntity( n );

            if ( info.type == 1 )
            {
                CVehicleSAInterface *vehicle = (*ppVehiclePool)->Get( info.poolIndex );

                if ( vehicle && !vehicle->m_pAttachedEntity )
                {
                    unsigned char status = vehicle->nStatus;

                    if ( status == 2 || status == 3 || status == 4 )
                        FlagSectorsForEntity( vehicle );
                }
            }
            else if ( info.type == 2 )
            {
                CPedSAInterface *ped = (*ppPedPool)->Get( info.poolIndex );

                if ( ped && !ped->m_pAttachedEntity )
                {
                    int status = ped->pedStatus;

                    if ( status != 54 && status != 55 )
                        FlagSectorsForEntity( ped );
                }
            }
        }

        // Loop through all pool entities.
        // We start at index 1, since 0 is our dummy id.
        m_pool->ForAllActiveEntries( ManageSector(), 1 );

        // If we were not calculating velocity into the position, we can disregard position loading
        // on next iteration.
        if ( !calcVelocity )
            m_doPositionLoad = false;
    }

    /*=========================================================
        Sectorizer::PrioritizeLocalStreaming

        Arguments:
            requestPos - position to request around
        Purpose:
            Loops through all active sectors and requests the one
            intersecting the definite request position. By executing
            this, the streaming sectors that are around the player
            are prioritized in the streaming loader.
        Known deriviates:
            (1.0 US and 1.0 EU): 0x004053F0 (IPL)
            (1.0 US and 1.0 EU): 0x00410AD0 (COL)
    =========================================================*/
    struct streamingPrioritization
    {
        __forceinline streamingPrioritization( const CVector& pos ) : reqPos( pos )
        {
        }

        void __forceinline OnEntry( sectorType *sector, unsigned int id )
        {
            if ( sector->IsStreamingDisabled() )
                return;

            if ( sector->IsSectorFlagged() )
            {
                if ( managerType::ValidatePrioritySectorPoint( sector, reqPos ) )
                {
                    modelId_t model = managerType::GetModelForPoolEntity( id );

                    if ( Streaming::GetModelLoadInfo( model ).m_eLoading != MODEL_LOADED )
                    {
                        // Enforce the loading of our sector.
                        // It should be prioritized until finally loaded.
                        Streaming::RequestModel( model, 0x18 );

                        // NOTE: I left out a piece of menu code here.

                        renderTimerPause();

                        Streaming::LoadAllRequestedModels( true );

                        renderTimerResume();
                    }
                }

                sector->FlagSector( false );
            }
        }

        const CVector& reqPos;
    };

    void PrioritizeLocalStreaming( const CVector& requestPos )
    {
        // Only proceed if sector management is enabled.
        if ( *(bool*)0x009654B0 )
            return;

        if ( GetCurrentArea() != *(unsigned int*)VAR_currArea )
            return;

        FlagSectorsByPosition( requestPos, 0xFFFFFFFF );

        m_pool->ForAllActiveEntries( streamingPrioritization( requestPos ), 1 );
    }

    // managerType interface requirements:
    /*
        void Init( void );
        void Shutdown( void );

        bool ShouldUnloadNonPersistentSectors( void );
        void (__cdecl*)( const sectorType *inst, unsigned int id ) ObtainSectorFlagCallback( const CVector& reqPos, unsigned int areaId );
        void (__cdecl*)( const sectorType *inst, unsigned int id ) ObtainDefaultSectorFlagCallback( const CVector& reqPos, unsigned int areaId );
        bool ValidatePrioritySectorPoint( sectorType *sector, const CVector& reqPos );
        modelId_t GetModelForPoolEntry( unsigned int entryIndex );
        void AddVelocityFeedback( CVector& pos, const CVector& velocity );
        void UnloadInstance( sectorType *inst );
    */

    // sectorType interface requirements:
    /*
        sectorType::sectorType( const char *name );

        bool IsSectorFlagged( void );
        void FlagSector( bool flagged );
        bool IsLoaded( void );
        bool CanUnload( void );
        bool IsStreamingDisabled( void );
        void OnStreamOut( void );
    */

    // Since these members are actually static, I am using a slow transcribing approach to get them static.
    // Unless we have rewritten all sectorizer code and properly hooked it, we will not remove the definitions
    // from here to account for code which still uses GTA:SA memory.
    // This rule has to transcend to the m_sectorMan interface.

    float               m_unk2[6];
    managerType         m_sectorMan;
    bool                m_doPositionLoad;
    quadTreeNode_t*     m_quadTree;
    pool_t*             m_pool;
    float               m_unk[6];
    CVector             m_posLoadVector;
};

#endif //_STREAMING_SECTORIZE_