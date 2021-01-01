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
    CLuaPhysicsShape(CBulletPhysics* pPhysics, std::unique_ptr<btCollisionShape> pShape);
    CLuaPhysicsShape(CBulletPhysics* pPhysics, std::unique_ptr<heightfieldTerrainShape> pHeightfieldTerrainShape);

public:
    ~CLuaPhysicsShape();

    btCollisionShape*              GetBtShape() const { return m_pBtShape.get(); }
    void                           AddRigidBody(std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody);
    void                           RemoveRigidBody(std::shared_ptr<CLuaPhysicsRigidBody> pRigidBody);
    void                           AddStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);
    void                           RemoveStaticCollision(std::shared_ptr<CLuaPhysicsStaticCollision> pStaticCollision);

    bool SetScale(CVector scale);
    const CVector& GetScale();
    bool GetBoundingBox(CVector& vecMin, CVector& vecMax);
    bool GetBoundingSphere(CVector& vecCenter, float& fRadius);
    void GetMargin(float& fMargin);

    void UpdateRigids();

    BroadphaseNativeTypes GetType() const;
    const char*           GetName();

    // for CLuaPhysicsHeightfieldTerrainShape shape
    float*                GetHeightfieldData() { return &m_vecHeightfieldData[0]; }
    bool   SupportRigidBody() const;
private:
    std::unique_ptr<btCollisionShape> m_pBtShape;
    std::vector<float> m_vecHeightfieldData;

    std::vector<std::shared_ptr<CLuaPhysicsRigidBody>>       m_vecRigidBodyList;
    std::vector<std::shared_ptr<CLuaPhysicsStaticCollision>> m_vecStaticCollisions;
};
