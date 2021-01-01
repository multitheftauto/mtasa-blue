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

class CLuaPhysicsCompoundShape : public CLuaPhysicsShape
{
public:
    CLuaPhysicsCompoundShape(CBulletPhysics* pPhysics, int iInitialChildCapacity);
    ~CLuaPhysicsCompoundShape();

    void AddShape(std::shared_ptr<CLuaPhysicsShape> pShape, CVector vecPosition, CVector vecRotation = CVector(0, 0, 0));

    std::vector<std::shared_ptr<CLuaPhysicsShape>> GetChildShapes() const { return m_vecChildShapes; }

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

private:
    std::vector<std::shared_ptr<CLuaPhysicsShape>> m_vecChildShapes;
};
