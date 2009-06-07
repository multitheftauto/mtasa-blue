/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAnimationManager.cpp
*  PURPOSE:     Animation block streaming class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>
using namespace std;

bool CAnimBlockRequest::IsRequester ( CClientAnimation * pRequester )
{
    vector < CClientAnimation * > ::iterator iter = Requesters.begin ();
    for ( ; iter != Requesters.end () ; iter++ )
    {
        if ( *iter == pRequester )
        {
            return true;
        }
    }
    return false;
}


CClientAnimationManager::CClientAnimationManager ( CClientManager * pManager )
{
    m_pManager = pManager;
}


CClientAnimationManager::~CClientAnimationManager ( void )
{
    // Clear up our block requests
    vector < CAnimBlockRequest * > ::iterator iterReq = m_Requests.begin ();
    for ( ; iterReq != m_Requests.end () ; iterReq++ )
    {
        delete *iterReq;
    }

    // Clear up our existing blocks
    vector < SExistingBlock * > ::iterator iterExistingBlock = m_ExistingBlocks.begin ();
    for ( ; iterExistingBlock != m_ExistingBlocks.end () ; iterExistingBlock++ )
    {
        delete *iterExistingBlock;
    }
}


void CClientAnimationManager::Request ( CAnimBlock * pBlock, CClientAnimation * pRequester )
{
    // Is the animation block already loaded?
    if ( pBlock->IsLoaded () )
    {
        // Let them play the animation now
        pRequester->OnBlockLoad ( pBlock );
    }
    else
    {
        // Store this request for later on
        CAnimBlockRequest * pRequest = GetRequest ( pBlock );
        if ( !pRequest )
        {
            pRequest = new CAnimBlockRequest ( pBlock );
            m_Requests.push_back ( pRequest );
        }
        if ( !pRequest->IsRequester ( pRequester ) ) pRequest->Requesters.push_back ( pRequester );

        // Add this block to our existing list
        if ( !IsExistingBlock ( pBlock ) )
        {
            SExistingBlock * pExistingBlock = new SExistingBlock;
            pExistingBlock->pBlock = pBlock;
            pExistingBlock->ulIdleTime = 0;
            m_ExistingBlocks.push_back ( pExistingBlock );
        }

        // Finally, request our new block
        g_pGame->GetStreaming ()->RequestAnimations ( pBlock->GetIndex () );        
    }
}


void CClientAnimationManager::DoPulse ( void )
{
    // Iterate our current requests
    CAnimBlockRequest * pRequest = NULL;
    vector < CAnimBlockRequest * > ::iterator iter = m_Requests.begin ();
    while ( iter != m_Requests.end () )
    {
        pRequest = *iter;
        // Has the block loaded?
        if ( pRequest->pBlock->IsLoaded () )
        {
            // Iterate all the requesters for this block
            CClientAnimation * pRequester = NULL;
            vector < CClientAnimation * > ::iterator iterRequester = pRequest->Requesters.begin ();
            for ( ; iterRequester != pRequest->Requesters.end () ; iterRequester++ )
            {
                // Tell them to play the animations
                pRequester = *iterRequester;
                pRequester->OnBlockLoad ( pRequest->pBlock );
            }
            // Remove this request from our list
            delete pRequest;
            iter = m_Requests.erase ( iter );
            continue;
        }
        iter++;
    }

    // Iterate all of our existing blocks (blocks being used)
    SExistingBlock * pExistingBlock = NULL;
    unsigned long ulTime = GetTickCount ();
    CAnimBlock * pBlock = NULL;
    unsigned long ulIdleTime = 0;
    unsigned short usRefs = 0;
    vector < SExistingBlock * > ::iterator iterExistingBlock = m_ExistingBlocks.begin ();
    while ( iterExistingBlock != m_ExistingBlocks.end () )
    {
        pExistingBlock = *iterExistingBlock;
        pBlock = pExistingBlock->pBlock;
        ulIdleTime = pExistingBlock->ulIdleTime;
        usRefs = pBlock->GetRefs ();

        // Is this block not being used anymore?
        if ( usRefs == 0 )
        {
            // Have we already set this block as idle?
            if ( ulIdleTime != 0 )
            {
                // Has this block been idle for over 5seconds?
                if ( ulTime > ( ulIdleTime + 5000 ) )
                {
                    // Time to remove this block
                    g_pGame->GetStreaming ()->RemoveAnimations ( pBlock->GetIndex () );
                    delete pExistingBlock;
                    iterExistingBlock = m_ExistingBlocks.erase ( iterExistingBlock );
                    continue;
                }
            }
            // Lets set it as idle now
            else pExistingBlock->ulIdleTime = ulTime;
        }
        // If we've set this block as idle but its being used now, reset our idle time
        else if ( ulIdleTime != 0 ) pExistingBlock->ulIdleTime = 0;

        iterExistingBlock++;
    }
}


void CClientAnimationManager::Cleanup ( CClientAnimation * pRequester )
{
    // Iterate our current requests
    CAnimBlockRequest * pRequest = NULL;
    vector < CAnimBlockRequest * > ::iterator iter = m_Requests.begin ();
    while ( iter != m_Requests.end () )
    {
        // Are we requesting a block for this one?
        pRequest = *iter;
        if ( pRequest->IsRequester ( pRequester ) )
        {
            // Is this the only one requesting it?
            if ( pRequest->Requesters.size () == 1 )
            {
                // Remove this request from our list
                delete pRequest;
                iter = m_Requests.erase ( iter );
                continue;
            }
            // If not, just remove this one
            else
            {
                // Find and delete the requester
                vector < CClientAnimation * > ::iterator iterReq = pRequest->Requesters.begin ();
                for ( ; iterReq != pRequest->Requesters.end () ; iterReq++ )
                {
                    if ( *iterReq == pRequester )
                    {
                        pRequest->Requesters.erase ( iterReq );
                        break;
                    }
                }
            }
        }
        iter++;
    }
}


CAnimBlockRequest * CClientAnimationManager::GetRequest ( CAnimBlock * pBlock )
{
    CAnimBlockRequest * pRequest = NULL;
    vector < CAnimBlockRequest * > ::iterator iter = m_Requests.begin ();
    for ( ; iter != m_Requests.end () ; iter++ )
    {
        pRequest = *iter;
        if ( pRequest->pBlock == pBlock )
        {
            return pRequest;
        }
    }
    return NULL;
}


bool CClientAnimationManager::IsExistingBlock ( CAnimBlock * pBlock )
{
    vector < SExistingBlock * > ::iterator iter = m_ExistingBlocks.begin ();
    for ( ; iter != m_ExistingBlocks.end () ; iter++ )
    {
        if ( (*iter)->pBlock == pBlock ) return true;
    }
    return false;
}