/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsSphereShape.cpp
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>
#include "CLuaPhysicsSharedLogic.h"
#include "CLuaPhysicsShape.h"
#include "CLuaPhysicsSphereShape.h"

CLuaPhysicsSphereShape::CLuaPhysicsSphereShape(CClientPhysics* pPhysics, float fRadius) : CLuaPhysicsShape(pPhysics)
{
    btSphereShape* pSphere = CLuaPhysicsSharedLogic::CreateSphere(fRadius);
    FinalizeInitialization(pSphere);
}

CLuaPhysicsSphereShape::~CLuaPhysicsSphereShape()
{

}

bool CLuaPhysicsSphereShape::SetRadius(float fRadius)
{
    ((btSphereShape*)GetBtShape())->setUnscaledRadius(fRadius);
    return true;
}

bool CLuaPhysicsSphereShape::GetRadius(float& fRadius)
{
    fRadius = ((btSphereShape*)GetBtShape())->getRadius();
    return true;
}
