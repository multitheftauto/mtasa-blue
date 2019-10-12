/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsShape.h
 *  PURPOSE:     Lua physics shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsShape;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"

class CLuaPhysicsShape
{
public:
    CLuaPhysicsShape();
    ~CLuaPhysicsShape();

    void RemoveScriptID();

    // for compound rigid bodies
    void AddBox(CVector& vecHalf);
    void AddSphere(float fRadius);

    btBoxShape*        InitializeWithBox(CVector& vecHalf);
    btSphereShape*     InitializeWithSphere(float fRadius);
    btCapsuleShape*    InitializeWithCapsule(float fRadius, float fHeight);
    btCompoundShape*   InitializeWithCompound(int initialChildCapacity = 0);
    btConeShape*       InitializeWithCone(float fRadius, float fHeight);
    btCylinderShape*   InitializeWithCylinder(CVector& half);
    btConvexHullShape* InitializeWithConvexHull(std::vector<CVector>& vecPoints);

    uint                  GetScriptID() const { return m_uiScriptID; }
    btCollisionShape*     GetBtShape() const { return m_pBtShape; }
    CLuaPhysicsRigidBody* GetRigidBody(int index) const { return m_pRigidBodyList[index]; }
    int                   GetRigidBodyNum() const { return m_pRigidBodyList.size(); }
    void                  AddRigidBody(CLuaPhysicsRigidBody* pRigidBody) {m_pRigidBodyList.push_back(pRigidBody); }
    void                  RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody) { ListRemove(m_pRigidBodyList, pRigidBody); }

    bool SetSize(CVector size);
    bool GetSize(CVector& size);
    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);
    bool GetBoundingSphere(CVector& vecCenter, float& fRadius);


    const char* GetType();

private:
    uint                     m_uiScriptID;
    btCollisionShape*        m_pBtShape;
    std::vector<CLuaPhysicsRigidBody*>    m_pRigidBodyList;
};
