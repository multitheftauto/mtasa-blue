/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaRuntimeCollisionDefs.cpp
 *  PURPOSE:     Runtime collision model Lua bindings
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaEngineDefs.h"
#include "../CClientColModel.h"
#include "../CRuntimeColModel.h"

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>

namespace
{
    constexpr std::size_t kMaxNativeCount = std::numeric_limits<std::uint16_t>::max();
    constexpr std::size_t kMaxNativeVertices = kMaxNativeCount + 1;

    int AbsoluteIndex(lua_State* luaVM, int index)
    {
        return index < 0 ? lua_gettop(luaVM) + index + 1 : index;
    }

    bool ReadFiniteNumber(lua_State* luaVM, int index, float& outValue, std::string& outError, const std::string& path)
    {
        if (lua_type(luaVM, index) != LUA_TNUMBER)
        {
            outError = path + " must be a number";
            return false;
        }

        const lua_Number value = lua_tonumber(luaVM, index);
        if (!std::isfinite(value) || value < -std::numeric_limits<float>::max() || value > std::numeric_limits<float>::max())
        {
            outError = path + " must be finite";
            return false;
        }

        outValue = static_cast<float>(value);
        return true;
    }

    bool ReadVectorTable(lua_State* luaVM, int index, CVector& outValue, std::string& outError, const std::string& path)
    {
        index = AbsoluteIndex(luaVM, index);
        if (!lua_istable(luaVM, index) || lua_objlen(luaVM, index) != 3)
        {
            outError = path + " must be a 3-number table";
            return false;
        }

        float values[3]{};
        for (int component = 0; component < 3; ++component)
        {
            lua_rawgeti(luaVM, index, component + 1);
            const bool success = ReadFiniteNumber(luaVM, -1, values[component], outError, path + "[" + std::to_string(component + 1) + "]");
            lua_pop(luaVM, 1);
            if (!success)
                return false;
        }

        outValue = CVector(values[0], values[1], values[2]);
        return true;
    }

    bool ReadVectorField(lua_State* luaVM, int tableIndex, const char* fieldName, CVector& outValue, std::string& outError, const std::string& path)
    {
        tableIndex = AbsoluteIndex(luaVM, tableIndex);
        lua_getfield(luaVM, tableIndex, fieldName);
        const bool success = ReadVectorTable(luaVM, -1, outValue, outError, path + "." + fieldName);
        lua_pop(luaVM, 1);
        return success;
    }

    bool ReadMaterialField(lua_State* luaVM, int tableIndex, std::uint8_t& outMaterial, std::string& outError, const std::string& path)
    {
        tableIndex = AbsoluteIndex(luaVM, tableIndex);
        lua_getfield(luaVM, tableIndex, "material");

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outMaterial = 0;
            return true;
        }

        if (lua_type(luaVM, -1) != LUA_TNUMBER)
        {
            lua_pop(luaVM, 1);
            outError = path + ".material must be an integer in range 0-178";
            return false;
        }

        const lua_Number value = lua_tonumber(luaVM, -1);
        lua_pop(luaVM, 1);

        if (!std::isfinite(value) || std::floor(value) != value || value < 0 || value > 178)
        {
            outError = path + ".material must be an integer in range 0-178";
            return false;
        }

        outMaterial = static_cast<std::uint8_t>(value);
        return true;
    }

    bool ParseSpheres(lua_State* luaVM, int rootIndex, RuntimeCollision::Model& outModel, std::string& outError)
    {
        rootIndex = AbsoluteIndex(luaVM, rootIndex);
        lua_getfield(luaVM, rootIndex, "spheres");

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            return true;
        }

        if (!lua_istable(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outError = "spheres must be a table";
            return false;
        }

        const int         arrayIndex = AbsoluteIndex(luaVM, -1);
        const std::size_t count = lua_objlen(luaVM, arrayIndex);
        if (count > kMaxNativeCount)
        {
            lua_pop(luaVM, 1);
            outError = "spheres exceeds the native limit of 65535";
            return false;
        }

        outModel.spheres.reserve(count);

        for (std::size_t i = 1; i <= count; ++i)
        {
            lua_rawgeti(luaVM, arrayIndex, static_cast<int>(i));
            if (!lua_istable(luaVM, -1))
            {
                lua_pop(luaVM, 2);
                outError = "spheres[" + std::to_string(i) + "] must be a table";
                return false;
            }

            RuntimeCollision::Sphere sphere;
            const std::string        path = "spheres[" + std::to_string(i) + "]";

            if (!ReadVectorField(luaVM, -1, "position", sphere.position, outError, path))
            {
                lua_pop(luaVM, 2);
                return false;
            }

            lua_getfield(luaVM, -1, "radius");
            const bool radiusOk = ReadFiniteNumber(luaVM, -1, sphere.radius, outError, path + ".radius");
            lua_pop(luaVM, 1);

            if (!radiusOk || !ReadMaterialField(luaVM, -1, sphere.material, outError, path))
            {
                lua_pop(luaVM, 2);
                return false;
            }

            outModel.spheres.push_back(sphere);
            lua_pop(luaVM, 1);
        }

        lua_pop(luaVM, 1);
        return true;
    }

    bool ParseBoxes(lua_State* luaVM, int rootIndex, RuntimeCollision::Model& outModel, std::string& outError)
    {
        rootIndex = AbsoluteIndex(luaVM, rootIndex);
        lua_getfield(luaVM, rootIndex, "boxes");

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            return true;
        }

        if (!lua_istable(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outError = "boxes must be a table";
            return false;
        }

        const int         arrayIndex = AbsoluteIndex(luaVM, -1);
        const std::size_t count = lua_objlen(luaVM, arrayIndex);
        if (count > kMaxNativeCount)
        {
            lua_pop(luaVM, 1);
            outError = "boxes exceeds the native limit of 65535";
            return false;
        }

        outModel.boxes.reserve(count);

        for (std::size_t i = 1; i <= count; ++i)
        {
            lua_rawgeti(luaVM, arrayIndex, static_cast<int>(i));
            if (!lua_istable(luaVM, -1))
            {
                lua_pop(luaVM, 2);
                outError = "boxes[" + std::to_string(i) + "] must be a table";
                return false;
            }

            RuntimeCollision::Box box;
            const std::string     path = "boxes[" + std::to_string(i) + "]";

            if (!ReadVectorField(luaVM, -1, "position", box.position, outError, path) || !ReadVectorField(luaVM, -1, "size", box.size, outError, path) ||
                !ReadMaterialField(luaVM, -1, box.material, outError, path))
            {
                lua_pop(luaVM, 2);
                return false;
            }

            outModel.boxes.push_back(box);
            lua_pop(luaVM, 1);
        }

        lua_pop(luaVM, 1);
        return true;
    }

    bool ParseMeshVertices(lua_State* luaVM, int meshIndex, RuntimeCollision::Mesh& outMesh, std::string& outError, const std::string& path)
    {
        meshIndex = AbsoluteIndex(luaVM, meshIndex);
        lua_getfield(luaVM, meshIndex, "vertices");

        if (!lua_istable(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outError = path + ".vertices must be a flat number table";
            return false;
        }

        const int         verticesIndex = AbsoluteIndex(luaVM, -1);
        const std::size_t componentCount = lua_objlen(luaVM, verticesIndex);
        if (componentCount == 0 || componentCount % 3 != 0)
        {
            lua_pop(luaVM, 1);
            outError = path + ".vertices must contain xyz triplets";
            return false;
        }

        if (componentCount / 3 > kMaxNativeVertices)
        {
            lua_pop(luaVM, 1);
            outError = path + ".vertices exceeds the native limit of 65536";
            return false;
        }

        outMesh.vertices.reserve(componentCount / 3);
        for (std::size_t i = 1; i <= componentCount; i += 3)
        {
            float components[3]{};
            for (int component = 0; component < 3; ++component)
            {
                const std::size_t componentIndex = i + component;
                lua_rawgeti(luaVM, verticesIndex, static_cast<int>(componentIndex));
                const bool success = ReadFiniteNumber(luaVM, -1, components[component], outError, path + ".vertices[" + std::to_string(componentIndex) + "]");
                lua_pop(luaVM, 1);
                if (!success)
                {
                    lua_pop(luaVM, 1);
                    return false;
                }
            }

            outMesh.vertices.emplace_back(components[0], components[1], components[2]);
        }

        lua_pop(luaVM, 1);
        return true;
    }

    bool ParseMeshIndices(lua_State* luaVM, int meshIndex, RuntimeCollision::Mesh& outMesh, std::string& outError, const std::string& path)
    {
        meshIndex = AbsoluteIndex(luaVM, meshIndex);
        lua_getfield(luaVM, meshIndex, "indices");

        if (!lua_istable(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outError = path + ".indices must be a flat integer table";
            return false;
        }

        const int         indicesIndex = AbsoluteIndex(luaVM, -1);
        const std::size_t indexCount = lua_objlen(luaVM, indicesIndex);
        if (indexCount == 0 || indexCount % 3 != 0)
        {
            lua_pop(luaVM, 1);
            outError = path + ".indices must contain triangle triplets";
            return false;
        }

        if (indexCount / 3 > kMaxNativeCount)
        {
            lua_pop(luaVM, 1);
            outError = path + ".indices exceeds the native triangle limit of 65535";
            return false;
        }

        outMesh.indices.reserve(indexCount);
        for (std::size_t i = 1; i <= indexCount; ++i)
        {
            lua_rawgeti(luaVM, indicesIndex, static_cast<int>(i));

            if (lua_type(luaVM, -1) != LUA_TNUMBER)
            {
                lua_pop(luaVM, 2);
                outError = path + ".indices[" + std::to_string(i) + "] must be a non-negative integer";
                return false;
            }

            const lua_Number value = lua_tonumber(luaVM, -1);
            lua_pop(luaVM, 1);

            if (!std::isfinite(value) || std::floor(value) != value || value < 0 || value > std::numeric_limits<std::uint32_t>::max())
            {
                lua_pop(luaVM, 1);
                outError = path + ".indices[" + std::to_string(i) + "] must be a non-negative integer";
                return false;
            }

            outMesh.indices.push_back(static_cast<std::uint32_t>(value));
        }

        lua_pop(luaVM, 1);
        return true;
    }

    bool ParseMeshes(lua_State* luaVM, int rootIndex, RuntimeCollision::Model& outModel, std::string& outError)
    {
        rootIndex = AbsoluteIndex(luaVM, rootIndex);
        lua_getfield(luaVM, rootIndex, "meshes");

        if (lua_isnil(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            return true;
        }

        if (!lua_istable(luaVM, -1))
        {
            lua_pop(luaVM, 1);
            outError = "meshes must be a table";
            return false;
        }

        const int         arrayIndex = AbsoluteIndex(luaVM, -1);
        const std::size_t count = lua_objlen(luaVM, arrayIndex);
        if (count > kMaxNativeCount)
        {
            lua_pop(luaVM, 1);
            outError = "meshes exceeds the native collection limit of 65535";
            return false;
        }

        outModel.meshes.reserve(count);

        std::size_t totalVertices = 0;
        std::size_t totalTriangles = 0;

        for (std::size_t i = 1; i <= count; ++i)
        {
            lua_rawgeti(luaVM, arrayIndex, static_cast<int>(i));
            if (!lua_istable(luaVM, -1))
            {
                lua_pop(luaVM, 2);
                outError = "meshes[" + std::to_string(i) + "] must be a table";
                return false;
            }

            RuntimeCollision::Mesh mesh;
            const std::string      path = "meshes[" + std::to_string(i) + "]";

            if (!ParseMeshVertices(luaVM, -1, mesh, outError, path) || !ParseMeshIndices(luaVM, -1, mesh, outError, path) ||
                !ReadMaterialField(luaVM, -1, mesh.material, outError, path))
            {
                lua_pop(luaVM, 2);
                return false;
            }

            if (mesh.vertices.size() > kMaxNativeVertices - totalVertices)
            {
                lua_pop(luaVM, 2);
                outError = "combined mesh vertex count exceeds 65536";
                return false;
            }

            const std::size_t triangleCount = mesh.indices.size() / 3;
            if (triangleCount > kMaxNativeCount - totalTriangles)
            {
                lua_pop(luaVM, 2);
                outError = "combined triangle count exceeds 65535";
                return false;
            }

            totalVertices += mesh.vertices.size();
            totalTriangles += triangleCount;

            outModel.meshes.push_back(std::move(mesh));
            lua_pop(luaVM, 1);
        }

        lua_pop(luaVM, 1);
        return true;
    }

    bool ParseCollisionTable(lua_State* luaVM, int index, RuntimeCollision::Model& outModel, std::string& outError)
    {
        if (!lua_istable(luaVM, index))
        {
            outError = "expected collision table";
            return false;
        }

        return ParseSpheres(luaVM, index, outModel, outError) && ParseBoxes(luaVM, index, outModel, outError) && ParseMeshes(luaVM, index, outModel, outError);
    }

    bool BuildCollisionBuffer(lua_State* luaVM, int index, std::string& outBuffer, std::string& outError)
    {
        RuntimeCollision::Model model;
        return ParseCollisionTable(luaVM, index, model, outError) && RuntimeCollision::BuildCOLBuffer(model, outBuffer, outError);
    }
}  // namespace

int CLuaEngineDefs::EngineLoadCOLFromTable(lua_State* luaVM)
{
    std::string buffer;
    std::string error;
    if (!BuildCollisionBuffer(luaVM, 1, buffer, error))
        return luaL_error(luaVM, "Bad argument @ 'engineLoadCOL' [Invalid collision data: %s]", error.c_str());

    CLuaMain*  luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    CResource* resource = luaMain ? luaMain->GetResource() : nullptr;
    if (!resource)
    {
        lua_pushboolean(luaVM, false);
        return 1;
    }

    CClientColModel* col = new CClientColModel(m_pManager, INVALID_ELEMENT_ID);
    if (!col->LoadFromGeneratedData(SString(buffer)))
    {
        delete col;
        lua_pushboolean(luaVM, false);
        return 1;
    }

    col->SetParent(resource->GetResourceCOLModelRoot());
    lua_pushelement(luaVM, col);
    return 1;
}

bool CLuaEngineDefs::EngineSetCOLData(lua_State* luaVM, CClientColModel* colModel)
{
    if (!lua_istable(luaVM, 2))
        luaL_error(luaVM, "Bad argument @ 'engineSetCOLData' [Expected table at argument 2]");

    std::string buffer;
    std::string error;
    if (!BuildCollisionBuffer(luaVM, 2, buffer, error))
        luaL_error(luaVM, "Bad argument @ 'engineSetCOLData' [Invalid collision data: %s]", error.c_str());

    return colModel->SetGeneratedData(SString(buffer));
}
