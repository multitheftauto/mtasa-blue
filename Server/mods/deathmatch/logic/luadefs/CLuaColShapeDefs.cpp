/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaColShapeDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"
#include "CColCircle.h"
#include "CColCuboid.h"
#include "CColRectangle.h"
#include "CColPolygon.h"
#include "CColTube.h"

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
        {"getColPolygonHeight", ArgumentParser<GetColPolygonHeight>},
        {"setColPolygonHeight", ArgumentParser<SetColPolygonHeight>},

        {"isInsideColShape", IsInsideColShape},
        {"getColShapeType", GetColShapeType},
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

    lua_classfunction(luaVM, "getRadius", "getColShapeRadius", GetColShapeRadius);
    lua_classfunction(luaVM, "setRadius", "setColShapeRadius", SetColShapeRadius);
    lua_classfunction(luaVM, "getSize", "getColShapeSize", OOP_GetColShapeSize);
    lua_classfunction(luaVM, "setSize", "setColShapeSize", SetColShapeSize);
    lua_classfunction(luaVM, "getPoints", "getColPolygonPoints", OOP_GetColPolygonPoints);
    lua_classfunction(luaVM, "getPointPosition", "getColPolygonPointPosition", OOP_GetColPolygonPointPosition);
    lua_classfunction(luaVM, "setPointPosition", "setColPolygonPointPosition", SetColPolygonPointPosition);
    lua_classfunction(luaVM, "addPoint", "addColPolygonPoint", AddColPolygonPoint);
    lua_classfunction(luaVM, "removePoint", "removeColPolygonPoint", RemoveColPolygonPoint);
    lua_classfunction(luaVM, "setHeight", "setColPolygonHeight", ArgumentParser<SetColPolygonHeight>);
    lua_classfunction(luaVM, "getHeight", "getColPolygonHeight", ArgumentParser<GetColPolygonHeight>);

    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_classvariable(luaVM, "radius", "setColShapeRadius", "getColShapeRadius", SetColShapeRadius, GetColShapeRadius);
    lua_classvariable(luaVM, "size", "setColShapeSize", "getColShapeSize", SetColShapeSize, OOP_GetColShapeSize);
    lua_classvariable(luaVM, "points", nullptr, "getColPolygonPoints", nullptr, OOP_GetColPolygonPoints);
    lua_classvariable(luaVM, "elementsWithin", nullptr, "getElementsWithinColShape");

    lua_registerclass(luaVM, "ColShape", "Element");
}

int CLuaColShapeDefs::GetColShapeType(lua_State* luaVM)
{
    // Verify the arguments
    CColShape*       pColShape = nullptr;
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
    CVector2D vecPosition;
    float     fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColCircle* pShape = CStaticFunctionDefinitions::CreateColCircle(pResource, vecPosition, fRadius);
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
    CVector vecPosition;
    CVector vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadVector3D(vecSize);

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0.0f)
            vecSize.fX = 0.1f;
        if (vecSize.fY < 0.0f)
            vecSize.fY = 0.1f;
        if (vecSize.fZ < 0.0f)
            vecSize.fZ = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColCuboid* pShape = CStaticFunctionDefinitions::CreateColCuboid(pResource, vecPosition, vecSize);
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

int CLuaColShapeDefs::CreateColSphere(lua_State* luaVM)
{
    CVector vecPosition;
    float   fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColSphere* pShape = CStaticFunctionDefinitions::CreateColSphere(pResource, vecPosition, fRadius);
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

int CLuaColShapeDefs::CreateColRectangle(lua_State* luaVM)
{
    CVector2D vecPosition;
    CVector2D vecSize;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector2D(vecPosition);
    argStream.ReadVector2D(vecSize);

    if (!argStream.HasErrors())
    {
        if (vecSize.fX < 0.0f)
            vecSize.fX = 0.1f;
        if (vecSize.fY < 0.0f)
            vecSize.fY = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColRectangle* pShape = CStaticFunctionDefinitions::CreateColRectangle(pResource, vecPosition, vecSize);
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

int CLuaColShapeDefs::CreateColPolygon(lua_State* luaVM)
{
    //  colshape createColPolygon ( float fX, float fY, float fX1, float fY1, float fX2, float fY2, float fX3, float fY3, ... )
    std::vector<CVector2D> vecPointList;

    CScriptArgReader argStream(luaVM);
    for (uint i = 0; !argStream.HasErrors() && (i < 4 || argStream.NextIsVector2D()); i++)
    {
        CVector2D vecPoint;
        argStream.ReadVector2D(vecPoint);
        vecPointList.push_back(vecPoint);
    }

    if (!argStream.HasErrors())
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                CColPolygon* pShape = CStaticFunctionDefinitions::CreateColPolygon(pResource, vecPointList);
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

int CLuaColShapeDefs::CreateColTube(lua_State* luaVM)
{
    CVector vecPosition;
    float   fHeight, fRadius;

    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);
    argStream.ReadNumber(fRadius);
    argStream.ReadNumber(fHeight);

    if (!argStream.HasErrors())
    {
        if (fRadius < 0.0f)
            fRadius = 0.1f;
        if (fHeight < 0.0f)
            fHeight = 0.1f;

        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            CResource* pResource = pLuaMain->GetResource();
            if (pResource)
            {
                // Create it and return it
                CColTube* pShape = CStaticFunctionDefinitions::CreateColTube(pResource, vecPosition, fRadius, fHeight);
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

int CLuaColShapeDefs::IsInsideColShape(lua_State* luaVM)
{
    //  bool isInsideColShape ( colshape theColShape, float posX, float posY, float posZ )
    CColShape* pColShape;
    CVector    vecPosition;

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
    CColShape* pColShape;

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
    CColShape* pColShape;
    float      fRadius;

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
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    switch (pColShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CColRectangle*>(pColShape)->GetSize();
            lua_pushnumber(luaVM, size.fX);
            lua_pushnumber(luaVM, size.fY);
            return 2;
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CColCuboid*>(pColShape)->GetSize();
            lua_pushnumber(luaVM, size.fX);
            lua_pushnumber(luaVM, size.fY);
            lua_pushnumber(luaVM, size.fZ);
            return 3;
        }
        case COLSHAPE_TUBE:
        {
            float fHeight = static_cast<CColTube*>(pColShape)->GetHeight();
            lua_pushnumber(luaVM, fHeight);
            return 1;
        }
    }

    argStream.SetCustomError("ColShape must be Rectangle, Cuboid or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::OOP_GetColShapeSize(lua_State* luaVM)
{
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    switch (pColShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CColRectangle*>(pColShape)->GetSize();
            lua_pushvector(luaVM, size);
            return 1;
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CColCuboid*>(pColShape)->GetSize();
            lua_pushvector(luaVM, size);
            return 1;
        }
        case COLSHAPE_TUBE:
        {
            float fHeight = static_cast<CColTube*>(pColShape)->GetHeight();
            lua_pushnumber(luaVM, fHeight);
            return 1;
        }
    }

    argStream.SetCustomError("ColShape must be Rectangle, Cuboid or Tube");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

int CLuaColShapeDefs::SetColShapeSize(lua_State* luaVM)
{
    CColShape* pColShape;
    CVector    vecSize;

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
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);

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
    CColShape* pColShape;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);

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
    CColShape* pColShape;
    uint       uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);
        CVector2D    vecPoint;
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
    CColShape* pColShape;
    uint       uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);
        CVector2D    vecPoint;
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
    CColShape* pColShape;
    uint       uiPointIndex;
    CVector2D  vecPoint;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);
    argStream.ReadVector2D(vecPoint);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);
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
    CColShape* pColShape;
    uint       uiPointIndex;
    CVector2D  vecPoint;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadVector2D(vecPoint);
    argStream.ReadNumber(uiPointIndex, 0);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);
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
    CColShape* pColShape;
    uint       uiPointIndex;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pColShape);
    argStream.ReadNumber(uiPointIndex);

    if (argStream.HasErrors())
        return luaL_error(luaVM, argStream.GetFullErrorMessage());

    if (pColShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        CColPolygon* pColPolygon = static_cast<CColPolygon*>(pColShape);
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

std::tuple<float, float> CLuaColShapeDefs::GetColPolygonHeight(CColPolygon* pColPolygon)
{
    float fFloor, fCeil;
    pColPolygon->GetHeight(fFloor, fCeil);
    return std::make_tuple(fFloor, fCeil);
}

bool CLuaColShapeDefs::SetColPolygonHeight(CColPolygon* pColPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil)
{
    //  bool SetColPolygonHeight ( colshape theColShape, float floor, float ceil )
    float fFloor, fCeil;

    if (std::holds_alternative<bool>(floor))
        fFloor = std::numeric_limits<float>::lowest();
    else
        fFloor = std::get<float>(floor);

    if (std::holds_alternative<bool>(ceil))
        fCeil = std::numeric_limits<float>::max();
    else
        fCeil = std::get<float>(ceil);

    if (fFloor > fCeil)
        std::swap(fFloor, fCeil);

    return CStaticFunctionDefinitions::SetColPolygonHeight(pColPolygon, fFloor, fCeil);
}
