/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CCacheSA.cpp
*  PURPOSE:     Engine caching system
*  DEVELOPERS:  Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>

extern CBaseModelInfoSAInterface **ppModelInfo;

// Available cache modes.
enum eCacheMode : unsigned int
{
    CACHE_DISABLED, // do nothing
    CACHE_READING,  // read data from game structures
    CACHE_WRITING   // set data to game structures
};

// To finish this CINFO.BIN caching system I have to wait for Jusonex's input.
// He has made important work on the IPL building loader, which is tied to the CINFO.BIN caching.
// I want to implement his work. (TODO)

/*=========================================================
    _Cache_Init

    Purpose:
        Initializes the collision caching architecture.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B31A0
=========================================================*/
// If you change any of the following definitions, you may loose support
// to the PS2 version of GTA:SA.
// This is a value compatible with MAX_COL_FILES and MAX_IPL_FILES.
#define MAX_COLLISION_AREA_ENTRIES          256

static unsigned int *const VAR_maximumNumberOfCollisions_atLoad = (unsigned int*)0x00BC409C;    // localizable

static unsigned char **const VAR_cachedAreaModelCount = (unsigned char**)0x00BC4098;            // localizable
static unsigned int *const VAR_cachedAreaModelCountSize = (unsigned int*)0x00BC40B4;            // localizable

struct cachedColFile  //size: 24
{
    cachedColFile( void )
    {
        m_bounds.m_minX = MAX_REGION;
        m_bounds.m_maxY = -MAX_REGION;
        m_bounds.m_maxX = MAX_REGION;
        m_bounds.m_minY = -MAX_REGION;
    }

    CBounds2D               m_bounds;       // 0
    short                   m_rangeStart;   // 16
    short                   m_rangeEnd;     // 18
    
    union
    {
        unsigned int        m_flags;        // 20

        struct
        {
            bool            m_isProcedural; // 20
            bool            m_isInterior;   // 21
        };
    };
};

struct cachedIPLFile    //size: 52
{
    cachedIPLFile( void )
    {
        m_bounds.m_minX = MAX_REGION;
        m_bounds.m_maxY = -MAX_REGION;
        m_bounds.m_maxX = MAX_REGION;
        m_bounds.m_minY = -MAX_REGION;
    }

    CBounds2D       m_bounds;               // 0
    char            m_name[18];             // 16

    short           m_buildingRangeStart;   // 34
    short           m_buildingRangeEnd;     // 36

    short           m_dummyRangeStart;      // 38
    short           m_dummyRangeEnd;        // 40

    short           m_iplId;                // 42

    bool            m_isInterior;           // 44
    bool            m_isLoaded;             // 45
    bool            m_streamingSectorLoad;  // 46, set by streaming to indicate that we want to load it
    bool            m_doNotStream;          // 47, 1
    bool            m_terminateAtUnload;    // 48
    bool            m_containsFarChunks;    // 49, is true if LOD distance of any instance is greater than 150.0f

    WORD            m_pad;                  // 50
};

static cachedIPLFile **const VAR_cachedIPLFiles = (cachedIPLFile**)0x00BC4094;                  // localizable

static cachedColFile **const VAR_cachedColFiles = (cachedColFile**)0x00BC4090;                  // localizable
static unsigned int *const VAR_cachedColFilesSize = (unsigned int*)0x00BC40B8;                  // localizable

struct cachedGameProperty //size: 20
{
    int                     m_buildingIndex;    // 0
    int                     m_lodBuildingIndex; // 4
    modelId_t               m_primaryModel;     // 8
    modelId_t               m_secondaryModel;   // 12

    union
    {
        unsigned int        m_flags;            // 16

        struct
        {
            bool            m_underwater : 1;   // 16
            bool            m_colLink : 1;      
            bool            m_normalLOD : 1;    
            bool            m_drawAdditive : 1; 
        };
    };
};

static cachedGameProperty **const VAR_cachedGameProperties = (cachedGameProperty**)0x00BC40AC;  // localizable
static unsigned int *const VAR_cachedGamePropertyIndex = (unsigned int*)0x00BC40B0;             // localizable

struct cachedCollisionEntry //size: 48
{
    CBoundingBox            m_bounds;               // 0, colModel bounds
    unsigned int            m_model;                // 40, model index
    unsigned char           m_colFile;              // 44, collision pool index
    bool                    m_isCollidable : 1;     // 45
    BYTE                    m_pad[2];               // 46
};

static eCacheMode *const VAR_collisionCacheStatus = (eCacheMode*)0x00BC40A0;                    // localizable
static cachedCollisionEntry **const VAR_cachedCollisionArray = (cachedCollisionEntry**)0x00BC40A4;  // localizable
static unsigned int *const VAR_cachedCollisionCount = (unsigned int*)0x00BC40A8;                // localizable

void __cdecl _Cache_Init( void )
{
    // todo: reimplement the commented-out code about cachedCollisionEntry.
    // it was commented out by R* to simplify modding of the game on PC.
    // the code is still present on the PS2 version.

    // Store the number of loaded collisions at load.
    *VAR_maximumNumberOfCollisions_atLoad = Pools::GetColModelPool()->GetMax();

    // Allocate management structures.
    *VAR_cachedAreaModelCount = new unsigned char[MAX_COLLISION_AREA_ENTRIES];
    *VAR_cachedIPLFiles = new cachedIPLFile[MAX_COLLISION_AREA_ENTRIES];
    *VAR_cachedColFiles = new cachedColFile[MAX_COLLISION_AREA_ENTRIES];
}

/*=========================================================
    _Cache_Shutdown

    Purpose:
        Saves important details about all currently loaded
        collision entries in a CINFO.BIN file and terminates
        the collision caching environment.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2AD0
=========================================================*/
void __forceinline WriteIfAvailable( CFile *file, const void *data, size_t size )
{
    // Write buffer data only if buffer is available.
    if ( data )
        file->Write( data, 1, size );
    else
    {
        // Otherwise zero out.
        char buf[4096];
        memset( buf, 0, sizeof(buf) );

        for ( size_t n = 0; n < size; n += sizeof(buf) )
        {
            size_t writeCount = std::min( sizeof(buf), size - n );

            if ( file->Write( buf, 1, writeCount ) != writeCount )
                break;
        }
    }
}

template <typename arrayType>
void __forceinline WriteArrayIfAvailable( CFile *file, const arrayType data[], unsigned int elemCount )
{
    if ( elemCount == 0 || !data )
    {
        file->WriteInt( 0 );
        return;
    }

    file->Write( &elemCount, 1, sizeof(elemCount) );
    file->Write( data, sizeof(arrayType), elemCount );
}

template <typename deleteType>
void __forceinline SafeDelete( deleteType*& delPtr )
{
    if ( deleteType *delPtrDirect = delPtr )
    {
        delete delPtrDirect;
        delPtr = NULL;
    }
}

void __cdecl _Cache_Shutdown( void )
{
    // Get all the important pointers.
    cachedCollisionEntry *cachedCollisionArray = *VAR_cachedCollisionArray;
    unsigned char *cachedAreaModelCountArray = *VAR_cachedAreaModelCount;
    cachedIPLFile *cachedAreaEntryArray = *VAR_cachedIPLFiles;
    cachedColFile *cachedColFilesArray = *VAR_cachedColFiles;
    cachedGameProperty *cachedGamePropertyArray = *VAR_cachedGameProperties;

    // Write the collision information if ready.
    // This means that we have read all game structures and are now writing the cache.
    if ( *VAR_collisionCacheStatus == CACHE_READING )
    {
        // The_GTA: in the original function the R* file functions were used.
        // Instead, I am using CFileSystem, the MTA:Eir implementation.
        CFile *cacheFile = gameFileRoot->Open( "MODELS\\\\CINFO.BIN", "wb" );

        // Bugfix: account for creation failure.
        if ( cacheFile )
        {
            // Write the engine status.
            // We can restore to the engine status this way.
            WriteArrayIfAvailable( cacheFile, cachedCollisionArray, *VAR_cachedCollisionCount );
            WriteArrayIfAvailable( cacheFile, cachedAreaModelCountArray, *VAR_cachedAreaModelCountSize );

            WriteIfAvailable( cacheFile, cachedAreaEntryArray, sizeof(*cachedAreaEntryArray) * MAX_COLLISION_AREA_ENTRIES );

            WriteArrayIfAvailable( cacheFile, cachedColFilesArray, *VAR_cachedColFilesSize );
            WriteArrayIfAvailable( cacheFile, cachedGamePropertyArray, *VAR_cachedGamePropertyIndex );

            delete cacheFile;
        }
    }

    // Free all resources if they have been used.
    SafeDelete( cachedGamePropertyArray );
    SafeDelete( cachedCollisionArray );
    SafeDelete( cachedAreaModelCountArray );
    SafeDelete( cachedAreaEntryArray );
    SafeDelete( cachedColFilesArray );
}

/*=========================================================
    Cache_IsWritingMode

    Purpose:
        Returns whether the cache is in writing mode. Writing
        mode means that game entities should be restored to
        a prior engine status by reading from the cache structs.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2AC0
=========================================================*/
bool __cdecl Cache_IsWritingMode( void )
{
    return *VAR_collisionCacheStatus == CACHE_WRITING;
}

/*=========================================================
    Cache_StoreCollision

    Arguments:
        id - model index to store
        col - collision model interface to take details from
    Purpose:
        Stores details about a given collision undes the model
        index in an allocated array.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2C20
    Note:
        The necessary functions for reading from the CINFO.bin
        must have been commented out by Rockstar. This cache
        system allows the map creator to cache the bounding
        boxes of all collision models. Using this feature
        conflicts modding, so R* prolly disabled it prior
        release.
=========================================================*/
void __cdecl Cache_StoreCollision( unsigned int id, CColModelSAInterface *col )
{
    // Only execute if we are in the correct phase.
    if ( *VAR_collisionCacheStatus != CACHE_READING )
        return;

    // Save important details about our collision.
    cachedCollisionEntry& cacheEntry = (*VAR_cachedCollisionArray)[*VAR_cachedCollisionCount];
    cacheEntry.m_bounds = col->m_bounds;
    cacheEntry.m_model = id;
    cacheEntry.m_colFile = col->m_colPoolIndex;
    cacheEntry.m_isCollidable = col->m_isCollidable;

    // Increase the array size.
    (*VAR_cachedCollisionCount)++;
}

/*=========================================================
    Cache_WriteCollision

    Purpose:
        Goes through all cached collision entries and sets
        their properties to the game model collisions,
        effectively restoring the engine collision system
        to a prior status.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2CC0
=========================================================*/
void __cdecl Cache_WriteCollision( void )
{
    if ( !Cache_IsWritingMode() )
        return;

    unsigned int max = *VAR_cachedCollisionCount;

    for ( unsigned int n = 0; n < max; n++ )
    {
        const cachedCollisionEntry& colEntry = (*VAR_cachedCollisionArray)[n];

        // Get the model info the collision entry is pointing to.
        CBaseModelInfoSAInterface *model = ppModelInfo[colEntry.m_model];

        // Allocate a new collision interface, which we populate.
        CColModelSAInterface *colModel = new CColModelSAInterface;
        colModel->m_bounds = colEntry.m_bounds;
        colModel->m_colPoolIndex = colEntry.m_colFile;
        colModel->m_isCollidable = colEntry.m_isCollidable;

        // Update the collision of the model.
        // Note that we assume that no collision was loaded prior to this.
        model->SetColModel( colModel, true );

        // Update the collision boundaries, which is faster than
        // scanning the whole collision library.
        RegisterCOLLibraryModel( colEntry.m_colFile, colEntry.m_model );
    }
}

/*=========================================================
    Cache_RestoreColFile

    Arguments:
        colFile - the colFile to restore data to
    Purpose:
        Writes data to the given colFile from our cache
        array sequentially.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2E60
=========================================================*/
void __cdecl Cache_RestoreColFile( CColFileSA *colFile )
{
    const cachedColFile& colFileEntry = (*VAR_cachedColFiles)[*VAR_cachedColFilesSize];

    colFile->m_bounds = colFileEntry.m_bounds;
    colFile->m_range.start = colFileEntry.m_rangeStart;
    colFile->m_range.end = colFileEntry.m_rangeEnd;
    colFile->m_isProcedural = colFileEntry.m_isProcedural;
    colFile->m_isInterior = colFileEntry.m_isInterior;

    // Increment the sequential index.
    (*VAR_cachedColFilesSize)++;
}

/*=========================================================
    Cache_StoreColFile

    Arguments:
        colFile - the colFile to save in our database
    Purpose:
        Stores the data from the given collision file in our
        cache array so that it can be restored from in a later
        cached write-action.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2DD0
=========================================================*/
void __cdecl Cache_StoreColFile( CColFileSA colFile )
{
    // Store data in our cache.
    cachedColFile& colFileEntry = (*VAR_cachedColFiles)[*VAR_cachedColFilesSize];

    colFileEntry.m_bounds = colFile.m_bounds;
    colFileEntry.m_rangeStart = colFile.m_range.start;
    colFileEntry.m_rangeEnd = colFile.m_range.end;
    colFileEntry.m_isProcedural = colFile.m_isProcedural;
    colFileEntry.m_isInterior = colFile.m_isInterior;

    // Increment the sequential index.
    (*VAR_cachedColFilesSize)++;
}

/*=========================================================
    Cache_RestoreIPLFile

    Arguments:
        iplFile - pointer to write IPL sector information to
        iplIndex - index of the IPL sector
    Purpose:
        Restores data about a given IPL sector from the persistent
        cache.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2EF0
=========================================================*/
void __cdecl Cache_RestoreIPLFile( CIPLFileSA& iplFile, unsigned int iplIndex )
{
    const cachedIPLFile& iplFileEntry = (*VAR_cachedIPLFiles)[ iplIndex ];

    iplFile.m_bounds = iplFileEntry.m_bounds;
    memcpy( iplFile.m_name, iplFileEntry.m_name, sizeof( iplFile.m_name ) );
    iplFile.m_buildingRange.start = iplFileEntry.m_buildingRangeStart;
    iplFile.m_buildingRange.end = iplFileEntry.m_buildingRangeEnd;
    iplFile.m_dummyRange.start = iplFileEntry.m_dummyRangeStart;
    iplFile.m_dummyRange.end = iplFileEntry.m_dummyRangeEnd;
    iplFile.m_iplId = iplFileEntry.m_iplId;
    iplFile.m_isInterior = iplFileEntry.m_isInterior;
    iplFile.m_isLoaded = iplFileEntry.m_isLoaded;
    iplFile.m_streamingSectorLoad = iplFileEntry.m_streamingSectorLoad;
    iplFile.m_doNotStream = iplFileEntry.m_doNotStream;
    iplFile.m_terminateAtUnload = iplFileEntry.m_terminateAtUnload;
    iplFile.m_containsFarChunks = iplFileEntry.m_containsFarChunks;
    iplFile.m_pad = iplFileEntry.m_pad;
}

/*=========================================================
    Cache_StoreIPLFile

    Arguments:
        iplIndex - index of the IPL sector
        iplFile - the IPL sector to store into the cache
    Purpose:
        Writes data about the IPL sector into the persistent
        cache. The engine can restore from that data on next
        boot.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2ED0
=========================================================*/
void __cdecl Cache_StoreIPLFile( unsigned int iplIndex, CIPLFileSA iplFile )
{
    cachedIPLFile& iplFileEntry = (*VAR_cachedIPLFiles)[ iplIndex ];

    iplFileEntry.m_bounds = iplFile.m_bounds;
    memcpy( iplFileEntry.m_name, iplFile.m_name, sizeof( iplFileEntry.m_name ) );
    iplFileEntry.m_buildingRangeStart = iplFile.m_buildingRange.start;
    iplFileEntry.m_buildingRangeEnd = iplFile.m_buildingRange.end;
    iplFileEntry.m_dummyRangeStart = iplFile.m_dummyRange.start;
    iplFileEntry.m_dummyRangeEnd = iplFile.m_dummyRange.end;
    iplFileEntry.m_iplId = iplFile.m_iplId;
    iplFileEntry.m_isInterior = iplFile.m_isInterior;
    iplFileEntry.m_isLoaded = iplFile.m_isLoaded;
    iplFileEntry.m_streamingSectorLoad = iplFile.m_streamingSectorLoad;
    iplFileEntry.m_doNotStream = iplFile.m_doNotStream;
    iplFileEntry.m_terminateAtUnload = iplFile.m_terminateAtUnload;
    iplFileEntry.m_containsFarChunks = iplFile.m_containsFarChunks;
    iplFileEntry.m_pad = iplFile.m_pad;
}

/*=========================================================
    Cache_LoadCollision

    Purpose:
        Preloads collision instances for caching purposes.
        In here it decides whether cached data is used or
        the collision libraries are read completely.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004113D0
=========================================================*/
void __cdecl Cache_LoadCollision( void )
{
    if ( Cache_IsWritingMode() )
    {
        Cache_WriteCollision();
    }
    else
    {
        Collision_Preload();
    }
}

/*=========================================================
    Cache_IncrementBySector

    Arguments:
        buildingArray - array of buildings for the sector
    Purpose:
        Optimization routine used to apply properties to
        buildings and models depending on the area's cached
        properties.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B2F10
    Note:
        This is an efficient way to cache model, entity and
        collision properties at the cost of modability.
=========================================================*/
void __cdecl Cache_IncrementBySector( CEntitySAInterface **buildingArray )
{
    eCacheMode status = *VAR_collisionCacheStatus;

    // Dispatch by current game loading process status.
    if ( status == CACHE_WRITING )
    {
        unsigned int max = *VAR_cachedAreaModelCount[*VAR_cachedAreaModelCountSize];
        unsigned int n = *VAR_cachedGamePropertyIndex;

        for ( ; n < max; n++ )
        {
            const cachedGameProperty& linkEntry = (*VAR_cachedGameProperties)[n];

            if ( linkEntry.m_underwater )
            {
                // Set selected building as underwater.
                buildingArray[linkEntry.m_lodBuildingIndex]->m_entityFlags |= ENTITY_UNDERWATER;
            }

            if ( linkEntry.m_drawAdditive )
            {
                // Set primary model to additive rendering.
                ppModelInfo[linkEntry.m_primaryModel]->bDrawAdditive = true;
            }

            if ( linkEntry.m_normalLOD )
            {
                // Set draw distance to 400.0f
                ppModelInfo[linkEntry.m_primaryModel]->fLodDistanceUnscaled = 400.0f;
            }

            if ( linkEntry.m_colLink )
            {
                // Inherit collision interfaces.
                CBaseModelInfoSAInterface *primary = ppModelInfo[linkEntry.m_primaryModel];
                CBaseModelInfoSAInterface *secondary = ppModelInfo[linkEntry.m_secondaryModel];

                secondary->UnsetColModel();
                secondary->SetColModel( primary->pColModel, false );
            }
        }
        // Update current selector index.
        *VAR_cachedGamePropertyIndex = n;

        (*VAR_cachedAreaModelCountSize)++;
    }
    else if ( status == CACHE_READING )
    {
        *VAR_cachedAreaModelCount[*VAR_cachedAreaModelCountSize] = *VAR_cachedGamePropertyIndex;

        (*VAR_cachedAreaModelCountSize)++;
    }
}

/*=========================================================
    Cache_WriteSectorInformation

    Arguments:
        buildingArray - array of buildings for the sector
        lodInstanceIndex - index to the LOD building
        buildingInstanceIndex - index to the actual building
    Purpose:
        Used by the engine to store information about the
        game world during startup.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x005B3040
=========================================================*/
void __cdecl Cache_WriteSectorInformation( CEntitySAInterface **buildingArray, int lodInstanceIndex, int buildingInstanceIndex )
{
    eCacheMode status = *VAR_collisionCacheStatus;

    if ( status == CACHE_READING )
    {
        // Write a cached entry.
        cachedGameProperty prop;

        CEntitySAInterface *building = buildingArray[ buildingInstanceIndex ];
        CEntitySAInterface *lodBuilding = building->m_pLod; // note: must not be NULL.

        prop.m_buildingIndex = buildingInstanceIndex;
        prop.m_flags = 0;   // reset flags.

        prop.m_primaryModel = building->GetModelIndex();
        prop.m_secondaryModel = lodBuilding->GetModelIndex();

        if ( lodBuilding->numLodChildren <= 0 )
        {
            CBaseModelInfoSAInterface *modelInfo = building->GetModelInfo();
            CCameraSAInterface& camera = Camera::GetInterface();
            float scaledLODDistance = modelInfo->GetLODDistance() * camera.LODDistMultiplier;

            if ( scaledLODDistance > EntityRender::GetGlobalDrawDistance() )
            {
                prop.m_drawAdditive = true;
            }
        }

        prop.m_lodBuildingIndex = -1;

        if ( lodInstanceIndex > 0 )
        {
            int lodIndex = 0;

            do
            {
                if ( buildingArray[ lodIndex ] == lodBuilding )
                {
                    prop.m_lodBuildingIndex = lodIndex;
                    break;
                }
            } while ( lodIndex < lodInstanceIndex );
        }

        CBaseModelInfoSAInterface *primaryModel = building->GetModelInfo();
        CBaseModelInfoSAInterface *secondaryModel = lodBuilding->GetModelInfo();

        if ( lodBuilding->numLodChildren == 1 )
        {
            if ( building->bUnderwater )
            {
                prop.m_underwater = true;
            }

            if ( primaryModel->pColModel != secondaryModel->pColModel && primaryModel->pColModel != NULL )
            {
                prop.m_colLink = true;
            }
        }
        else if ( primaryModel->bDrawAdditive )
        {
            prop.m_normalLOD = true;
        }

        // Only write entry if a change has been detected.
        if ( prop.m_flags != 0 )
        {
            (*VAR_cachedGameProperties)[ (*VAR_cachedGamePropertyIndex)++ ] = prop;
        }
    }
}

void Cache_Init( void )
{
    // Hook stuff.
    HookInstall( 0x005B31A0, (DWORD)_Cache_Init, 5 );
    HookInstall( 0x005B2AD0, (DWORD)_Cache_Shutdown, 5 );
    HookInstall( 0x005B2C20, (DWORD)Cache_StoreCollision, 5 );
    HookInstall( 0x005B2CC0, (DWORD)Cache_WriteCollision, 5 );
    HookInstall( 0x005B2E60, (DWORD)Cache_RestoreColFile, 5 );
    HookInstall( 0x005B2DD0, (DWORD)Cache_StoreColFile, 5 );
    HookInstall( 0x005B2EF0, (DWORD)Cache_RestoreIPLFile, 5 );
    HookInstall( 0x005B2ED0, (DWORD)Cache_StoreIPLFile, 5 );
    HookInstall( 0x004113D0, (DWORD)Cache_LoadCollision, 5 );
    HookInstall( 0x005B2F10, (DWORD)Cache_IncrementBySector, 5 );
    HookInstall( 0x005B3040, (DWORD)Cache_WriteSectorInformation, 5 );
}

void Cache_Shutdown( void )
{
}