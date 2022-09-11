/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsBoxShape.cpp
 *  PURPOSE:     Lua physics box shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

#include "CBulletPhysicsCommon.h"
#include "CBulletPhysics.h"
#include "CLuaPhysicsSharedLogic.h"

CLuaPhysicsBoxShape::CLuaPhysicsBoxShape(CVector half) : CLuaPhysicsConvexShape(CPhysicsSharedLogic::CreateBox(half))
{
}

