/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamingSA.ipl.cpp
*  PURPOSE:     IPL sector streaming
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

// Instantiate single copies of IPL sector manager members.
unsigned int CIPLSectorManagerSA::m_loadAreaId = 0;

inline CColModelSAInterface* GetOriginalColModel( modelId_t modelIndex )
{
    if ( CColModelSA *colReplace = g_colReplacement[modelIndex] )
    {
        bool isDynamic;

        return colReplace->GetOriginal( modelIndex, isDynamic );
    }

    return ppModelInfo[ modelIndex ]->pColModel;
}

/*=========================================================
    IsEntityVisibleByDefault

    Arguments:
        entity - the entity to check for always-visible
    Purpose:
        Checks whether the given entity should be visible at
        all times.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x0046A760
=========================================================*/
bool __cdecl IsEntityVisibleByDefault( CEntitySAInterface *entity )
{
    bool alwaysVisible = false;

    if ( entity->nType == ENTITY_TYPE_OBJECT )
    {
        CAtomicModelInfoSA *atomicModelInfo = entity->GetModelInfo()->GetAtomicModelInfo();

        if ( atomicModelInfo )
        {
            alwaysVisible = ( atomicModelInfo->atomicType == 4 || atomicModelInfo->atomicType == 5 );
        }
    }

    return alwaysVisible;
}

/*=========================================================
    SetMatrixTraincross2Rotation

    Arguments:
        matrix - the matrix to write the rotation to
        radians - rotation in radians
    Purpose:
        Updates the rotation of the specified matrix by the
        given rotation value.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00538090
=========================================================*/
void __cdecl SetMatrixTraincross2Rotation( RwMatrix& matrix, float radians )
{
    CVector vecUp( 0.0f, 0.0f, 1.0f );
    CVector vecFront = vecUp; vecFront.CrossProduct( matrix.vRight );

    float cosinus = cos( radians );
    float sinus = sin( radians );

    float cx = vecFront.fX * cosinus;
    float cy = vecFront.fY * cosinus;
    float sz = vecFront.fZ * sinus;

    CVector vecRot( sz * 0.0f + cx, sz * 0.0f + cy, sz * 1.0f + cosinus );

    CVector newUpVector = matrix.vRight;
    newUpVector.CrossProduct( vecRot );

    matrix.vUp = newUpVector;
    matrix.vFront = vecRot;
}

/*=========================================================
    AllocateIPLInstanceArray

    Arguments:
        instanceCount - the count of instances the array
                        should be capable to hold
    Purpose:
        Returns an index to a newly allocated static array
        that can hold instanceCount amount of IPL buildings.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00404780
=========================================================*/
static int __cdecl AllocateIPLInstanceArray( unsigned int instanceCount )
{
    return Streaming::GetIPLEnvironment().m_sectorMan.AllocateIPLInstanceArray( instanceCount );
}

/*=========================================================
    GetIPLInstanceArray

    Arguments:
        instanceIndex - the index of the IPL instance array
    Purpose:
        Returns the static array that can hold an amount of
        building entity pointers. Can return NULL if an invalid
        index has been passed to this function.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00404780
=========================================================*/
static CEntitySAInterface** __cdecl GetIPLInstanceArray( int instanceIndex )
{
    return Streaming::GetIPLEnvironment().m_sectorMan.GetIPLInstanceArray( instanceIndex );
}

/*=========================================================
    CreateIPLBuilding

    Arguments:
        instance - information about this building
        instanceName - name of the instance (can be NULL), unused
    Purpose:
        Creates an IPL game instance from the information
        as described by the instance parameter.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00538090
=========================================================*/
struct iplInstance_t
{
    CVector                 position;           // 0
    CQuat                   quatRotation;       // 12
    modelId_t               modelIndex;         // 28
    union
    {
        struct
        {
            unsigned char   areaIndex;          // 32
            unsigned char   flags;              // 33
            unsigned char   pad[2];             // 34
        };
        unsigned int        uiFlagNumber;
    };
    int                     lodIndex;           // 36, index inside of the .ipl file pointing at the LOD instance.
};

CEntitySAInterface* __cdecl CreateIPLBuilding( iplInstance_t *instance, const char *instanceName )
{
    CBaseModelInfoSAInterface *modelInfo = ppModelInfo[ instance->modelIndex ];

    if ( !modelInfo )
        return NULL;

    CEntitySAInterface *building = NULL;
    bool isVisible = false;

    if ( modelInfo->usDynamicIndex != -1 )
    {
        building = new CDummySAInterface;

        building->SetModelIndexNoCreate( instance->modelIndex );

        isVisible = !( IsEntityVisibleByDefault( building ) && building->GetModelInfo()->atomicType == 5 );
    }
    else
    {
        if ( modelInfo->GetModelType() == (eModelType)5 && modelInfo->bCollisionless )
        {
            building = new CNoCOLBuildingSAInterface;
        }
        else
        {
            building = new CBuildingSAInterface;
        }

        building->SetModelIndexNoCreate( instance->modelIndex );

        if ( modelInfo->bDontWriteZBuffer )
        {
            building->bDontCastShadowsOn = true;
        }

        isVisible = ( modelInfo->GetLODDistance() >= 2.0f );
    }

    if ( !isVisible )
    {
        building->bIsVisible = false;
    }

    if ( instance->modelIndex == 3999 )
        __asm nop

    if ( instance->quatRotation.x <= 0.05f && instance->quatRotation.y <= 0.05f &&
         ( IS_ANY_FLAG( instance->flags, 0x200 ) || instance->quatRotation.x <= 0.0f || instance->quatRotation.y <= 0.0f ) )
    {
        bool isFlipped = ( instance->quatRotation.z >= 0.0f );

        float heading = acos( instance->quatRotation.w );

        heading *= ( isFlipped ? -2.0f : 2.0f );

        building->Placeable.SetHeading( heading );
    }
    else
    {
        // Invert the quat.
        instance->quatRotation.x = -instance->quatRotation.x;
        instance->quatRotation.y = -instance->quatRotation.y;
        instance->quatRotation.z = -instance->quatRotation.z;

        building->AllocateMatrix();

        CTransformSAInterface*& trans = building->Placeable.matrix;

        if ( !trans )
        {
            building->AcquaintMatrix();

            building->Placeable.GetMatrixFromHeading( *trans );
        }

        CQuat::ToMatrix( instance->quatRotation, *trans );
    }

    building->Placeable.SetPosition( instance->position );

    // Set some initialization flags.
    if ( IS_ANY_FLAG( instance->flags, 0x4 ) )
    {
        building->bUnderwater = true;
    }

    if ( IS_ANY_FLAG( instance->flags, 0x8 ) )
    {
        building->bTunnel = true;
    }

    if ( IS_ANY_FLAG( instance->flags, 0x10 ) )
    {
        building->bTunnelTransition = true;
    }

    if ( IS_ANY_FLAG( instance->flags, 0x1 ) )
    {
        building->bUnimportantStream = true;
    }

    building->m_areaCode = instance->areaIndex;
    building->m_iLodIndex = instance->lodIndex;

    // Do some fix for traincross2 (?)
    if ( building->GetModelIndex() == *(unsigned short*)0x008CD6B4 )
    {
        CTransformSAInterface*& trans = building->Placeable.matrix;
        
        if ( !trans )
        {
            building->AcquaintMatrix();

            building->Placeable.GetMatrixFromHeading( *trans );
        }

        building->AllocateMatrix();

        if ( !trans )
        {
            building->AcquaintMatrix();

            building->Placeable.GetMatrixFromHeading( *trans );
        }

        SetMatrixTraincross2Rotation( *trans, (float)DEG2RAD( 77.4f ) );
    }

    CColModelSAInterface *colModel = GetOriginalColModel( building->GetModelIndex() );

    // This may be the only time this is actually done in the engine.
    if ( colModel )
    {
        if ( !colModel->m_isCollidable )
        {
            building->bUsesCollision = false;
        }
        else if ( colModel->m_colPoolIndex != 0 )
        {
            CBounds2D outBounds;

            const CBounds2D& bounds = building->GetBoundingBox( outBounds );

            Streaming::GetCOLEnvironment().m_pool->Get( colModel->m_colPoolIndex )->ExtendBounds( bounds );
        }

        const CVector& buildingPos = building->Placeable.GetPosition();

        if ( buildingPos.fZ + colModel->m_bounds.vecBoundMin.fZ < 0.0f )
        {
            building->bUnderwater = true;
        }
    }

    return building;
}

/*=========================================================
    CreateIPLBuildingFromString

    Arguments:
        strPtr - string to create an IPL instance of
    Purpose:
        Creates an IPL game instance from a string and returns
        it.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00538690
=========================================================*/
CEntitySAInterface* __cdecl CreateIPLBuildingFromString( const char *strPtr )
{
    iplInstance_t instance;
    char nameBuffer[24];

    sscanf( strPtr, "%d %s %d %f %f %f %f %f %f %f %d",
        &instance.modelIndex, nameBuffer, &instance.uiFlagNumber,
        &instance.position.fX, &instance.position.fY, &instance.position.fZ,
        &instance.quatRotation.x, &instance.quatRotation.y, &instance.quatRotation.z, &instance.quatRotation.w,
        &instance.lodIndex
    );

    return CreateIPLBuilding( &instance, nameBuffer );
}

/*=========================================================
    RegisterIPLFileEntityPoolIndex

    Arguments:
        iplIndex - index of the IPL sector
        entity - the entity to register the model of
    Purpose:
        Registers the entity's pool index to the IPL sector index
        ranges. Index ranges are used to speed up looping when
        required.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00404C90
=========================================================*/
void __cdecl RegisterIPLFileEntityPoolIndex( int iplIndex, CEntitySAInterface *entity )
{
    CIPLFileSA *iplFile = Streaming::GetIPLEnvironment().m_pool->Get( iplIndex );

    if ( entity->nType == ENTITY_TYPE_BUILDING )
    {
        int buildingIndex = Pools::GetBuildingPool()->GetIndex( (CBuildingSAInterface*)entity );

        iplFile->m_buildingRange.Add( buildingIndex );
    }
    else if ( entity->nType == ENTITY_TYPE_DUMMY )
    {
        int dummyIndex = Pools::GetDummyPool()->GetIndex( (CDummySAInterface*)entity );

        iplFile->m_dummyRange.Add( dummyIndex );
    }
}

inline void AddEntityRectToSector( CEntitySAInterface *entity, CBounds2D& sector )
{
    CBounds2D outBounds;

    const CBounds2D& useBounds = entity->GetBoundingBox( outBounds );

    sector.AddBounds( outBounds );
}

/*=========================================================
    ReadIPLSectorGeneral

    Arguments:
        iplId - index of the loading ipl sector
        buf - loading buffer for the ipl sector
        size - size in bytes of the loading buffer
    Purpose:
        Loads an IPL sector from a file buffer into game memory.
        This function caches the boundaries of the target IPL
        instance. It is only called during engine startup.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00405C00
=========================================================*/
struct binaryIPLHeader_t
{
    int             numIPLInstances;        // 0
    BYTE            pad[20];                // 4
    unsigned int    iplOffset;              // 24
};

template <typename managerType>
struct IPLContainerLoader
{
    AINLINE IPLContainerLoader( managerType& manager ) : m_manager( manager )
    { }

    AINLINE void __cdecl RegisterInstance( CEntitySAInterface *entity, CEntitySAInterface **lodArray, CIPLFileSA *iplFile, unsigned char iplId )
    {
        entity->m_iplIndex = iplId;

        // Set up the level of detail entity.
        CEntitySAInterface *lodEntity = NULL;

        // Bugfix: make sure the instance array is valid.
        if ( lodArray != NULL && entity->m_iLodIndex != -1 )
        {
            lodEntity = lodArray[ entity->m_iLodIndex ];

            lodEntity->numLodChildren++;

            m_manager.OnLODInstance( entity, iplFile );
        }

        entity->m_pLod = lodEntity;

        m_manager.OnBaseInstance( entity, iplFile );

        entity->AddToWorld();

        RegisterIPLFileEntityPoolIndex( iplId, entity );

        m_manager.OnRegisterInstance( entity, iplFile );
    }
    
    AINLINE bool __cdecl LoadContainer( unsigned char iplId, const char *buf, int size )
    {
        CIPLFileSA *iplFile = Streaming::GetIPLEnvironment().m_pool->Get( iplId );

        CEntitySAInterface **lodArray = NULL;

        if ( iplFile->m_iplId != -1 )
        {
            lodArray = GetIPLInstanceArray( iplFile->m_iplId );
        }

        bool successful = false;

        // Check whether the file is in binary mode.
        if ( strncmp( buf, "bnry", 4 ) == 0 )
        {
            const binaryIPLHeader_t& header = *(binaryIPLHeader_t*)( buf + 4 );

            short numIPLInstances = (short)header.numIPLInstances;

            iplInstance_t *instanceArray = (iplInstance_t*)( buf + header.iplOffset );

            for ( unsigned int n = 0; n < (unsigned int)numIPLInstances; n++ )
            {
                iplInstance_t& instance = instanceArray[n];

                CEntitySAInterface *entity = CreateIPLBuilding( &instance, NULL );

                RegisterInstance( entity, lodArray, iplFile, iplId );
            }

            m_manager.OnReadAdditionalSections( buf, header );

            successful = true;
        }
        else
        {
            bool isInsideInstanceList = false;
            
            while ( const char *configLine = FileMgr::GetConfigLineFromBuffer( &buf, &size ) )
            {
                const char c = *configLine;

                if ( c == '\0' || c == '#' )
                    continue;

                if ( isInsideInstanceList )
                {
                    if ( strncmp( configLine, "end", 3 ) == 0 )
                    {
                        isInsideInstanceList = false;
                        continue;
                    }
                }
                else
                {
                    if ( strncmp( configLine, "inst", 4 ) == 0 )
                    {
                        isInsideInstanceList = true;
                        continue;
                    }
                }

                if ( isInsideInstanceList )
                {
                    CEntitySAInterface *building = CreateIPLBuildingFromString( configLine );

                    RegisterInstance( building, lodArray, iplFile, iplId );
                }
            }
        }

        return successful;
    }

    managerType& m_manager;
};

struct CachedIPLLoaderManager
{
    AINLINE void OnLODInstance( CEntitySAInterface *lodEntity, CIPLFileSA *iplFile )
    {
        Streaming::GetIPLEnvironment().m_sectorMan.RegisterLODInstance( lodEntity );
    }

    AINLINE void OnBaseInstance( CEntitySAInterface *entity, CIPLFileSA *iplFile )
    {
        CBaseModelInfoSAInterface *modelInfo = entity->GetModelInfo();

        if ( modelInfo->GetLODDistance() > IPL_SECTOR_FAR_LOD )
        {
            iplFile->m_containsFarChunks = true;
        }
    }

    AINLINE void OnRegisterInstance( CEntitySAInterface *entity, CIPLFileSA *iplFile )
    {
        AddEntityRectToSector( entity, iplFile->m_bounds );
    }

    AINLINE void OnReadAdditionalSections( const char *buf, const binaryIPLHeader_t& header )
    {
        return;
    }
};

struct RuntimeIPLLoaderManager
{
    AINLINE void OnLODInstance( CEntitySAInterface *lodEntity, CIPLFileSA *iplFile )
    {
        return;
    }

    AINLINE void OnBaseInstance( CEntitySAInterface *entity, CIPLFileSA *iplFile )
    {
        return;
    }

    AINLINE void OnRegisterInstance( CEntitySAInterface *entity, CIPLFileSA *iplFile )
    {
        return;
    }

    AINLINE void OnReadAdditionalSections( const char *buf, const binaryIPLHeader_t& header )
    {
        // Ignore parked cars, as we do not need them in MTA.
        return;
    }
};

bool __cdecl ReadIPLSectorGeneral( unsigned char iplId, const char *buf, int size )
{
    CachedIPLLoaderManager loaderMan;

    IPLContainerLoader <CachedIPLLoaderManager> loader( loaderMan );

    return loader.LoadContainer( iplId, buf, size );
}

/*=========================================================
    ReadIPLSectorBounds

    Arguments:
        buf - loading buffer for the ipl sector
        size - size in bytes of the loading buffer
        iplId - index of the loading ipl sector
    Purpose:
        Loads an IPL sector from a file buffer into game memory.
        It uses the IPL boundaries as set by ReadIPLSectorGeneral
        to accelerate loading.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00406185 (inlined)
=========================================================*/
bool __cdecl ReadIPLSectorBounds( unsigned char iplId, const char *buf, int size )
{
    RuntimeIPLLoaderManager loaderMan;

    IPLContainerLoader <RuntimeIPLLoaderManager> loader( loaderMan );

    return loader.LoadContainer( iplId, buf, size );
}

bool __thiscall CIPLSectorManagerSA::LoadInstance( CIPLFileSA *iplFile, unsigned int iplId, const char *buf, size_t bufSize )
{
    bool successful = false;

    if ( iplFile->m_bounds.m_minX > iplFile->m_bounds.m_maxX ||
         iplFile->m_bounds.m_minY > iplFile->m_bounds.m_maxY )
    {
        successful = ReadIPLSectorGeneral( iplId, buf, bufSize );

        if ( successful )
        {
            float extendBy = 200.0f;

            if ( iplFile->m_containsFarChunks )
            {
                extendBy += IPL_SECTOR_FAR_LOD;
            }

            // Extend the sector boundaries somewhat.
            iplFile->m_bounds.ExtendBy( extendBy );

            Cache_StoreIPLFile( iplId, *iplFile );

            // Link this IPL file into the IPL file quad tree.
            Streaming::GetIPLEnvironment().m_quadTree->LinkToEntity( iplFile, iplFile->m_bounds );
        }
    }
    else
    {
        successful = ReadIPLSectorBounds( iplId, buf, bufSize );
    }

    iplFile->m_isLoaded = true;

    return successful;
}

/*=========================================================
    _SetIPLLoadPosition

    Arguments:
        pos - position to load IPL sectors around
    Purpose:
        Sets the loading position that is used to request
        IPL sectors. By doing that, the IPL sector does another
        marking by position.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004045B0
=========================================================*/
static void __cdecl _SetIPLLoadPosition( const CVector& pos )
{
    Streaming::GetIPLEnvironment().SetLoadPosition( pos );
}

/*=========================================================
    _StreamIPLSectors

    Arguments:
        requestPos - position to request around
        calcVelocity - weird position modificator
    Purpose:
        Manages all present IPL instances. IPL sectors that
        are pending to be loaded are requested. Unused IPL
        sectors are unloaded. IPL sectors are loaded for all
        mission vehicles and peds.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00405170
=========================================================*/
static void __cdecl _StreamIPLSectors( CVector requestPos, bool calcVelocity )
{
    Streaming::GetIPLEnvironment().StreamSectors( requestPos, calcVelocity );
}

/*=========================================================
    _PrioritizeIPLStreaming

    Arguments:
        requestPos - position to request around
    Purpose:
        Loops through all active IPL sectors and loads
        the ones intersecting with the given position.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004053F0
=========================================================*/
static void __cdecl _PrioritizeIPLStreaming( const CVector& requestPos )
{
    Streaming::GetIPLEnvironment().PrioritizeLocalStreaming( requestPos );
}

/*=========================================================
    _IPL_FreeInstance

    Purpose:
        Frees instance information as used by the given IPL
        sector.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00404B20
=========================================================*/
static void __cdecl _IPL_FreeInstance( unsigned int iplIndex )
{
    Streaming::GetIPLEnvironment().UnloadSector( iplIndex );
}

/*=========================================================
    _IPL_Destroy

    Purpose:
        Deallocates the memory associated with the IPL sector
        manager.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00405FA0
=========================================================*/
static void __cdecl _IPL_Destroy( void )
{
    Streaming::GetIPLEnvironment().Shutdown();
}

void StreamingIPL_Init( void )
{
    // Hook stuff.
    HookInstall( 0x00404780, (DWORD)AllocateIPLInstanceArray, 5 );
    HookInstall( 0x004047B0, (DWORD)GetIPLInstanceArray, 5 );
    HookInstall( 0x00538690, (DWORD)CreateIPLBuildingFromString, 5 );
    HookInstall( 0x004045B0, (DWORD)_SetIPLLoadPosition, 5 );
    HookInstall( 0x00405170, (DWORD)_StreamIPLSectors, 5 );
    HookInstall( 0x004053F0, (DWORD)_PrioritizeIPLStreaming, 5 );
    HookInstall( 0x00404B20, (DWORD)_IPL_FreeInstance, 5 );
    HookInstall( 0x00405FA0, (DWORD)_IPL_Destroy, 5 );
}

void StreamingIPL_Shutdown( void )
{
}