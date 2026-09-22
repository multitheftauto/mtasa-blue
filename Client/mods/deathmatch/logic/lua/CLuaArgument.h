/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaArgument.h
 *  PURPOSE:     Lua argument class header
 *
 *****************************************************************************/

#pragma once

extern "C"
{
#include "lua.h"
}
#include <net/bitstream.h>
#include <string>
#include "json.h"
#include "CStringName.h"
#include <CVector.h>
#include <CVector2D.h>
#include <CVector4D.h>
#include <CMatrix.h>

class CClientEntity;
class CLuaArguments;

#define LUA_TTABLEREF    9
#define LUA_TSTRING_LONG 10
#define LUA_TVECTOR2     11
#define LUA_TVECTOR3     12
#define LUA_TVECTOR4     13
#define LUA_TMATRIX      14

class CLuaArgument
{
public:
    CLuaArgument();
    CLuaArgument(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL);
    CLuaArgument(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    ~CLuaArgument();

    const CLuaArgument& operator=(const CLuaArgument& Argument);
    bool                operator==(const CLuaArgument& Argument);
    bool                operator!=(const CLuaArgument& Argument);

    void Read(lua_State* luaVM, int iArgument, CFastHashMap<const void*, CLuaArguments*>* pKnownTables = NULL);
    void ReadBool(bool bBool);
    void ReadNumber(double dNumber);
    void ReadString(const std::string& string);
    void ReadString(const std::string_view& string);
    void ReadString(const CStringName& string);
    void ReadString(const char* string);
    void ReadElement(CClientEntity* pElement);
    void ReadScriptID(uint uiScriptID);
    void ReadElementID(ElementID ID);
    void ReadTable(class CLuaArguments* table);
    void ReadVector(const CVector2D& vector);
    void ReadVector(const CVector& vector);
    void ReadVector(const CVector4D& vector);
    void ReadMatrix(const CMatrix& matrix);

    void Push(lua_State* luaVM, CFastHashMap<CLuaArguments*, int>* pKnownTables = NULL) const;

    int GetType() const { return m_iType; };
    int GetIndex() const { return m_iIndex; };

    bool             GetBoolean() const { return m_bBoolean; };
    lua_Number       GetNumber() const { return m_Number; };
    const SString&   GetString() { return m_strString; };
    void*            GetUserData() const { return m_pUserData; };
    CLuaArguments*   GetTable() const { return m_pTableData; }
    CClientEntity*   GetElement() const;
    const CVector4D& GetVector() const noexcept { return m_vectorData; }
    const CMatrix*   GetMatrix() const noexcept { return m_matrixData; }

    bool         ReadFromBitStream(NetBitStreamInterface& bitStream, std::vector<CLuaArguments*>* pKnownTables = NULL, unsigned int uiDepth = 0);
    bool         WriteToBitStream(NetBitStreamInterface& bitStream, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL) const;
    json_object* WriteToJSONObject(bool bSerialize = false, CFastHashMap<CLuaArguments*, unsigned long>* pKnownTables = NULL);
    bool         ReadFromJSONObject(json_object* object, std::vector<CLuaArguments*>* pKnownTables = NULL);
    char*        WriteToString(char* szBuffer, int length);

    [[nodiscard]] bool IsString() const noexcept { return m_iType == LUA_TSTRING; }

    [[nodiscard]] bool TryGetString(std::string_view& string) const noexcept
    {
        if (IsString())
        {
            string = m_strString;
            return true;
        }

        string = {};
        return false;
    }

    [[nodiscard]] bool IsNumber() const noexcept { return m_iType == LUA_TNUMBER; }

    [[nodiscard]] bool TryGetNumber(lua_Number& number) const noexcept
    {
        if (IsNumber())
        {
            number = m_Number;
            return true;
        }

        number = {};
        return false;
    }

    [[nodiscard]] bool IsTable() const noexcept;

    [[nodiscard]] bool TryGetTable(CLuaArguments*& table)
    {
        if (IsTable())
        {
            table = m_pTableData;
            return true;
        }

        table = nullptr;
        return false;
    }

    [[nodiscard]] bool IsVector2() const noexcept { return m_iType == LUA_TVECTOR2; }
    [[nodiscard]] bool IsVector3() const noexcept { return m_iType == LUA_TVECTOR3; }
    [[nodiscard]] bool IsVector4() const noexcept { return m_iType == LUA_TVECTOR4; }
    [[nodiscard]] bool IsMatrix() const noexcept { return m_iType == LUA_TMATRIX && m_matrixData != nullptr; }

private:
    void LogUnableToPacketize(const char* szMessage) const;

    int            m_iType;
    int            m_iIndex;
    bool           m_bBoolean;
    lua_Number     m_Number;
    SString        m_strString;
    void*          m_pUserData;
    CLuaArguments* m_pTableData;
    bool           m_bWeakTableRef;
    CVector4D      m_vectorData;
    CMatrix*       m_matrixData;

#ifdef MTA_DEBUG
    std::string m_strFilename;
    int         m_iLine;
#endif

    void CopyRecursive(const CLuaArgument& Argument, CFastHashMap<CLuaArguments*, CLuaArguments*>* pKnownTables = NULL);
    bool CompareRecursive(const CLuaArgument& Argument, std::set<CLuaArguments*>* pKnownTables = NULL);
    void DeleteTableData();
};
