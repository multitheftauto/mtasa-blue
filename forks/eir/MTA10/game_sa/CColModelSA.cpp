/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/CColModelSA.cpp
*  PURPOSE:     Collision model entity
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               arc_
*               Martin Turski <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "gamesa_renderware.h"

extern CBaseModelInfoSAInterface **ppModelInfo;


CColModelSAInterface::CColModelSAInterface( void )
{
    _asm
    {
        mov     ecx,this
        mov     eax,FUNC_CColModel_Constructor
        call    eax
    }
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F700
CColModelSAInterface::~CColModelSAInterface( void )
{
    if ( m_releaseDataOnDestroy )
        ReleaseData();
}

void CColModelSAInterface::_DestructorHook( void )
{
    this->~CColModelSAInterface();
}

// Used for vehicle models so that they use non-segmented collision data.
// This prevents a memory leak when allocating and using runtime-created suspension line data.
// It is important to call this function on every vehicle collision interface.
void CColModelSAInterface::UnsegmentizeData( void )
{
    __asm
    {
        mov eax,0x0040F740
        call eax
    }
}

// Binary offsets: (1.0 US and 1.0 EU): 0x0040F9E0
void __thiscall CColModelSAInterface::ReleaseData( void )
{
    CColDataSA *colData = pColData;

    if ( colData )
    {
        if ( this->m_isColDataSegmented )
        {
            colData->SegmentedClear();

            RwFree( colData );
        }
        else
        {
            colData->UnsegmentedClear();

            _freeMemGame( colData );
        }

        pColData = NULL;
    }
}

void* CColModelSAInterface::operator new( size_t )
{
    return Pools::GetColModelPool()->Allocate();
}

void CColModelSAInterface::operator delete( void *ptr )
{
    Pools::GetColModelPool()->Free( (CColModelSAInterface*)ptr );
}

CColDataSA::CColDataSA( void )
{
    // Binary offsets: (1.0 US and 1.0 EU): 0x0040F030
    numSpheres = 0;
    numBoxes = 0;
    numTriangles = 0;
    ucNumWheels = 0;

    unkFlag1 = false;
    hasFaceGroups = false;
    hasShadowMeshFaces = false;

    pColSpheres = NULL;
    pColBoxes = NULL;
    pSuspensionLines = NULL;
    pColVertices = NULL;
    pColTriangles = NULL;
    pColTrianglePlanes = NULL;

    numShadowMeshFaces = 0;
    numShadowMeshVertices = 0;

    pShadowMeshVertices = NULL;
    pShadowMeshFaces = NULL;
}

/*=========================================================
    Collision_Preload

    Purpose:
        Preloads the collision files so they are cached in
        memory.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00410E60
=========================================================*/
struct colPreload
{
    void __forceinline OnEntry( CColFileSA *colFile, unsigned int id )
    {
        // Request every colFile we find to cache it's structure.
        Streaming::RequestModel( id + DATA_COLL_BLOCK, 0 );

        Streaming::LoadAllRequestedModels( false );

        Streaming::FreeModel( id + DATA_COLL_BLOCK );
    }
};

void __cdecl Collision_Preload( void )
{
    Streaming::GetCOLEnvironment().m_pool->ForAllActiveEntries( colPreload(), 1 );
}

/*=========================================================
    Collision_Sectorize

    Purpose:
        Loops through all loaded collision files and assigns
        them to the quad tree nodes.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x00410EC0
=========================================================*/
struct colSectorize
{
    void __forceinline OnEntry( CColFileSA *colFile, unsigned int id )
    {
        // Check whether the cache is in writing mode.
        if ( Cache_IsWritingMode() )
        {
            // Update the collision file definition then.
            Cache_RestoreColFile( colFile );
        }
        else
        {
            // Expand the colFile sector.
            // May be debug code or a quick bugfix or a performance improvement.
            colFile->m_bounds.m_minX -= 120.0f;
            colFile->m_bounds.m_maxX += 120.0f;
            colFile->m_bounds.m_minY -= 120.0f;
            colFile->m_bounds.m_maxY += 120.0f;

            // Write it into our cache.
            Cache_StoreColFile( *colFile );
        }

        // Sectorize our colFile.
        Collision::GetCollisionQuadTree()->LinkToEntity( colFile, colFile->m_bounds );
    }
};

void __cdecl Collision_Sectorize( void )
{
    Streaming::GetCOLEnvironment().m_pool->ForAllActiveEntries( colSectorize(), 1 );
}

/*=========================================================
    _Collision_Init

    Purpose:
        Initializes the quad tree node system by allocating
        the pool.
    Binary offsets:
        (1.0 US and 1.0 EU): 0x004113F0
=========================================================*/
static void __cdecl _Collision_Init( void )
{
    Streaming::GetCOLEnvironment().Init();
}

void ColModel_Init( void )
{
    // Hook important routines.
    HookInstall( 0x00410E60, (DWORD)Collision_Preload, 5 );
    HookInstall( 0x00410EC0, (DWORD)Collision_Sectorize, 5 );
    HookInstall( 0x004113F0, (DWORD)_Collision_Init, 5 );
}

void ColModel_Shutdown( void )
{
}

CColModelSA::CColModelSA( void )
{
    m_pInterface = new CColModelSAInterface;
    m_original = NULL;
    m_bDestroyInterface = true;
}

CColModelSA::CColModelSA( CColModelSAInterface *pInterface, bool destroy )
{
    m_pInterface = pInterface;
    m_original = NULL;
    m_bDestroyInterface = destroy;
}

CColModelSA::~CColModelSA( void )
{
    RestoreAll();

    if ( m_bDestroyInterface )
        delete m_pInterface;
}

CColModelSA::colImport_t* CColModelSA::FindImport( modelId_t id, colImports_t::iterator& findIter )
{
    for ( colImports_t::iterator iter = m_imported.begin(); iter != m_imported.end(); iter++ )
    {
        colImport_t& import = *iter;

        if ( import.modelIndex == id )
        {
            findIter = iter;
            return &import;
        }
    }

    return NULL;
}

const CColModelSA::colImport_t* CColModelSA::FindImport( modelId_t id ) const
{
    for ( colImports_t::const_iterator iter = m_imported.begin(); iter != m_imported.end(); iter++ )
    {
        const colImport_t& import = *iter;

        if ( import.modelIndex == id )
            return &import;
    }

    return NULL;
}

bool CColModelSA::Replace( modelId_t id )
{
    if ( id > MAX_MODELS-1 )
        return false;

    if ( IsReplaced( id ) )
        return true;

    // Restore the previous association
    if ( g_colReplacement[id] )
        g_colReplacement[id]->Restore( id );

    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;
    CBaseModelInfoSAInterface *model = ppModelInfo[id];
    
    // Store the original so we can restore it again
    SetOriginal( model->pColModel, model->IsDynamicCol() );

    eRwType rwType = model->GetRwModelType();

    CColModelSAInterface *replaceColModel = NULL;
    bool replaceDynamic = false;

    if ( rwType == RW_CLUMP )
    {
        eModelType modelType = model->GetModelType();

        if ( modelType == MODEL_VEHICLE )
        {
            // Vehicles need a special collision interface.
            replaceColModel = m_pInterface->Clone();

            // We must unsegmentize the interface to prevent memory leaks.
            replaceColModel->UnsegmentizeData();
        }
    }

    if ( replaceColModel == NULL )
        replaceColModel = m_pInterface;

    model->SetCollision( replaceColModel, replaceDynamic );

    g_colReplacement[id] = this;

    colImport_t importStruct;
    importStruct.replaceCollision = replaceColModel;
    importStruct.modelIndex = id;

    m_imported.push_back( importStruct );
    return true;
}

bool CColModelSA::IsReplaced( modelId_t id ) const
{
    return FindImport( id ) != NULL;
}

bool CColModelSA::Restore( modelId_t id )
{
    colImports_t::iterator iter;
    colImport_t *import = FindImport( id, iter );

    if ( !import )
        return false;

    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;
    CBaseModelInfoSAInterface *model = ppModelInfo[id];

    assert( import->replaceCollision == model->pColModel );

    if ( import->replaceCollision != m_pInterface )
    {
        // Since we cloned the interface, we destroy it here.
        // This is made because every vehicle type must have a specialized collision interface.
        delete import->replaceCollision;

        model->pColModel = NULL;
    }

    switch( model->GetRwModelType() )
    {
    case RW_ATOMIC:
        // Restore the original colmodel no matter what
        model->SetCollision( m_original, m_originalDynamic );

        // Destroy it's data if not used anymore
        if ( m_originalDynamic && !Streaming::GetCOLEnvironment().m_pool->Get( m_original->m_colPoolIndex )->m_loaded )
            m_original->ReleaseData();

        break;
    case RW_CLUMP:
        if ( info->m_eLoading == MODEL_LOADED )
            model->SetCollision( m_original, m_originalDynamic );
        else
        {
            // Clumps delete collision at freeing them
            delete m_original;

            model->pColModel = NULL;
        }
        break;
    }

    g_colReplacement[id] = NULL;

    m_imported.erase( iter );
    return true;
}

void CColModelSA::RestoreAll( void )
{
    while ( !m_imported.empty() )
    {
        const colImport_t& import = *m_imported.begin();

        Restore( import.modelIndex );
    }
}

CColModelSA::imports_t CColModelSA::GetImportList( void ) const
{
    imports_t importsVirtual;

    for ( colImports_t::const_iterator iter = m_imported.begin(); iter != m_imported.end(); iter++ )
    {
        const colImport_t& import = *iter;

        importsVirtual.push_back( import.modelIndex );
    }

    return importsVirtual;
}

void* CColFileSA::operator new ( size_t )
{
    return Streaming::GetCOLEnvironment().m_pool->Allocate();
}

void CColFileSA::operator delete ( void *ptr )
{
    Streaming::GetCOLEnvironment().m_pool->Free( (CColFileSA*)ptr );
}

void Collision_Init( void )
{
    // Install a memory leak fix.
    HookInstall( 0x0040F9E0, h_memFunc( &CColModelSAInterface::ReleaseData ), 5 );
    HookInstall( 0x0040F700, h_memFunc( &CColModelSAInterface::_DestructorHook ), 5 );
}

void Collision_Shutdown( void )
{
}