/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAnimation.cpp
*  PURPOSE:     Ped entity class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include <StdInc.h>
#include "../../sdk/game/CAnimBlendAssociation.h"
#include "lua/CLuaMain.h"

using namespace std;

CClientAnimation::CClientAnimation ( void )
{
    // Grab and store our manager's pointer
    m_pAnimManager = g_pGame->GetAnimManager ();
}


CClientAnimation::~CClientAnimation ( void )
{
    // Make sure we clear the animations we have left
    vector < CAnimationItem * > ::const_iterator iter = m_Animations.begin ();
    for ( ; iter != m_Animations.end () ; iter++ )
    {
        delete *iter;
    }
}


CAnimationItem * CClientAnimation::GetCurrentAnimation ( void )
{
    // Do we have any animations?
    if ( !m_Animations.empty () )
    {
        // The most recent added would be the current
        return m_Animations.back ();
    }
    return NULL;
}


void CClientAnimation::DoPulse ( void )
{
    // Iterate our list of animations
    CAnimationItem * pAnim = NULL;
    CAnimBlock * pBlock = NULL;
    vector < CAnimationItem * > ::const_iterator iter = m_Animations.begin ();
    while ( iter != m_Animations.end () )
    {
        pAnim = *iter;
        // Are we waiting for the animations to load?
        if ( pAnim->requesting )
        {
            // Has our block loaded now?
            pBlock = pAnim->block;
            if ( pBlock && pBlock->IsLoaded () )
            {
                // Grab our set of animations
                CAnimBlendHierarchy * pHierarchy = m_pAnimManager->GetAnimation ( pAnim->name, pBlock );
                if ( pHierarchy )
                {
                    // We're no longer waiting for these animations, play them
                    pAnim->requesting = false;
                    pAnim->hierarchy = pHierarchy;
                    BlendAnimation ( pAnim );                    
                }
                else
                {
                    // If we can't find the animation in a loaded block it doesnt exist, remove it
                    delete pAnim;
                    iter = m_Animations.erase ( iter );
                    continue;
                }
            }
        }
        iter++;
    }
}


void CClientAnimation::OnCreation ( void )
{
    // TODO: restore animations (possibly just the most recent)
}


void CClientAnimation::BlendAnimation ( CAnimationItem * pAnim )
{
    RpClump * pClump = GetClump ();
    if ( pClump )
    {
        // Set our playing flags
        int flags = 0;
        if ( pAnim->loop ) flags |= 2; // loop
        flags |= 16; // plays properly
        if ( pAnim->updatePosition ) flags |= 64;
        
        CAnimBlendAssociation * pAssoc = m_pAnimManager->BlendAnimation ( pClump, pAnim->hierarchy, flags, pAnim->blendSpeed );
        if ( pAssoc )
        {
            // Fixes setting the same animation as the previous with a different loop flag
            if ( pAnim->loop != pAssoc->IsFlagSet ( 2 ) )
            {
                if ( pAnim->loop ) pAssoc->SetFlag ( 2 );
                else pAssoc->ClearFlag ( 2 );
            }
            pAssoc->SetSpeed ( pAnim->speed );
            pAssoc->SetTime ( pAnim->startTime );

            // Add a callback handler which'll be called when its finished being used
            pAssoc->SetFinishCallback ( CClientAnimation::StaticBlendAssocFinish, this );
            pAnim->assoc = pAssoc;
        }
    }
}


bool CClientAnimation::BlendAnimation ( const char * szBlockName, const char * szName, float fSpeed, float fBlendSpeed, float fStartTime, bool bLoop, bool bUpdatePosition, CLuaMain * pMain, int iFunction, CLuaArguments * pArguments )
{
    // Is this a valid block name?
    CAnimBlock * pBlock = m_pAnimManager->GetAnimationBlock ( szBlockName );
    if ( pBlock )
    {        
        CAnimationItem * pAnim = new CAnimationItem;
        pAnim->block = pBlock;
        pAnim->name = new char [ strlen ( szName ) + 1 ];
        strcpy ( pAnim->name, szName );
        pAnim->speed = fSpeed;
        pAnim->blendSpeed = fBlendSpeed;
        pAnim->startTime = fStartTime;
        pAnim->loop = bLoop;
        pAnim->updatePosition = bUpdatePosition;
        pAnim->luaMain = pMain;
        pAnim->luaFunction = iFunction;
        if ( pArguments ) pAnim->luaArguments = *pArguments;

        // Is the block already loaded?
        if ( pBlock->IsLoaded () )
        {            
            // Is this a valid animation inside this block?
            CAnimBlendHierarchy * pHierarchy = m_pAnimManager->GetAnimation ( szName, pBlock );
            if ( pHierarchy )
            {
                // No problems, play our animation
                pAnim->hierarchy = pHierarchy;
                BlendAnimation ( pAnim );
            }
            else
            {
                // Animation doesn't exist, remove it
                delete pAnim;
                return false;
            }
        }
        else
        {
            // The block isn't loaded yet, request it and handle the animation later
            g_pGame->GetStreaming ()->RequestAnimations ( pBlock->GetIndex (), 8 );
            pAnim->requesting = true;
        }     

        // We need to clear any previous animations that are the same because..
        // ..the finish callback won't be called for them.
        FindAndClear ( pBlock, szName );
        
        // Finally, save our new animation
        m_Animations.push_back ( pAnim );        
        return true;
    }
    
    return false;
}


void CClientAnimation::FinishAnimation ( void )
{
    CAnimationItem * pCurrent = GetCurrentAnimation ();
    if ( pCurrent )
    {
        CAnimBlendAssociation * pAssoc = pCurrent->assoc;
        if ( pAssoc )
        {
            // Make sure its no longer looping
            pAssoc->ClearFlag ( 2 );
            // Go straight to the end of the animation
            pAssoc->SetTime ( pAssoc->GetTotalTime () );            
        }
    }
}


void CClientAnimation::StaticBlendAssocFinish ( CAnimBlendAssociation * pAssoc, void * pData )
{    
    CClientAnimation * pElement = reinterpret_cast < CClientAnimation * > ( pData );    

    // Iterate our animations
    CAnimationItem * pAnim = NULL;
    vector < CAnimationItem * > ::const_iterator iter = pElement->m_Animations.begin ();
    for ( ; iter != pElement->m_Animations.end () ; iter++ )
    {
        pAnim = *iter;
        // There should only be one matching assoc
        if ( pAnim->assoc == pAssoc )
        {
            // Call our lua callback function if we have one
            if ( pAnim->luaMain ) pAnim->luaArguments.Call ( pAnim->luaMain, pAnim->luaFunction );

            // Remove this animation from the list now its finished
            delete pAnim;
            pElement->m_Animations.erase ( iter );

            // Finish here as there should only be 1 match.
            return;
        }
    }
}


void CClientAnimation::FindAndClear ( CAnimBlock * pBlock, const char * szName )
{
    // Iterate our animations
    CAnimationItem * pAnim = NULL;
    vector < CAnimationItem * > ::const_iterator iter = m_Animations.begin ();
    for ( ; iter != m_Animations.end () ; iter++ )
    {
        pAnim = *iter;
        // Check the block matches
        if ( pAnim->block == pBlock )
        {
            // Same name too?
            if ( strcmp ( pAnim->name, szName ) == 0 )
            {
                // Call our lua callback function if we have one
                if ( pAnim->luaMain ) pAnim->luaArguments.Call ( pAnim->luaMain, pAnim->luaFunction );
                
                // Remove this animation from the list now its finished
                delete pAnim;
                m_Animations.erase ( iter );
                
                // Finish here as there should only be 1 match.
                return;
            }
        }
    }
}