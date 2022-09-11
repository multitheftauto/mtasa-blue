/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsConvexShape.cpp
 *  PURPOSE:     Lua physics convex shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"

CLuaPhysicsConvexShape::CLuaPhysicsConvexShape(btCollisionShape* pBtShape) : CLuaPhysicsShape(pBtShape)
{
}
