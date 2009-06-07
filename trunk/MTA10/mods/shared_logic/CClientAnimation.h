/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientAnimation.h
*  PURPOSE:     Animation running class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#ifndef __CCLIENTANIMATION_H
#define __CCLIENTANIMATION_H

#include <vector>
#include "lua/CLuaArguments.h"

class CClientAnimationManager;
class CAnimBlock;

class CAnimationItem
{
public:
    CAnimationItem ( void )
    {
        block = NULL; hierarchy = NULL; assoc = NULL; name = NULL; speed = 1.0f; blendSpeed = 1.0f;
        loop = true; startTime = 0.0f;updatePosition = true; luaMain = NULL; luaFunction = -1;
        requesting = false;
    }
    ~CAnimationItem ( void )
    {
        delete [] name;
    }
    CAnimBlock *            block;
    CAnimBlendHierarchy *   hierarchy;
    CAnimBlendAssociation * assoc;
    char *                  name;
    float                   speed;
    float                   blendSpeed;
    float                   startTime;
    bool                    loop;
    bool                    updatePosition;
    CLuaMain *              luaMain;
    int                     luaFunction;
    CLuaArguments           luaArguments;
    bool                    requesting;
};

class CClientAnimation
{
public:
                                        CClientAnimation            ( CClientAnimationManager * pManager );
                                        ~CClientAnimation           ( void );

    virtual RpClump *                   GetClump                    ( void ) = 0;

    inline unsigned int                 CountAnimations             ( void )    { return ( unsigned int ) m_Animations.size (); }
    CAnimationItem *                    GetCurrentAnimation         ( void );
    bool                                GetCurrentAnimation         ( AssocGroupId & animGroup, AnimationId & animID );

    void                                OnCreation                  ( void );
    void                                OnBlockLoad                 ( CAnimBlock * pBlock );

    bool                                BlendAnimation              ( const char * szBlockName, const char * szName, float fSpeed = 1.0f, float fBlendSpeed = 1.0f, float fStartTime = 0.0f, bool bLoop = true, bool bUpdatePosition = true, CLuaMain * pMain = NULL, int iFunction = -1, CLuaArguments * pArguments = NULL );
    void                                BlendAnimation              ( AssocGroupId animGroup, AnimationId animID, float fSpeed = 1.0f, float fBlendSpeed = 1.0f, float fStartTime = 0.0f, bool bLoop = true, bool bUpdatePosition = true );
    void                                SyncAnimation               ( CClientAnimation & Animation );
    void                                FinishAnimation             ( void );

    static void                         StaticBlendAssocFinish      ( CAnimBlendAssociation * pAssoc, void * pData );
    
private:
    void                                BlendAnimation              ( CAnimationItem * pAnim );
    void                                FindAndClear                ( CAnimBlock * pBlock, const char * szName );

    CClientAnimationManager *           m_pAnimManager;
    std::vector < CAnimationItem * >    m_Animations;
};

#endif