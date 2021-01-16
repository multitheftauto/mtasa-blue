/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"

void CLuaColShapeDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createColCircle", CreateColCircle},
        {"createColCuboid", CreateColCuboid},
        {"createColSphere", CreateColSphere},
        {"createColRectangle", CreateColRectangle},
        {"createColPolygon", CreateColPolygon},
        {"createColTube", CreateColTube},

        {"getColShapeRadius", GetColShapeRadius},
        {"setColShapeRadius", SetColShapeRadius},
        {"getColShapeSize", GetColShapeSize},
        {"setColShapeSize", SetColShapeSize},
        {"getColPolygonPoints", GetColPolygonPoints},
        {"getColPolygonPointPosition", GetColPolygonPointPosition},
        {"setColPolygonPointPosition", SetColPolygonPointPosition},
        {"addColPolygonPoint", AddColPolygonPoint},
        {"removeColPolygonPoint", RemoveColPolygonPoint},

        {"isInsideColShape", IsInsideColShape},
        {"getColShapeType", GetColShapeType},
        {"setColPolygonHeight", ArgumentParser<SetColPolygonHeight>},
        {"getColPolygonHeight", ArgumentParser<GetColPolygonHeight>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaColShapeDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "Circle", "createColCircle");
    lua_classfunction(luaVM, "Cuboid", "createColCuboid");
    lua_classfunction(luaVM, "Rectangle", "createColRectangle");
    lua_classfunction(luaVM, "Sphere", "createColSphere");
    lua_classfunction(luaVM, "Tube", "createColTube");
    lua_classfunction(luaVM, "Polygon", "createColPolygon");

    lua_classfunction(luaVM, "getElementsWithin", "getElementsWithinColShape");
    lua_classfunction(luaVM, "isInside", "isInsideColShape");
    lua_classfunction(luaVM, "getShapeType", "getColShapeType");

    lua_classfunction(luaVM, "getRadius", GetColShapeRadius);
    lua_classfunction(luaVM, "setRadius", SetColShapeRadius);
    lua_classfunction(luaVM, "getSize", OOP_GetColShapeSize);
    lua_classfunction(luaVM, "setSize", SetColShapeSize);
    lua_classfunction(luaVM, "getPoints", OOP_GetColPolygonPoints);
    lua_classfunction(luaVM, "getPointPosition", OOP_GetColPolygonPointPosition);
    lua_classfunction(luaVM, "setPointPosition", SetColPolygonPointPosition);
    lua_classfunction(luaVM, "addPoint", AddColPolygonPoint);
    lua_classfunction(luaVM, "removePoint", RemoveColPolygonPoint);
    lua_classfunction(luaVM, "setHeight", ArgumentParser<SetColPolygonHeight>);
    lua_classfunction(luaVM, "getHeight", ArgumentParser<GetColPolygonHeight>);

    lua_classvariable(luaVM, "elementsWithin", nullptr, "getElementsWithinColShape");
    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_classvariable(luaVM, "radius", SetColShapeRadius, GetColShapeRadius);
    lua_classvariable(luaVM, "size", SetColShapeSize, OOP_GetColShapeSize);
    lua_classvariable(luaVM, "points", nullptr, OOP_GetColPolygonPoints);

    lua_registerclass(luaVM, "ColShape", "Element");
}

int CLuaColShapeDefs::GetColShapeType(lua_State* luaVM)
{
    // Verify the arguments
    CClientColShape* pColShape = nullptr;
    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            lua_pushnumber(luaVM, pColShape->GetShapeType());
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    // Failed
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColCircle(lua_State* luaVM)
{
    CVector2D        vecPosition;
    float            fRadius = 0.1f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (fRadius < 0.0f)
    {
        fRadius = 0.1f;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle(*pResource, vecPosition, fRadius);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColCuboid(lua_State* luaVM)
{
    CVector          vecPosition, vecSize;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecSize);

    if (vecSize.fX < 0.0f)
    {
        vecSize.fX = 0.1f;
    }
    if (vecSize.fY < 0.0f)
    {
        vecSize.fY = 0.1f;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid(*pResource, vecPosition, vecSize);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColSphere(lua_State* luaVM)
{
    CVector          vecPosition;
    float            fRadius = 0.1f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (fRadius < 0.0f)
    {
        fRadius = 0.1f;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere(*pResource, vecPosition, fRadius);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColRectangle(lua_State* luaVM)
{
    CVector2D        vecPosition;
    CVector2D        vecSize;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);

    if (vecSize.fX < 0.0f)
    {
        vecSize.fX = 0.1f;
    }
    if (vecSize.fY < 0.0f)
    {
        vecSize.fY = 0.1f;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle(*pResource, vecPosition, vecSize);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColPolygon(lua_State* luaVM)
{
    CVector2D        vecPosition;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);

    // Get the points
    std::vector<CVector2D> vecPointList;
    for (uint i = 0; i < 3 || argStream.NextIsVector2D(); i++)
    {
        CVector2D vecPoint;
        argStream.ReadVector2D(vecPoint);
        vecPointList.push_back(vecPoint);
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon(*pResource, vecPosition);
                if (pShape)
                {
                    // Add the points
                    for (uint i = 0; i < vecPointList.size(); i++)
                    {
                        pShape->AddPoint(vecPointList[i]);
                    }

                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add(pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::CreateColTube(lua_State* luaVM)
{
    CVector          vecPosition;
    float            fRadius = 0.1f, fHeight = 0.1f;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);
    argStream.ReadNumber(fHeight);

    if (fRadius < 0.0f)
    {
        fRadius = 0.1f;
    }

    if (fHeight < 0.0f)
    {
        fHeight = 0.1f;
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CClientColTube* pShape = CStaticFunctionDefinitions::CreateColTube(*pResource, vecPosition, fRadius, fHeight);
                if (pShape)
                {
                    CElementGroup* pGroup = pResource->GetElementGroup();
                    if (pGroup)
                    {
                        pGroup->Add((CClientEntity*)pShape);
                    }
                    lua_pushelement(luaVM, pShape);
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType(luaVM);

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::IsInsideColShape(lua_State* luaVM)
{
    //  bool isInsideColShape ( colshape theColShape, float posX, float posY, float posZ )
    CClientColShape* pColShape;
    CVector          vecPosition;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        bool bInside = false;
        if (CStaticFunctionDefinitions::IsInsideColShape(pColShape, vecPosition, bInside))
        {
            lua_pushboolean(luaVM, bInside);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaColShapeDefs::GetColShapeRadius(lua_State* luaVM)
{
    CClientColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    float fRadius;
    if (CStaticFunctionDefinitions::GetColShapeRadius(pColShape, fRadius))
    {
        lua_pushnumber(luaVM, fRadius);
        return 1;
    }

    argStream.SetCustomError("ColShape must be Circle, Sphere or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::SetColShapeRadius(lua_State* luaVM)
{
    CClientColShape* pColShape;
    float            fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(fRadius);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (CStaticFunctionDefinitions::SetColShapeRadius(pColShape, fRadius))
    {
        lua_pushboolean(luaVM, true);
        return 1;
    }

    argStream.SetCustomError("ColShape must be Circle, Sphere or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::GetColShapeSize(lua_State* luaVM)
{
    CClientColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    switch (pColShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CClientColRectangle*>(pColShape)->GetSize();
            lua_pushnumber(luaVM, size.fX);
            lua_pushnumber(luaVM, size.fY);
            return 2;
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CClientColCuboid*>(pColShape)->GetSize();
            lua_pushnumber(luaVM, size.fX);
            lua_pushnumber(luaVM, size.fY);
            lua_pushnumber(luaVM, size.fZ);
            return 3;
        }
        case COLSHAPE_TUBE:
        {
            float fHeight = static_cast<CClientColTube*>(pColShape)->GetHeight();
            lua_pushnumber(luaVM, fHeight);
            return 1;
        }
    }

    argStream.SetCustomError("ColShape must be Rectangle, Cuboid or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::OOP_GetColShapeSize(lua_State* luaVM)
{
    CClientColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    switch (pColShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CClientColRectangle*>(pColShape)->GetSize();
            lua_pushvector(luaVM, size);
            return 1;
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CClientColCuboid*>(pColShape)->GetSize();
            lua_pushvector(luaVM, size);
            return 1;
        }
        case COLSHAPE_TUBE:
        {
            float fHeight = static_cast<CClientColTube*>(pColShape)->GetHeight();
            lua_pushnumber(luaVM, fHeight);
            return 1;
        }
    }

    argStream.SetCustomError("ColShape must be Rectangle, Cuboid or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::SetColShapeSize(lua_State* luaVM)
{
    CClientColShape* pColShape;
    CVector          vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    switch (pColShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D vecRectangleSize;
            argStream.ReadVector2D(vecRectangleSize);

            if (!argStream.HasErrors())
            {
                vecSize.fX = vecRectangleSize.fX;
                vecSize.fY = vecRectangleSize.fY;
            }
            break;
        }
        case COLSHAPE_CUBOID:
        {
            argStream.ReadVector3D(vecSize);
            break;
        }
        case COLSHAPE_TUBE:
        {
            argStream.ReadNumber(vecSize.fX);
            break;
        }
        default:
            argStream.SetCustomError("ColShape must be Rectangle, Cuboid or Tube");
    }

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    CStaticFunctionDefinitions::SetColShapeSize(pColShape, vecSize);

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaColShapeDefs::GetColPolygonPoints(lua_State* luaVM)
{
    CClientColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);

        lua_newtable(luaVM);

        uint uiIndex = 0;
        for (auto iter = pColPolygon->IterBegin(); iter != pColPolygon->IterEnd(); ++iter)
        {
            CVector2D vecPoint = *iter;
            lua_pushnumber(luaVM, ++uiIndex);
            lua_newtable(luaVM);
            {
                lua_pushnumber(luaVM, 1);
                lua_pushnumber(luaVM, vecPoint.fX);
                lua_settable(luaVM, -3);

                lua_pushnumber(luaVM, 2);
                lua_pushnumber(luaVM, vecPoint.fY);
                lua_settable(luaVM, -3);
            }
            lua_settable(luaVM, -3);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::OOP_GetColPolygonPoints(lua_State* luaVM)
{
    CClientColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);

        lua_newtable(luaVM);

        uint uiIndex = 0;
        for (auto iter = pColPolygon->IterBegin(); iter != pColPolygon->IterEnd(); ++iter)
        {
            lua_pushnumber(luaVM, ++uiIndex);
            lua_pushvector(luaVM, *iter);
            lua_settable(luaVM, -3);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::GetColPolygonPointPosition(lua_State* luaVM)
{
    CClientColShape* pColShape;
    uint             uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);
        CVector2D          vecPoint;
        if (uiPointIndex > 0 && CStaticFunctionDefinitions::GetColPolygonPointPosition(pColPolygon, uiPointIndex - 1, vecPoint))
        {
            lua_pushnumber(luaVM, vecPoint.fX);
            lua_pushnumber(luaVM, vecPoint.fY);
            return 2;
        }

        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        lua_pushboolean(luaVM, false);
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::OOP_GetColPolygonPointPosition(lua_State* luaVM)
{
    CClientColShape* pColShape;
    uint             uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);
        CVector2D          vecPoint;
        if (uiPointIndex > 0 && CStaticFunctionDefinitions::GetColPolygonPointPosition(pColPolygon, uiPointIndex - 1, vecPoint))
        {
            lua_pushvector(luaVM, vecPoint);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
            lua_pushboolean(luaVM, false);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::SetColPolygonPointPosition(lua_State* luaVM)
{
    CClientColShape* pColShape;
    uint             uiPointIndex;
    CVector2D        vecPoint;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);
    argStream.ReadVector2D(vecPoint);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);
        if (uiPointIndex > 0 && CStaticFunctionDefinitions::SetColPolygonPointPosition(pColPolygon, uiPointIndex - 1, vecPoint))
        {
            lua_pushboolean(luaVM, true);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
            lua_pushboolean(luaVM, false);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::AddColPolygonPoint(lua_State* luaVM)
{
    CClientColShape* pColShape;
    uint             uiPointIndex;
    CVector2D        vecPoint;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadVector2D(vecPoint);
    argStream.ReadNumber(uiPointIndex, 0);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);
        if (uiPointIndex == 0)
        {
            lua_pushboolean(luaVM, CStaticFunctionDefinitions::AddColPolygonPoint(pColPolygon, vecPoint));
        }
        else if (CStaticFunctionDefinitions::AddColPolygonPoint(pColPolygon, uiPointIndex - 1, vecPoint))
        {
            lua_pushboolean(luaVM, true);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
            lua_pushboolean(luaVM, false);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::RemoveColPolygonPoint(lua_State* luaVM)
{
    CClientColShape* pColShape;
    uint             uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CClientColPolygon* pColPolygon = static_cast<CClientColPolygon*>(pColShape);
        if (pColPolygon->CountPoints() <= 3)
        {
            m_pScriptDebugging->LogWarning(luaVM, "Can't remove the last 3 points");
            lua_pushboolean(luaVM, false);
        }
        else if (uiPointIndex > 0 && CStaticFunctionDefinitions::RemoveColPolygonPoint(pColPolygon, uiPointIndex - 1))
        {
            lua_pushboolean(luaVM, true);
        }
        else
        {
            m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
            lua_pushboolean(luaVM, false);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be Polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

std::tuple<float, float> CLuaColShapeDefs::GetColPolygonHeight(CClientColPolygon* pColPolygon)
{
    float fFloor, fCeil;
    pColPolygon->GetHeight(fFloor, fCeil);
    return std::make_tuple(fFloor, fCeil);
}

bool CLuaColShapeDefs::SetColPolygonHeight(CClientColPolygon* pColPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil)
{
    //  bool SetColPolygonHeight ( colshape theColShape, float floor, float ceil )
    float fFloor, fCeil;

    if (std::holds_alternative<bool>(floor))
        fFloor = std::numeric_limits<float>::min();
    else
        fFloor = std::get<float>(floor);

    if (std::holds_alternative<bool>(ceil))
        fCeil = std::numeric_limits<float>::max();
    else
        fCeil = std::get<float>(ceil);

    if (fFloor > fCeil)
        std::swap(fFloor, fCeil);

    if (pColPolygon->SetHeight(fFloor, fCeil))
    {
        CStaticFunctionDefinitions::RefreshColShapeColliders(pColPolygon);
        return true;
    }

    return false;
}
