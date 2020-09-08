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

class CLuaPhysicsShape;
class CLuaPhysicsElement;

#pragma once

// Define includes
#include "LuaCommon.h"
#include "CLuaArguments.h"
#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsShape.h"

class CLuaPhysicsShape : public CLuaPhysicsElement
{
public:
    CLuaPhysicsShape(CClientPhysics* pPhysics);
    ~CLuaPhysicsShape();

    //// for compound rigid bodies
    // void AddBox(CVector& vecHalf);
    // void AddSphere(float fRadius);

    void AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation = CVector(0, 0, 0));

    void                       FinalizeInitialization(btCollisionShape* pShape);

    btCollisionShape*              GetBtShape() const { return m_pBtShape; }
    void                           AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                           RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                           AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    void                           RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    bool SetSize(CVector vecSize);
    bool GetSize(CVector& vecSize);
    bool SetRadius(float fRadius);
    bool GetRadius(float& fRadius);
    bool SetHeight(float fHeight);
    bool GetHeight(float& fHeight);
    bool SetScale(CVector scale);
    bool GetScale(CVector& scale);
    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);
    bool GetBoundingSphere(CVector& vecCenter, float& fRadius);
    void GetMargin(float& fMargin);

    void UpdateRigids();

    BroadphaseNativeTypes GetType();
    const char*           GetName();

private:
    btCollisionShape*        m_pBtShape;
    heightfieldTerrainShape* m_heightfieldTerrainData = nullptr;

    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;
};
