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

void CLuaColShapeDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createColCircle", CreateColCircle},
        {"createColCuboid", CreateColCuboid},
        {"createColSphere", CreateColSphere},
        {"createColRectangle", CreateColRectangle},
        {"createColPolygon", CreateColPolygon},
        {"createColTube", CreateColTube},

        {"getColShapeRadius", ArgumentParser<GetColShapeRadius>},
        {"setColShapeRadius", ArgumentParser<SetColShapeRadius>},
        {"getColShapeSize", ArgumentParser<GetColShapeSize>},
        {"setColShapeSize", SetColShapeSize},
        {"getColPolygonPoints", GetColPolygonPoints},
        {"getColPolygonPointPosition", ArgumentParser<GetColPolygonPointPosition>},
        {"setColPolygonPointPosition", ArgumentParser<SetColPolygonPointPosition>},
        {"addColPolygonPoint", ArgumentParser<AddColPolygonPoint>},
        {"removeColPolygonPoint", ArgumentParser<RemoveColPolygonPoint>},

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

    lua_classfunction(luaVM, "getRadius", ArgumentParser<GetColShapeRadius>);
    lua_classfunction(luaVM, "setRadius", ArgumentParser<SetColShapeRadius>);
    lua_classfunction(luaVM, "getSize", ArgumentParser<GetColShapeSize_OOP>);
    lua_classfunction(luaVM, "setSize", SetColShapeSize);
    lua_classfunction(luaVM, "getPoints", GetColPolygonPoints);
    lua_classfunction(luaVM, "getPointPosition", ArgumentParser<GetColPolygonPointPosition_OOP>);
    lua_classfunction(luaVM, "setPointPosition", ArgumentParser<SetColPolygonPointPosition>);
    lua_classfunction(luaVM, "addPoint", ArgumentParser<AddColPolygonPoint>);
    lua_classfunction(luaVM, "removePoint", ArgumentParser<RemoveColPolygonPoint>);

    lua_classvariable(luaVM, "elementsWithin", nullptr, "getElementsWithinColShape");
    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_classvariable(luaVM, "radius", ArgumentParser<SetColShapeRadius>, ArgumentParser<GetColShapeRadius>);
    lua_classvariable(luaVM, "size", ArgumentParser<SetColShapeSize>, ArgumentParser<GetColShapeSize>);
    lua_classvariable(luaVM, "points", nullptr, GetColPolygonPoints);

    lua_registerclass(luaVM, "ColShape", "Element");
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

float CLuaColShapeDefs::GetColShapeRadius(CClientColShape* const colShape)
{
    float radius;
    if (!CStaticFunctionDefinitions::GetColShapeRadius(colShape, radius))
        throw std::invalid_argument("ColShape must be either a circle, sphere or tube");
    return radius;
}

bool CLuaColShapeDefs::SetColShapeRadius(CClientColShape* const colShape, const float radius)
{
    if (!CStaticFunctionDefinitions::SetColShapeRadius(colShape, std::max(0.0f, radius)))
        throw std::invalid_argument("ColShape must be either a circle, sphere or tube");
    return true;
}

std::variant<CVector, CVector2D, float> CLuaColShapeDefs::GetColShapeSize_OOP(CClientColShape* const colShape)
{
    switch (colShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
            return static_cast<CClientColRectangle*>(colShape)->GetSize();
        case COLSHAPE_CUBOID:
            return static_cast<CClientColCuboid*>(colShape)->GetSize();
        case COLSHAPE_TUBE:
            return static_cast<CClientColTube*>(colShape)->GetHeight();
        default:
            throw std::invalid_argument("ColShape must be either a rectangle, cuboid or tube");
    }
}

// TODO: refactor this, when std::optional support(to Push) is added to the new arg parser
std::variant<std::tuple<float>, std::tuple<float, float>, std::tuple<float, float, float>> CLuaColShapeDefs::GetColShapeSize(CClientColShape* const colShape)
{
    switch (colShape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            const auto& size = static_cast<CClientColRectangle*>(colShape)->GetSize();
            return std::tuple<float, float>{size.fX, size.fY};
        }
        case COLSHAPE_CUBOID:
        {
            const auto& size = static_cast<CClientColCuboid*>(colShape)->GetSize();
            return std::tuple<float, float, float>{size.fX, size.fY, size.fZ};
        }
        case COLSHAPE_TUBE:
        {
            return std::tuple<float>(static_cast<CClientColTube*>(colShape)->GetHeight());
        }
        default:
            throw std::invalid_argument("ColShape must be either a rectangle, cuboid or tube");
    }
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
            argStream.SetCustomError("ColShape must be rectangle, cuboid or tube");
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
            lua_pushnumber(luaVM, ++uiIndex);
            lua_pushvector(luaVM, *iter);
            lua_settable(luaVM, -3);
        }
        return 1;
    }

    argStream.SetCustomError("ColShape must be polygon");
    return luaL_error(luaVM, argStream.GetFullErrorMessage());
}

const CVector2D& CLuaColShapeDefs::GetColPolygonPointPosition_OOP(CClientColShape* const colShape, unsigned int pointIndex)
{
    if (colShape->GetShapeType() == COLSHAPE_POLYGON)
    {
        // Lua indexing starts at 1, we start at 0
        pointIndex--;

        CClientColPolygon* const colPolygon = static_cast<CClientColPolygon*>(colShape);
        if (pointIndex < colPolygon->CountPoints())
        {
            return *(colPolygon->IterBegin() + pointIndex);
        }
    }
}

bool CLuaColShapeDefs::SetColPolygonPointPosition(CClientColShape* const colShape, unsigned int pointIndex, const CVector2D point)
{
    if (colShape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be a polygon");

    pointIndex--;            // lua starts at 1, we at 0

    if (!CStaticFunctionDefinitions::SetColPolygonPointPosition(static_cast<CClientColPolygon*>(colShape), pointIndex, point))
        std::invalid_argument("Invalid point index");

    return true;
}

bool CLuaColShapeDefs::AddColPolygonPoint(CClientColShape* const colShape, const CVector2D point, const std::optional<unsigned int> optPointIndex)
{
    if (colShape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be polygon");

    const auto colPolygon = static_cast<CClientColPolygon*>(colShape);
    if (!optPointIndex)
        return CStaticFunctionDefinitions::AddColPolygonPoint(colPolygon, point);
    else
    {
        // - 1 because lua starts at 1, and we at 0
        return CStaticFunctionDefinitions::AddColPolygonPoint(colPolygon, optPointIndex.value() - 1, point);
    }

    throw std::invalid_argument("Invalid point index");
}

bool CLuaColShapeDefs::RemoveColPolygonPoint(CClientColShape* const colShape, unsigned int pointIndex)
{
    if (colShape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be polygon");

    // Lua indexing starts at 1, we start at 0.
    pointIndex--;

    const auto colPolygon = static_cast<CClientColPolygon*>(colShape);
    if (colPolygon->CountPoints() <= 3)
        throw std::invalid_argument("Can't remove the last 3 points");

    if (!CStaticFunctionDefinitions::RemoveColPolygonPoint(colPolygon, pointIndex - 1))
        throw std::invalid_argument("Invalid point index");

    return true;
}
