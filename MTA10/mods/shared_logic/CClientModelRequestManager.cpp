/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientModelRequestManager.cpp
*  PURPOSE:     Entity model streaming manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientModelRequestManager::CClientModelRequestManager ( void )
{
    m_bDoingPulse = false;
}


CClientModelRequestManager::~CClientModelRequestManager ( void )
{
    // Delete all our requests.
    list < SClientModelRequest* > ::iterator iter;
    for ( iter = m_Requests.begin (); iter != m_Requests.end (); iter++ )
    {
        delete *iter;
    }

    m_Requests.clear ();
}


bool CClientModelRequestManager::IsLoaded ( unsigned short usModelID )
{
    // Grab the model info
    CModelInfo* pInfo = g_pGame->GetModelInfo ( usModelID );
    if ( pInfo )
    {
        return pInfo->IsLoaded () ? true:false;
    }

    return false;
}


bool CClientModelRequestManager::IsRequested ( CModelInfo* pModelInfo )
{
    // Look through the list
    std::list < SClientModelRequest* > :: iterator iter = m_Requests.begin ();
    for ( ; iter != m_Requests.end (); iter++ )
    {
        // Same model as this entry?
        if ( (*iter)->pModel == pModelInfo )
        {
            return true;
        }
    }

    // Not in request list
    return false;
}


bool CClientModelRequestManager::HasRequested ( CClientEntity* pRequester )
{
    assert ( pRequester );

    // Look through the list
    std::list < SClientModelRequest* > :: iterator iter = m_Requests.begin ();
    for ( ; iter != m_Requests.end (); iter++ )
    {
        // Same requester as we check for? He has requested something.
        if ( (*iter)->pEntity == pRequester )
        {
            return true;
        }
    }

    // Not requested anything
    return false;
}



CModelInfo* CClientModelRequestManager::GetRequestedModelInfo ( CClientEntity* pRequester )
{
    assert ( pRequester );

    // Look through the list
    std::list < SClientModelRequest* > :: iterator iter = m_Requests.begin ();
    for ( ; iter != m_Requests.end (); iter++ )
    {
        // Same requester as we check for? He has requested something.
        if ( (*iter)->pEntity == pRequester )
        {
            // Return the model info he requested
            return (*iter)->pModel;
        }
    }

    // Not requested anything
    return NULL;
}


bool CClientModelRequestManager::RequestBlocking ( unsigned short usModelID )
{
    // Grab the model info
    CModelInfo* pInfo = g_pGame->GetModelInfo ( usModelID );
    if ( pInfo )
    {
        pInfo->Request ( TRUE, TRUE );
        pInfo->MakeCustomModel ();
        return true;
    }

    // Bad model ID probably.
    return false;
}


bool CClientModelRequestManager::Request ( unsigned short usModelID, CClientEntity* pRequester )
{
    assert ( pRequester );
    SClientModelRequest* pEntry;

    // Grab the model info for that model
    CModelInfo* pInfo = g_pGame->GetModelInfo ( usModelID );
    if ( pInfo )
    {
        // Has it already requested something?
        list < SClientModelRequest* > ::iterator iter;
        if ( GetRequestEntry ( pRequester, iter ) )
        {
            // Get the entry
            pEntry = *iter;

            // The same model?
            if ( pInfo == pEntry->pModel )
            {
                // He has to wait more for it
                return false;
            }
            else
            {
                // Remove the reference to the old model
                pEntry->pModel->RemoveRef ();

                // Is it loaded?
                if ( pInfo->IsLoaded () )
                {
                    // Make sure custom things are replaced
                    pInfo->MakeCustomModel ();

                    // Delete it, remove the it from the list and return true.
                    delete pEntry;
                    m_Requests.erase ( iter );
                    return true;
                }
                else
                {
                    // If not loaded. Replace the model we're going to load.
                    // Also remember that we requested it now.
                    pEntry->pModel = pInfo;
                    pEntry->dwTimeRequested = timeGetTime ();

                    // Start loading the new model.
                    pInfo->AddRef ( false );

                    // He has to wait for it.
                    return false;
                }
            }
        }
        else
        {
            // Already loaded? Don't bother adding to the list.
            if ( pInfo->IsLoaded () )
            {
                // Make sure custom things are replaced
                pInfo->MakeCustomModel ();

                return true;
            }

            // Request it
            pInfo->AddRef ( false );

            // Add him to the list over models we're waiting for.
            pEntry = new SClientModelRequest;
            pEntry->pModel = pInfo;
            pEntry->pEntity = pRequester;
            pEntry->dwTimeRequested = timeGetTime ();
            m_Requests.push_back ( pEntry );

            // Return false. Caller needs to wait.
            return false;
        }
    }

    // Error, model is bad. Caller should not do this.
    return false;
}


void CClientModelRequestManager::Cancel ( CClientEntity* pEntity )
{
    assert ( pEntity );

    // Are we inside a pulse? Add it to a list to delete after or we'll crash.
    // If not, cancel now.
    if ( m_bDoingPulse )
    {
        m_CancelQueue.push_back ( pEntity );
    }
    else
    {
        // Got any items?
        if ( !m_Requests.empty () )
        {
            // Anything requested by the given class?
            SClientModelRequest* pEntry;
            list < SClientModelRequest* > ::iterator iter;
            for ( iter = m_Requests.begin (); iter != m_Requests.end (); )
            {
                pEntry = *iter;

                // If the requesting entity matches the given entity, delete and NULL it
                if ( pEntry->pEntity == pEntity )
                {
                    // Unreference the reference we added to it.
                    pEntry->pModel->RemoveRef ();

				    // Delete the entry
                    delete *iter;
                    
				    // Remove from the list
				    iter = m_Requests.erase ( iter );
                }
                else
                {
                    // Increment iterator otherwize
                    ++iter;
                }
            }
        }
    }
}


void CClientModelRequestManager::DoPulse ( void )
{
    // Any requests?
    if ( m_Requests.size () > 0 )
    {
        // We are now doing the pulse
        m_bDoingPulse = true;

        // Grab the current time
        DWORD dwTimeNow = timeGetTime ();

        // Call the callback for those finished loading and remove them from the list
        SClientModelRequest* pEntry;
        list < SClientModelRequest* > ::iterator iter;
        for ( iter = m_Requests.begin (); iter != m_Requests.end (); )
        {
            pEntry = *iter;

            // Is it loaded?
            if ( pEntry->pModel->IsLoaded () )
            {
                // Make sure custom things are replaced
                pEntry->pModel->MakeCustomModel ();

                // Call the callback
                pEntry->pEntity->ModelRequestCallback ( pEntry->pModel );

                // Unreference us from the model (callback should've added a reference!)
                pEntry->pModel->RemoveRef ();

				// Delete the request entry. Remove from the list and continue from after it
                delete *iter;
				iter = m_Requests.erase ( iter );
            }
            else
            {
                // Been more than 2 seconds since we requested it? Request it again.
                if ( dwTimeNow - pEntry->dwTimeRequested >= 2000 )
                {
                    // Request it again. Don't add reference, or we screw up the
                    // reference count.
                    pEntry->pModel->Request ( TRUE, FALSE );

                    // Remember now as the time we requested it.
                    pEntry->dwTimeRequested = dwTimeNow;
                }

                // Increment iterator
                ++iter;
            }
        }

        // No longer doing the pulse
        m_bDoingPulse = false;

        // Cancel what we've scheduled for cancel now if anything
        if ( m_CancelQueue.size () > 0 )
        {
            // Cancel every entity in our cancel list
            list < CClientEntity* > ::iterator iter = m_CancelQueue.begin ();
            for ( ; iter != m_CancelQueue.end (); iter++ )
            {
                Cancel ( *iter );
            }

            // Clear our cancel list
            m_CancelQueue.clear ();
        }
    }
}


bool CClientModelRequestManager::GetRequestEntry ( CClientEntity* pRequester, list < SClientModelRequest* > ::iterator& iterOut )
{
    // Look through the list
    std::list < SClientModelRequest* > :: iterator iter = m_Requests.begin ();
    for ( ; iter != m_Requests.end (); iter++ )
    {
        // Same requester as we check for? He has requested something.
        if ( (*iter)->pEntity == pRequester )
        {
            // Pass out the iterator entry and return true
            iterOut = iter;
            return true;
        }
    }

    // Not requested anything
    return false;
}
