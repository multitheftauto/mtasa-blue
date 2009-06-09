/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAnimation.cpp
*  PURPOSE:     Animation running class
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

CClientAnimation::CClientAnimation ( CClientAnimationManager * pManager )
{
    // Grab and store our manager pointers
    m_pAnimManager = pManager;
}


CClientAnimation::~CClientAnimation ( void )
{
    // Tell our manager to remove any of our crap
    m_pAnimManager->Cleanup ( this );

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


bool CClientAnimation::GetCurrentAnimation ( AssocGroupId & animGroup, AnimationId & animID )
{
    // Do we have a clump?
    RpClump * pClump = GetClump ();
    if ( pClump )
    {
        // Grab the first association (which should be the current)
        CAnimBlendAssociation * pAssoc = g_pGame->GetAnimManager ()->RpAnimBlendClumpGetFirstAssociation ( pClump );
        if ( pAssoc )
        {
            animGroup = pAssoc->GetAnimGroup ();
            animID = pAssoc->GetAnimID ();
            return true;
        }
    }
    return false;
}


void CClientAnimation::OnCreation ( void )
{
    // TODO: restore animations (possibly just the most recent)
}


void CClientAnimation::OnBlockLoad ( CAnimBlock * pBlock )
{
    // Iterate our list of animations
    CAnimationItem * pAnim = NULL;
    vector < CAnimationItem * > ::const_iterator iter = m_Animations.begin ();
    while ( iter != m_Animations.end () )
    {
        pAnim = *iter;
        // Are we waiting for the block to load for this animation?
        if ( pAnim->requesting )
        {
            // Is this the right block?
            if ( pAnim->block == pBlock )
            {
                // Grab our set of animations
                CAnimBlendHierarchy * pHierarchy = g_pGame->GetAnimManager ()->GetAnimation ( pAnim->name, pBlock );
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
        
        CAnimBlendAssociation * pAssoc = g_pGame->GetAnimManager ()->BlendAnimation ( pClump, pAnim->hierarchy, flags, pAnim->blendSpeed );
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
    CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
    if ( pBlock )
    {        
        // We need to clear any previous animations that are the same because..
        // ..the finish callback won't be called for them.
        FindAndClear ( pBlock, szName );

        // Do we have a previous animation finished and waiting to be deleted?
        CAnimationItem * pCurrentAnim = GetCurrentAnimation ();
        if ( pCurrentAnim && pCurrentAnim->finished )
        {
            // Lets remove it
            delete pCurrentAnim;
            m_Animations.pop_back ();
        }

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
        pAnim->requesting = true;
        m_Animations.push_back ( pAnim ); 

        // Let our manager handle the block loading
        m_pAnimManager->Request ( pBlock, this );
               
        return true;
    }
    
    return false;
}


void CClientAnimation::BlendAnimation ( AssocGroupId animGroup, AnimationId animID, float fSpeed, float fBlendSpeed, float fStartTime, bool bLoop, bool bUpdatePosition )
{
    RpClump * pClump = GetClump ();
    if ( pClump )
    {                
        CAnimBlendAssociation * pAssoc = g_pGame->GetAnimManager ()->BlendAnimation ( pClump, animGroup, animID, fBlendSpeed );
        if ( pAssoc )
        {
            // Fixes setting the same animation as the previous with a different loop flag
            if ( bLoop != pAssoc->IsFlagSet ( 2 ) )
            {
                if ( bLoop ) pAssoc->SetFlag ( 2 );
                else pAssoc->ClearFlag ( 2 );
            }
            pAssoc->SetSpeed ( fSpeed );
            pAssoc->SetTime ( fStartTime );
        }
    }
}


void CClientAnimation::SyncAnimation ( CClientAnimation & Animation )
{
    RpClump * pClumpClone = Animation.GetClump ();
    if ( pClumpClone )
    {                
        CAnimBlendAssociation * pAssocClone = g_pGame->GetAnimManager ()->RpAnimBlendClumpGetFirstAssociation ( pClumpClone );
        if ( pAssocClone )
        {
            AssocGroupId animGroupClone = pAssocClone->GetAnimGroup ();
            AnimationId animIDClone = pAssocClone->GetAnimID ();

            RpClump * pClump = GetClump ();
            if ( pClump )
            {
                CAnimBlendAssociation * pAssoc = g_pGame->GetAnimManager ()->RpAnimBlendClumpGetFirstAssociation ( pClump );
                if ( !pAssoc ) pAssoc =  g_pGame->GetAnimManager ()->BlendAnimation ( pClump, animGroupClone, animIDClone, 4.0f );
                else
                {
                    AssocGroupId animGroup = pAssoc->GetAnimGroup ();
                    AnimationId animID = pAssoc->GetAnimID ();

                    if ( animGroup != animGroupClone || animID != animIDClone )
                    {
                        pAssoc = g_pGame->GetAnimManager ()->BlendAnimation ( pClump, animGroupClone, animIDClone, 4.0f );
                    }                    
                }
                pAssoc->SyncAnimation ( pAssocClone );
            }
        }
    }
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
            /* Go straight to the end of the animation
            (note: not completely as our finish event wont get called) */
            pAssoc->SetTime ( pAssoc->GetTotalTime () - 0.001f );            
        }
    }

    // Clear up our animations
    vector < CAnimationItem * > ::const_iterator iter = m_Animations.begin ();
    for ( ; iter != m_Animations.end () ; iter++ )
    {
        delete *iter;
    }
    m_Animations.clear ();
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

            // Is this our final animation?
            if ( pAnim == pElement->m_Animations.back () )
            {
                // Set it as finished
                pAnim->finished = true;
            }
            else
            {
                // Remove this animation from the list now its finished
                delete pAnim;
                pElement->m_Animations.erase ( iter );
            }

            // Finish here as there should only be 1 match.
            return;
        }
    }
}


bool CClientAnimation::AllowBlendAnimation ( AssocGroupId animGroup, AnimationId animID, float fBlendDelta )
{
    // Grab our last animation
    CAnimationItem * pAnim = GetCurrentAnimation ();
    if ( pAnim )
    {        
        // We have a current animation that isnt finished, dont allow a new one to be played
        return false;
    }
    // Allow GTA to set a new animation
    return true;
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