/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CLuaPhysicsCompoundShape.h
 *  PURPOSE:     Lua physics compound shape class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CLuaPhysicsCompoundShape;

#pragma once

class CLuaPhysicsCompoundShape : public CLuaPhysicsShape, CShapeHolder<btCompoundShape>
{
public:
    CLuaPhysicsCompoundShape(CBulletPhysics* pPhysics, int iInitialChildCapacity);
    ~CLuaPhysicsCompoundShape();

    void AddShape(CLuaPhysicsShape* pShape);

    std::vector<CLuaPhysicsShape*> GetChildShapes() const { return m_vecChildShapes; }

    bool RemoveChildShape(int index);
    bool RemoveChildShape(CLuaPhysicsShape* pShape);

    size_t GetChildShapesNum() const noexcept { return m_vecChildShapes.size(); }

    void    SetChildShapePosition(int iIndex, const CVector vecPosition);
    void    SetChildShapeRotation(int iIndex, const CVector vecRotation);
    CVector GetChildShapePosition(int iIndex);
    CVector GetChildShapeRotation(int iIndex);

    void Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::CompoundShape; }
    btCompoundShape*            GetBtShape() const { return (btCompoundShape*)InternalGetBtShape(); }

private:
    std::vector<CLuaPhysicsShape*> m_vecChildShapes;
};
