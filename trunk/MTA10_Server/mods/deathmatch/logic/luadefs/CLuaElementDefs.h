/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaElementDefs.h
*  PURPOSE:     Lua element definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CLUAELEMENTDEFS_H
#define __CLUAELEMENTDEFS_H

#include "CLuaDefs.h"

class CLuaElementDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    // Create/destroy
    static int      createElement                       ( lua_State* luaVM );
    static int      destroyElement                      ( lua_State* luaVM );
    static int      cloneElement                        ( lua_State* luaVM );

    // Is/get
    static int      isElement                           ( lua_State* luaVM );
    static int      isElementWithinColShape             ( lua_State* luaVM );
    static int      isElementWithinMarker               ( lua_State* luaVM );

    static int      getElementChildren                  ( lua_State* luaVM );
    static int      getElementChild                     ( lua_State* luaVM );
    static int      getElementChildrenCount             ( lua_State* luaVM );
    static int      getElementID                        ( lua_State* luaVM );
    static int      getElementByID                      ( lua_State* luaVM );
    static int      getElementByIndex                   ( lua_State* luaVM );
    static int      getAllElementData                   ( lua_State* luaVM );
    static int      getElementParent                    ( lua_State* luaVM );
    static int      getElementPosition                  ( lua_State* luaVM );
    static int      getElementVelocity                  ( lua_State* luaVM );
    static int      getElementType                      ( lua_State* luaVM );
    static int      getElementsByType                   ( lua_State* luaVM );
    static int      getElementInterior                  ( lua_State* luaVM );
    static int      getElementsWithinColShape           ( lua_State* luaVM );
    static int      getElementDimension                 ( lua_State* luaVM );
    static int      getElementZoneName                  ( lua_State* luaVM );
    static int      getElementColShape                  ( lua_State* luaVM );
    static int      getElementAlpha                     ( lua_State* luaVM );
    static int      getElementHealth                    ( lua_State* luaVM );
    static int      getElementModel                     ( lua_State* luaVM );
    static int      isElementInWater                    ( lua_State* luaVM );
    static int      getElementSyncer                    ( lua_State* luaVM );

    // Visible to
    static int      clearElementVisibleTo               ( lua_State* luaVM );
    static int      isElementVisibleTo                  ( lua_State* luaVM );
    static int      setElementVisibleTo                 ( lua_State* luaVM );

    // Element data
    static int      getElementData                      ( lua_State* luaVM );
    static int      setElementData                      ( lua_State* luaVM );
    static int      removeElementData                   ( lua_State* luaVM );

    // Attachement
    static int      attachElements                      ( lua_State* luaVM );
    static int      detachElements            ( lua_State* luaVM );
    static int      isElementAttached                   ( lua_State* luaVM );
    static int      getAttachedElements                 ( lua_State* luaVM );
    static int      getElementAttachedTo                ( lua_State* luaVM );

    // Set
    static int      setElementID                        ( lua_State* luaVM );
    static int      setElementParent                    ( lua_State* luaVM );
    static int      setElementPosition                  ( lua_State* luaVM );
    static int      setElementVelocity                  ( lua_State* luaVM );
    static int      setElementInterior                  ( lua_State* luaVM );
    static int      setElementDimension                 ( lua_State* luaVM );
    static int      setElementAlpha                     ( lua_State* luaVM );
    static int      setElementHealth                    ( lua_State* luaVM );
    static int      setElementModel                     ( lua_State* luaVM );
};

#endif
