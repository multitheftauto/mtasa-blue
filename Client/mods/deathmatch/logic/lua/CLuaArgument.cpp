/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaArgument.cpp
 *  PURPOSE:     Lua argument class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

#define ARGUMENT_TYPE_INT       9
#define ARGUMENT_TYPE_FLOAT     10

#ifndef VERIFY_ENTITY
#define VERIFY_ENTITY(entity) (CStaticFunctionDefinitions::GetRootElement()->IsMyChild(entity,true)&&!entity->IsBeingDeleted())
#endif

#ifndef VERIFY_RESOURCE
#define VERIFY_RESOURCE(resource) (g_pClientGame->GetResourceManager()->Exists(resource))
#endif

extern CClientGame* g_pClientGame;

CLuaArgument::CLuaArgument() noexcept
{
    m_iType = LUA_TNIL;
    m_iIndex = -1;
    m_pTableData = nullptr;
    m_pUserData = nullptr;
}

CLuaArgument::CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables) noexcept
{
    // Initialize and call our = on the argument
    m_pTableData = nullptr;
    CopyRecursive(Argument, pKnownTables);
}

CLuaArgument::CLuaArgument(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    m_pTableData = nullptr;
    ReadFromBitStream(bitStream, pKnownTables);
}

CLuaArgument::CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables) noexcept
{
    // Read the argument out of the lua VM
    m_pTableData = nullptr;
    m_iIndex = iArgument;
    Read(luaVM, iArgument, pKnownTables);
}

CLuaArgument::~CLuaArgument() noexcept
{
    // Eventually destroy our table
    DeleteTableData();
}

void CLuaArgument::CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables) noexcept
{
    // Clear the string
    m_strString = "";

    // Destroy our old tabledata if neccessary
    DeleteTableData();

#ifdef MTA_DEBUG
    // Copy over line and filename too
    m_strFilename = Argument.m_strFilename;
    m_iLine = Argument.m_iLine;
#endif

    // Set our variable equally to the copy class
    m_iType = Argument.m_iType;
    switch (m_iType)
    {
        case LUA_TBOOLEAN:
        {
            m_bBoolean = Argument.m_bBoolean;
            break;
        }

        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        {
            m_pUserData = Argument.m_pUserData;
            break;
        }

        case LUA_TNUMBER:
        {
            m_Number = Argument.m_Number;
            break;
        }

        case LUA_TTABLE:
        {
            if (pKnownTables && (m_pTableData = MapFindRef(*pKnownTables, Argument.m_pTableData)))
            {
                m_bWeakTableRef = true;
            }
            else
            {
                m_pTableData = new CLuaArguments(*Argument.m_pTableData, pKnownTables);
                m_bWeakTableRef = false;
            }
            break;
        }

        case LUA_TSTRING:
        {
            m_strString = Argument.m_strString;
            break;
        }

        default:
            break;
    }
}

const CLuaArgument& CLuaArgument::operator=(const CLuaArgument& Argument) noexcept
{
    CopyRecursive(Argument);

    // Return the given class allowing for chaining
    return Argument;
}

bool CLuaArgument::operator==(const CLuaArgument& Argument) noexcept
{
    std::set<CLuaArguments*> knownTables;
    return CompareRecursive(Argument, &knownTables);
}

bool CLuaArgument::operator!=(const CLuaArgument& Argument) noexcept
{
    return !operator==(Argument);
}

bool CLuaArgument::CompareRecursive(const CLuaArgument& Argument, std::set<CLuaArguments*>* pKnownTables) noexcept
{
    // If the types differ, they're not matching
    if (Argument.m_iType != m_iType)
        return false;

    // Compare the variables depending on the type
    switch (m_iType)
    {
        case LUA_TBOOLEAN:
        {
            return m_bBoolean == Argument.m_bBoolean;
        }

        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        {
            return m_pUserData == Argument.m_pUserData;
        }

        case LUA_TNUMBER:
        {
            return m_Number == Argument.m_Number;
        }

        case LUA_TTABLE:
        {
            if (m_pTableData->Count() != Argument.m_pTableData->Count())
                return false;

            pKnownTables->insert(m_pTableData);

            return m_pTableData == Argument.m_pTableData;
        }
        case LUA_TSTRING:
        {
            return m_strString == Argument.m_strString;
        }
    }

    return true;
}

void CLuaArgument::Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables) noexcept
{
    // Store debug data for later retrieval
#ifdef MTA_DEBUG
    m_iLine = 0;
    m_strFilename = "";
    lua_Debug debugInfo;
    if (lua_getstack(luaVM, 1, &debugInfo))
    {
        lua_getinfo(luaVM, "nlS", &debugInfo);

        m_strFilename = debugInfo.source;
        m_iLine = debugInfo.currentline;
    }
#endif

    // Eventually delete our previous string
    m_strString = "";

    DeleteTableData();

    // Grab the argument type
    m_iType = lua_type(luaVM, iArgument);
    if (m_iType == LUA_TNONE)
        return;

    // Read out the content depending on the type
    switch (m_iType)
    {
        case LUA_TNIL:
            break;

        case LUA_TBOOLEAN:
        {
            m_bBoolean = lua_toboolean(luaVM, iArgument) ? true : false;
            break;
        }

        case LUA_TTABLE:
        {
            if (pKnownTables && (m_pTableData = MapFindRef(*pKnownTables, lua_topointer(luaVM, iArgument))))
            {
                m_bWeakTableRef = true;
            }
            else
            {
                m_pTableData = new CLuaArguments();
                m_pTableData->ReadTable(luaVM, iArgument, pKnownTables);
                m_bWeakTableRef = false;
            }
            break;
        }

        case LUA_TLIGHTUSERDATA:
        {
            m_pUserData = lua_touserdata(luaVM, iArgument);
            break;
        }

        case LUA_TUSERDATA:
        {
            m_pUserData = *((void**)lua_touserdata(luaVM, iArgument));
            break;
        }

        case LUA_TNUMBER:
        {
            m_Number = lua_tonumber(luaVM, iArgument);
            break;
        }

        case LUA_TSTRING:
        {
            // Grab the lua string and its size
            const char* szLuaString = lua_tostring(luaVM, iArgument);
            size_t      sizeLuaString = lua_strlen(luaVM, iArgument);

            // Set our string
            m_strString.assign(szLuaString, sizeLuaString);
            break;
        }

        case LUA_TFUNCTION:
        {
            // TODO: add function reading (has to work inside tables too)
            m_iType = LUA_TNIL;
            break;
        }

        case LUA_TTHREAD:
        {
            m_iType = LUA_TNIL;
            break;
        }

        default:
        {
            m_iType = LUA_TNONE;
            break;
        }
    }
}

void CLuaArgument::ReadBool(bool bBool) noexcept
{
    m_strString = "";
    m_iType = LUA_TBOOLEAN;
    DeleteTableData();
    m_bBoolean = bBool;
}

void CLuaArgument::ReadNumber(double dNumber) noexcept
{
    m_strString = "";
    m_iType = LUA_TNUMBER;
    DeleteTableData();
    m_Number = dNumber;
}

void CLuaArgument::ReadString(const char* string)noexcept
{
    m_iType = LUA_TSTRING;
    DeleteTableData();
    m_strString = string;
}

void CLuaArgument::ReadString(const std::string& string) noexcept
{
    m_iType = LUA_TSTRING;
    DeleteTableData();
    m_strString = string;
}

void CLuaArgument::ReadString(const std::string_view& string) noexcept
{
    m_iType = LUA_TSTRING;
    DeleteTableData();
    m_strString = std::string{string};
}

void CLuaArgument::ReadScriptID(std::uint32_t uiScriptID) noexcept
{
    m_strString = "";
    DeleteTableData();
    m_iType = LUA_TUSERDATA;
    m_pUserData = reinterpret_cast<void*>(uiScriptID);
}

void CLuaArgument::ReadElement(const CClientEntity* pElement) noexcept
{
    m_strString = "";
    DeleteTableData();
    if (!pElement)
    {
        m_iType = LUA_TNIL;
        return;
    }

    m_iType = LUA_TUSERDATA;
    m_pUserData = (void*)reinterpret_cast<std::uint32_t*>(pElement->GetID().Value());
}

void CLuaArgument::ReadElementID(ElementID ID) noexcept
{
    m_strString = "";
    DeleteTableData();
    m_iType = LUA_TUSERDATA;
    m_pUserData = (void*)reinterpret_cast<std::uint32_t*>(ID.Value());
}

void CLuaArgument::ReadTable(const CLuaArguments* table) noexcept
{
    m_strString = "";
    DeleteTableData();
    m_pTableData = new CLuaArguments(*table);
    m_bWeakTableRef = false;
    m_iType = LUA_TTABLE;
}

CClientEntity* CLuaArgument::GetElement() const noexcept
{
    ElementID ID = TO_ELEMENTID(m_pUserData);
    return CElementIDs::GetElement(ID);
}

void CLuaArgument::Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables) const noexcept
{
    // WARNING:
    // Stuff using this function is kinda unsafe, we expect
    // it to successfully push something, when it actually may not,
    // corrupting the Lua stack
    if (m_iType == LUA_TNONE)
        return;

    // Make sure the stack has enough room
    LUA_CHECKSTACK(luaVM, 1);

    // Push it depending on the type
    switch (m_iType)
    {
        case LUA_TNIL:
        {
            lua_pushnil(luaVM);
            break;
        }

        case LUA_TBOOLEAN:
        {
            lua_pushboolean(luaVM, m_bBoolean);
            break;
        }

        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        {
            lua_pushuserdata(luaVM, m_pUserData);
            break;
        }

        case LUA_TNUMBER:
        {
            lua_pushnumber(luaVM, m_Number);
            break;
        }

        case LUA_TTABLE:
        {
            int* pTableId;
            if (!pKnownTables || !(pTableId = MapFind(*pKnownTables, m_pTableData)))
            {
                m_pTableData->PushAsTable(luaVM, pKnownTables);
                break;
            }

            lua_getfield(luaVM, LUA_REGISTRYINDEX, "cache");
            lua_pushnumber(luaVM, *pTableId);
            lua_gettable(luaVM, -2);
            lua_remove(luaVM, -2);
            break;
        }

        case LUA_TSTRING:
        {
            lua_pushlstring(luaVM, m_strString.c_str(), m_strString.length());
            break;
        }
    }
}

// Can't use bitStream.Version() here as it is sometimes not set
bool CLuaArgument::ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    DeleteTableData();
    SLuaTypeSync type;

    // Read out the type
    if (!bitStream.Read(&type))
        return true;

    // Depending on what type...
    switch (type.data.ucType)
    {
        // Nil type
        case LUA_TNIL:
        {
            m_iType = LUA_TNIL;
            break;
        }

        // Boolean type
        case LUA_TBOOLEAN:
        {
            bool bValue;
            if (bitStream.ReadBit(bValue))
                ReadBool(bValue);
            break;
        }

        // Number type
        case LUA_TNUMBER:
        {
            if (bitStream.ReadBit())
            {
                // Should be in high precision mode
                dassert(g_pClientGame->IsHighFloatPrecision());
                if (bitStream.ReadBit())
                {
                    double dNum;
                    if (bitStream.Read(dNum))
                        ReadNumber(dNum);
                }
                else
                {
                    float fNum;
                    if (bitStream.Read(fNum))
                        ReadNumber(RoundFromFloatSource(fNum));
                }
            }
            else
            {
                int iNum;
                if (bitStream.ReadCompressed(iNum))
                    ReadNumber(iNum);
            }
            break;
        }

        // Table type
        case LUA_TTABLE:
        {
            m_pTableData = new CLuaArguments(bitStream, pKnownTables);
            m_bWeakTableRef = false;
            m_iType = LUA_TTABLE;
            m_pTableData->ValidateTableKeys();
            break;
        }

        // Table reference (self-referencing tables)
        case LUA_TTABLEREF:
        {
            unsigned long ulTableRef;
            if (bitStream.ReadCompressed(ulTableRef))
            {
                if (pKnownTables && ulTableRef < pKnownTables->size())
                {
                    m_pTableData = pKnownTables->at(ulTableRef);
                    m_bWeakTableRef = true;
                    m_iType = LUA_TTABLE;
                }
            }
            break;
        }

        // String type
        case LUA_TSTRING:
        {
            // Read out the string length
            unsigned short usLength;
            if (bitStream.ReadCompressed(usLength) && usLength)
            {
                // Allocate a buffer and read the string into it
                char* szValue = new char[usLength + 1];
                if (bitStream.Read(szValue, usLength))
                {
                    // Put it into us
                    ReadString(std::string(szValue, usLength));
                }

                // Delete the buffer
                delete[] szValue;
            }
            else
                ReadString("");

            break;
        }

        // Long string type
        case LUA_TSTRING_LONG:
        {
            // Read out the string length
            uint uiLength;
            if (bitStream.ReadCompressed(uiLength) && uiLength)
            {
                bitStream.AlignReadToByteBoundary();

                // Allocate a buffer and read the string into it
                char* szValue = new char[uiLength + 1];
                assert(szValue);
                if (bitStream.Read(szValue, uiLength))
                {
                    // Put it into us
                    ReadString(std::string(szValue, uiLength));
                }

                // Delete the buffer
                delete[] szValue;
            }
            else
                ReadString("");

            break;
        }

        // Element type
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
        {
            // Read out the elemnt ID
            ElementID ElementID;
            if (bitStream.Read(ElementID))
            {
                ReadElementID(ElementID);
            }
            break;
        }
    }

    return true;
}

// Can't use bitStream.Version() here as it is sometimes not set
bool CLuaArgument::WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables) const noexcept
{
    SLuaTypeSync type;

    switch (GetType())
    {
        // Nil type
        case LUA_TNIL:
        {
            type.data.ucType = LUA_TNIL;
            bitStream.Write(&type);
            break;
        }

        // Boolean type
        case LUA_TBOOLEAN:
        {
            type.data.ucType = LUA_TBOOLEAN;
            bitStream.Write(&type);

            // Write the boolean to it
            bitStream.WriteBit(GetBoolean());
            break;
        }

        // Table argument
        case LUA_TTABLE:
        {
            std::uint32_t* pTableId;
            if (pKnownTables && (pTableId = MapFind(*pKnownTables, m_pTableData)))
            {
                // Self-referencing table
                type.data.ucType = LUA_TTABLEREF;
                bitStream.Write(&type);
                bitStream.WriteCompressed(*pTableId);
            }
            else
            {
                type.data.ucType = LUA_TTABLE;
                bitStream.Write(&type);

                // Write the subtable to the bitstream
                m_pTableData->WriteToBitStream(bitStream, pKnownTables);
            }
            break;
        }

        // Number argument?
        case LUA_TNUMBER:
        {
            type.data.ucType = LUA_TNUMBER;
            bitStream.Write(&type);

            int       iNumber;
            float     fNumber;
            double    dNumber;
            EDataType dataType = GetDataTypeToUse(GetNumber(), &iNumber, &fNumber, &dNumber);
            if (dataType == DATA_TYPE_INT)
            {
                bitStream.WriteBit(false);
                bitStream.WriteCompressed(iNumber);
            }
            else if (dataType == DATA_TYPE_FLOAT)
            {
                bitStream.WriteBit(true);
                bitStream.WriteBit(false);
                bitStream.Write(fNumber);
            }
            else
            {
                bitStream.WriteBit(true);
                bitStream.WriteBit(true);
                bitStream.Write(dNumber);
            }
            break;
        }

        // String argument
        case LUA_TSTRING:
        {
            // Grab the string and its length. Is it short enough to be sendable?
            const char* szTemp = m_strString.c_str();
            std::size_t sizeTemp = m_strString.length();
            auto usLength = static_cast<std::uint16_t>(sizeTemp);
            if (sizeTemp == usLength)
            {
                // This is a string argument
                type.data.ucType = LUA_TSTRING;
                bitStream.Write(&type);

                // Write its length
                bitStream.WriteCompressed(usLength);

                // Write the content too if it's not empty
                if (usLength > 0)
                {
                    bitStream.Write(szTemp, usLength);
                }
            }
            else
            {
                // This is a long string argument
                type.data.ucType = LUA_TSTRING_LONG;
                bitStream.Write(&type);

                // Write its length
                uint uiLength = sizeTemp;
                bitStream.WriteCompressed(uiLength);

                // Write the content too if it's not empty
                if (uiLength > 0)
                {
                    bitStream.AlignWriteToByteBoundary();
                    bitStream.Write(szTemp, uiLength);
                }
            }
            break;
        }

        // Element packet
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
        {
            // Got a valid element to send?
            CClientEntity* pElement = GetElement();
            if (pElement)
            {
                // Clientside element?
                if (!pElement->IsLocalEntity())
                {
                    type.data.ucType = LUA_TUSERDATA;
                    bitStream.Write(&type);
                    bitStream.Write(pElement->GetID());
                }
                else
                {
                    // Write a nil though so other side won't get out of sync
                    type.data.ucType = LUA_TNIL;
                    bitStream.Write(&type);
                    return false;
                }
            }
            else
            {
                // Write a nil though so other side won't get out of sync
                type.data.ucType = LUA_TNIL;
                bitStream.Write(&type);
                return false;
            }
            break;
        }

        // Unpacketizable type.
        default:
        {
            // Unpacketizable
            LogUnableToPacketize("Couldn't packetize argument list, unknown type specified.");

            // Write a nil though so other side won't get out of sync
            type.data.ucType = LUA_TNIL;
            bitStream.Write(&type);
            return false;
        }
    }

    // Success
    return true;
}

bool CLuaArgument::IsTable() const noexcept
{
    if (m_iType != LUA_TTABLE)
        return false;
    if (!m_pTableData)
        return false;
    return m_pTableData->Count() % 2 == 0;
}

void CLuaArgument::LogUnableToPacketize(const char* szMessage) const noexcept
{
#ifdef MTA_DEBUG
    if (!m_strFilename.empty())
    {
        g_pClientGame->GetScriptDebugging()->LogWarning(nullptr, "%s:%d: %s", ConformResourcePath(m_strFilename.c_str()).c_str(), m_iLine, szMessage);
    }
    else
#endif
    {
        g_pClientGame->GetScriptDebugging()->LogWarning(nullptr, "Unknown: %s", szMessage);
    }
}

void CLuaArgument::DeleteTableData() noexcept
{
    if (!m_pTableData)
        return;

    if (!m_bWeakTableRef)
        delete m_pTableData;
    m_pTableData = nullptr;
}

json_object* CLuaArgument::WriteToJSONObject(bool bSerialize, CFastHashMap<CLuaArguments*, std::uint32_t>* pKnownTables) noexcept
{
    switch (GetType())
    {
        case LUA_TNIL:
        {
            return json_object_new_int(0);
        }
        case LUA_TBOOLEAN:
        {
            return json_object_new_boolean(GetBoolean());
        }
        case LUA_TTABLE:
        {
            std::uint32_t* pTableId;
            if (pKnownTables && (pTableId = MapFind(*pKnownTables, m_pTableData)))
            {
                // Self-referencing table
                char szTableID[10];
                snprintf(szTableID, sizeof(szTableID), "^T^%lu", *pTableId);
                return json_object_new_string(szTableID);
            }
            else
            {
                return m_pTableData->WriteTableToJSONObject(bSerialize, pKnownTables);
            }
        }
        case LUA_TNUMBER:
        {
            int iNumber;
            if (ShouldUseInt(GetNumber(), &iNumber))
            {
                return json_object_new_int(iNumber);
            }
            else
            {
                return json_object_new_double(GetNumber());
            }
            break;
        }
        case LUA_TSTRING:
        {
            SString strTemp = GetString();
            if (strTemp.length() > 3 && strTemp[0] == '^' && strTemp[2] == '^' && strTemp[1] != '^')
            {
                // Prevent clash with how MTA stores elements, resources and table refs as strings
                strTemp[2] = '~';
            }
            if (strTemp.length() <= USHRT_MAX)
            {
                return json_object_new_string_len(strTemp.c_str(), strTemp.length());
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError(nullptr,
                                                              "Couldn't convert argument list to JSON. Invalid string specified, limit is 65535 characters.");
            }
            break;
        }
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
        {
            CClientEntity* pElement = GetElement();
            CResource*     pResource = g_pClientGame->GetResourceManager()->GetResourceFromScriptID(reinterpret_cast<unsigned long>(GetUserData()));

            // Elements are dynamic, so storing them is potentially unsafe
            if (pElement && bSerialize)
            {
                char szElementID[10] = {0};
                snprintf(szElementID, 9, "^E^%d", (int)pElement->GetID().Value());
                return json_object_new_string(szElementID);
            }
            else if (VERIFY_RESOURCE(pResource))
            {
                char szElementID[MAX_RESOURCE_NAME_LENGTH + 4] = {0};
                snprintf(szElementID, MAX_RESOURCE_NAME_LENGTH + 3, "^R^%s", pResource->GetName() /*.c_str ()*/);
                return json_object_new_string(szElementID);
            }
            else
            {
                if (pElement)            // eg toJSON() with valid element
                    g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Couldn't convert userdata argument to JSON, elements not allowed for this function.");
                else if (!bSerialize)            // eg toJSON() with invalid element
                    g_pClientGame->GetScriptDebugging()->LogError(
                        nullptr, "Couldn't convert userdata argument to JSON, only valid resources can be included for this function.");
                else
                    g_pClientGame->GetScriptDebugging()->LogError(
                        nullptr, "Couldn't convert userdata argument to JSON, only valid elements or resources can be included.");
                return nullptr;
            }
            break;
        }
        default:
        {
            g_pClientGame->GetScriptDebugging()->LogError(
                nullptr, "Couldn't convert argument list to JSON, unsupported data type. Use Table, Nil, String, Number, Boolean, Resource or Element.");
            return nullptr;
        }
    }
    return nullptr;
}

char* CLuaArgument::WriteToString(char* szBuffer, int length) noexcept
{
    switch (GetType())
    {
        case LUA_TNIL:
        {
            snprintf(szBuffer, length, "0");
            return szBuffer;
        }
        case LUA_TBOOLEAN:
        {
            if (GetBoolean())
                snprintf(szBuffer, length, "true");
            else
                snprintf(szBuffer, length, "false");
            return szBuffer;
        }
        case LUA_TTABLE:
        {
            g_pClientGame->GetScriptDebugging()->LogError(
                nullptr, "Cannot convert table to string (do not use tables as keys in tables if you want to send them over http/JSON).");
            return nullptr;
        }
        case LUA_TNUMBER:
        {
            int iNumber;
            if (ShouldUseInt(GetNumber(), &iNumber))
            {
                snprintf(szBuffer, length, "%d", iNumber);
                return szBuffer;
            }
            else
            {
                snprintf(szBuffer, length, "%f", GetNumber());
                return szBuffer;
            }
            break;
        }
        case LUA_TSTRING:
        {
            const char* szTemp = GetString();
            auto usLength = static_cast<std::uint16_t>(strlen(szTemp));
            if (strlen(szTemp) == usLength)
            {
                snprintf(szBuffer, length, "%s", szTemp);
                return szBuffer;
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError(nullptr, "String is too long. Limit is 65535 characters.");
            }
            break;
        }
        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
        {
            CClientEntity* pElement = GetElement();
            CResource*     pResource = reinterpret_cast<CResource*>(GetUserData());
            if (pElement)
            {
                snprintf(szBuffer, length, "#E#%d", (int)pElement->GetID().Value());
                return szBuffer;
            }
            else if (VERIFY_RESOURCE(pResource))
            {
                snprintf(szBuffer, length, "#R#%s", pResource->GetName() /*.c_str ()*/);
                return szBuffer;
            }
            else
            {
                g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Couldn't convert element to string, only valid elements can be sent.");
                return nullptr;
            }
            break;
        }
        default:
        {
            g_pClientGame->GetScriptDebugging()->LogError(
                nullptr, "Couldn't convert argument to string, unsupported data type. Use String, Number, Boolean or Element.");
            return nullptr;
        }
    }
    return nullptr;
}

bool CLuaArgument::ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables) noexcept
{
    DeleteTableData();

    if (!object)
    {
        m_iType = LUA_TNIL;
        return true;
    }

    switch (json_object_get_type(object))
    {
        case json_type_null:
            m_iType = LUA_TNIL;
            break;
        case json_type_boolean:
            if (json_object_get_boolean(object) == TRUE)
                ReadBool(true);
            else
                ReadBool(false);
            break;
        case json_type_double:
        case json_type_int:
            ReadNumber(json_object_get_double(object));
            break;
        case json_type_object:
            m_pTableData = new CLuaArguments();
            m_pTableData->ReadFromJSONObject(object, pKnownTables);
            m_bWeakTableRef = false;
            m_iType = LUA_TTABLE;
            break;
        case json_type_array:
            m_pTableData = new CLuaArguments();
            m_pTableData->ReadFromJSONArray(object, pKnownTables);
            m_bWeakTableRef = false;
            m_iType = LUA_TTABLE;
            break;
        case json_type_string:
        {
            int     iLength = json_object_get_string_len(object);
            SString strString;
            strString.assign(json_object_get_string(object), iLength);
            if (iLength <= 3 || strString[0] != '^' || strString[2] != '^' || strString[1] == '^')
            {
                ReadString(strString);
                break;
            }

            switch (strString[1])
            {
                case 'E':            // element
                {
                    int id = atoi(strString.c_str() + 3);
                    CClientEntity* element = nullptr;
                    if (id != INT_MAX && id != INT_MIN && id != 0)
                        element = CElementIDs::GetElement(id);
                    if (element)
                    {
                        ReadElement(element);
                    }
                    else
                    {
                        // Appears sometimes when a player quits
                        // g_pClientGame->GetScriptDebugging()->LogError ( nullptr, "Invalid element specified in JSON string '%s'.", szString );
                        m_iType = LUA_TNIL;
                    }
                    break;
                }
                case 'R':            // resource
                {
                    CResource* resource = g_pClientGame->GetResourceManager()->GetResource(strString.c_str() + 3);
                    if (resource)
                    {
                        ReadScriptID(resource->GetScriptID());
                    }
                    else
                    {
                        g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Invalid resource specified in JSON string '%s'.", strString.c_str());
                        m_iType = LUA_TNIL;
                    }
                    break;
                }
                case 'T':            // Table reference
                {
                    unsigned long ulTableID = static_cast<unsigned long>(atol(strString.c_str() + 3));
                    if (pKnownTables && ulTableID < pKnownTables->size())
                    {
                        m_pTableData = pKnownTables->at(ulTableID);
                        m_bWeakTableRef = true;
                        m_iType = LUA_TTABLE;
                    }
                    else
                    {
                        g_pClientGame->GetScriptDebugging()->LogError(nullptr, "Invalid table reference specified in JSON string '%s'.",
                                                                        strString.c_str());
                        m_iType = LUA_TNIL;
                    }
                    break;
                }
            }
        }
        default:
            return false;
    }
    return true;
}


CLuaArgument*& CLuaArguments::at(const std::size_t index) noexcept
{
    return m_Arguments.at(index);
}
CLuaArgument* const& CLuaArguments::at(const std::size_t index) const noexcept
{
    return m_Arguments.at(index);
}
