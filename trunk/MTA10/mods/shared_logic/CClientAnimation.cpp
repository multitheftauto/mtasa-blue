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
    // Do we have a current animation?
    CAnimationItem * pAnim = GetCurrentAnimation ();
    if ( pAnim )
    {
        // Reapply this animation
        BlendAnimation ( pAnim );
    }
}


void CClientAnimation::OnDestruction ( void )
{
    // Do we have a current animation?
    CAnimationItem * pAnim = GetCurrentAnimation ();
    if ( pAnim && pAnim->type == ANIM_TYPE_MANAGED )
    {
        // Save our current animations time/position to reapply later
        pAnim->blendSpeed = 8.0f;
        if ( pAnim->assoc ) pAnim->startTime = pAnim->assoc->GetTime ();
    }
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

        if ( pAnim->type == ANIM_TYPE_MANAGED )
        {        
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
                if ( pAnim->finished ) pAssoc->SetTime ( pAssoc->GetTotalTime () );
                else pAssoc->SetTime ( pAnim->startTime );

                // Add a callback handler which'll be called when its finished being used
                pAssoc->SetFinishCallback ( CClientAnimation::StaticBlendAssocFinish, this );
                pAnim->assoc = pAssoc;
            }
        }
        else if ( pAnim->type == ANIM_TYPE_TASK )
        {
            CClientPed * pPed = dynamic_cast < CClientPed * > ( this );
            CPlayerPed * pPlayerPed = pPed->GetGamePlayer ();
            if ( pPlayerPed )
            {
                // Kill any higher priority tasks if we dont want this anim interuptable
                if ( !pAnim->interruptable )
                {
                    pPed->KillTask ( TASK_PRIORITY_PHYSICAL_RESPONSE );
                    pPed->KillTask ( TASK_PRIORITY_EVENT_RESPONSE_TEMP );
                    pPed->KillTask ( TASK_PRIORITY_EVENT_RESPONSE_NONTEMP );
                }

                // Time arg is always -1 (which means the task will stay until removed)
                CTask * pTask = g_pGame->GetTasks ()->CreateTaskSimpleRunNamedAnim ( pAnim->name, pAnim->block->GetName (), flags, pAnim->blendSpeed, pAnim->time, !pAnim->interruptable );
                if ( pTask )
                {
                    pTask->SetAsPedTask ( pPlayerPed, TASK_PRIORITY_PRIMARY );
                }
            }
        }
    }
}


bool CClientAnimation::BlendAnimation ( const char * szBlockName, const char * szName, float fSpeed, float fBlendSpeed, float fStartTime, bool bLoop, bool bUpdatePosition, bool bInterruptable, CLuaMain * pMain, int iFunction, CLuaArguments * pArguments )
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
        pAnim->interruptable = bInterruptable;
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


void CClientAnimation::FinishAnimation ( void )
{
    CAnimationItem * pCurrent = GetCurrentAnimation ();
    if ( pCurrent )
    {
        CClientPed * pPed = dynamic_cast < CClientPed * > ( this );
        
        if ( pCurrent->type == ANIM_TYPE_MANAGED )
        {
            // Set our ped back to an idle stance
            RpClump * pClump = GetClump ();
            if ( pClump )
            {
                // "ped" animation block should always be loaded
                CAnimBlendHierarchy * pHierarchy = g_pGame->GetAnimManager ()->GetAnimation ( "idle_stance", "ped" );
                if ( pHierarchy )
                {
                    // Set our playing flags
                    int flags = 0;
                    flags |= 2; // loop
                    flags |= 16; // plays properly
                    flags |= 64; // update position
          
                    g_pGame->GetAnimManager ()->BlendAnimation ( pClump, pHierarchy, flags, 4.0f );
                }
            }
        }
        else if ( pCurrent->type == ANIM_TYPE_TASK )
        {
            CPlayerPed * pPlayerPed = pPed->GetGamePlayer ();
            CTaskManager * pTaskManager = pPed->GetTaskManager ();
            if ( pPlayerPed && pTaskManager )
            {                
                // Grab the primary task
                CTask* pTask = pTaskManager->GetTask ( TASK_PRIORITY_PRIMARY );
                if ( pTask )
                {
                    // Is it an animation task we've set?
                    int iTaskType = pTask->GetTaskType ();
                    if ( iTaskType == TASK_SIMPLE_NAMED_ANIM || iTaskType == TASK_SIMPLE_ANIM )
                    {
                        // Stop and destroy this task
                        pTask->MakeAbortable ( pPlayerPed, ABORT_PRIORITY_IMMEDIATE, NULL );
                        pTask->Destroy ();
                        pTaskManager->RemoveTask ( TASK_PRIORITY_PRIMARY );
                    }
                }
            }
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


bool CClientAnimation::RunNamedAnimation ( const char * szBlockName, const char * szName, int iTime, bool bLoop, bool bUpdatePosition, bool bInterruptable )
{
    // Is this a valid block name?
    CAnimBlock * pBlock = g_pGame->GetAnimManager ()->GetAnimationBlock ( szBlockName );
    if ( pBlock )
    {
        CAnimationItem * pAnim = new CAnimationItem;
        pAnim->block = pBlock;
        pAnim->name = new char [ strlen ( szName ) + 1 ];
        strcpy ( pAnim->name, szName );
        pAnim->speed = 1.0f;
        pAnim->blendSpeed = 4.0f;
        pAnim->startTime = 0.0f;
        pAnim->loop = bLoop;
        pAnim->updatePosition = bUpdatePosition;
        pAnim->interruptable = bInterruptable;
        pAnim->time = iTime;
        pAnim->requesting = true;
        pAnim->type = ANIM_TYPE_TASK;
        m_Animations.push_back ( pAnim ); 

        // Let our manager handle the block loading
        m_pAnimManager->Request ( pBlock, this );
        
        return true;
    }
    return false;
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
                pAnim->assoc = NULL;
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


bool CClientAnimation::OnBlendAnimation ( AssocGroupId animGroup, AnimationId animID, float fBlendDelta )
{
    // Grab our last animation
    CAnimationItem * pAnim = GetCurrentAnimation ();
    if ( pAnim && pAnim->type == ANIM_TYPE_MANAGED )
    {        
        // Is this animation not interruptable?
        if ( !pAnim->interruptable )
        {           
            // TODO: find a cleaner way to stop CAnimManager::BlendAnimation calls
            /*
            // We have a current animation that isnt interruptable, dont allow a new one to be played
            return false;
            */
        }
        else
        {
            // Was it not finished yet?
            if ( !pAnim->finished )
            {
                // Call our lua callback function if we have one
                if ( pAnim->luaMain ) pAnim->luaArguments.Call ( pAnim->luaMain, pAnim->luaFunction );
            }

            // Lets remove it
            delete pAnim;
            m_Animations.pop_back ();
        }
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