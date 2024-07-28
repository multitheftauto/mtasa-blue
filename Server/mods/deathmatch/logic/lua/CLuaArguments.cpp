/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArguments.cpp
 *  PURPOSE:     Lua argument manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaArguments.h"
#include "CLuaMain.h"
#include "CGame.h"
#include "CScriptDebugging.h"
#include "CPerfStatManager.h"
#include "CDatabaseManager.h"
#include "CBan.h"
#include "CAccount.h"
#include "CAccessControlList.h"
#include "CAccessControlListGroup.h"

#ifndef _WIN32
#include <clocale>
#endif

extern CGame* g_pGame;

CLuaArguments::CLuaArguments(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables) noexcept
{
    // Copy all the arguments
    CopyRecursive(Arguments, pKnownTables);
}

CLuaArgument* CLuaArguments::operator[](const std::size_t uiPosition) const noexcept
{
    if (uiPosition < m_Arguments.size())
        return m_Arguments.at(uiPosition);
    return nullptr;
}

// Slow if used with a constructor as it does a copy twice
const CLuaArguments& CLuaArguments::operator=(const CLuaArguments& Arguments) noexcept
{
    CopyRecursive(Arguments);

    // Return the given reference allowing for chaining
    return Arguments;
}

void CLuaArguments::CopyRecursive(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables) noexcept
{
    // Clear our previous list if any
    DeleteArguments();

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair((CLuaArguments*)&Arguments, (CLuaArguments*)this));

    // Copy all the arguments
    for (const auto& pArg : Arguments)
    {
        m_Arguments.push_back(new CLuaArgument(*pArg, pKnownTables));
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArguments(lua_State* luaVM, int uiIndexBegin) noexcept
{
    // Delete the previous arguments if any
    DeleteArguments();

    CFastHashMap<const void*, CLuaArguments*> knownTables;

    // Start reading arguments until there are none left
    while (lua_type(luaVM, uiIndexBegin) != LUA_TNONE)
    {
        // Create an argument, let it read out the argument and add it to our vector
        m_Arguments.push_back(new CLuaArgument(luaVM, uiIndexBegin++, &knownTables));

        knownTables.clear();
    }
}

void CLuaArguments::ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables) noexcept
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<const void*, CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair(lua_topointer(luaVM, iIndexBegin), this));

    // Delete the previous arguments if any
    DeleteArguments();

    LUA_CHECKSTACK(luaVM, 2);
    lua_pushnil(luaVM); /* first key */
    if (iIndexBegin < 0)
        iIndexBegin--;

    while (lua_next(luaVM, iIndexBegin) != 0)
    {
        /* uses 'key' (at index -2) and 'value' (at index -1) */

        // push the key first
        m_Arguments.push_back(new CLuaArgument(luaVM, -2, pKnownTables));

        // then the value
        m_Arguments.push_back(new CLuaArgument(luaVM, -1, pKnownTables));

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArgument(lua_State* luaVM, int iIndex) noexcept
{
    m_Arguments.push_back(new CLuaArgument(luaVM, iIndex));
}

void CLuaArguments::PushArguments(lua_State* luaVM) const noexcept
{
    // Push all our arguments
    for (const auto& pArg : m_Arguments)
    {
        pArg->Push(luaVM);
    }
}

void CLuaArguments::PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables) const noexcept
{
    // Ensure there is enough space on the Lua stack
    LUA_CHECKSTACK(luaVM, 4);

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, int>();
        bKnownTablesCreated = true;

        lua_newtable(luaVM);
        // using registry to make it fail safe, else we'd have to carry
        // either lua top or current depth variable between calls
        lua_setfield(luaVM, LUA_REGISTRYINDEX, "cache");
    }

    lua_newtable(luaVM);

    // push it onto the known tables
    int size = static_cast<int>(pKnownTables->size());
    lua_getfield(luaVM, LUA_REGISTRYINDEX, "cache");
    lua_pushnumber(luaVM, static_cast<lua_Number>(++size));
    lua_pushvalue(luaVM, -3);
    lua_settable(luaVM, -3);
    lua_pop(luaVM, 1);
    pKnownTables->insert(std::make_pair((CLuaArguments*)this, size));

    std::vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end() && (iter + 1) != m_Arguments.end(); ++iter)
    {
        (*iter)->Push(luaVM, pKnownTables);            // index
        ++iter;
        (*iter)->Push(luaVM, pKnownTables);            // value
        lua_settable(luaVM, -3);
    }

    if (!bKnownTablesCreated)
        return;
        
    // clear the cache
    lua_pushnil(luaVM);
    lua_setfield(luaVM, LUA_REGISTRYINDEX, "cache");
    delete pKnownTables;
}

void CLuaArguments::PushArguments(const CLuaArguments& args) noexcept
{
    for (const auto& arg : args)
    {
        m_Arguments.push_back(new CLuaArgument(*arg));
    }
}

bool CLuaArguments::Call(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues) const noexcept
{
    assert(pLuaMain);
    TIMEUS startTime = GetTimeUs();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine();
    assert(luaVM);
    LUA_CHECKSTACK(luaVM, 1);
    int luaStackPointer = lua_gettop(luaVM);
    lua_getref(luaVM, iLuaFunction.ToInt());

    // Push our arguments onto the stack
    PushArguments(luaVM);

    // Call the function with our arguments
    pLuaMain->ResetInstructionCount();

    int iret = pLuaMain->PCall(luaVM, static_cast<int>(m_Arguments.size()), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        SString strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                returnValues->ReadArgument(luaVM, i);
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, pLuaMain->GetFunctionTag(iLuaFunction.ToInt()), GetTimeUs() - startTime);
    return true;
}

bool CLuaArguments::CallGlobal(CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues) const noexcept
{
    assert(pLuaMain);
    assert(szFunction);
    TIMEUS startTime = GetTimeUs();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine();
    assert(luaVM);
    LUA_CHECKSTACK(luaVM, 1);
    int luaStackPointer = lua_gettop(luaVM);
    lua_pushstring(luaVM, szFunction);
    lua_gettable(luaVM, LUA_GLOBALSINDEX);

    // If that function doesn't exist, return false
    if (lua_isnil(luaVM, -1))
    {
        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;
    }

    // Push our arguments onto the stack
    PushArguments(luaVM);

    // Reset function call timer (checks long-running functions)
    pLuaMain->ResetInstructionCount();

    // Call the function with our arguments
    int iret = pLuaMain->PCall(luaVM, static_cast<int>(m_Arguments.size()), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        std::string strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                returnValues->ReadArgument(luaVM, i);
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, szFunction, GetTimeUs() - startTime);
    return true;
}

CLuaArgument* CLuaArguments::PushNil() noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBoolean(bool bBool) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadBool(bBool);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTable(const CLuaArguments* table) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadTable(table);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushNumber(double dNumber) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadNumber(dNumber);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushArgument(const CLuaArgument& argument) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument(argument);            // create a copy
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushString(const char* string) noexcept
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushString(const std::string& string) noexcept
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushString(const std::string_view& string)noexcept
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushElement(const CElement* pElement) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadElement(pElement);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBan(const CBan* pBan) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pBan->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACL(const CAccessControlList* pACL) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACL->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACLGroup(const CAccessControlListGroup* pACLGroup) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACLGroup->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushAccount(const CAccount* pAccount) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pAccount->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushResource(const CResource* pResource) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pResource->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextDisplay(const CTextDisplay* pTextDisplay) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextDisplay->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextItem(const CTextItem* pTextItem) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextItem->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTimer(const CLuaTimer* pLuaTimer) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pLuaTimer->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushDbQuery(const CDbJobData* pJobData) noexcept
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(static_cast<std::uint32_t>(pJobData->GetId()));
    m_Arguments.push_back(pArgument);
    return pArgument;
}

void CLuaArguments::DeleteArguments() noexcept
{
    // Delete each item
    for (const auto& pArg : m_Arguments)
    {
        delete pArg;
    }

    // Clear the vector
    m_Arguments.clear();
}

// Gets rid of the last argument in the list
void CLuaArguments::Pop() noexcept
{
    // Delete the last element
    delete m_Arguments.back();

    // Pop it out of the vector
    m_Arguments.pop_back();
}

void CLuaArguments::ValidateTableKeys() noexcept
{
    // Iterate over m_Arguments as pairs
    // If first is LUA_TNIL, then remove pair
    vector<CLuaArgument*>::iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end();)
    {
        // Check first in pair
        if ((*iter)->GetType() == LUA_TNIL)
        {
            // TODO - Handle ref in KnownTables
            // Remove pair
            delete *iter;
            iter = m_Arguments.erase(iter);
            if (iter != m_Arguments.end())
            {
                delete *iter;
                iter = m_Arguments.erase(iter);
            }
            // Check if end
            if (iter == m_Arguments.end())
                break;
            continue;
        }

        // Skip second in pair
        ++iter;
        // Check if end
        if (iter == m_Arguments.end())
            break;

        ++iter;
    }
}

bool CLuaArguments::ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    std::uint32_t uiNumArgs;
    if (bitStream.ReadCompressed(uiNumArgs))
    {
        pKnownTables->push_back(this);
        for (std::uint32_t i = 0; i < uiNumArgs; i++)
        {
            CLuaArgument* pArgument = new CLuaArgument();
            if (!pArgument->ReadFromBitStream(bitStream, pKnownTables))
            {
                delete pArgument;
                if (bKnownTablesCreated)
                    delete pKnownTables;
                return false;
            }
            m_Arguments.push_back(pArgument);
        }
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return true;
}

bool CLuaArguments::WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables) const noexcept
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, std::uint32_t>();
        bKnownTablesCreated = true;
    }

    bool bSuccess = true;
    pKnownTables->insert(make_pair((CLuaArguments*)this, pKnownTables->size()));
    bitStream.WriteCompressed(static_cast<std::uint32_t>(m_Arguments.size()));

    for (const auto& pArg : m_Arguments)
    {
        if (pArg->WriteToBitStream(bitStream, pKnownTables))
            continue;
            
        bSuccess = false;
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::WriteToJSONString(std::string& strJSON, bool bSerialize, int flags) noexcept
{
    json_object* my_array = WriteToJSONArray(bSerialize);
    if (!my_array)
        return false;
        
    strJSON = json_object_to_json_string_ext(my_array, flags);
    json_object_put(my_array);            // dereference - causes a crash, is actually commented out in the example too
    return true;
}

json_object* CLuaArguments::WriteToJSONArray(bool bSerialize) noexcept
{
    json_object* my_array = json_object_new_array();
    for (const auto& pArg : m_Arguments)
    {
        json_object* object = pArg->WriteToJSONObject(bSerialize);
        if(!object)
            break;
         
        json_object_array_add(my_array, object);
    }
    return my_array;
}

json_object* CLuaArguments::WriteTableToJSONObject(bool bSerialize, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables) noexcept
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, std::uint32_t>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair(this, pKnownTables->size()));

    bool bIsArray = true;
    std::uint32_t iArrayPos = 1;            // lua arrays are 1 based
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter += 2)
    {
        CLuaArgument* pArgument = *iter;
        if (pArgument->GetType() != LUA_TNUMBER)
        {
            bIsArray = false;
            break;
        }
        
        double num = pArgument->GetNumber();
        auto iNum = static_cast<std::uint32_t>(num);
        if (num != iNum)
        {
            bIsArray = false;
            break;
        }
        
        // check if the value matches its index in the table
        if (iArrayPos != iNum)
        {
            bIsArray = false;
            break;
        }
        iArrayPos++;
    }

    if (bIsArray)
    {
        json_object*                          my_array = json_object_new_array();
        vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
        for (; iter != m_Arguments.end(); ++iter)
        {
            iter++;            // skip the key values
            CLuaArgument* pArgument = *iter;
            json_object* object = pArgument->WriteToJSONObject(bSerialize, pKnownTables);
            if (!object)
                break;
                
            json_object_array_add(my_array, object);
        }
        if (bKnownTablesCreated)
            delete pKnownTables;

        return my_array;
    }
    
    json_object* my_object = json_object_new_object();
    iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        char szKey[255]{0};
        CLuaArgument* pArgument = *iter;
        if (!pArgument->WriteToString(szKey, 255)) // index
            break;
        ++iter;
        pArgument = *iter;
        json_object* object = pArgument->WriteToJSONObject(bSerialize, pKnownTables); // value

        if (!object)
            break;
            
        json_object_object_add(my_object, szKey, object);
    }
    if (bKnownTablesCreated)
        delete pKnownTables;
    return my_object;
}

bool CLuaArguments::ReadFromJSONString(const char* szJSON) noexcept
{
    // Fast isJSON check: Check first non-white space character is '[' or '{'
    const char* ptr = szJSON;
    while(true) {
        char c = *ptr++;
        if (c == '[' || c == '{')
            break;
        if (isspace((uchar)c))
            continue;
        return false;
    }

    json_object* object = json_tokener_parse(szJSON);
    if (!object)
        return false;
        
    if (json_object_get_type(object) == json_type_array)
    {
        bool bSuccess = true;

        std::vector<CLuaArguments*> knownTables;

        for (std::size_t i = 0; i < json_object_array_length(object); i++)
        {
            json_object* arrayObject = json_object_array_get_idx(object, i);
            CLuaArgument* pArgument = new CLuaArgument();
            bSuccess = pArgument->ReadFromJSONObject(arrayObject, &knownTables);
            m_Arguments.push_back(pArgument);            // then the value
            if (!bSuccess)
                break;
        }
        json_object_put(object);            // dereference
        return bSuccess;
    }
    else if (json_object_get_type(object) == json_type_object)
    {
        std::vector<CLuaArguments*> knownTables;
        CLuaArgument* pArgument = new CLuaArgument();
        bool bSuccess = pArgument->ReadFromJSONObject(object, &knownTables);
        m_Arguments.push_back(pArgument);            // value
        json_object_put(object);

        return bSuccess;
    }
    
    // dereference
    json_object_put(object);
    
    return false;
}

bool CLuaArguments::ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    if (!object)
        return false;
        
    if (json_object_get_type(object) != json_type_object)
        return false;
        
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->push_back(this);

    bool bSuccess = true;
    json_object_object_foreach(object, key, val)
    {
        // push the key first
        CLuaArgument* pArgument = new CLuaArgument();
        pArgument->ReadString(key);
        m_Arguments.push_back(pArgument);

        // then the value
        pArgument = new CLuaArgument();
        bSuccess = pArgument->ReadFromJSONObject(val, pKnownTables);
        m_Arguments.push_back(pArgument);

        if (!bSuccess)
            break;
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    if (!object)
        return false;
        
    if (json_object_get_type(object) != json_type_array)
        return false;
        
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->push_back(this);

    bool bSuccess = true;
    for (uint i = 0; i < json_object_array_length(object); i++)
    {
        json_object* arrayObject = json_object_array_get_idx(object, i);
        
        // push the key
        CLuaArgument* pArgument = new CLuaArgument();
        pArgument->ReadNumber(i + 1);
        m_Arguments.push_back(pArgument);

        // then the valoue
        pArgument = new CLuaArgument();
        bSuccess = pArgument->ReadFromJSONObject(arrayObject, pKnownTables);
        m_Arguments.push_back(pArgument);
        if (!bSuccess)
            break;
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::IsEqualTo(const CLuaArguments& compareTo, std::set<const CLuaArguments*>* knownTables) const noexcept
{
    if (m_Arguments.size() != compareTo.m_Arguments.size())
        return false;

    if (knownTables)
    {
        if (knownTables->find(&compareTo) != knownTables->end())
            return true;

        knownTables->insert(&compareTo);
    }

    return std::equal(std::begin(m_Arguments), std::end(m_Arguments), std::begin(compareTo.m_Arguments),
                      [knownTables](const CLuaArgument* lhs, const CLuaArgument* rhs) { return lhs->IsEqualTo(*rhs, knownTables); });
}
