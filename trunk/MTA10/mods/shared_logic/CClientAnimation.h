/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPed.h
*  PURPOSE:     Ped entity class
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

class CAnimationItem
{
public:
    CAnimationItem ( void )
    {
        block = NULL; hierarchy = NULL; assoc = NULL; name = NULL; blendDelta = 1.0f; loop = true;
        updatePosition = true; luaMain = NULL; luaFunction = -1; requesting = false;
    }
    ~CAnimationItem ( void )
    {
        delete [] name;
    }
    CAnimBlock *            block;
    CAnimBlendHierarchy *   hierarchy;
    CAnimBlendAssociation * assoc;
    char *                  name;
    float                   blendDelta;
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
                                        CClientAnimation            ( void );
                                        ~CClientAnimation           ( void );

    virtual RpClump *                   GetClump                    ( void ) = 0;

    inline unsigned int                 CountAnimations             ( void )    { return ( unsigned int ) m_Animations.size (); }
    CAnimationItem *                    GetCurrentAnimation         ( void );

    void                                DoPulse                     ( void );
    void                                OnCreation                  ( void );

    bool                                BlendAnimation              ( const char * szBlockName, const char * szName, float fBlendDelta, bool bLoop = true, bool bUpdatePosition = true, CLuaMain * pMain = NULL, int iFunction = -1, CLuaArguments * pArguments = NULL );
    
    static void                         StaticBlendAssocFinish      ( CAnimBlendAssociation * pAssoc, void * pData );

private:
    void                                BlendAnimation              ( CAnimationItem * pAnim );
    void                                FindAndClear                ( CAnimBlock * pBlock, const char * szName );

    CAnimManager *                      m_pAnimManager;
    std::vector < CAnimationItem * >    m_Animations;
};

#endif