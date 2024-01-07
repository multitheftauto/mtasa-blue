/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>

extern "C"
{
    #include "lua.h"
}
#include "../common/CBitStream.h"

// temp fix #c2039
#define _snprintf snprintf

// json parser
#include "simdjson.h"

// json writer
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class CElement;
class CLuaArguments;
class CResource;
class CResourceManager;

#define LUA_TTABLEREF 9
#define LUA_TSTRING_LONG 10

class CLuaArgument
{
public:
    CLuaArgument();
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    ~CLuaArgument();

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument) const;
    bool                operator!=(const CLuaArgument& Argument) const;

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = NULL) const;

    void ReadBool(bool bBool);
    void ReadNumber(double dNumber);
    void ReadString(const std::string& strString);
    void ReadElement(CElement* pElement);
    void ReadElementID(ElementID ID);
    void ReadScriptID(uint uiScriptID);
    void ReadTable(class CLuaArguments* table);

    int GetType() const { return m_iType; };

    bool               GetBoolean() const { return m_bBoolean; };
    lua_Number         GetNumber() const { return m_Number; };
    const std::string& GetString() { return m_strString; };
    void*              GetUserData() const { return m_pUserData; };
    CElement*          GetElement() const;
    bool               GetAsString(SString& strBuffer);

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    char*        WriteToString(char* szBuffer, int length);

    bool         IsEqualTo(const CLuaArgument& compareTo, std::set<const CLuaArguments*>* knownTables = nullptr) const;

    // simdjson parser
    bool         DeserializeValueFromJSON(simdjson::dom::element& element, std::vector<CLuaArguments*>* pKnownTables = NULL);

    // raipdjson serializer
    bool GetResourceNameFromUserData(std::string& result) const;

    template <class Writer>
    void         SerializeToJSON(Writer* writer, bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = false)
    {
        switch (GetType())
        {
            case LUA_TNIL:
                writer->Null();
                break;
            case LUA_TBOOLEAN:
                writer->Bool(GetBoolean());
                break;
            case LUA_TTABLE:
            {
                /*
                for now its disabled

                ulong* pTableId;
                if (pKnownTables && (pTableId = MapFind(*pKnownTables, m_pTableData)))
                {
                    // Self-referencing table
                    char szTableID[10];
                    snprintf(szTableID, sizeof(szTableID), "^T^%lu", *pTableId);
                    writer->String(szTableID, static_cast<rapidjson::SizeType>(strlen(szTableID)));
                } else*/

                m_pTableData->ConvertTableToJSON(writer, bSerialize, pKnownTables);
                break;
            }
            case LUA_TNUMBER:
            {
                int number;
                if (ShouldUseInt(GetNumber(), &number))
                    writer->Int64(number);
                else
                    writer->Double(GetNumber());

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
                    writer->String(strTemp.c_str(), static_cast<rapidjson::SizeType>(strTemp.length()));
                else
                {
                    g_pGame->GetScriptDebugging()->LogError(NULL, "Invalid string specified, limit is 65535 characters.");
                    writer->Null();
                }
                break;
            }
            case LUA_TLIGHTUSERDATA:
            case LUA_TUSERDATA:
            {
                CElement*   pElement = GetElement();
                std::string pResourceName;

                if (pElement && bSerialize)
                {
                    SString elementID = SString("^E^%d", (int)pElement->GetID().Value());
                    writer->String(elementID.c_str(), static_cast<rapidjson::SizeType>(strlen(elementID)));
                }
                else if (GetResourceNameFromUserData(pResourceName))
                {
                    SString resourceName = SString("^R^%s", pResourceName.c_str());
                    writer->String(resourceName.c_str(), static_cast<rapidjson::SizeType>(strlen(resourceName)));
                }
                else
                {
                    if (pElement)            // eg toJSON() with valid element
                        g_pGame->GetScriptDebugging()->LogError(NULL, "Couldn't convert userdata argument to JSON, elements not allowed for this function.");
                    else if (!bSerialize)            // eg toJSON() with invalid element
                        g_pGame->GetScriptDebugging()->LogError(
                            NULL, "Couldn't convert userdata argument to JSON, only valid resources can be included for this function.");
                    else
                        g_pGame->GetScriptDebugging()->LogError(
                            NULL, "Couldn't convert userdata argument to JSON, only valid elements or resources can be included.");
                    writer->Null();
                }
                break;
            }
            case LUA_TFUNCTION:
            case LUA_TTHREAD:
            case LUA_TNONE:
            default:
            {
                g_pGame->GetScriptDebugging()->LogError(NULL, "Unsupported data type. Use Table, Nil, String, Number, Boolean, Resource or Element.");
                writer->Null();
            }
        }
    }

private:
    void LogUnableToPacketize(const char* szMessage) const;

    int            m_iType;
    bool           m_bBoolean;
    lua_Number     m_Number;
    std::string    m_strString;
    void*          m_pUserData;
    CLuaArguments* m_pTableData;
    bool           m_bWeakTableRef;

#ifdef MTA_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
