/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CStreamerSA.cpp
*  PURPOSE:     GTA:SA entity streamer extensions
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

static const DWORD pft = 0x00404180;
static const DWORD pft2 = 0x00404139;

// Callback class for removing all references to a certain building.
struct _buildingRefRemoval
{
    __forceinline _buildingRefRemoval( CBuildingSAInterface *instance ) : m_instance( instance )
    { }

    void __forceinline OnSector( Streamer::streamSectorEntry& sector )
    {
        sector.RemoveItem( m_instance, false );
    }

    CBuildingSAInterface *m_instance;
};

static void __cdecl _CBuilding__RemoveReferences( CBuildingSAInterface *building )
{
    Streamer::ForAllStreamerSectors( _buildingRefRemoval( building ), true, false, false, false, false );
}

static void __declspec(naked) _CBuilding__Delete( void )
{
    // We have to remove the building from the internal streamer
    __asm
    {
        push esi
        push edi
        mov edi,ecx
        call pft
        push edi
        call _CBuilding__RemoveReferences
        pop edi
        jmp pft2
    }
}

namespace Streamer
{
    /*=========================================================
        Streamer::ResetScanIndices

        Purpose:
            Goes through every single streamed in entity and
            sets its m_scanIndex to zero.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00563470
    =========================================================*/
    struct _resetScanIndex
    {
        void __forceinline OnSector( streamSectorEntry& sector )
        {
            for ( streamSectorEntry::ptrNode_t *iter = sector.GetList(); iter != NULL; iter = iter->m_next )
                iter->data->m_nScanCode = 0;
        }
    };

    void __cdecl ResetScanIndices( void )
    {
        ForAllStreamerSectors( _resetScanIndex(), true, true, true, true, true );
    }

    /*=========================================================
        Streamer::RequestStaticEntitiesOfSector

        Arguments:
            entryPtr - pointer to the streamer sector entry
            reqFlags - flags to pass to the RequestModel function
        Purpose:
            Loops through all streamed-in entities of a sector
            and requests the models of all static ones.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x0040C450
    =========================================================*/
    // Check whether we can request the model of said entity for special streaming.
    // Note that this function is only valid in the context of static building streaming.
    static inline bool IsEntityStreamable( CEntitySAInterface *entity )
    {
        // Only proceed if the model is not already loaded.
        if ( !Streaming::GetModelLoadInfo( entity->GetModelIndex() ).m_eLoading == MODEL_LOADED )
            return false;

        // Only process static entities.
        if ( !IS_ANY_FLAG( entity->m_entityFlags, ENTITY_STATIC ) )
            return false;

        // Entity has to reside in streamable area.
        if ( !Streaming::IsValidStreamingArea( entity->m_areaCode ) )
            return false;

        // Has to be marked as stream-able.
        if ( IS_ANY_FLAG( entity->m_entityFlags, ENTITY_NOSTREAM ) )
            return false;

        // Has to be marked as visible.
        if ( !IS_ANY_FLAG( entity->m_entityFlags, ENTITY_VISIBLE ) )
            return false;

        // The entity has passed general tests.
        return true;
    }

    // Decide whether we can request the model depending on a _heat-up_ phase.
    static inline bool PreRequest( CEntitySAInterface *entity, CBaseModelInfoSAInterface *model )
    {
        // If there is no time describing said entities' streaming time,
        // it is valid to stream it all the time.
        if ( ModelInfo::timeInfo *info = model->GetTimeInfo() )
        {
            // The time has to be right ;)
            if ( !Streaming::IsTimeRightForStreaming( info ) )
                return false;
        }

        // The_GTA: this appears to be a hack/bugfix by R*, since entities
        // could lose their RenderWare objects while they are still loaded.
        // Does not make sense to me if the system should be without limits.
        if ( model->pRwObject && !entity->GetRwObject() )
            entity->CreateRwObject();

        return true;
    }

    void __cdecl RequestStaticEntitiesOfSector( const streamSectorEntry& entryPtr, unsigned int reqFlags )
    {
        for ( const Streamer::entityLink_t *entry = entryPtr.GetList(); entry != NULL; entry = entry->m_next )
        {
            CEntitySAInterface *entity = entry->data;

            // Ignore certain entities whose scan code should be ignored.
            unsigned short applyScanCode = *(unsigned short*)0x00B7CD78;

            if ( entity->m_nScanCode != applyScanCode )
            {
                entity->m_nScanCode = applyScanCode;

                // Can we stream the entity at all?
                if ( !IsEntityStreamable( entity ) )
                    return;

                CBaseModelInfoSAInterface *model = entity->GetModelInfo();

                if ( PreRequest( entity, model ) )
                {
                    // Request it!
                    Streaming::RequestModel( entity->GetModelIndex(), reqFlags );
                }
            }
        }
    }

    /*=========================================================
        Streamer::RequestStaticEntities

        Arguments:
            entryPtr - pointer to the streamer sector entry
            reqFlags - flags to pass to the RequestModel function
        Purpose:
            Loops through all streamed-in entities of a sector
            and requests models matching the sector criteria.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x0040C270
    =========================================================*/
    void __cdecl RequestStaticEntities(
        const streamSectorEntry& entryPtr, float x, float y,
        float minX, float minY, float maxX, float maxY,
        float camFarClip, unsigned int reqFlags
    )
    {
        // We deal with squared distances because we save on sqrt operations
        // when calculating camera->entity distance later on.
        camFarClip *= camFarClip;

        for ( const Streamer::entityLink_t *entry = entryPtr.GetList(); entry != NULL; entry = entry->m_next )
        {
            CEntitySAInterface *entity = entry->data;

            // Ignore certain entities whose scan code should be ignored.
            unsigned short applyScanCode = *(unsigned short*)0x00B7CD78;

            if ( entity->m_nScanCode != applyScanCode )
            {
                entity->m_nScanCode = applyScanCode;

                // Can we stream the entity at all?
                if ( !IsEntityStreamable( entity ) )
                    return;

                CBaseModelInfoSAInterface *model = entity->GetModelInfo();
                CCameraSAInterface& camera = Camera::GetInterface();
                
                float scaledLODDistance = model->GetLODDistance() * camera.LODDistMultiplier;
                scaledLODDistance *= scaledLODDistance;

                // Maximize the used LOD distance so we avoid popping of buildings
                // when just using camera far clip distance.
                float highFarClip = std::max( scaledLODDistance, camFarClip );

                // Check whether the entity is inside our streaming quad.
                const CVector& entityPos = entity->Placeable.GetPosition();

                if ( entityPos.fX <= minX || entityPos.fY >= maxX ||
                     entityPos.fY <= minY || entityPos.fY >= maxY )
                    continue;

                // Calculate the squared entity->camera distance.
                float cameraEntityDistSquared = ( CVector2D( x, y ) - CVector2D( entityPos.fX, entityPos.fY ) ).LengthSquared();

                if ( cameraEntityDistSquared >= highFarClip )
                    continue;

                if ( PreRequest( entity, model ) )
                {
                    // Request it!
                    Streaming::RequestModel( entity->GetModelIndex(), reqFlags );
                }
            }
        }
    }

    /*=========================================================
        Streamer::RequestSquaredSectorEntities

        Arguments:
            reqPos - position to request around of
            reqFlags - flags to pass to the RequestModel function
        Purpose:
            Requests models of entities around the given reqPos
            position. The position given to it should be the
            camera's origin, since the request quad has camFarClip
            width and height without rotation.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x0040C520
    =========================================================*/
    static inline void IncrementScanCode( void )
    {
        unsigned int streamScanCode = *(unsigned short*)0x00B7CD78;

        if ( streamScanCode == 0xFFFF )
        {
            ResetScanIndices();

            // We start again with index 1.
            // 0 is reserved for new entities.
            *(unsigned short*)0x00B7CD78 = 1;
        }
        else
            *(unsigned short*)0x00B7CD78 += 1;
    }

    template <typename numType>
    inline numType CropDimensionMin( numType num )
    {
        return std::max( num, 0 );
    }

    template <typename numType>
    inline numType CropDimensionMax( numType num, const int arraySize )
    {
        return std::min( num, (numType)( arraySize - 1 ) );
    }

    template <typename numType>
    inline numType GetValidSectorCoord( numType num, const int arraySize )
    {
        return CropDimensionMax( CropDimensionMin( num ), arraySize );
    }

    template <typename arrayType>
    __forceinline arrayType& GetSectorEntry( arrayType sector[], const int arraySize, int x, int y )
    {
        // Even though the sector validity check may be redundant here, this is how R* defined it.
        // The optimizer has not removed it for some reason.
        return sector[ GetValidSectorCoord( x, arraySize ) + GetValidSectorCoord( y, arraySize ) * arraySize ];
    }

    template <typename callbackType>
    void __forceinline LoopArrayWorldSectors(
        float x, float y, float sectorSize, callbackType& cb
    )
    {
        // Create the request quad.
        // sectorSize is expected to be a positive number.
        float minX = x - sectorSize;
        float minY = y - sectorSize;
        float maxX = x + sectorSize;
        float maxY = y + sectorSize;

        // Get the sectorSize parameters in array cell width/height.
        // Since this is a width/height, we do not offset it by sectorCenterOffset.
        int sectorSizeArray = (int)( sectorSize * cb.GetPosStride() );

        // Get the array position of the center coordinates.
        int sectorCenterX = cb.CoordToIndex( x );
        int sectorCenterY = cb.CoordToIndex( y );

        // Get the coordinates for the sector scan.
        // These are the beginning units for the array reference.
        int sectorX = cb.CoordToIndex( minX );
        int sectorY = cb.CoordToIndex( minY );

        // The coordinates must start at 0
        // This enforces that streaming does not happen outside the GTA:SA boundaries.
        // The streaming arrays do not work otherwise anyway.
        sectorX = CropDimensionMin( sectorX );
        sectorY = CropDimensionMin( sectorY );

        // Now for the maximum sector coordinates.
        // These are the maximum units for the array reference.
        int sectorMaxX = cb.CoordToIndex( maxX );
        int sectorMaxY = cb.CoordToIndex( maxY );

        // The coordinates may not overshoot arraySize.
        // These sectors appear to be a*a quads.
        sectorMaxX = CropDimensionMax( sectorMaxX, cb.GetArraySize() );
        sectorMaxY = CropDimensionMax( sectorMaxY, cb.GetArraySize() );

        // Notify that we are about to enter loop.
        cb.Init();

        // Loop through the sector.
        for ( int iter_y = sectorY; iter_y <= sectorMaxY; iter_y++ )
        {
            for ( int iter_x = sectorX; iter_x <= sectorMaxX; iter_x++ )
            {
                // Notify the callback structure.
                cb.OnEntry( iter_x, iter_y, sectorCenterX, sectorCenterY, minX, minY, maxX, maxY, sectorSize, sectorSizeArray );
            }
        }
    }

    template <int arraySize, unsigned int maxPos>
    struct DefaultStreamerZoner
    {
        int __forceinline GetArraySize( void ) const
        {
            return arraySize;
        }

        double __forceinline GetPosStride( void ) const
        {
            // Because the selection rectangle expands from the middle of (x,y),
            // it can only extend to (maxPos / 2.0). arraySize / maxPos is the stride
            // for every item inside of the array.
            return (double)arraySize / maxPos / 2.0f;
        }

        double __forceinline GetSectorCenterOffset( void ) const
        {
            // Offset to center all positions into sector center.
            return (double)arraySize / 2.0f;
        }

        int __forceinline CoordToIndex( double coord ) const
        {
            return (int)( coord * GetPosStride() + GetSectorCenterOffset() );
        }

        double __forceinline IndexToCoord( int index ) const
        {
            return (double)( index - ( arraySize / 2 ) ) / GetPosStride() + GetSectorCenterOffset();
        }

        float __forceinline GetDistanceToZoneSquared( int x, int y, const CVector& worldPos )
        {
            return ( CVector2D( (float)IndexToCoord( x ), (float)IndexToCoord( y ) ) - CVector2D( worldPos[0], worldPos[1] ) ).Length();
        }
    };

    struct _requestSquared : DefaultStreamerZoner <NUM_StreamUpdateSectorRows, WORLD_BOUNDS>
    {
        __forceinline _requestSquared( unsigned int reqFlags, const CVector& reqPos ) : m_reqFlags( reqFlags ), m_reqPos( reqPos )
        {
        }

        void __forceinline Init( void )
        {
            // Increment the scan code to next valid.
            IncrementScanCode();
        }

        void __forceinline OnEntry( int x, int y, int centerX, int centerY, float minX, float minY, float maxX, float maxY, float sectorSize, int sectorSizeArray )
        {
            RequestStaticEntities( *( (const streamSectorEntry*)ARRAY_StreamUpdateSectors + x + y * NUM_StreamUpdateSectorRows ), m_reqPos.fX, m_reqPos.fY, minX, minY, maxX, maxY, sectorSize, m_reqFlags );
        }

        unsigned int m_reqFlags;
        const CVector& m_reqPos;
    };

    void __cdecl RequestSquaredSectorEntities( const CVector& reqPos, unsigned int reqFlags )
    {
        LoopArrayWorldSectors( reqPos.fX, reqPos.fY, *(float*)0x00B76848, _requestSquared( reqFlags, reqPos ) );
    }

    /*=========================================================
        Streamer::RequestAdvancedSquaredSectorEntities

        Arguments:
            reqPos - position to request around of
            reqFlags - flags to pass to the RequestModel function
        Purpose:
            Requests models of entities around the given reqPos
            position. The position given to it should be the
            camera's origin, since the request quad has camFarClip
            width and height without rotation.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x0040D3F0
        Note:
            This is a more sophisticated version of
            RequestSquaredSectorEntities, as it operates with
            streamer sector distances to abstract away far clip
            operations. They probably wanted to improve the engine's
            performance by using this function in special scenarios.
    =========================================================*/
    struct _requestSquaredFast : DefaultStreamerZoner <NUM_StreamStaticSectorRows, WORLD_BOUNDS>
    {
        __forceinline _requestSquaredFast( unsigned int reqFlags, const CVector& reqPos ) : m_reqFlags( reqFlags ), m_reqPos( reqPos )
        {
        }

        void __forceinline Init( void )
        {
            // Increment the scan code to next valid.
            IncrementScanCode();
        }

        void __forceinline OnEntry( int x, int y, int centerX, int centerY, float minX, float minY, float maxX, float maxY, float sectorSize, int sectorSizeArray )
        {
            // Provide the distance of the sector center to the current select array cell.
            // These variables are massively optimized in original GTA:SA.
            int sectorCenterToLeftDistance = centerX - x;
            int sectorCenterToTopDistance = centerY - y;

            // Get the request entries of interest.
            const streamStaticSectorEntry& staticSelector = GetSectorEntry( (const streamStaticSectorEntry*)ARRAY_StreamStaticSectors, NUM_StreamStaticSectorRows, x, y );
            const streamDynamicSectorEntry& dynamicSelector = GetSectorEntry( (const streamDynamicSectorEntry*)ARRAY_StreamDynamicSectors, NUM_StreamDynamicSectorRows, x, y );

            // * We request them in this order.
            const streamSectorEntry& building = staticSelector.building;
            const streamSectorEntry& ped = dynamicSelector.ped;
            const streamSectorEntry& dummy = staticSelector.dummy;

            // Check whether we should request with high quality or not.
            // This function ain't chicken wuss, you see.
            // Learn true Kappa by seeing what this optimizes to.
            if ( ( sectorCenterToLeftDistance * sectorCenterToLeftDistance + sectorCenterToTopDistance * sectorCenterToTopDistance ) <= ( sectorSizeArray * sectorSizeArray ) )
            {
                // We can optimize this sauce, because (x,y) is assumed to be the camera position.
                // That removes the need for far clip checks.
                RequestStaticEntitiesOfSector( building, m_reqFlags );
                RequestStaticEntitiesOfSector( ped, m_reqFlags );
                RequestStaticEntitiesOfSector( dummy, m_reqFlags );
            }
            else
            {
                // Go through the entities of said sector carefully.
                RequestStaticEntities( building, m_reqPos.fX, m_reqPos.fY, minX, minY, maxX, maxY, sectorSize, m_reqFlags );
                RequestStaticEntities( ped, m_reqPos.fX, m_reqPos.fY, minX, minY, maxX, maxY, sectorSize, m_reqFlags );
                RequestStaticEntities( dummy, m_reqPos.fX, m_reqPos.fY, minX, minY, maxX, maxY, sectorSize, m_reqFlags );
            }

            // All in all, this is a sophisticated quad-tree engine.
            // ... a function a day keeps the worries away ...
        }

        unsigned int m_reqFlags;
        const CVector& m_reqPos;
    };

    void __cdecl RequestAdvancedSquaredSectorEntities( const CVector& reqPos, unsigned int reqFlags )
    {
        float sectorSize = ( *(unsigned int*)VAR_currArea == 0 ) ? 80.0f : 40.0f;

        LoopArrayWorldSectors( reqPos.fX, reqPos.fY, sectorSize, _requestSquaredFast( reqFlags, reqPos ) );
    }

    /*=========================================================
        Streamer::SelectWorldSector

        Arguments:
            column - y offset inside the global sector array
            row - x offset inside the global sector array
        Purpose:
            Selects the world streaming sector that should be used
            for looping.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00553540
    =========================================================*/
    unsigned int GetRepeatedStreamerArrayIndex( int column, int row, int numCols, int numRows )
    {
        return (unsigned int)( ( (unsigned int)column % (unsigned int)numCols ) + ( (unsigned int)row % (unsigned int)numRows ) * (unsigned int)numCols );
    }

#define NUM_LINKED_SECTORS  5

    struct sectorialLink_t
    {
        // Stores building, object, vehicle, ped and dummy sectors (if available).
        // Found at 0x00C8E0C8
        streamSectorEntry *sectors[NUM_LINKED_SECTORS];
    };

    void __cdecl SelectWorldSector( int column, int row )
    {
        // The dynamic array is repeated across the world.
        int arrayIndex = GetRepeatedStreamerArrayIndex( column, row, NUM_StreamDynamicSectorCols, NUM_StreamDynamicSectorRows );

        streamDynamicSectorEntry& dynamicSector = *( (streamDynamicSectorEntry*)ARRAY_StreamDynamicSectors + arrayIndex );
        streamStaticSectorEntry *staticSector = NULL;

        if ( column >= 0 && row >= 0 && column < NUM_StreamStaticSectorCols && row < NUM_StreamStaticSectorRows )
            staticSector = &GetSectorEntry( (streamStaticSectorEntry*)ARRAY_StreamStaticSectors, NUM_StreamStaticSectorRows, column, row );

        // Set up the global link.
        sectorialLink_t& link = *(sectorialLink_t*)0x00C8E0C8;
        link.sectors[0] = staticSector ? &staticSector->building : NULL;
        link.sectors[1] = &dynamicSector.object;
        link.sectors[2] = &dynamicSector.vehicle;
        link.sectors[3] = &dynamicSector.ped;
        link.sectors[4] = staticSector ? &staticSector->dummy : NULL;
    }

    /*=========================================================
        Streamer::SectorRenderCallback

        Arguments:
            column - x offset inside the global sector array
            row - y offset inside the global sector array
        Purpose:
            Renders all entities inside of the streamer sector
            that is denoted by the coordinates. The streamer sector
            is taken from the global entity streaming array.
        Binary offsets:
            (1.0 US and 1.0 EU): 0x00554840
    =========================================================*/
    template <typename callbackType>
    void __forceinline LoopSectorEntities( int column, int row, callbackType& cb )
    {
        bool unimportantSector = false;

        float zoneDistanceSquared = cb.GetDistanceToZoneSquared( column, row, *(CVector*)0x00B76870 );

        if ( zoneDistanceSquared >= ( 100 * 100 ) ||
             fabs( NormalizeRadians( atan2( zoneDistanceSquared, -zoneDistanceSquared ) - *(float*)0x00B7684C ) ) < 0.36f )
        {
            unimportantSector = true;
        }

        SelectWorldSector( column, row );

        const sectorialLink_t& link = *(sectorialLink_t*)0x00C8E0C8;

        for ( unsigned int n = 0; n < NUM_LINKED_SECTORS; n++ )
        {
            streamSectorEntry *sectorEntry = link.sectors[n];

            if ( sectorEntry )
            {
                streamSectorEntry::ptrNode_t *node = sectorEntry->GetList();

                while ( node != NULL )
                {
                    CEntitySAInterface *entity = node->data;

                    node = node->m_next;

                    unsigned short applyScanCode = *(unsigned short*)0x00B7CD78;

                    if ( entity->m_nScanCode != applyScanCode )
                    {
                        entity->m_nScanCode = applyScanCode;

                        cb.OnEntity( entity, unimportantSector );
                    }
                }
            }
        }
    }

    inline bool IsLoaderTooBusy( void )
    {
        return *(bool*)0x00B76850 && *(unsigned int*)0x008E4CB8 >= 1;
    }

    // Temporary definitions (to avoid even bigger commit)
    namespace EntityRender
    {
        int __cdecl SetupEntityVisibility( CEntitySAInterface *entity, float& camDistance )
        {
            return ((int (__cdecl*)( CEntitySAInterface *entity, float& camDistance ))0x00554230)( entity, camDistance );
        }

        void __cdecl PushEntityOnRenderQueue( CEntitySAInterface *entity, float camDistance )
        {
            ((void (__cdecl*)( CEntitySAInterface *eneity, float camDistance ))0x005534B0)( entity, camDistance );
        }
    };

    enum eRenderType : unsigned int
    {
        ENTITY_RENDER_CROSS_ZONES,
        ENTITY_RENDER_DEFAULT,
        ENTITY_RENDER_CONTROVERIAL,
        ENTITY_RENDER_REQUEST_MODEL
    };

    struct _entityZoneRender : DefaultStreamerZoner <NUM_StreamStaticSectorRows, WORLD_BOUNDS>
    {
        void __forceinline OnEntity( CEntitySAInterface *entity, bool unimportantSector )
        {
            entity->m_entityFlags &= ~ENTITY_OFFSCREEN;

            float camDistance;

            switch( EntityRender::SetupEntityVisibility( entity, camDistance ) )
            {
            case ENTITY_RENDER_REQUEST_MODEL:
                if ( *(bool*)0x009654B0 || !entity->IsOnScreen() || *(bool*)0x00B76851 )
                    return;

                if ( Streaming::GetModelLoadInfo( entity->GetModelIndex() ).m_eLoading != MODEL_LOADED )
                {
                    if ( !entity->CheckScreenValidity() && unimportantSector )
                    {
                        *(bool*)0x00B76850 = true;
                    }
                    else
                        unimportantSector = false;
                }
                
                if ( !unimportantSector && IsLoaderTooBusy() )
                    return;

                Streaming::RequestModel( entity->GetModelIndex(), 0 );
                break;
            case ENTITY_RENDER_DEFAULT:
                EntityRender::PushEntityOnRenderQueue( entity, camDistance );
                break;
            case ENTITY_RENDER_CROSS_ZONES:
                if ( entity->nType != ENTITY_TYPE_OBJECT )
                    return;

                {
                    CAtomicModelInfoSA *atomicInfo = entity->GetModelInfo()->GetAtomicModelInfo();

                    if ( !atomicInfo )
                        return;

                    if ( atomicInfo->atomicType != 1 && atomicInfo->atomicType != 3 )
                        return;
                }

            case ENTITY_RENDER_CONTROVERIAL:
                entity->m_entityFlags |= ENTITY_OFFSCREEN;

                if ( !IS_ANY_FLAG( entity->m_entityFlags, ENTITY_PRERENDERED ) )
                    return;

                const CVector& camDist = *(CVector*)0x00B76870;
                const CVector& entityPos = entity->Placeable.GetPosition();

                float unkDist = 30.0f;

                if ( entity->nType == ENTITY_TYPE_VEHICLE && ((CVehicleSAInterface*)entity)->m_nVehicleFlags.bFireGun )
                    unkDist = 200.0f;

                if ( CBounds2D( unkDist, -unkDist, -unkDist, unkDist ).IsInside( CVector2D( camDist.fX - entityPos.fX, camDist.fY - entityPos.fY ) ) )
                    return;

                // TEMPORARY CHANGE HERE.
                unsigned int lodCount = *(unsigned int*)0x00B7683C;

                if ( lodCount < 1000 )
                {
                    ( (CEntitySAInterface**)0x00B745D8 )[ lodCount++ ] = entity;
                    
                    *(unsigned int*)0x00B7683C = lodCount;
                }
                break;
            }
        }
    };

    static void __cdecl SectorRenderCallback( int column, int row )
    {
        LoopSectorEntities( column, row, _entityZoneRender() );
    }
};

void Streamer_Init( void )
{
    // Hook utility functions.
    HookInstall( 0x00563470, (DWORD)Streamer::ResetScanIndices, 5 );
    HookInstall( 0x0040C450, (DWORD)Streamer::RequestStaticEntitiesOfSector, 5 );
    HookInstall( 0x0040C520, (DWORD)Streamer::RequestSquaredSectorEntities, 5 );
    HookInstall( 0x0040D3F0, (DWORD)Streamer::RequestAdvancedSquaredSectorEntities, 5 );
    HookInstall( 0x00554840, (DWORD)Streamer::SectorRenderCallback, 5 );

    // Hook special fixes
    HookInstall( 0x00404130, (DWORD)_CBuilding__Delete, 5 );
}

void Streamer_Shutdown( void )
{

}