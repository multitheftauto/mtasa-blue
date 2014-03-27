/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.collision.h
*  PURPOSE:     Collision data streaming
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef _STREAMING_COLLISION_
#define _STREAMING_COLLISION_

// Streaming utilities
void __cdecl RegisterCOLLibraryModel( unsigned short collId, unsigned short modelId );

// Used by streaming.
bool __cdecl LoadCOLLibrary( unsigned char collId, const char *buf, size_t size );
void __cdecl FreeCOLLibrary( unsigned char collId );

// Module initialization.
void StreamingCOL_Init( void );
void StreamingCOL_Shutdown( void );

struct CColSectorManagerSA
{
    void Init( void )
    {
    }

    void Shutdown( void )
    {
    }

    // Methods used by the sectorizer.
    bool ShouldUnloadNonPersistentSectors( void )           { return false; }

    typedef void (__cdecl*intersectCallback_t)( const CVector2D& pos, CColFileSA *ipl );

    static unsigned int m_loadAreaId;

    // Binary offsets: (1.0 US and 1.0 EU): 0x004103D0
    static void __cdecl FlagCOLSector_samedimm( const CVector2D& pos, CColFileSA *colFile )
    {
        if ( m_loadAreaId == 0 && colFile->m_isInterior )
            return;

        if ( colFile->m_bounds.IsInside( pos ) )
            colFile->FlagSector( true );
    }

    // Binary offsets: (1.0 US and 1.0 EU): 0x00410470
    static void __cdecl FlagCOLSector_diffdimm( const CVector2D& pos, CColFileSA *colFile )
    {
        if ( colFile->m_bounds.ContainsPoint( pos, -80 ) )
        {
            if ( m_loadAreaId == 0 && !colFile->m_isInterior ||
                 m_loadAreaId != 0 && colFile->m_isInterior )
            {
                colFile->FlagSector( true );
            }
        }
    }

    intersectCallback_t ObtainDefaultSectorFlagCallback( const CVector& pos, unsigned int areaId )
    {
        m_loadAreaId = areaId;

        return FlagCOLSector_diffdimm;
    }

    intersectCallback_t ObtainSectorFlagCallback( const CVector& reqPos, unsigned int areaId )
    {
        if ( areaId == *(unsigned int*)VAR_currArea )
        {
            m_loadAreaId = areaId;

            return FlagCOLSector_samedimm;
        }

        return ObtainDefaultSectorFlagCallback( reqPos, areaId );
    }

    static bool ValidatePrioritySectorPoint( CColFileSA *colFile, const CVector& pos )
    {
        return colFile->m_bounds.ContainsPoint( pos, -110 );
    }

    static modelId_t GetModelForPoolEntity( unsigned int index )
    {
        return DATA_COLL_BLOCK + index;
    }

    void AddVelocityFeedback( CVector& pos, const CVector& velocity )
    {
        pos[0] += velocity[0] * 20.0f;
        pos[1] += velocity[1] * 20.0f;
    }

    void UnloadInstance( CColFileSA *colFile, unsigned int collId )
    {
        // Mark this COL library as unloaded.
        colFile->m_loaded = false;

        for ( short n = colFile->m_rangeStart; n <= colFile->m_rangeEnd; n++ )
        {
            CBaseModelInfoSAInterface *info = ppModelInfo[n];

            if ( !info )
                continue;

            CColModelSAInterface *colModel;
            bool isDynamic;

            // MTA extension: Handle collision replacements by MTA.
            // We should free the original collision, as we will obtain it again later
            // during COL library loading. This saves quite some memory in total
            // conversions.
            if ( CColModelSA *c_col = g_colReplacement[n] )
            {
                colModel = c_col->GetOriginal( n, isDynamic );
            }
            else
            {
                colModel = info->pColModel;
                isDynamic = info->IsDynamicCol();
            }

            if ( colModel && isDynamic && colModel->m_colPoolIndex == collId )
                colModel->ReleaseData();
        }
    }

    unsigned int    m_unused;       // actually m_loadAreaId in GTA:SA memory
};

typedef Sectorizer <CColFileSA, 255, CColSectorManagerSA> COLEnv_t;

namespace Streaming
{
    inline COLEnv_t&    GetCOLEnvironment( void )
    {
        return *(COLEnv_t*)0x0096553C;
    }
};

#endif //_STREAMING_COLLISION_