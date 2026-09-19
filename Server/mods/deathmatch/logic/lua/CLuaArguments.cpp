/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArguments.cpp
 *  PURPOSE:     Lua argument manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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

#ifndef WIN32
    #include <clocale>
#endif

extern CGame* g_pGame;

#ifndef VERIFY_ELEMENT
    #define VERIFY_ELEMENT(element) (g_pGame->GetMapManager()->GetRootElement()->IsMyChild(element, true) && !element->IsBeingDeleted())
#endif

CLuaArguments::CLuaArguments(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables)
{
    // Copy all the arguments
    CopyRecursive(Arguments, pKnownTables);
}

CLuaArgument* CLuaArguments::operator[](const unsigned int uiPosition) const
{
    if (uiPosition < m_Arguments.size())
        return m_Arguments.at(uiPosition);
    return NULL;
}

// Slow if used with a constructor as it does a copy twice
const CLuaArguments& CLuaArguments::operator=(const CLuaArguments& Arguments)
{
    CopyRecursive(Arguments);

    // Return the given reference allowing for chaining
    return Arguments;
}

void CLuaArguments::CopyRecursive(const CLuaArguments& Arguments, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables)
{
    if (this == &Arguments)
        return;
    DeleteArguments();
    CFastHashMap<CLuaArguments*, CLuaArguments*> knownTables;
    if (!pKnownTables)
        pKnownTables = &knownTables;
    pKnownTables->insert({const_cast<CLuaArguments*>(&Arguments), this});
    std::vector<std::pair<const CLuaArguments*, CLuaArguments*>> pending{{&Arguments, this}};
    for (size_t next = 0; next < pending.size(); ++next)
    {
        const auto [source, destination] = pending[next];
        for (CLuaArgument* argument : source->m_Arguments)
        {
            bool ownsTable = false;
            if (argument->m_iType == LUA_TTABLE && !MapFindRef(*pKnownTables, argument->m_pTableData))
            {
                CLuaArguments* child = new CLuaArguments();
                pKnownTables->insert({argument->m_pTableData, child});
                pending.emplace_back(argument->m_pTableData, child);
                ownsTable = true;
            }
            // Every table is registered first, so the argument copy only copies a reference.
            CLuaArgument* copied = new CLuaArgument(*argument, pKnownTables);
            if (ownsTable)
                copied->m_bWeakTableRef = false;
            destination->m_Arguments.push_back(copied);
        }
    }
}

bool CLuaArguments::ReadArguments(lua_State* luaVM, signed int uiIndexBegin)
{
    // Delete the previous arguments if any
    DeleteArguments();

    CFastHashMap<const void*, CLuaArguments*> knownTables;

    // Start reading arguments until there are none left
    while (lua_type(luaVM, uiIndexBegin) != LUA_TNONE)
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument();
        if (!pArgument->Read(luaVM, uiIndexBegin++, &knownTables))
        {
            delete pArgument;
            DeleteArguments();
            return false;
        }
        m_Arguments.push_back(pArgument);

        knownTables.clear();
    }

    return true;
}

bool CLuaArguments::ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables)
{
    DeleteArguments();
    if (!lua_checkstack(luaVM, 8))
        return false;

    const int stackTop = lua_gettop(luaVM);
    if (iIndexBegin < 0 && iIndexBegin > LUA_REGISTRYINDEX)
        iIndexBegin += stackTop + 1;

    CFastHashMap<const void*, CLuaArguments*> knownTables;
    if (!pKnownTables)
        pKnownTables = &knownTables;
    pKnownTables->insert({lua_topointer(luaVM, iIndexBegin), this});

    // Anchor pending Lua tables in one table instead of keeping their keys on the Lua stack.
    lua_newtable(luaVM);
    const int pendingIndex = lua_gettop(luaVM);
    lua_pushvalue(luaVM, iIndexBegin);
    lua_rawseti(luaVM, pendingIndex, 1);
    std::vector<CLuaArguments*> pending{this};
    for (size_t next = 0; next < pending.size(); ++next)
    {
        CLuaArguments* table = pending[next];
        lua_rawgeti(luaVM, pendingIndex, static_cast<int>(next + 1));
        const int tableIndex = lua_gettop(luaVM);
        lua_pushnil(luaVM);
        while (lua_next(luaVM, tableIndex))
        {
            for (int index : {-2, -1})
            {
                CLuaArgument* argument = new CLuaArgument();
                bool          ownsTable = false;
                if (lua_istable(luaVM, index))
                {
                    const void* identity = lua_topointer(luaVM, index);
                    if (!MapFindRef(*pKnownTables, identity))
                    {
                        CLuaArguments* child = new CLuaArguments();
                        pKnownTables->insert({identity, child});
                        pending.push_back(child);
                        lua_pushvalue(luaVM, index);
                        lua_rawseti(luaVM, pendingIndex, static_cast<int>(pending.size()));
                        ownsTable = true;
                    }
                }
                // Registered tables are read as references; only their first argument owns them.
                argument->Read(luaVM, index, pKnownTables);
                if (ownsTable)
                    argument->m_bWeakTableRef = false;
                table->m_Arguments.push_back(argument);
            }
            lua_pop(luaVM, 1);
        }
        lua_pop(luaVM, 1);
    }
    lua_settop(luaVM, stackTop);
    return true;
}

bool CLuaArguments::ReadArgument(lua_State* luaVM, int iIndex)
{
    CLuaArgument* pArgument = new CLuaArgument();
    if (!pArgument->Read(luaVM, iIndex))
    {
        delete pArgument;
        return false;
    }
    m_Arguments.push_back(pArgument);
    return true;
}

void CLuaArguments::PushArguments(lua_State* luaVM) const
{
    // Push all our arguments
    std::vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        (*iter)->Push(luaVM);
    }
}

void CLuaArguments::PushAsTable(lua_State* luaVM, bool isArray) const
{
    luaL_checkstack(luaVM, 8, "Cannot push Lua table: insufficient stack space");
    lua_newtable(luaVM);
    const int                               cacheIndex = lua_gettop(luaVM);
    CFastHashMap<const CLuaArguments*, int> knownTables;
    std::vector<const CLuaArguments*>       pending;
    auto                                    pushTable = [&](const CLuaArguments* table)
    {
        if (int* existingId = MapFind(knownTables, table))
            lua_rawgeti(luaVM, cacheIndex, *existingId);
        else
        {
            pending.push_back(table);
            const int id = static_cast<int>(pending.size());
            knownTables.insert({table, id});
            lua_newtable(luaVM);
            lua_pushvalue(luaVM, -1);
            lua_rawseti(luaVM, cacheIndex, id);
        }
    };
    pushTable(this);
    for (size_t next = 0; next < pending.size(); ++next)
    {
        const CLuaArguments* table = pending[next];
        lua_rawgeti(luaVM, cacheIndex, static_cast<int>(next + 1));
        if (isArray && table == this)
        {
            for (size_t index = 0; index < table->m_Arguments.size(); ++index)
            {
                CLuaArgument* argument = table->m_Arguments[index];
                if (argument->m_iType == LUA_TTABLE && argument->m_pTableData)
                    pushTable(argument->m_pTableData);
                else
                    argument->Push(luaVM);
                lua_rawseti(luaVM, -2, static_cast<int>(index + 1));
            }
        }
        else
        {
            for (size_t index = 0; index + 1 < table->m_Arguments.size(); index += 2)
            {
                for (size_t offset = 0; offset < 2; ++offset)
                {
                    CLuaArgument* argument = table->m_Arguments[index + offset];
                    if (argument->m_iType == LUA_TTABLE && argument->m_pTableData)
                        pushTable(argument->m_pTableData);
                    else
                        argument->Push(luaVM);
                }
                lua_rawset(luaVM, -3);
            }
        }
        lua_pop(luaVM, 1);
    }
    lua_remove(luaVM, cacheIndex);
}

void CLuaArguments::PushArguments(const CLuaArguments& Arguments)
{
    for (CLuaArgument* argument : Arguments)
    {
        m_Arguments.push_back(new CLuaArgument(*argument));
    }
}

bool CLuaArguments::Call(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues) const
{
    assert(pLuaMain);
    const bool   timingActive = CPerfStatLuaTiming::GetSingleton()->IsActive();
    const TIMEUS startTime = timingActive ? GetTimeUs() : 0;

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

    int iret = pLuaMain->PCall(luaVM, m_Arguments.size(), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        SString strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;  // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                if (!returnValues->ReadArgument(luaVM, i))
                {
                    returnValues->DeleteArguments();
                    lua_settop(luaVM, luaStackPointer);
                    g_pGame->GetScriptDebugging()->LogError(luaVM, "Cannot read return values: insufficient Lua stack space");
                    return false;
                }
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    if (timingActive)
    {
        CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, pLuaMain->GetFunctionTag(iLuaFunction.ToInt()), GetTimeUs() - startTime);
    }
    return true;
}

bool CLuaArguments::CallGlobal(CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues) const
{
    assert(pLuaMain);
    assert(szFunction);
    const bool   timingActive = CPerfStatLuaTiming::GetSingleton()->IsActive();
    const TIMEUS startTime = timingActive ? GetTimeUs() : 0;

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
    int iret = pLuaMain->PCall(luaVM, m_Arguments.size(), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        std::string strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;  // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
        {
            for (int i = -iReturns; i <= -1; i++)
            {
                if (!returnValues->ReadArgument(luaVM, i))
                {
                    returnValues->DeleteArguments();
                    lua_settop(luaVM, luaStackPointer);
                    g_pGame->GetScriptDebugging()->LogError(luaVM, "Cannot read return values: insufficient Lua stack space");
                    return false;
                }
            }
        }

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);
    }

    if (timingActive)
    {
        CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, szFunction, GetTimeUs() - startTime);
    }
    return true;
}

CLuaArgument* CLuaArguments::PushNil()
{
    CLuaArgument* pArgument = new CLuaArgument;
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBoolean(bool bBool)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadBool(bBool);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTable(CLuaArguments* table)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadTable(table);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushNumber(double dNumber)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadNumber(dNumber);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushArgument(const CLuaArgument& argument)
{
    CLuaArgument* pArgument = new CLuaArgument(argument);  // create a copy
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushString(const std::string& string)
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushString(const std::string_view& string)
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushString(const CStringName& string)
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushString(const char* string)
{
    CLuaArgument* arg = new CLuaArgument();
    arg->ReadString(string);
    m_Arguments.push_back(arg);
    return arg;
}

CLuaArgument* CLuaArguments::PushElement(CElement* pElement)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadElement(pElement);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushBan(CBan* pBan)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pBan->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACL(CAccessControlList* pACL)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACL->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushACLGroup(CAccessControlListGroup* pACLGroup)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pACLGroup->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushAccount(CAccount* pAccount)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pAccount->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushResource(CResource* pResource)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pResource->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextDisplay(CTextDisplay* pTextDisplay)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextDisplay->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTextItem(CTextItem* pTextItem)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pTextItem->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushTimer(CLuaTimer* pLuaTimer)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pLuaTimer->GetScriptID());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushDbQuery(CDbJobData* pJobData)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadScriptID(pJobData->GetId());
    m_Arguments.push_back(pArgument);
    return pArgument;
}

void CLuaArguments::DeleteArguments()
{
    // Detach owning edges before deleting nodes so argument destructors cannot recurse.
    std::vector<CLuaArguments*> pending;
    CLuaArguments*              table = this;
    for (;;)
    {
        for (CLuaArgument* argument : table->m_Arguments)
        {
            if (argument->m_pTableData && !argument->m_bWeakTableRef)
                pending.push_back(argument->m_pTableData);
            argument->m_pTableData = nullptr;
            delete argument;
        }
        table->m_Arguments.clear();
        if (table != this)
            delete table;
        if (pending.empty())
            break;
        table = pending.back();
        pending.pop_back();
    }
}

// Gets rid of the last argument in the list
void CLuaArguments::Pop()
{
    // Delete the last element
    CLuaArgument* item = m_Arguments.back();
    delete item;

    // Pop it out of the vector
    m_Arguments.pop_back();
}

void CLuaArguments::ValidateTableKeys()
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
        }
        else
        {
            // Skip second in pair
            ++iter;
            // Check if end
            if (iter == m_Arguments.end())
                break;

            ++iter;
        }
    }
}

bool CLuaArguments::ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables, unsigned int uiDepth)
{
    if (uiDepth > MaxBitStreamTableReadDepth)
        return false;

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    unsigned int uiNumArgs;
    if (bitStream.ReadCompressed(uiNumArgs))
    {
        // Each argument needs at least 4 bits (SLuaTypeSync), reject obviously corrupt counts
        int unreadBits = bitStream.GetNumberOfUnreadBits();
        if (unreadBits < 0 || uiNumArgs > static_cast<unsigned int>(unreadBits) / 4)
        {
            if (bKnownTablesCreated)
                delete pKnownTables;
            return false;
        }

        pKnownTables->push_back(this);
        for (unsigned int ui = 0; ui < uiNumArgs; ++ui)
        {
            CLuaArgument* pArgument = new CLuaArgument();
            if (!pArgument->ReadFromBitStream(bitStream, pKnownTables, uiDepth + 1))
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

bool CLuaArguments::WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables) const
{
    CFastHashMap<CLuaArguments*, unsigned long> knownTables;
    if (!pKnownTables)
        pKnownTables = &knownTables;
    struct Frame
    {
        const CLuaArguments* table;
        size_t               next = 0;
    };
    auto beginTable = [&](const CLuaArguments* table)
    {
        pKnownTables->insert({const_cast<CLuaArguments*>(table), static_cast<unsigned long>(pKnownTables->size())});
        bitStream.WriteCompressed(static_cast<unsigned int>(table->m_Arguments.size()));
    };
    bool success = true;
    beginTable(this);
    std::vector<Frame> pending{{this}};
    while (!pending.empty())
    {
        Frame& frame = pending.back();
        if (frame.next == frame.table->m_Arguments.size())
        {
            pending.pop_back();
            continue;
        }
        CLuaArgument* argument = frame.table->m_Arguments[frame.next++];
        if (argument->GetType() == LUA_TTABLE && !MapFind(*pKnownTables, argument->GetTable()))
        {
            SLuaTypeSync type;
            type.data.ucType = LUA_TTABLE;
            bitStream.Write(&type);
            beginTable(argument->GetTable());
            pending.push_back({argument->GetTable()});
        }
        else if (!argument->WriteToBitStream(bitStream, pKnownTables))
            success = false;
    }
    return success;
}

bool CLuaArguments::WriteToJSONString(std::string& strJSON, bool bSerialize, int flags)
{
    json_object* my_array = WriteToJSONArray(bSerialize);
    if (my_array)
    {
        const char* json = json_object_to_json_string_ext(my_array, flags);
        if (json)
            strJSON = json;
        json_object_put(my_array);
        return json != nullptr;
    }
    return false;
}

json_object* CLuaArguments::WriteToJSONArray(bool bSerialize)
{
    json_object*                          my_array = json_object_new_array();
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = *iter;
        json_object*  object = pArgument->WriteToJSONObject(bSerialize);
        if (object)
        {
            json_object_array_add(my_array, object);
        }
        else
        {
            break;
        }
    }
    return my_array;
}

json_object* CLuaArguments::WriteTableToJSONObject(bool bSerialize, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables)
{
    CFastHashMap<CLuaArguments*, unsigned long> knownTables;
    if (!pKnownTables)
        pKnownTables = &knownTables;

    struct Frame
    {
        CLuaArguments*                                       table;
        json_object*                                         object;
        bool                                                 isArray;
        std::vector<std::pair<std::uint32_t, CLuaArgument*>> sorted;
        size_t                                               next = 0;
    };
    auto makeFrame = [&](CLuaArguments* table) -> Frame
    {
        pKnownTables->insert({table, static_cast<unsigned long>(pKnownTables->size())});
        bool                                                 bIsArray = true;
        std::vector<std::pair<std::uint32_t, CLuaArgument*>> vecSortedArguments;  // lua arrays are not necessarily sorted
        std::vector<CLuaArgument*>::const_iterator           iter = table->m_Arguments.begin();
        for (; iter != table->m_Arguments.end() && (iter + 1) != table->m_Arguments.end(); iter += 2)
        {
            CLuaArgument* pArgument = *iter;
            if (pArgument->GetType() == LUA_TNUMBER)
            {
                double const num = pArgument->GetNumber();
                auto const   iNum = static_cast<std::uint32_t>(num);

                vecSortedArguments.push_back({iNum, *(iter + 1)});
            }
            else
            {
                bIsArray = false;
                break;
            }
        }

        if (bIsArray && !vecSortedArguments.empty())  // the table could possibly be an array
        {
            // sort the table based on the keys (already handled correctly by std::pair)
            std::sort(vecSortedArguments.begin(), vecSortedArguments.end());

            // only the first and last element are checked, everything else is correct by default because the vector was sorted
            // the last key should match the size of vecSortedArguments to ensure there are no gaps in this array-like table
            auto const iFirstKey = vecSortedArguments.front().first;
            auto const iLastKey = vecSortedArguments.back().first;

            auto const iFirstArrayPos = 1U;  // lua arrays are 1 based
            auto const iLastArrayPos = static_cast<std::uint32_t>(vecSortedArguments.size());

            if (iFirstKey != iFirstArrayPos || iLastKey != iLastArrayPos)
            {
                bIsArray = false;
            }
        }

        return {table, bIsArray ? json_object_new_array() : json_object_new_object(), bIsArray, std::move(vecSortedArguments)};
    };

    std::vector<Frame> pending;
    pending.push_back(makeFrame(this));
    json_object* result = pending.back().object;
    // Depth-first order preserves the existing JSON table-reference numbers.
    while (!pending.empty())
    {
        Frame&        frame = pending.back();
        CLuaArgument* argument;
        char          key[255] = {};
        if (frame.isArray)
        {
            if (frame.next == frame.sorted.size())
            {
                pending.pop_back();
                continue;
            }
            argument = frame.sorted[frame.next++].second;
        }
        else
        {
            if (frame.next + 1 >= frame.table->m_Arguments.size() || !frame.table->m_Arguments[frame.next]->WriteToString(key, sizeof(key)))
            {
                pending.pop_back();
                continue;
            }
            argument = frame.table->m_Arguments[frame.next + 1];
            frame.next += 2;
        }

        const bool newTable = argument->GetType() == LUA_TTABLE && !MapFind(*pKnownTables, argument->GetTable());
        if (newTable)
        {
            Frame child = makeFrame(argument->GetTable());
            if (frame.isArray)
                json_object_array_add(frame.object, child.object);
            else
                json_object_object_add(frame.object, key, child.object);
            pending.push_back(std::move(child));
        }
        else
        {
            json_object* object = argument->WriteToJSONObject(bSerialize, pKnownTables);
            if (!object)
                pending.pop_back();
            else if (frame.isArray)
                json_object_array_add(frame.object, object);
            else
                json_object_object_add(frame.object, key, object);
        }
    }
    return result;
}

bool CLuaArguments::ReadFromJSONString(const char* szJSON)
{
    // Fast isJSON check: Check first non-white space character is '[' or '{'
    for (const char* ptr = szJSON; true;)
    {
        char c = *ptr++;
        if (c == '[' || c == '{')
            break;
        if (isspace((uchar)c))
            continue;
        return false;
    }

    json_object* object = json_tokener_parse(szJSON);
    if (object)
    {
        if (json_object_get_type(object) == json_type_array)
        {
            bool bSuccess = true;

            std::vector<CLuaArguments*> knownTables;

            for (uint i = 0; i < json_object_array_length(object); i++)
            {
                json_object*  arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument* pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(arrayObject, &knownTables);
                m_Arguments.push_back(pArgument);  // then the value
                if (!bSuccess)
                    break;
            }
            json_object_put(object);  // dereference
            return bSuccess;
        }
        else if (json_object_get_type(object) == json_type_object)
        {
            std::vector<CLuaArguments*> knownTables;
            CLuaArgument*               pArgument = new CLuaArgument();
            bool                        bSuccess = pArgument->ReadFromJSONObject(object, &knownTables);
            m_Arguments.push_back(pArgument);  // value
            json_object_put(object);

            return bSuccess;
        }
        json_object_put(object);  // dereference
    }
    //    else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    //   else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse HTTP POST request, ensure data uses JSON.");
    return false;
}

bool CLuaArguments::ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables)
{
    if (object)
    {
        if (json_object_get_type(object) == json_type_object)
        {
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
                CLuaArgument* pArgument = new CLuaArgument();
                pArgument->ReadString(key);
                m_Arguments.push_back(pArgument);  // push the key first
                pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(val, pKnownTables);  // then the value
                m_Arguments.push_back(pArgument);
                if (!bSuccess)
                    break;
            }

            if (bKnownTablesCreated)
                delete pKnownTables;
            return bSuccess;
        }
    }
    //   else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}

bool CLuaArguments::ReadFromJSONArray(json_object* object, std::vector<CLuaArguments*>* pKnownTables)
{
    if (object)
    {
        if (json_object_get_type(object) == json_type_array)
        {
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
                json_object*  arrayObject = json_object_array_get_idx(object, i);
                CLuaArgument* pArgument = new CLuaArgument();
                pArgument->ReadNumber(i + 1);  // push the key
                m_Arguments.push_back(pArgument);

                pArgument = new CLuaArgument();
                bSuccess = pArgument->ReadFromJSONObject(arrayObject, pKnownTables);
                m_Arguments.push_back(pArgument);  // then the valoue
                if (!bSuccess)
                    break;
            }

            if (bKnownTablesCreated)
                delete pKnownTables;
            return bSuccess;
        }
    }
    //    else
    //        g_pGame->GetScriptDebugging()->LogError ( "Could not parse invalid JSON object.");
    return false;
}

bool CLuaArguments::IsEqualTo(const CLuaArguments& compareTo, std::set<const CLuaArguments*>* knownTables) const
{
    std::set<const CLuaArguments*> visited;
    if (!knownTables)
        knownTables = &visited;
    struct Frame
    {
        const CLuaArguments* left;
        const CLuaArguments* right;
        size_t               next = 0;
    };
    std::vector<Frame> pending{{this, &compareTo}};
    while (!pending.empty())
    {
        Frame& frame = pending.back();
        if (frame.next == 0)
        {
            if (frame.left->m_Arguments.size() != frame.right->m_Arguments.size())
                return false;
            if (!knownTables->insert(frame.right).second)
            {
                pending.pop_back();
                continue;
            }
        }
        if (frame.next == frame.left->m_Arguments.size())
        {
            pending.pop_back();
            continue;
        }
        const CLuaArgument* left = frame.left->m_Arguments[frame.next];
        const CLuaArgument* right = frame.right->m_Arguments[frame.next++];
        if (left->GetType() == LUA_TTABLE && right->GetType() == LUA_TTABLE)
            pending.push_back({left->GetTable(), right->GetTable()});
        else if (!left->IsEqualTo(*right, knownTables))
            return false;
    }
    return true;
}
