/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.ipl.h
*  PURPOSE:     IPL sector streaming
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _IPL_SECTOR_STREAMING_
#define _IPL_SECTOR_STREAMING_

#define IPL_SECTOR_CLOSEN_MOD           (-140.0f)

// Module initialization.
void StreamingIPL_Init( void );
void StreamingIPL_Shutdown( void );

struct CIPLSectorManagerSA
{
    void Init( void )
    {
        m_unkPtr = NULL;
        m_count = 0;
    }

    void Shutdown( void )
    {
        // Delete some stuff.
        // That is, call the GTA:SA memory free on it.
        for ( unsigned int n = 0; n < m_count; n++ )
            ((void (__cdecl*)( void* ))0x008213AE)( m_unkData[n] );
    }

    // Methods used by the sectorizer.
    bool ShouldUnloadNonPersistentSectors( void )           { return true; }

    typedef void (__cdecl*intersectCallback_t)( const CVector2D& pos, CIPLFileSA *ipl );

    static unsigned int m_loadAreaId;

    static void __cdecl FlagRelatedIPLSector_farLoad( const CVector2D& pos, CIPLFileSA *ipl )
    {
        if ( ipl->m_isInterior && ipl->m_bounds.ContainsPoint( pos, -160 ) )
        {
            ipl->FlagSector( true );
        }
    }

    static void __cdecl FlagRelatedIPLSector_areaCurrent( const CVector2D& pos, CIPLFileSA *ipl )
    {
        if ( ipl->m_isInterior )
        {
            FlagRelatedIPLSector_farLoad( pos, ipl );
        }
        else if ( ipl->m_bounds.IsInside( pos ) )
        {
            ipl->FlagSector( true );
        }
    }

    static void __cdecl FlagRelatedIPLSector_diffDimm( const CVector2D& pos, CIPLFileSA *ipl )
    {
        if ( ipl->m_bounds.ContainsPoint( pos, -160 ) )
        {
            if ( m_loadAreaId == 0 && !ipl->m_isInterior ||
                 m_loadAreaId != 0 && ipl->m_isInterior )
            {
                ipl->FlagSector( true );
            }
        }
    }

    intersectCallback_t     ObtainDefaultSectorFlagCallback( const CVector& pos, unsigned int areaId )
    {
        m_loadAreaId = areaId;

        return FlagRelatedIPLSector_diffDimm;
    }

    intersectCallback_t     ObtainSectorFlagCallback( const CVector& pos, unsigned int areaId )
    {
        if ( areaId == 0 && pos.fZ >= 900.0f )
        {
            m_loadAreaId = areaId;

            return FlagRelatedIPLSector_farLoad;
        }
        else if ( *(unsigned int*)VAR_currArea == 0 )
        {
            m_loadAreaId = areaId;

            return FlagRelatedIPLSector_areaCurrent;
        }

        // The default callback.
        return ObtainDefaultSectorFlagCallback( pos, areaId );
    }

    inline void AddVelocityFeedback( CVector& pos, const CVector& velocity )
    {
        pos[0] += (float)std::min( velocity[0] * 80.0f, 40.0f );
        pos[1] += (float)std::min( velocity[1] * 80.0f, 40.0f );
    }

    static bool ValidatePrioritySectorPoint( CIPLFileSA *ipl, const CVector& point )
    {
        return ipl->m_bounds.ContainsPoint( point, -190 );
    }

    static modelId_t GetModelForPoolEntity( unsigned int index )
    {
        return DATA_IPL_BLOCK + index;
    }

    struct ObjectSectorUnload
    {
        ObjectSectorUnload( unsigned int iplIndex ) : m_iplIndex( iplIndex )
        {
        }

        void __forceinline OnEntry( CObjectSAInterface *obj, unsigned int index )
        {
            if ( obj->m_iplIndex == m_iplIndex )
            {
                if ( CEntitySAInterface *dummy = obj->pGarageDoorDummy )
                    World::AddEntity( dummy );

                World::RemoveEntity( obj );

                delete obj;
            }
        }

        unsigned int m_iplIndex;
    };

    inline void UnloadInstance( CIPLFileSA *iplFile, unsigned int index )
    {
        // Mark the IPL sector as unloaded.
        iplFile->m_isLoaded = false;

        // Loop through all buildings.
        for ( int n = iplFile->m_buildingRangeStart; n <= iplFile->m_buildingRangeEnd; n++ )
        {
            CBuildingSAInterface *building = (*ppBuildingPool)->Get( n );

            if ( building && building->m_iplIndex == index )
            {
                World::RemoveEntity( building );

                delete building;
            }
        }

        // Loop through all objects.
        (*ppObjectPool)->ForAllActiveEntries( ObjectSectorUnload( index ), 0 );

        // Loop through the dummies.
        for ( int n = iplFile->m_dummyRangeStart; n <= iplFile->m_dummyRangeEnd; n++ )
        {
            CDummySAInterface *dummy = (*ppDummyPool)->Get( n );

            if ( dummy && dummy->m_iplIndex == index )
            {
                World::RemoveEntity( dummy );

                delete dummy;
            }
        }

        // Call some unknown function.
        ((void (__cdecl*)( unsigned char iplIndex ))0x006F3240)( index );
    }

    unsigned int    m_unused;       // actually m_loadAreaId in GTA:SA memory
    void**          m_unkPtr;
    unsigned int    m_count;
    unsigned int    m_unused2;

    void*           m_unkData[40];
};

typedef Sectorizer <CIPLFileSA, MAX_IPL, CIPLSectorManagerSA> IPLEnv_t;

namespace Streaming
{
    inline IPLEnv_t&    GetIPLEnvironment( void )
    {
        return *(IPLEnv_t*)0x008E3EE0;
    }
};

#endif //_IPL_SECTOR_STREAMING_