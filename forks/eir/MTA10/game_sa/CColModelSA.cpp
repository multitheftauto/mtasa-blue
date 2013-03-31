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

CColFilePool **ppColFilePool = (CColFilePool**)CLASS_CColFilePool;

#define FUNC_ColFilePoolInit    0x004113F0

extern CBaseModelInfoSAInterface **ppModelInfo;

SetCachedCollision_t    SetCachedCollision =                (SetCachedCollision_t)0x005B2C20;

CColModelSAInterface::CColModelSAInterface( void )
{
    _asm
    {
        mov     ecx,this
        mov     eax,FUNC_CColModel_Constructor
        call    eax
    }
}

CColModelSAInterface::~CColModelSAInterface( void )
{
    _asm
    {
        mov     ecx,this
        mov     eax,FUNC_CColModel_Destructor
        call    eax
    }
}

void CColModelSAInterface::ReleaseData( void )
{
    __asm
    {
        mov ecx,this
        mov eax,0x0040F9E0
        call eax
    }
}

void* CColModelSAInterface::operator new( size_t )
{
    return (*ppColModelPool)->Allocate();
}

void CColModelSAInterface::operator delete( void *ptr )
{
    (*ppColModelPool)->Free( (CColModelSAInterface*)ptr );
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

bool CColModelSA::Replace( unsigned short id )
{
    if ( id > DATA_TEXTURE_BLOCK-1 )
        return false;

    if ( IsReplaced( id ) )
        return true;

    // Restore the previous association
    if ( g_colReplacement[id] )
        g_colReplacement[id]->Restore( id );

    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;
    CBaseModelInfoSAInterface *model = ppModelInfo[id];
    
    // Store the original so we can restore it again
    m_original = model->pColModel;
    m_originalDynamic = model->IsDynamicCol();

    model->SetCollision( m_pInterface, false );

    g_colReplacement[id] = this;

    m_imported.push_back( id );
    return true;
}

bool CColModelSA::IsReplaced( unsigned short id ) const
{
    return std::find( m_imported.begin(), m_imported.end(), id ) != m_imported.end();
}

bool CColModelSA::Restore( unsigned short id )
{
    imports_t::const_iterator iter = std::find( m_imported.begin(), m_imported.end(), id );

    if ( iter == m_imported.end() )
        return false;

    CModelLoadInfoSA *info = (CModelLoadInfoSA*)ARRAY_CModelLoadInfo + id;
    CBaseModelInfoSAInterface *model = ppModelInfo[id];

    switch( model->GetRwModelType() )
    {
    case RW_ATOMIC:
        // Restore the original colmodel no matter what
        model->SetCollision( m_original, m_originalDynamic );

        // Destroy it's data if not used anymore
        if ( m_originalDynamic && !(*ppColFilePool)->Get( m_original->m_colPoolIndex )->m_loaded )
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
        Restore( m_imported.front() );
}

void CColModelSA::Apply( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    m_original = info->pColModel;

    info->SetColModel( m_pInterface, false );
}

void* CColFileSA::operator new ( size_t )
{
    return (*ppColFilePool)->Allocate();
}

void CColFileSA::operator delete ( void *ptr )
{
    (*ppColFilePool)->Free( (CColFileSA*)ptr );
}