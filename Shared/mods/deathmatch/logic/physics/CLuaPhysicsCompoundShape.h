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

    void AddShape(CLuaPhysicsShape* pShape, CVector vecPosition, CVector vecRotation = CVector(0, 0, 0));

    std::vector<CLuaPhysicsShape*> GetChildShapes() const { return m_vecChildShapes; }

    bool   RemoveChildShape(int index);
    size_t GetChildShapesNum() const noexcept { return m_vecChildShapes.size(); }
    // Be sure index is valid using 'GetChildShapesCounts' method
    const CVector& GetChildShapePosition(int iIndex);
    // Be sure index is valid using 'GetChildShapesCounts' method
    const CVector& GetChildShapeRotation(int iIndex);
    // Be sure index is valid using 'GetChildShapesCounts' method
    void SetChildShapePosition(int iIndex, const CVector& vecPosition);
    // Be sure index is valid using 'GetChildShapesCounts' method
    void SetChildShapeRotation(int iIndex, const CVector& vecRotation);

    void Update() {}

    virtual ePhysicsElementType GetType() const { return ePhysicsElementType::CompoundShape; }
    btCompoundShape*            GetBtShape() const { return (btCompoundShape*)InternalGetBtShape(); }

private:
    std::vector<CLuaPhysicsShape*> m_vecChildShapes;
};
