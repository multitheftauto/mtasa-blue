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

#pragma once

#include "CLuaPhysicsElement.h"
#include "CLuaPhysicsStaticCollision.h"

class heightfieldTerrainShape;

class CLuaPhysicsShape : public CLuaPhysicsElement
{
protected:
    CLuaPhysicsShape(CBulletPhysics* pPhysics, btCollisionShape* pShape);
    CLuaPhysicsShape(CBulletPhysics* pPhysics, heightfieldTerrainShape* pHeightfieldTerrainShape);

public:
    ~CLuaPhysicsShape();
    void Unlink(); // removes all related static collisions and rigid bodies

    btCollisionShape*              GetBtShape() const { return m_pBtShape.get(); }
    void                           AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                           RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                           AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    void                           RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

    bool SetScale(CVector scale);
    const CVector& GetScale();
    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);
    bool GetBoundingSphere(CVector& vecCenter, float& fRadius);
    void GetMargin(float& fMargin);

    void UpdateRigids();

    BroadphaseNativeTypes GetBtType() const;
    const char*           GetBtName();

    // for CLuaPhysicsHeightfieldTerrainShape shape
    float*                GetHeightfieldData() { return &m_vecHeightfieldData[0]; }
    bool                  SupportRigidBody() const;

    const std::vector<CLuaPhysicsRigidBody*>& GetRigidBodies() const { return m_vecRigidBodyList; }
    const std::vector<CLuaPhysicsStaticCollision*>& GetStaticCollisions() const { return m_vecStaticCollisions; }
    
    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::Shape; }

private:
    std::unique_ptr<btCollisionShape> m_pBtShape;
    std::vector<float> m_vecHeightfieldData;

    std::vector<CLuaPhysicsRigidBody*>        m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;
};
