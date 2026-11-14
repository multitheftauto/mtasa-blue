/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaColShapeDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "lua/CLuaFunctionParser.h"
#include "CClientColManager.h"

void CLuaColShapeDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        {"createColCircle", ArgumentParserWarn<false, CreateColCircle>},
        {"createColCuboid", ArgumentParserWarn<false, CreateColCuboid>},
        {"createColSphere", ArgumentParserWarn<false, CreateColSphere>},
        {"createColRectangle", ArgumentParserWarn<false, CreateColRectangle>},
        {"createColPolygon", ArgumentParserWarn<false, CreateColPolygon>},
        {"createColTube", ArgumentParserWarn<false, CreateColTube>},

        {"getColShapeRadius", ArgumentParserWarn<false, GetColShapeRadius>},
        {"setColShapeRadius", ArgumentParserWarn<false, SetColShapeRadius>},
        {"getColShapeSize", ArgumentParserWarn<false, GetColShapeSize>},
        {"setColShapeSize", ArgumentParserWarn<false, SetColShapeSize>},
        {"getColPolygonPoints", ArgumentParserWarn<false, GetColPolygonPoints>},
        {"getColPolygonPointPosition", ArgumentParserWarn<false, GetColPolygonPointPosition>},
        {"setColPolygonPointPosition", ArgumentParserWarn<false, SetColPolygonPointPosition>},
        {"addColPolygonPoint", ArgumentParserWarn<false, AddColPolygonPoint>},
        {"removeColPolygonPoint", ArgumentParserWarn<false, RemoveColPolygonPoint>},
        {"getColPolygonHeight", ArgumentParser<GetColPolygonHeight>},
        {"setColPolygonHeight", ArgumentParser<SetColPolygonHeight>},

        {"isInsideColShape", ArgumentParserWarn<false, IsInsideColShape>},
        {"getColShapeType", ArgumentParserWarn<false, GetColShapeType>},

        {"showCol", ArgumentParser<SetShowCollision>},
        {"isShowCollisionsEnabled", ArgumentParser<IsShowCollisionsEnabled>}};

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

    lua_classfunction(luaVM, "getRadius", ArgumentParserWarn<false, GetColShapeRadius>);
    lua_classfunction(luaVM, "setRadius", ArgumentParserWarn<false, SetColShapeRadius>);
    lua_classfunction(luaVM, "getSize", ArgumentParserWarn<false, OOP_GetColShapeSize>);
    lua_classfunction(luaVM, "setSize", ArgumentParserWarn<false, SetColShapeSize>);
    lua_classfunction(luaVM, "getPoints", ArgumentParserWarn<false, OOP_GetColPolygonPoints>);
    lua_classfunction(luaVM, "getPointPosition", ArgumentParserWarn<false, OOP_GetColPolygonPointPosition>);
    lua_classfunction(luaVM, "setPointPosition", ArgumentParserWarn<false, SetColPolygonPointPosition>);
    lua_classfunction(luaVM, "addPoint", ArgumentParserWarn<false, AddColPolygonPoint>);
    lua_classfunction(luaVM, "removePoint", ArgumentParserWarn<false, RemoveColPolygonPoint>);
    lua_classfunction(luaVM, "setHeight", ArgumentParser<SetColPolygonHeight>);
    lua_classfunction(luaVM, "getHeight", ArgumentParser<GetColPolygonHeight>);

    lua_classvariable(luaVM, "elementsWithin", nullptr, "getElementsWithinColShape");
    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_classvariable(luaVM, "radius", ArgumentParserWarn<false, SetColShapeRadius>, ArgumentParserWarn<false, GetColShapeRadius>);
    lua_classvariable(luaVM, "size", ArgumentParserWarn<false, SetColShapeSize>, ArgumentParserWarn<false, OOP_GetColShapeSize>);
    lua_classvariable(luaVM, "points", nullptr, ArgumentParserWarn<false, OOP_GetColPolygonPoints>);

    lua_registerclass(luaVM, "ColShape", "Element");
}

template <typename T, typename... Args>
T* CLuaColShapeDefs::CreateColShape(CResource& resource, Args&&... args)
{
    auto* shape = new T(m_pManager, INVALID_ELEMENT_ID, std::forward<Args>(args)...);
    shape->SetParent(resource.GetResourceDynamicEntity());

    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(shape);

    return shape;
}

void CLuaColShapeDefs::RefreshColliders(CClientColShape* shape)
{
    CVector rootPosition;
    m_pRootEntity->GetPosition(rootPosition);
    m_pColManager->DoHitDetection(rootPosition, 0.0f, m_pRootEntity, shape, true);
}

std::variant<CClientColCircle*, bool> CLuaColShapeDefs::CreateColCircle(lua_State* luaVM, CVector2D position, std::optional<float> radius)
{
    float circleRadius = radius.value_or(0.1f);

    if (circleRadius < 0.0f)
        circleRadius = 0.1f;

    return CreateColShape<CClientColCircle>(lua_getownerresource(luaVM), position, circleRadius);
}

std::variant<CClientColCuboid*, bool> CLuaColShapeDefs::CreateColCuboid(lua_State* luaVM, CVector position, CVector size)
{
    if (size.fX < 0.0f)
        size.fX = 0.1f;
    if (size.fY < 0.0f)
        size.fY = 0.1f;

    return CreateColShape<CClientColCuboid>(lua_getownerresource(luaVM), position, size);
}

std::variant<CClientColSphere*, bool> CLuaColShapeDefs::CreateColSphere(lua_State* luaVM, CVector position, std::optional<float> radius)
{
    float sphereRadius = radius.value_or(0.1f);

    if (sphereRadius < 0.0f)
        sphereRadius = 0.1f;

    return CreateColShape<CClientColSphere>(lua_getownerresource(luaVM), position, sphereRadius);
}

std::variant<CClientColRectangle*, bool> CLuaColShapeDefs::CreateColRectangle(lua_State* luaVM, CVector2D position, CVector2D size)
{
    if (size.fX < 0.0f)
        size.fX = 0.1f;
    if (size.fY < 0.0f)
        size.fY = 0.1f;

    return CreateColShape<CClientColRectangle>(lua_getownerresource(luaVM), position, size);
}

std::variant<CClientColPolygon*, bool> CLuaColShapeDefs::CreateColPolygon(lua_State* luaVM, CVector2D position, CVector2D pointA, CVector2D pointB,
                                                                          CVector2D pointC, LuaVarArgs extraPoints)
{
    std::vector<CVector2D> pointList{pointA, pointB, pointC};

    if (extraPoints.has_value())
    {
        const CLuaArguments& arguments = extraPoints.value();
        pointList.reserve(3 + arguments.Count());

        for (std::uint32_t i = 0; i < arguments.Count(); ++i)
        {
            CLuaArgument* argument = arguments[i];

            if (argument->IsNumber())
            {
                if (i + 1 >= arguments.Count() || !arguments[i + 1]->IsNumber())
                    throw std::invalid_argument("Expected a point as two numbers or a vector2");

                pointList.emplace_back(static_cast<float>(argument->GetNumber()), static_cast<float>(arguments[++i]->GetNumber()));
                continue;
            }

            void* userData = argument->GetUserData();

            if (CLuaVector2D* vector = UserDataCast(reinterpret_cast<CLuaVector2D*>(userData), luaVM))
                pointList.emplace_back(*vector);
            else if (CLuaVector3D* vector = UserDataCast(reinterpret_cast<CLuaVector3D*>(userData), luaVM))
                pointList.emplace_back(vector->fX, vector->fY);
            else if (CLuaVector4D* vector = UserDataCast(reinterpret_cast<CLuaVector4D*>(userData), luaVM))
                pointList.emplace_back(vector->fX, vector->fY);
            else
                throw std::invalid_argument("Expected a point as two numbers or a vector2");
        }
    }

    CClientColPolygon* shape = CreateColShape<CClientColPolygon>(lua_getownerresource(luaVM), position);
    for (const CVector2D& point : pointList)
        shape->AddPoint(point);

    return shape;
}

std::variant<CClientColTube*, bool> CLuaColShapeDefs::CreateColTube(lua_State* luaVM, CVector position, std::optional<float> radius,
                                                                    std::optional<float> height)
{
    float tubeRadius = radius.value_or(0.1f);
    float tubeHeight = height.value_or(0.1f);

    if (tubeRadius < 0.0f)
        tubeRadius = 0.1f;

    if (tubeHeight < 0.0f)
        tubeHeight = 0.1f;

    return CreateColShape<CClientColTube>(lua_getownerresource(luaVM), position, tubeRadius, tubeHeight);
}

int CLuaColShapeDefs::GetColShapeType(CClientColShape* shape) noexcept
{
    return static_cast<int>(shape->GetShapeType());
}

bool CLuaColShapeDefs::IsInsideColShape(CClientColShape* shape, CVector position)
{
    return shape->DoHitDetection(position, 0);
}

float CLuaColShapeDefs::GetColShapeRadius(CClientColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_CIRCLE:
            return static_cast<CClientColCircle*>(shape)->GetRadius();
        case COLSHAPE_SPHERE:
            return static_cast<CClientColSphere*>(shape)->GetRadius();
        case COLSHAPE_TUBE:
            return static_cast<CClientColTube*>(shape)->GetRadius();
    }

    throw std::invalid_argument("ColShape must be Circle, Sphere or Tube");
}

bool CLuaColShapeDefs::SetColShapeRadius(CClientColShape* shape, float radius)
{
    if (radius < 0.0f)
        radius = 0.0f;

    switch (shape->GetShapeType())
    {
        case COLSHAPE_CIRCLE:
            static_cast<CClientColCircle*>(shape)->SetRadius(radius);
            break;
        case COLSHAPE_SPHERE:
            static_cast<CClientColSphere*>(shape)->SetRadius(radius);
            break;
        case COLSHAPE_TUBE:
            static_cast<CClientColTube*>(shape)->SetRadius(radius);
            break;
        default:
            throw std::invalid_argument("ColShape must be Circle, Sphere or Tube");
    }

    RefreshColliders(shape);
    return true;
}

std::variant<CLuaMultiReturn<float, float>, CLuaMultiReturn<float, float, float>, float> CLuaColShapeDefs::GetColShapeSize(CClientColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CClientColRectangle*>(shape)->GetSize();
            return CLuaMultiReturn<float, float>(size.fX, size.fY);
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CClientColCuboid*>(shape)->GetSize();
            return CLuaMultiReturn<float, float, float>(size.fX, size.fY, size.fZ);
        }
        case COLSHAPE_TUBE:
            return static_cast<CClientColTube*>(shape)->GetHeight();
    }

    throw std::invalid_argument("ColShape must be Rectangle, Cuboid or Tube");
}

std::variant<CVector2D, CVector, float> CLuaColShapeDefs::OOP_GetColShapeSize(CClientColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
            return static_cast<CClientColRectangle*>(shape)->GetSize();
        case COLSHAPE_CUBOID:
            return static_cast<CClientColCuboid*>(shape)->GetSize();
        case COLSHAPE_TUBE:
            return static_cast<CClientColTube*>(shape)->GetHeight();
    }

    throw std::invalid_argument("ColShape must be Rectangle, Cuboid or Tube");
}

bool CLuaColShapeDefs::SetColShapeSize(CClientColShape* shape, std::variant<CVector, CVector2D, float> size)
{
    CVector newSize;

    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            if (auto* rectangleSize = std::get_if<CVector2D>(&size))
                newSize = CVector(rectangleSize->fX, rectangleSize->fY, 0.0f);
            else if (auto* vectorSize = std::get_if<CVector>(&size))
                newSize = CVector(vectorSize->fX, vectorSize->fY, 0.0f);
            else
                throw std::invalid_argument("ColShape must be Rectangle");

            break;
        }
        case COLSHAPE_CUBOID:
        {
            if (auto* cuboidSize = std::get_if<CVector>(&size))
                newSize = *cuboidSize;
            else
                throw std::invalid_argument("ColShape must be Cuboid");

            break;
        }
        case COLSHAPE_TUBE:
        {
            if (auto* tubeHeight = std::get_if<float>(&size))
                newSize.fX = *tubeHeight;
            else
                throw std::invalid_argument("ColShape must be Tube");

            break;
        }
        default:
            throw std::invalid_argument("ColShape must be Rectangle, Cuboid or Tube");
    }

    newSize.fX = std::max(newSize.fX, 0.0f);
    newSize.fY = std::max(newSize.fY, 0.0f);
    newSize.fZ = std::max(newSize.fZ, 0.0f);

    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
            static_cast<CClientColRectangle*>(shape)->SetSize(newSize);
            break;
        case COLSHAPE_CUBOID:
            static_cast<CClientColCuboid*>(shape)->SetSize(newSize);
            break;
        case COLSHAPE_TUBE:
            static_cast<CClientColTube*>(shape)->SetHeight(newSize.fX);
            break;
    }

    RefreshColliders(shape);
    return true;
}

std::vector<std::tuple<float, float>> CLuaColShapeDefs::GetColPolygonPoints(CClientColPolygon* polygon)
{
    std::vector<std::tuple<float, float>> points;
    points.reserve(polygon->CountPoints());

    for (auto iter = polygon->IterBegin(); iter != polygon->IterEnd(); ++iter)
        points.emplace_back(iter->fX, iter->fY);

    return points;
}

std::vector<CVector2D> CLuaColShapeDefs::OOP_GetColPolygonPoints(CClientColPolygon* polygon)
{
    return std::vector<CVector2D>(polygon->IterBegin(), polygon->IterEnd());
}

std::variant<CLuaMultiReturn<float, float>, bool> CLuaColShapeDefs::GetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex)
{
    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || index >= polygon->CountPoints())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    const CVector2D point = *(polygon->IterBegin() + index);
    return CLuaMultiReturn<float, float>(point.fX, point.fY);
}

std::variant<CVector2D, bool> CLuaColShapeDefs::OOP_GetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex)
{
    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || index >= polygon->CountPoints())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    return *(polygon->IterBegin() + index);
}

bool CLuaColShapeDefs::SetColPolygonPointPosition(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex, CVector2D point)
{
    if (pointIndex <= 0 || !polygon->SetPointPosition(static_cast<unsigned int>(pointIndex - 1), point))
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    RefreshColliders(polygon);
    return true;
}

bool CLuaColShapeDefs::AddColPolygonPoint(lua_State* luaVM, CClientColPolygon* polygon, CVector2D point, std::optional<int> pointIndex)
{
    const int index = pointIndex.value_or(0);

    if (index == 0)
    {
        if (!polygon->AddPoint(point))
            return false;
    }
    else if (!polygon->AddPoint(point, static_cast<unsigned int>(index - 1)))
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    RefreshColliders(polygon);
    return true;
}

bool CLuaColShapeDefs::RemoveColPolygonPoint(lua_State* luaVM, CClientColPolygon* polygon, int pointIndex)
{
    if (polygon->CountPoints() <= 3)
    {
        m_pScriptDebugging->LogWarning(luaVM, "Can't remove the last 3 points");
        return false;
    }

    if (pointIndex > 0 && polygon->RemovePoint(static_cast<unsigned int>(pointIndex - 1)))
    {
        RefreshColliders(polygon);
        return true;
    }

    m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
    return false;
}

CLuaMultiReturn<float, float> CLuaColShapeDefs::GetColPolygonHeight(CClientColPolygon* colPolygon)
{
    float floor;
    float ceil;
    colPolygon->GetHeight(floor, ceil);
    return {floor, ceil};
}

bool CLuaColShapeDefs::SetColPolygonHeight(CClientColPolygon* colPolygon, std::variant<bool, float> floor, std::variant<bool, float> ceil)
{
    float lowest = std::holds_alternative<bool>(floor) ? std::numeric_limits<float>::lowest() : std::get<float>(floor);
    float highest = std::holds_alternative<bool>(ceil) ? std::numeric_limits<float>::max() : std::get<float>(ceil);

    if (lowest > highest)
        std::swap(lowest, highest);

    if (!colPolygon->SetHeight(lowest, highest))
        return false;

    RefreshColliders(colPolygon);
    return true;
}

bool CLuaColShapeDefs::SetShowCollision(bool state)
{
    if (!g_pClientGame->GetDevelopmentMode())
        return false;

    g_pClientGame->SetShowCollision(state);
    return true;
}

bool CLuaColShapeDefs::IsShowCollisionsEnabled()
{
    return g_pClientGame->GetShowCollision();
}
