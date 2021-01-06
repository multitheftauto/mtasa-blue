/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsStaticCollision.h
 *  PURPOSE:     Static collision class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsStaticCollision;

#include "physics/CPhysicsStaticCollisionProxy.h"
#include "physics/CLuaPhysicsWorldElement.h"

#pragma once

class CLuaPhysicsStaticCollision : public CLuaPhysicsWorldElement
{
public:
    CLuaPhysicsStaticCollision(CLuaPhysicsShape* pShape);
    ~CLuaPhysicsStaticCollision();

    void    Initialize();

    void          SetPosition(CVector vecPosition, bool dontCommitChanges = false);
    const CVector GetPosition() const;
    void          SetRotation(CVector vecRotation, bool dontCommitChanges = false);
    const CVector GetRotation() const;
    void          SetScale(const CVector& vecScale);
    const CVector GetScale() const;
    void    SetMatrix(const CMatrix& matrix);
    const CMatrix GetMatrix() const;

    void          RemoveDebugColor();
    void          SetDebugColor(const SColor& color);
    const SColor GetDebugColor() const;

    int  GetFilterGroup() const;
    void SetFilterGroup(int iGroup);
    int  GetFilterMask() const;
    void SetFilterMask(int mask);

    void Unlink();

    CPhysicsStaticCollisionProxy* GetCollisionObject() const { return m_btCollisionObject.get(); }
    CLuaPhysicsShape*             GetShape() const { return m_pShape; }

    void Update() {}
private:
    std::unique_ptr<CPhysicsStaticCollisionProxy> m_btCollisionObject;
    CLuaPhysicsShape*  m_pShape;

    mutable std::mutex m_lock;
    mutable std::mutex m_matrixLock;
    mutable CMatrix m_matrix;
};
