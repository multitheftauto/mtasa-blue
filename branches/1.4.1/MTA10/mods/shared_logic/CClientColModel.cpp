/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModel.cpp
*  PURPOSE:     Model collision (.col file) entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
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


bool CClientColModel::LoadCol ( const SString& strFile, bool bIsRawData )
{
    // Not already got a col model?
    if ( !m_pColModel )
    {
        SString strFilename;
        CBuffer buffer;
        if ( !bIsRawData )
        {
            strFilename = strFile;
            buffer.LoadFromFile( strFilename );
            g_pClientGame->GetResourceManager()->ValidateResourceFile( strFilename, buffer );
        }
        else
        {
            buffer = CBuffer( strFile, strFile.length() );
        }

        if ( !g_pCore->GetNetwork()->CheckFile( "col", strFilename, buffer ) )
            return false;

        // Load the collision file
        m_pColModel = g_pGame->GetRenderWare ()->ReadCOL ( buffer );

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
    {
        // Check if anyone has replaced that model already. If so make it forget that
        // it replaced it. We're the new replacer.
        CClientColModel* pAlready = m_pColModelManager->GetElementThatReplaced ( usModel );
        if ( pAlready )
        {
            pAlready->m_Replaced.remove ( usModel );
        }

        // Replace the collisions
        CModelInfo* pModelInfo = g_pGame->GetModelInfo ( usModel );
        pModelInfo->SetColModel ( m_pColModel );

        // Remember that we replaced it
        m_Replaced.push_back ( usModel );

        // Success
        return true;
    }

    // Failed
    return false;
}


void CClientColModel::Restore ( unsigned short usModel )
{
    // Restore it
    InternalRestore ( usModel );

    // Remove the restored id from the list
    m_Replaced.remove ( usModel );
}


void CClientColModel::RestoreAll ( void )
{
    // Loop through our replaced ids
    std::list < unsigned short > ::iterator iter = m_Replaced.begin ();
    for ( ; iter != m_Replaced.end (); iter++ )
    {
        // Restore this model
        InternalRestore ( *iter );
    }

    // Clear the list
    m_Replaced.clear ();
}


bool CClientColModel::HasReplaced ( unsigned short usModel )
{
    // Loop through our replaced ids
    std::list < unsigned short > ::iterator iter = m_Replaced.begin ();
    for ( ; iter != m_Replaced.end (); iter++ )
    {
        // Is this the given ID
        if ( *iter == usModel )
        {
            // We have replaced it
            return true;
        }
    }

    // We have not replaced it
    return false;
}


void CClientColModel::InternalRestore ( unsigned short usModel )
{
    // Grab the model info and restore it
    CModelInfo* pModel = g_pGame->GetModelInfo ( usModel );
    pModel->RestoreColModel ();
}


// Return true if data looks like COL file contents
bool CClientColModel::IsCOLData ( const SString& strData )
{
    return strData.length() > 32 && memcmp( strData, "COL", 3 ) == 0 && strData[7] == 0;
}
