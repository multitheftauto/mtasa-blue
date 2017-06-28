/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModelManager.cpp
*  PURPOSE:     Model manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

CClientModelManager::CClientModelManager ( CClientManager* pManager )
{
    for ( ushort i = 0; i <= MAX_MODEL_ID; i++ )
    {
        m_Models [ i ] = NULL;
    }
}


CClientModelManager::~CClientModelManager ( void )
{
    // Delete all our models
    RemoveAll();
}


void CClientModelManager::RemoveAll ( void )
{
    for (int i = 0; i <= MAX_MODEL_ID; i++)
    {
        Remove ( m_Models[ i ] );
    }
}

void CClientModelManager::Add ( CClientModel* pModel )
{
    if ( m_Models[ pModel->GetModelID () ] == NULL )
    {
        m_Models[ pModel->GetModelID () ] = pModel;
    }
}

bool CClientModelManager::Remove ( CClientModel* pModel )
{
    if ( pModel && m_Models[ pModel->GetModelID () ] != NULL )
    {
        m_Models[ pModel->GetModelID () ] = NULL;
        return true;
    }
    return false;
}

int CClientModelManager::GetFirstFreeModelID ( void )
{
    for (int i = 0; i <= MAX_MODEL_ID; i++)
    {
        CModelInfo* pModelInfo = g_pGame->GetModelInfo ( i, true );
        if ( !pModelInfo->IsValid () )
        {
            return i;
        }
    }
    return NULL;
}

CClientModel * CClientModelManager::FindModelByID ( int iModelID )
{
    if ( iModelID <= MAX_MODEL_ID )
    {
        return m_Models[ iModelID ];
    }
    return NULL;
}