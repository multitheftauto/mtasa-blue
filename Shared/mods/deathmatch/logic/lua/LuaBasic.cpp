/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include <StdInc.h>
#include "LuaBasic.h"

namespace lua
{
    template <>
    std::string PopPrimitive<std::string>(lua_State* L, std::size_t& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string outValue;
        outValue.assign(lua_tostring(L, index++), uiLength);
        return outValue;
    }


    template <>
    std::string_view PopPrimitive<std::string_view>(lua_State* L, std::size_t& index)
    {
        uint        uiLength = lua_strlen(L, index);
        std::string_view outValue(lua_tostring(L, index++), uiLength);
        return outValue;
    }

    template <>
    int PopPrimitive<int>(lua_State* L, std::size_t& index)
    {
        return static_cast<int>(lua_tonumber(L, index++));
    }

    template <>
    unsigned int PopPrimitive<unsigned int>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned int>(lua_tonumber(L, index++));
    }

    template <>
    short PopPrimitive<short>(lua_State* L, std::size_t& index)
    {
        return static_cast<short>(lua_tonumber(L, index++));
    }

    template <>
    unsigned short PopPrimitive<unsigned short>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned short>(lua_tonumber(L, index++));
    }

    template <>
    char PopPrimitive<char>(lua_State* L, std::size_t& index)
    {
        return static_cast<char>(lua_tonumber(L, index++));
    }

    template <>
    unsigned char PopPrimitive<unsigned char>(lua_State* L, std::size_t& index)
    {
        return static_cast<unsigned char>(lua_tonumber(L, index++));
    }

    template <>
    int64_t PopPrimitive<int64_t>(lua_State* L, std::size_t& index)
    {
        return static_cast<int64_t>(lua_tonumber(L, index++));
    }

    template <>
    uint64_t PopPrimitive<uint64_t>(lua_State* L, std::size_t& index)
    {
        return static_cast<uint64_t>(lua_tonumber(L, index++));
    }

    template <>
    float PopPrimitive<float>(lua_State* L, std::size_t& index)
    {
        return static_cast<float>(lua_tonumber(L, index++));
    }

    template <>
    double PopPrimitive<double>(lua_State* L, std::size_t& index)
    {
        return static_cast<double>(lua_tonumber(L, index++));
    }

    template <>
    bool PopPrimitive<bool>(lua_State* L, std::size_t& index)
    {
        return static_cast<bool>(lua_toboolean(L, index++));
    }

    template <>
    void* PopPrimitive<void*>(lua_State* L, std::size_t& index)
    {
        return lua_touserdata(L, index++);
    }

    // Lua userdata pushing. Cover your eyes!
    namespace detail
    {
        void PushUserDataObject(lua_State* L, size_t id, const char* className, bool skipCache = false)
        {
            static_assert(sizeof(size_t) == sizeof(void*));

            // We store the `id` value in a userdata, not pointers to objects (as its intended to lol)
            const auto PushLightUserdata = [L, id] { lua_pushlightuserdata(L, (void*)reinterpret_cast<unsigned int*>(id)); };
            if (className)
            {
                const auto NewUserData = [L, id] {*(size_t*)lua_newuserdata(L, sizeof(size_t)) = id; };

                if (skipCache)
                    NewUserData();
                else
                {
                    // Lookup the userdata in the cache table
                    lua_pushstring(L, "ud");
                    lua_rawget(L, LUA_REGISTRYINDEX);

                    dassert(lua_istable(L, -1));

                    // First we want to check if we have a userdata for this already
                    PushLightUserdata();
                    lua_rawget(L, -2);

                    if (lua_isnil(L, -1))
                    {
                        lua_pop(L, 1);

                        // we don't have it, create it
                        NewUserData();

                        // save in ud table
                        PushLightUserdata();
                        lua_pushvalue(L, -2);
                        lua_rawset(L, -4);
                    }

                    // userdata is already on the stack, just remove the table
                    lua_remove(L, -2);
                }

                // Assign the class metatable
                lua_getclass(L, className);
                lua_setmetatable(L, -2);            // element
            }
            else
                PushLightUserdata();
        }

        // TODO: Refactor CLuaClassDefs madness
    #ifdef MTA_CLIENT
        const char* GetClassName(const CElement* value) { return CLuaClassDefs::GetEntityClass(const_cast<CElement*>(value)); }
    #else
        const char* GetClassName(const CElement* value) { return CLuaClassDefs::GetElementClass(const_cast<CElement*>(value)); }
    #endif
        const char* GetClassName(const CResource* value) { return CLuaClassDefs::GetResourceClass(const_cast<CResource*>(value)); }
        const char* GetClassName(const CLuaTimer* value) { return CLuaClassDefs::GetTimerClass(const_cast<CLuaTimer*>(value)); }
        const char* GetClassName(const CXMLNode* value) { return CLuaClassDefs::GetXmlNodeClass(const_cast<CXMLNode*>(value)); }
        const char* GetClassName(const void* value) { return CLuaClassDefs::GetObjectClass(const_cast<void*>(value)); }

        template<typename T>
        void Push(lua_State* L, const T* o, size_t id)
        {
    #ifdef MTA_CLIENT
            CLuaMain* main = g_pClientGame->GetLuaManager()->GetVirtualMachine(L);
    #else
            CLuaMain* main = g_pGame->GetLuaManager()->GetVirtualMachine(L);
    #endif
            assert(main);
            PushUserDataObject(L, id, main->IsOOPEnabled() ? GetClassName(o) : (const char*)nullptr);
        }
    };

#ifdef MTA_CLIENT
#ifndef CElement
    using CElement = CClientEntity;
#endif
#else
    void Push(lua_State* L, const CAccount* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CAccessControlList* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CAccessControlListGroup* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CBan* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CTextDisplay* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CTextItem* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CDbJobData* value)
    {
        detail::Push(L, value, value->GetId());
    }

#endif
    void Push(lua_State* L, const CElement* value)
    {
        if (value)
        {
            if (value->IsBeingDeleted())
            {
                lua_pushboolean(L, false);
                return;
            }

            ElementID ID = value->GetID();
            if (ID != INVALID_ELEMENT_ID)
            {
                detail::Push(L, value, ID.Value());
                return;
            }
        }
        lua_pushnil(L);
    }

    void Push(lua_State* L, const CResource* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CXMLNode* value)
    {
        detail::Push(L, value, const_cast<CXMLNode*>(value)->GetID());
    }

    void Push(lua_State* L, const CLuaTimer* value)
    {
        detail::Push(L, value, value->GetScriptID());
    }

    void Push(lua_State* L, const CVector2D& value)
    {
        size_t id = (new CLuaVector2D(value))->GetScriptID();
        detail::PushUserDataObject(L, id, "Vector2", true);
        lua_addtotalbytes(L, LUA_GC_EXTRA_BYTES);
    }

    void Push(lua_State* L, const CVector& value)
    {
        size_t id = (new CLuaVector3D(value))->GetScriptID();
        detail::PushUserDataObject(L, id, "Vector3", true);
        lua_addtotalbytes(L, LUA_GC_EXTRA_BYTES);
    }

    void Push(lua_State* L, const CVector4D& value)
    {
        size_t id = (new CLuaVector4D(value))->GetScriptID();
        detail::PushUserDataObject(L, id, "Vector4", true);
        lua_addtotalbytes(L, LUA_GC_EXTRA_BYTES);
    }

    void Push(lua_State* L, const CMatrix& value)
    {
        size_t id = (new CLuaMatrix(value))->GetScriptID();
        detail::PushUserDataObject(L, id, "Matrix", true);
        lua_addtotalbytes(L, LUA_GC_EXTRA_BYTES);
    }

    void Push(lua_State* L, const CLuaArgument& arg)
    {
        if (arg.GetType() == LUA_TNONE)
        {
            // Calling lua::Push with a LUA_TNONE type value is not allowed, since this is rather error-prone
            // as many callers would not check the stack position after pushing.
            // Hence throw & abort here and let developers fix it.
            throw std::logic_error("lua::Push");
        }
        arg.Push(L);
    }

    void Push(lua_State* L, const CLuaArguments& args)
    {
        args.PushAsTable(L);
    }

    void Push(lua_State* L, const std::string& value)
    {
        lua_pushlstring(L, value.data(), value.length());
    }
}            // namespace mta::impl
