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
#include "CColCircle.h"
#include "CColCuboid.h"
#include "CColManager.h"
#include "CColPolygon.h"
#include "CColRectangle.h"
#include "CColSphere.h"
#include "CColTube.h"
#include "CDummy.h"
#include "CResource.h"
#include "common/CBitStream.h"
#include "packets/CEntityAddPacket.h"
#include "packets/CElementRPCPacket.h"
#include <net/SyncStructures.h>
#include <net/rpc_enums.h>

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

    lua_classfunction(luaVM, "getRadius", "getColShapeRadius", ArgumentParserWarn<false, GetColShapeRadius>);
    lua_classfunction(luaVM, "setRadius", "setColShapeRadius", ArgumentParserWarn<false, SetColShapeRadius>);
    lua_classfunction(luaVM, "getSize", "getColShapeSize", ArgumentParserWarn<false, OOP_GetColShapeSize>);
    lua_classfunction(luaVM, "setSize", "setColShapeSize", ArgumentParserWarn<false, SetColShapeSize>);
    lua_classfunction(luaVM, "getPoints", "getColPolygonPoints", ArgumentParserWarn<false, OOP_GetColPolygonPoints>);
    lua_classfunction(luaVM, "getPointPosition", "getColPolygonPointPosition", ArgumentParserWarn<false, OOP_GetColPolygonPointPosition>);
    lua_classfunction(luaVM, "setPointPosition", "setColPolygonPointPosition", ArgumentParserWarn<false, SetColPolygonPointPosition>);
    lua_classfunction(luaVM, "addPoint", "addColPolygonPoint", ArgumentParserWarn<false, AddColPolygonPoint>);
    lua_classfunction(luaVM, "removePoint", "removeColPolygonPoint", ArgumentParserWarn<false, RemoveColPolygonPoint>);
    lua_classfunction(luaVM, "setHeight", "setColPolygonHeight", ArgumentParser<SetColPolygonHeight>);
    lua_classfunction(luaVM, "getHeight", "getColPolygonHeight", ArgumentParser<GetColPolygonHeight>);

    lua_classvariable(luaVM, "shapeType", nullptr, "getColShapeType");

    lua_classvariable(luaVM, "radius", "setColShapeRadius", "getColShapeRadius", ArgumentParserWarn<false, SetColShapeRadius>,
                      ArgumentParserWarn<false, GetColShapeRadius>);
    lua_classvariable(luaVM, "size", "setColShapeSize", "getColShapeSize", ArgumentParserWarn<false, SetColShapeSize>,
                      ArgumentParserWarn<false, OOP_GetColShapeSize>);
    lua_classvariable(luaVM, "points", nullptr, "getColPolygonPoints", nullptr, ArgumentParserWarn<false, OOP_GetColPolygonPoints>);
    lua_classvariable(luaVM, "elementsWithin", nullptr, "getElementsWithinColShape");

    lua_registerclass(luaVM, "ColShape", "Element");
}

template <typename T, typename... Args>
T* CLuaColShapeDefs::CreateColShape(CResource& resource, Args&&... args)
{
    auto* shape = new T(m_pColManager, resource.GetDynamicElementRoot(), std::forward<Args>(args)...);
    RefreshColliders(shape);

    if (resource.IsClientSynced())
    {
        CEntityAddPacket packet;
        packet.Add(shape);
        m_pPlayerManager->BroadcastOnlyJoined(packet);
    }

    if (CElementGroup* elementGroup = resource.GetElementGroup())
        elementGroup->Add(shape);

    return shape;
}

void CLuaColShapeDefs::RefreshColliders(CColShape* shape)
{
    m_pColManager->DoHitDetection(m_pRootElement->GetPosition(), m_pRootElement, shape, true);
}

std::variant<CColCircle*, bool> CLuaColShapeDefs::CreateColCircle(lua_State* luaVM, CVector2D position, float radius)
{
    if (radius < 0.0f)
        radius = 0.1f;

    return CreateColShape<CColCircle>(lua_getownerresource(luaVM), position, radius);
}

std::variant<CColCuboid*, bool> CLuaColShapeDefs::CreateColCuboid(lua_State* luaVM, CVector position, CVector size)
{
    if (size.fX < 0.0f)
        size.fX = 0.1f;
    if (size.fY < 0.0f)
        size.fY = 0.1f;
    if (size.fZ < 0.0f)
        size.fZ = 0.1f;

    return CreateColShape<CColCuboid>(lua_getownerresource(luaVM), position, size);
}

std::variant<CColSphere*, bool> CLuaColShapeDefs::CreateColSphere(lua_State* luaVM, CVector position, float radius)
{
    if (radius < 0.0f)
        radius = 0.1f;

    return CreateColShape<CColSphere>(lua_getownerresource(luaVM), position, radius);
}

std::variant<CColRectangle*, bool> CLuaColShapeDefs::CreateColRectangle(lua_State* luaVM, CVector2D position, CVector2D size)
{
    if (size.fX < 0.0f)
        size.fX = 0.1f;
    if (size.fY < 0.0f)
        size.fY = 0.1f;

    return CreateColShape<CColRectangle>(lua_getownerresource(luaVM), position, size);
}

std::variant<CColPolygon*, bool> CLuaColShapeDefs::CreateColPolygon(lua_State* luaVM, CVector2D center, CVector2D pointA, CVector2D pointB, CVector2D pointC,
                                                                    LuaVarArgs extraPoints)
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

    auto* polygon = CreateColShape<CColPolygon>(lua_getownerresource(luaVM), CVector(center.fX, center.fY, 0.0f));
    for (const CVector2D& point : pointList)
        polygon->AddPoint(point);

    return polygon;
}

std::variant<CColTube*, bool> CLuaColShapeDefs::CreateColTube(lua_State* luaVM, CVector position, float radius, float height)
{
    if (radius < 0.0f)
        radius = 0.1f;
    if (height < 0.0f)
        height = 0.1f;

    return CreateColShape<CColTube>(lua_getownerresource(luaVM), position, radius, height);
}

int CLuaColShapeDefs::GetColShapeType(CColShape* shape) noexcept
{
    return static_cast<int>(shape->GetShapeType());
}

bool CLuaColShapeDefs::IsInsideColShape(CColShape* shape, CVector position)
{
    return shape->DoHitDetection(position);
}

float CLuaColShapeDefs::GetColShapeRadius(CColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_CIRCLE:
            return static_cast<CColCircle*>(shape)->GetRadius();
        case COLSHAPE_SPHERE:
            return static_cast<CColSphere*>(shape)->GetRadius();
        case COLSHAPE_TUBE:
            return static_cast<CColTube*>(shape)->GetRadius();
    }

    throw std::invalid_argument("ColShape must be Circle, Sphere or Tube");
}

bool CLuaColShapeDefs::SetColShapeRadius(CColShape* shape, float radius)
{
    if (radius < 0.0f)
        radius = 0.0f;

    switch (shape->GetShapeType())
    {
        case COLSHAPE_CIRCLE:
            static_cast<CColCircle*>(shape)->SetRadius(radius);
            break;
        case COLSHAPE_SPHERE:
            static_cast<CColSphere*>(shape)->SetRadius(radius);
            break;
        case COLSHAPE_TUBE:
            static_cast<CColTube*>(shape)->SetRadius(radius);
            break;
        default:
            throw std::invalid_argument("ColShape must be Circle, Sphere or Tube");
    }

    RefreshColliders(shape);

    CBitStream bitStream;
    bitStream.pBitStream->Write(radius);
    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(shape, SET_COLSHAPE_RADIUS, *bitStream.pBitStream));

    return true;
}

std::variant<CLuaMultiReturn<float, float>, CLuaMultiReturn<float, float, float>, float> CLuaColShapeDefs::GetColShapeSize(CColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
        {
            CVector2D size = static_cast<CColRectangle*>(shape)->GetSize();
            return CLuaMultiReturn<float, float>(size.fX, size.fY);
        }
        case COLSHAPE_CUBOID:
        {
            CVector size = static_cast<CColCuboid*>(shape)->GetSize();
            return CLuaMultiReturn<float, float, float>(size.fX, size.fY, size.fZ);
        }
        case COLSHAPE_TUBE:
            return static_cast<CColTube*>(shape)->GetHeight();
    }

    throw std::invalid_argument("ColShape must be Rectangle, Cuboid or Tube");
}

std::variant<CVector2D, CVector, float> CLuaColShapeDefs::OOP_GetColShapeSize(CColShape* shape)
{
    switch (shape->GetShapeType())
    {
        case COLSHAPE_RECTANGLE:
            return static_cast<CColRectangle*>(shape)->GetSize();
        case COLSHAPE_CUBOID:
            return static_cast<CColCuboid*>(shape)->GetSize();
        case COLSHAPE_TUBE:
            return static_cast<CColTube*>(shape)->GetHeight();
    }

    throw std::invalid_argument("ColShape must be Rectangle, Cuboid or Tube");
}

bool CLuaColShapeDefs::SetColShapeSize(CColShape* shape, std::variant<CVector, CVector2D, float> size)
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
            static_cast<CColRectangle*>(shape)->SetSize(newSize);
            break;
        case COLSHAPE_CUBOID:
            static_cast<CColCuboid*>(shape)->SetSize(newSize);
            break;
        case COLSHAPE_TUBE:
            static_cast<CColTube*>(shape)->SetHeight(newSize.fX);
            break;
    }

    RefreshColliders(shape);

    CBitStream bitStream;
    bitStream.pBitStream->WriteVector(newSize.fX, newSize.fY, newSize.fZ);
    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(shape, SET_COLSHAPE_SIZE, *bitStream.pBitStream));

    return true;
}

std::vector<std::tuple<float, float>> CLuaColShapeDefs::GetColPolygonPoints(CColShape* shape)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    std::vector<std::tuple<float, float>> points;
    points.reserve(polygon->CountPoints());

    for (auto iter = polygon->IterBegin(); iter != polygon->IterEnd(); ++iter)
        points.emplace_back(iter->fX, iter->fY);

    return points;
}

std::vector<CVector2D> CLuaColShapeDefs::OOP_GetColPolygonPoints(CColShape* shape)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    return std::vector<CVector2D>(polygon->IterBegin(), polygon->IterEnd());
}

std::variant<CLuaMultiReturn<float, float>, bool> CLuaColShapeDefs::GetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || index >= polygon->CountPoints())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    const CVector2D point = *(polygon->IterBegin() + index);
    return CLuaMultiReturn<float, float>(point.fX, point.fY);
}

std::variant<CVector2D, bool> CLuaColShapeDefs::OOP_GetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || index >= polygon->CountPoints())
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    return *(polygon->IterBegin() + index);
}

bool CLuaColShapeDefs::SetColPolygonPointPosition(lua_State* luaVM, CColShape* shape, int pointIndex, CVector2D point)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || !polygon->SetPointPosition(index, point))
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    RefreshColliders(polygon);

    CBitStream      bitStream;
    SPosition2DSync position(false);
    position.data.vecPosition = point;
    bitStream.pBitStream->Write(&position);
    bitStream.pBitStream->Write(index);
    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(polygon, UPDATE_COLPOLYGON_POINT, *bitStream.pBitStream));

    return true;
}

bool CLuaColShapeDefs::AddColPolygonPoint(lua_State* luaVM, CColShape* shape, CVector2D point, std::optional<int> pointIndex)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

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

    CBitStream      bitStream;
    SPosition2DSync position(false);
    position.data.vecPosition = point;
    bitStream.pBitStream->Write(&position);

    if (index > 0)
        bitStream.pBitStream->Write(static_cast<unsigned int>(index - 1));

    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(polygon, ADD_COLPOLYGON_POINT, *bitStream.pBitStream));

    return true;
}

bool CLuaColShapeDefs::RemoveColPolygonPoint(lua_State* luaVM, CColShape* shape, int pointIndex)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
        throw std::invalid_argument("ColShape must be Polygon");

    auto* polygon = static_cast<CColPolygon*>(shape);

    if (polygon->CountPoints() <= 3)
    {
        m_pScriptDebugging->LogWarning(luaVM, "Can't remove the last 3 points");
        return false;
    }

    const auto index = static_cast<unsigned int>(pointIndex - 1);
    if (pointIndex <= 0 || !polygon->RemovePoint(index))
    {
        m_pScriptDebugging->LogWarning(luaVM, "Invalid point index");
        return false;
    }

    RefreshColliders(polygon);

    CBitStream bitStream;
    bitStream.pBitStream->Write(index);
    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(polygon, REMOVE_COLPOLYGON_POINT, *bitStream.pBitStream));

    return true;
}

CLuaMultiReturn<float, float> CLuaColShapeDefs::GetColPolygonHeight(CColShape* shape)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
    {
        throw std::invalid_argument("Shape must be a polygon");
    }

    auto* polygon = static_cast<CColPolygon*>(shape);

    float floor;
    float ceil;

    polygon->GetHeight(floor, ceil);

    return {floor, ceil};
}

bool CLuaColShapeDefs::SetColPolygonHeight(CColShape* shape, std::variant<bool, float> floor, std::variant<bool, float> ceil)
{
    if (shape->GetShapeType() != COLSHAPE_POLYGON)
    {
        throw std::invalid_argument("Shape must be a polygon");
    }

    auto* polygon = static_cast<CColPolygon*>(shape);

    float lowest = std::holds_alternative<bool>(floor) ? std::numeric_limits<float>::lowest() : std::get<float>(floor);
    float highest = std::holds_alternative<bool>(ceil) ? std::numeric_limits<float>::max() : std::get<float>(ceil);

    if (lowest > highest)
    {
        std::swap(lowest, highest);
    }

    if (!polygon->SetHeight(lowest, highest))
        return false;

    RefreshColliders(polygon);

    CBitStream bitStream;
    bitStream.pBitStream->Write(lowest);
    bitStream.pBitStream->Write(highest);
    m_pPlayerManager->BroadcastOnlyJoined(CElementRPCPacket(polygon, SET_COLPOLYGON_HEIGHT, *bitStream.pBitStream));

    return true;
}
