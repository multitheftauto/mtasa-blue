/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegistry.h
 *  PURPOSE:     SQLite registry abstraction class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CLogger.h"
#include <list>
#include <vector>
#include <string>
#include <sqlite/sqlite3.h>

typedef CAutoRefedPointer<struct CRegistryResultData> CRegistryResult;

class CRegistry
{
    friend class CRegistryManager;
    CRegistry(const std::string& strFileName);
    ~CRegistry();

public:
    void SuspendBatching(uint uiTicks);
    void Load(const std::string& strFileName);
    bool IntegrityCheck();

    void CreateTable(const std::string& strTable, const std::string& strDefinition, bool bSilent = false);
    void DropTable(const std::string& strTable);

    bool Delete(const std::string& strTable, const std::string& strWhere);
    bool Insert(const std::string& strTable, const std::string& strValues, const std::string& strColumns);
    bool Select(const std::string& strColumns, const std::string& strTable, const std::string& strWhere, unsigned int uiLimit, CRegistryResult* pResult);
    bool Update(const std::string& strTable, const std::string& strSet, const std::string& strWhere);

    bool Query(const std::string& strQuery, class CLuaArguments* pArgs, CRegistryResult* pResult);
    bool Query(const char* szQuery, ...);
    bool Query(CRegistryResult* pResult, const char* szQuery, ...);

    const SString& GetLastError() const { return m_strLastErrorMessage; }

protected:
    bool SetLastErrorMessage(const std::string& strLastErrorMessage, const std::string& strQuery);
    bool Exec(const std::string& strQuery);
    bool ExecInternal(const char* szQuery);
    bool Query(CRegistryResult* pResult, const char* szQuery, va_list vl);
    bool QueryInternal(const char* szQuery, CRegistryResult* pResult);
    void BeginAutomaticTransaction();
    void EndAutomaticTransaction();

    sqlite3*  m_db;
    bool      m_bOpened;
    bool      m_bInAutomaticTransaction;
    long long m_llSuspendBatchingEndTime;
    SString   m_strLastErrorMessage;
    SString   m_strLastErrorQuery;
    SString   m_strFileName;

private:
    bool Query(const char* szQuery, CRegistryResult* pResult);            // Not defined to catch incorrect usage
};

struct CRegistryResultCell
{
    CRegistryResultCell()
    {
        nType = SQLITE_NULL;
        nLength = 0;
        value.pVal = NULL;
    }
    CRegistryResultCell(const CRegistryResultCell& cell)
    {
        set(cell);
        if ((nType == SQLITE_BLOB || nType == SQLITE_TEXT) && cell.value.pVal && nLength > 0)
        {
            value.pVal = new uchar[nLength];
            memcpy(value.pVal, cell.value.pVal, nLength);
        }
    };
    ~CRegistryResultCell() { SAFE_DELETE(value.pVal); }

    CRegistryResultCell& operator=(const CRegistryResultCell& cell)
    {
        SAFE_DELETE(value.pVal);

        set(cell);

        if ((nType == SQLITE_BLOB || nType == SQLITE_TEXT) && cell.value.pVal && nLength > 0)
        {
            value.pVal = new uchar[nLength];
            memcpy(value.pVal, cell.value.pVal, nLength);
        }
        return *this;
    }

    template <class T>
    void GetNumber(T& outValue) const
    {
        outValue = GetNumber<T>();
    }

    template <class T>
    T GetNumber() const
    {
        if (nType == SQLITE_INTEGER)
            return static_cast<T>(value.nVal);
        if (nType == SQLITE_FLOAT)
            return static_cast<T>(value.fVal);
        return 0;
    }

    int nType;              // Type identifier, SQLITE_*
    int nLength;            // Length in bytes if nType == SQLITE_BLOB or SQLITE_TEXT
                            //    (includes zero terminator if TEXT)
    union
    {
        std::int64_t nVal;
        float        fVal;
        uchar*       pVal;
    } value;

    void swap(
        const std::int64_t nVal = 0,
        const float fVal = 0,
        const char* const pVal = 0,
        const size_t length = 0
    ) noexcept {
        if (nVal)
        {
            value.nVal = nVal;
            value.fVal = 0;
            value.pVal = 0;
        }
        else if (fVal)
        {
            value.nVal = 0;
            value.fVal = fVal;
            value.pVal = 0;
        }
        else if (pVal && length)
        {
            value.nVal = 0;
            value.fVal = 0;
            SAFE_DELETE(value.pVal);
            value.pVal = new uchar[length];
            memcpy(value.pVal, reinterpret_cast<const uchar*>(pVal), length);
        }
    }
    const char* GetString() const noexcept {
        return reinterpret_cast<const char*>(value.pVal);
    }

private:
    CRegistryResultCell& set(const CRegistryResultCell& cell) noexcept {
        nType = cell.nType;
        nLength = cell.nLength;
        value.nVal = cell.value.nVal;
        value.fVal = cell.value.fVal;
        value.pVal = NULL;
    }
};

typedef std::vector<CRegistryResultCell>              CRegistryResultRow;
typedef std::list<CRegistryResultRow>::const_iterator CRegistryResultIterator;

struct CRegistryResultData
{
    CRegistryResultData()
    {
        nRows = 0;
        nColumns = 0;
        uiNumAffectedRows = 0;
        ullLastInsertId = 0;
        pNextResult = nullptr;
    }
    ~CRegistryResultData() { SAFE_DELETE(pNextResult); }
    std::vector<SString>          ColNames;
    std::list<CRegistryResultRow> Data;
    int                           nRows;
    int                           nColumns;
    uint                          uiNumAffectedRows;
    uint64                        ullLastInsertId;
    CRegistryResultData*          pNextResult;

    CRegistryResultData*    GetThis() { return this; }
    CRegistryResultIterator begin() const { return Data.begin(); }
    CRegistryResultIterator end() const { return Data.end(); }
};
