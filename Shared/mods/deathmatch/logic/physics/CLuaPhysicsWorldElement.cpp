/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsWorldElement.cpp
 *  PURPOSE:     Lua physics common functionality for elements whichin world
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <StdInc.h>

CLuaPhysicsWorldElement::CLuaPhysicsWorldElement(CBulletPhysics* pPhysics, EIdClass::EIdClassType classType) : CLuaPhysicsElement(pPhysics, classType)
{
}

CLuaPhysicsWorldElement::~CLuaPhysicsWorldElement()
{
}
