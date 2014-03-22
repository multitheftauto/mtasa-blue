/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        game_sa/RenderWare/CModelSA.cpp
*  PURPOSE:     DFF model management entity
*               RenderWare extension
*  DEVELOPERS:  The_GTA <quiret@gmx.de>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "../gamesa_renderware.h"

void CModelSA::RpClumpAssignObjects( CRwObjectSA *obj, CModelSA *model )
{
    eRwType type = obj->GetType();

    if ( type == RW_ATOMIC )
    {
        CRpAtomicSA *atom = (CRpAtomicSA*)obj;

        if ( atom->GetObject()->clump == model->GetObject() )
        {
            atom->m_model = model;
            model->m_atomics.push_front( atom );
        }
    }
    else if ( type == RW_LIGHT )
    {
        CRpLightSA *light = (CRpLightSA*)obj;

        if ( light->GetObject()->clump == model->GetObject() )
        {
            light->m_model = model;
            model->m_lights.push_front( light );
        }
    }
    else if ( type == RW_CAMERA )
    {
        CRwCameraSA *cam = (CRwCameraSA*)obj;

        if ( cam->GetObject()->clump == model->GetObject() )
        {
            cam->m_model = model;
            model->m_cameras.push_front( cam );
        }
    }
}

static void RpClumpObjectAssociation( CRwFrameSA *frame, CModelSA *model )
{
    // Proceed through children
    {
        const CRwFrameSA::childList_t& list = frame->GetChildren();

        for ( CRwFrameSA::childList_t::const_iterator iter = list.begin(); iter != list.end(); ++iter )
            RpClumpObjectAssociation( dynamic_cast <CRwFrameSA*> ( *iter ), model );
    }

    // Proceed through objects
    {
        const CRwFrameSA::objectList_t& list = frame->GetObjects();

        for ( CRwFrameSA::objectList_t::const_iterator iter = list.begin(); iter != list.end(); ++iter )
            CModelSA::RpClumpAssignObjects( dynamic_cast <CRwObjectSA*> ( *iter ), model );
    }
}

CModelSA::CModelSA( RpClump *clump, CColModelSA *col ) : CRwObjectSA( clump )
{
    m_col = col;

    // Assign the frame hierarchy
    m_frame = new CRwFrameSA( clump->parent );
    clump->parent->Unlink();
    clump->parent->Update();

    // Register all atomics to us
    RpClumpObjectAssociation( m_frame, this );

    // Add us to the model manager.
    CModelManagerSA::models_t& models = pGame->GetModelManager()->m_models;

    models.push_back( this );
}

CModelSA::~CModelSA( void )
{
    RestoreAll();

    // Destroy all assigned cameras (if present)
    while ( !m_cameras.empty() )
        delete m_cameras.front();

    // Destroy all assigned lights (if present)
    while ( !m_lights.empty() )
        delete m_lights.front();

    // Destroy all assigned atomics (if present)
    while ( !m_atomics.empty() )
        delete m_atomics.front();

    RpClumpDestroy( GetObject() );
    delete m_col;

    CModelManagerSA::models_t& models = pGame->GetModelManager()->m_models;

    // Remove ourself from the list
    models.erase( std::remove( models.begin(), models.end(), this ) );
}

const char* CModelSA::GetName( void ) const
{
    return GetFrame()->GetName();
}

unsigned int CModelSA::GetHash( void ) const
{
    return pGame->GetKeyGen()->GetUppercaseKey( GetName() );
}

CModel* CModelSA::Clone( void ) const
{
    return new CModelSA( RpClumpClone( GetObject() ), m_col );
}

void CModelSA::Render( void )
{
    // No point in rendering if no camera is set
    if ( !RenderWare::GetInterface()->m_renderCam )
        return;

    GetObject()->Render();
}

void CModelSA::ForAllImports( importIterCallback_t cb, void *ud )
{
    importMap_t::const_iterator iter = m_imported.begin();

    for ( ; iter != m_imported.end(); ++iter )
        cb( (*iter).first, ud );
}

bool CModelSA::Replace( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > MAX_MODELS-1 )
        return false;

    if ( !info )
        return false;

    if ( info->GetRwModelType() == RW_ATOMIC )
    {
        // Backwards compatibility for MTA:BLUE; notify our first atomic that it should replace that id
        if ( m_atomics.empty() )
            return false;

        return m_atomics.front()->Replace( id );
    }
    // We are a clump for sure

    // Make sure we are compatible with the model info.
    if ( !info->ValidateResource( id, GetObject() ) )
        return false;

    if ( IsReplaced( id ) )
        return true;

    // Remove previous references to this model to prevent collision
    pGame->GetModelManager()->RestoreModel( id );

    CClumpModelInfoSAInterface *cinfo = (CClumpModelInfoSAInterface*)info;
    CStreamingSA *streaming = pGame->GetStreaming();

    bool isLoaded = cinfo->GetRwObject() != NULL;

    // Cancel current model
    streaming->FreeModel( id );

    // Register global structure
    g_replObjectNative[id] = this;

    if ( m_col )
        m_col->Replace( id );

    m_imported[id] = true;

    // Reload ourselves
    if ( isLoaded )
        streaming->RequestModel( id, 0x16 );

    return true;
}

bool CModelSA::IsReplaced( unsigned short id ) const
{
    return m_imported.find( id ) != m_imported.end();
}

bool CModelSA::Restore( unsigned short id )
{
    CBaseModelInfoSAInterface *info = ppModelInfo[id];

    if ( id > DATA_TEXTURE_BLOCK-1 )
        return false;

    importMap_t::iterator iter = m_imported.find( id );

    if ( iter == m_imported.end() )
        return false;

    if ( !info )
    {
        // This should not happen, but if it does, prepare for the worst
        m_imported.erase( iter );
        return false;
    }

    if ( info->GetRwModelType() == RW_ATOMIC )
    {
        // Backwards compatibility for MTA:BLUE; notify our first atomic that it should restore that id
        if ( m_atomics.empty() )
            return false;

        return m_atomics.front()->Restore( id );
    }

    CClumpModelInfoSAInterface *cinfo = (CClumpModelInfoSAInterface*)info;
    CStreamingSA *streaming = pGame->GetStreaming();

    if ( m_col )
        m_col->Restore( id );

    g_replObjectNative[id] = NULL;

    // We can only restore if the model is actively loaded
    if ( cinfo->GetRwObject() )
    {
        // Do not allow destruction of collision if it belongs to us
        streaming->FreeModel( id );
        streaming->RequestModel( id, 0x10 );
    }

    m_imported.erase( iter );
    return true;
}

void CModelSA::RestoreAll( void )
{
    while ( !m_imported.empty() )
        Restore( (*m_imported.begin()).first );
}