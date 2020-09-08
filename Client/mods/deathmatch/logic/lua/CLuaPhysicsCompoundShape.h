/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsCompoundShape;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsCompoundShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsCompoundShape(CClientPhysics* pPhysics, int iInitialChildCapacity);
    ~CLuaPhysicsCompoundShape();

    void AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation = CVector(0, 0, 0));

    std::vector<CLuaPhysicsShape*> GetChildShapes();
    bool                           RemoveChildShape(int index);
    bool                           GetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation);
    bool                           SetChildShapeOffsets(int index, CVector& vecPosition, CVector& vecRotation);

};
