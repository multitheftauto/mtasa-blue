/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.2
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModel.cpp
*  PURPOSE:     Model collision (.col file) entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Martin Turski <quiret@gmx.de>
*
*****************************************************************************/

#include "StdInc.h"

CClientColModel::CClientColModel ( CClientManager* pManager, ElementID ID ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pColModelManager = pManager->GetColModelManager ();
    m_pColModel = NULL;

    SetTypeName ( "col" );

    // Add us to DFF manager's list
    m_pColModelManager->AddToList ( this );
}


CClientColModel::~CClientColModel ( void )
{
    // Remove us from DFF manager's list
    m_pColModelManager->RemoveFromList ( this );

    // Unreplace our cols
    RestoreAll ();

    if ( m_pColModel )
        delete m_pColModel;
}


bool CClientColModel::LoadCol ( const char* szFile )
{
    // Not already got a col model?
    if ( !m_pColModel )
    {
        // Load the collision file
        m_pColModel = g_pGame->GetRenderWare ()->ReadCOL ( szFile );

        // Success if the col model is != NULL
        return ( m_pColModel != NULL );
    }

    // Failed. Already loaded
    return false;
}


bool CClientColModel::Replace ( unsigned short usModel )
{
    // We have a model loaded?
    if ( m_pColModel )
        return m_pColModel->Replace( usModel );

    // Failed
    return false;
}


void CClientColModel::Restore ( unsigned short usModel )
{
    if ( m_pColModel )
        m_pColModel->Restore( usModel );
}


void CClientColModel::RestoreAll ( void )
{
    if ( m_pColModel )
        m_pColModel->RestoreAll();
}


bool CClientColModel::HasReplaced ( unsigned short usModel )
{
    if ( m_pColModel )
        return m_pColModel->IsReplaced( usModel );

    return false;
}
