/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaArguments.cpp
 *  PURPOSE:     Lua arguments manager class
 *
 *****************************************************************************/

#include "StdInc.h"
#define DECLARE_PROFILER_SECTION_CLuaArguments
#include "profiler/SharedUtil.Profiler.h"

using namespace std;

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif

extern CClientGame* g_pClientGame;

CLuaArguments::CLuaArguments(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables)
{
    ReadFromBitStream(bitStream, pKnownTables);
}

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
    vector<CLuaArgument*>::const_iterator iter = Arguments.m_Arguments.begin();
    for (; iter != Arguments.m_Arguments.end(); iter++)
    {
        CLuaArgument* pArgument = new CLuaArgument(**iter, pKnownTables);
        m_Arguments.push_back(pArgument);
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArguments(lua_State* luaVM, signed int uiIndexBegin)
{
    // Delete the previous arguments if any
    DeleteArguments();

    CFastHashMap<const void*, CLuaArguments*> knownTables;

    // Start reading arguments until there are none left
    while (lua_type(luaVM, uiIndexBegin) != LUA_TNONE)
    {
        // Create an argument, let it read out the argument and add it to our vector
        CLuaArgument* pArgument = new CLuaArgument(luaVM, uiIndexBegin++, &knownTables);
        m_Arguments.push_back(pArgument);

        knownTables.clear();
    }
}

void CLuaArguments::ReadTable(lua_State* luaVM, int iIndexBegin, CFastHashMap<const void*, CLuaArguments*>* pKnownTables)
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
        CLuaArgument* pArgument = new CLuaArgument(luaVM, -2, pKnownTables);
        m_Arguments.push_back(pArgument);            // push the key first

        pArgument = new CLuaArgument(luaVM, -1, pKnownTables);
        m_Arguments.push_back(pArgument);            // then the value

        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(luaVM, 1);
    }

    if (bKnownTablesCreated)
        delete pKnownTables;
}

void CLuaArguments::ReadArgument(lua_State* luaVM, int iIndex)
{
    CLuaArgument* pArgument = new CLuaArgument(luaVM, iIndex);
    m_Arguments.push_back(pArgument);
}

void CLuaArguments::PushArguments(lua_State* luaVM) const
{
    // Push all our arguments
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter++)
    {
        (*iter)->Push(luaVM);
    }
}

void CLuaArguments::PushAsTable(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables) const
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
    int size = pKnownTables->size();
    lua_getfield(luaVM, LUA_REGISTRYINDEX, "cache");
    lua_pushnumber(luaVM, ++size);
    lua_pushvalue(luaVM, -3);
    lua_settable(luaVM, -3);
    lua_pop(luaVM, 1);
    pKnownTables->insert(std::make_pair((CLuaArguments*)this, size));

    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end() && (iter + 1) != m_Arguments.end(); iter++)
    {
        (*iter)->Push(luaVM, pKnownTables);            // index
        iter++;
        (*iter)->Push(luaVM, pKnownTables);            // value
        lua_settable(luaVM, -3);
    }

    if (bKnownTablesCreated)
    {
        // clear the cache
        lua_pushnil(luaVM);
        lua_setfield(luaVM, LUA_REGISTRYINDEX, "cache");
        delete pKnownTables;
    }
}

void CLuaArguments::PushArguments(const CLuaArguments& Arguments)
{
    vector<CLuaArgument*>::const_iterator iter = Arguments.IterBegin();
    for (; iter != Arguments.IterEnd(); iter++)
    {
        CLuaArgument* pArgument = new CLuaArgument(**iter);
        m_Arguments.push_back(pArgument);
    }
}

bool CLuaArguments::Call(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, CLuaArguments* returnValues) const
{
    assert(pLuaMain);
    TIMEUS startTime = GetTimeUs();

    // Add the function name to the stack and get the event from the table
    lua_State* luaVM = pLuaMain->GetVirtualMachine();
    assert(luaVM);
    LUA_CHECKSTACK(luaVM, 2);
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
        g_pClientGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
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

    CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, pLuaMain->GetFunctionTag(iLuaFunction.ToInt()), GetTimeUs() - startTime);
    return true;
}

bool CLuaArguments::CallGlobal(CLuaMain* pLuaMain, const char* szFunction, CLuaArguments* returnValues) const
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
    int iret = pLuaMain->PCall(luaVM, m_Arguments.size(), LUA_MULTRET, 0);
    if (iret == LUA_ERRRUN || iret == LUA_ERRMEM)
    {
        std::string strRes = ConformResourcePath(lua_tostring(luaVM, -1));
        g_pClientGame->GetScriptDebugging()->LogPCallError(luaVM, strRes);

        // cleanup the stack
        while (lua_gettop(luaVM) - luaStackPointer > 0)
            lua_pop(luaVM, 1);

        return false;            // the function call failed
    }
    else
    {
        int iReturns = lua_gettop(luaVM) - luaStackPointer;

        if (returnValues != NULL)
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

    CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pLuaMain, szFunction, GetTimeUs() - startTime);
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

CLuaArgument* CLuaArguments::PushNumber(double dNumber)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadNumber(dNumber);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushString(const std::string& strString)
{
    CLuaArgument* pArgument = new CLuaArgument();
    pArgument->ReadString(strString);
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

CLuaArgument* CLuaArguments::PushElement(CClientEntity* pElement)
{
    CLuaArgument* pArgument = new CLuaArgument;
    pArgument->ReadElement(pElement);
    m_Arguments.push_back(pArgument);
    return pArgument;
}

CLuaArgument* CLuaArguments::PushArgument(const CLuaArgument& Argument)
{
    CLuaArgument* pArgument = new CLuaArgument(Argument);
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

// Gets rid of all the arguments in the list
void CLuaArguments::DeleteArguments()
{
    // Delete each item
    vector<CLuaArgument*>::iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter++)
    {
        delete *iter;
    }

    // Clear the vector
    m_Arguments.clear();
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
            iter++;
            // Check if end
            if (iter == m_Arguments.end())
                break;

            iter++;
        }
    }
}

bool CLuaArguments::ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables)
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    unsigned int uiNumArgs;
    if (bitStream.ReadCompressed(uiNumArgs))
    {
        pKnownTables->push_back(this);
        for (unsigned int ui = 0; ui < uiNumArgs; ++ui)
        {
            CLuaArgument* pArgument = new CLuaArgument(bitStream, pKnownTables);
            m_Arguments.push_back(pArgument);
        }
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return true;
}

bool CLuaArguments::WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables) const
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, unsigned long>();
        bKnownTablesCreated = true;
    }

    bool bSuccess = true;
    pKnownTables->insert(make_pair((CLuaArguments*)this, pKnownTables->size()));
    bitStream.WriteCompressed(static_cast<unsigned int>(m_Arguments.size()));

    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter++)
    {
        CLuaArgument* pArgument = *iter;
        if (!pArgument->WriteToBitStream(bitStream, pKnownTables))
        {
            bSuccess = false;
        }
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::ReadJSONString(const char* szJSON)
{
    rapidjson::Document    doc;
    rapidjson::ParseResult result = doc.Parse(szJSON);

    if (!result)
    {
        g_pClientGame->GetScriptDebugging()->LogError(nullptr, "JSON Parser Error: %s (%u)", rapidjson::GetParseError_En(result.Code()), result.Offset());
        return false;
    }

    std::vector<CLuaArguments*> knownTables;
    bool                        bSuccess = true;

    CLuaArgument* pArg = new CLuaArgument();
    bSuccess = pArg->DeserializeValueFromJSON(doc, &knownTables);
    m_Arguments.push_back(pArg);

    // PushArguments(*this);

    return bSuccess;
}

bool CLuaArguments::ReadJSONArray(const rapidjson::Value& obj, std::vector<CLuaArguments*>* pKnownTables)
{
    if (!obj.IsArray())
        return false;

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->push_back(this);

    bool bSuccess = true;
    int  count = 1;

    for (const auto& field : obj.GetArray())
    {
        // push key
        CLuaArgument* pArg = new CLuaArgument();
        pArg->ReadNumber(count);
        m_Arguments.push_back(pArg);

        // push value
        pArg = new CLuaArgument();
        bSuccess = pArg->DeserializeValueFromJSON(field, pKnownTables);
        m_Arguments.push_back(pArg);

        if (!bSuccess)
            break;

        // do not touch
        count += 1;
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

bool CLuaArguments::ReadJSONObject(const rapidjson::Value& obj, std::vector<CLuaArguments*>* pKnownTables)
{
    if (!obj.IsObject())
        return false;

    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new std::vector<CLuaArguments*>();
        bKnownTablesCreated = true;
    }

    pKnownTables->push_back(this);

    bool bSuccess = true;

    for (rapidjson::Value::ConstMemberIterator itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr)
    {
        std::string_view view(itr->name.GetString());

        // push key
        CLuaArgument* pArg = new CLuaArgument();
        pArg->ReadString(view.data());
        m_Arguments.push_back(pArg);

        // push value
        pArg = new CLuaArgument();
        bSuccess = pArg->DeserializeValueFromJSON(itr->value, pKnownTables);
        m_Arguments.push_back(pArg);

        if (!bSuccess)
            break;
    }

    if (bKnownTablesCreated)
        delete pKnownTables;

    return bSuccess;
}

///////////////////////////////////////////////////////////
bool CLuaArguments::SerializeToJSONString(rapidjson::StringBuffer* buffer, bool bSerialize, int flags, bool bBackwardsCompatibility)
{
    try
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(*buffer);
        if (flags != JSON_PRETTIFY_NONE)
        {
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(*buffer);

            switch (flags)
            {
                case JSON_PRETTIFY_SPACES:
                    writer.SetIndent(' ', 1);
                    writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
                    break;

                case JSON_PRETTIFY_TABS:
                    writer.SetIndent('\t', 2);
                    writer.SetFormatOptions(rapidjson::PrettyFormatOptions::kFormatDefault);
                    break;
            }

            if (bBackwardsCompatibility)
                SerializeAsJSONArray(writer, bSerialize);
            else
                SerializeAsJSONObject(writer, bSerialize);
        }
        else
        {
            rapidjson::Writer<rapidjson::StringBuffer> writer(*buffer);

            if (bBackwardsCompatibility)
                SerializeAsJSONArray(writer, bSerialize);
            else
                SerializeAsJSONObject(writer, bSerialize);
        }

        return true;
    }
    catch (const std::exception& e)
    {
        g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Error while encoding JSON: %s", e.what());
    }
    catch (...)
    {
        g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Unknown error while encoding!");
    }
    return false;
}

template <typename Writer>
void CLuaArguments::SerializeAsJSONObject(Writer& writer, bool bSerialize)
{
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = *iter;
        pArgument->SerializeToJSON(writer, bSerialize);
    }
}

template <typename Writer>
void CLuaArguments::SerializeAsJSONArray(Writer& writer, bool bSerialize)
{
    writer.StartArray();

    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); ++iter)
    {
        CLuaArgument* pArgument = *iter;
        pArgument->SerializeToJSON(writer, bSerialize);
    }

    writer.EndArray();
}

template <typename Writer>
void CLuaArguments::ConvertTableToJSON(Writer& writer, bool bSerialize, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables)
{
    bool bKnownTablesCreated = false;
    if (!pKnownTables)
    {
        pKnownTables = new CFastHashMap<CLuaArguments*, unsigned long>();
        bKnownTablesCreated = true;
    }

    pKnownTables->insert(std::make_pair(this, pKnownTables->size()));

    bool                                  bIsArray = true;
    unsigned int                          iArrayPos = 1;            // lua arrays are 1 based
    vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
    for (; iter != m_Arguments.end(); iter += 2)
    {
        CLuaArgument* pArgument = *iter;
        if (pArgument->GetType() == LUA_TNUMBER)
        {
            double       num = pArgument->GetNumber();
            unsigned int iNum = static_cast<unsigned int>(num);
            if (num == iNum)
            {
                if (iArrayPos != iNum)            // check if the value matches its index in the table
                {
                    bIsArray = false;
                    break;
                }
            }
            else
            {
                bIsArray = false;
                break;
            }
        }
        else
        {
            bIsArray = false;
            break;
        }
        iArrayPos++;
    }

    if (bIsArray)
    {
        writer.StartArray();

        vector<CLuaArgument*>::const_iterator iter = m_Arguments.begin();
        for (; iter != m_Arguments.end(); ++iter)
        {
            iter++;            // skip the key values
            CLuaArgument* pArgument = *iter;
            pArgument->SerializeToJSON(writer, bSerialize, pKnownTables);
        }

        writer.EndArray();

        if (bKnownTablesCreated)
            delete pKnownTables;
    }
    else
    {
        writer.StartObject();

        iter = m_Arguments.begin();
        for (; iter != m_Arguments.end(); ++iter)
        {
            char key[512]{};

            // could we get the key direct from lua?
            CLuaArgument* pArgument = *iter;
            if (!pArgument->WriteToString(key, 512))            // index
                break;

            writer.Key(key, static_cast<rapidjson::SizeType>(strlen(key)));

            ++iter;
            pArgument = *iter;
            pArgument->SerializeToJSON(writer, bSerialize, pKnownTables);
        }

        writer.EndObject();

        if (bKnownTablesCreated)
            delete pKnownTables;
    }
}

template void CLuaArguments::ConvertTableToJSON<rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>, rapidjson::CrtAllocator>>>(
    rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>, rapidjson::CrtAllocator>>& writer, bool bSerialize,
    CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables);
template void CLuaArguments::ConvertTableToJSON<rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>, rapidjson::CrtAllocator>>>(
    rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<char>, rapidjson::CrtAllocator>>& writer, bool bSerialize,
    CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables);
