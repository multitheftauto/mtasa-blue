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
    CLuaPhysicsShape(CClientPhysics* pPhysics);
    ~CLuaPhysicsShape();

    void RemoveScriptID();

    //// for compound rigid bodies
    //void AddBox(CVector& vecHalf);
    //void AddSphere(float fRadius);

    void AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation);

    btBoxShape*              InitializeWithBox(CVector& vecHalf);
    void                     FinalizeInitialization(btCollisionShape* pShape);
    btSphereShape*           InitializeWithSphere(float fRadius);
    btCapsuleShape*          InitializeWithCapsule(float fRadius, float fHeight);
    btCompoundShape*         InitializeWithCompound(int initialChildCapacity = 0);
    btConeShape*             InitializeWithCone(float fRadius, float fHeight);
    btCylinderShape*         InitializeWithCylinder(CVector& half);
    btConvexHullShape*       InitializeWithConvexHull(std::vector<CVector>& vecPoints);
    btBvhTriangleMeshShape*  InitializeWithTriangleMesh(std::vector<CVector>& vecIndices);
    heightfieldTerrainShape* InitializeWithHeightfieldTerrain(int iSizeX, int iSizeY, std::vector<float>& vecHeightData);

    CClientPhysics*       GetPhysics() const { return m_pPhysics; }
    uint                  GetScriptID() const { return m_uiScriptID; }
    btCollisionShape*     GetBtShape() const { return m_pBtShape; }
    void                  AddRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                  RemoveRigidBody(CLuaPhysicsRigidBody* pRigidBody);
    void                  AddStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);
    void                  RemoveStaticCollision(CLuaPhysicsStaticCollision* pStaticCollision);

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
    const char* GetName();

private:
    uint                               m_uiScriptID;
    CClientPhysics*                    m_pPhysics;
    btCollisionShape*                  m_pBtShape;

    std::vector<CLuaPhysicsRigidBody*>       m_vecRigidBodyList;
    std::vector<CLuaPhysicsStaticCollision*> m_vecStaticCollisions;
};
