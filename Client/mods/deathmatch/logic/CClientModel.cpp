/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModel.cpp
*  PURPOSE:     Model handling class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

CClientModel::CClientModel ( CClientManager* pManager, ElementID ID, ushort usModelID, eClientModelType eModelType ) : ClassInit ( this ), CClientEntity ( ID )
{
    // Init
    m_pManager = pManager;
    m_pModelManager = pManager->GetModelManager ();
    m_usModelID = usModelID;
    m_eModelType = eModelType;

    SetTypeName ( "model" );
    m_pModelManager->Add ( this );
}


CClientModel::~CClientModel ( void )
{
    Deallocate ();

    m_pModelManager->Remove ( this );
}

bool CClientModel::Allocate ( void )
{
    m_bAllocatedByUs = true;

    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( m_usModelID, true );

    // Allocate only on free IDs
    if ( !pModelInfo->IsValid () )
    {
        pModelInfo->MakePedModel ( "PSYCHO" );
        return true;
    }
    return false;
}

bool CClientModel::Deallocate ( void )
{
    if ( !m_bAllocatedByUs )
        return false;
    
    CModelInfo* pModelInfo = g_pGame->GetModelInfo ( m_usModelID, true );

    // ModelInfo must be valid
    if ( pModelInfo->IsValid () )
    {
        if ( m_eModelType == CCLIENTMODELPED )
        {
            // If some ped is using this ID, change him to CJ
            CClientPedManager * pPedManager = g_pClientGame->GetManager()->GetPedManager();
            std::vector < CClientPed* > ::const_iterator iter = pPedManager->IterBegin ();
            for ( ; iter != pPedManager->IterEnd (); iter++ )
            {
                if ( (*iter)->GetModel () == m_usModelID )
                {
                    if ( (*iter)->IsStreamedIn () )
                    {
                        (*iter)->StreamOutForABit ();
                    }
                    (*iter)->SetModel ( 0 );
                }
            }
        }

        // Restore DFF/TXD
        g_pClientGame->GetManager()->GetDFFManager()->RestoreModel ( m_usModelID );

        // Restore COL (for non ped models)
        if ( m_eModelType != CCLIENTMODELPED )
        {
            g_pClientGame->GetManager()->GetColModelManager()->RestoreModel ( m_usModelID );
        }

        pModelInfo->DeallocateModel ();
        return true;
    }
    return false;
}
