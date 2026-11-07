/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CClientPerfStatModule.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

//
// CClientPerfStatResult
//
// Result of GetStats
//
class CClientPerfStatResult
{
    std::vector<SString> colNames;
    std::vector<SString> cellList;
    int                  iNumColumns;
    int                  iNumRows;

public:
    CClientPerfStatResult()
    {
        iNumColumns = 0;
        iNumRows = 0;
    }

    const SString& ColumnName(unsigned long c) const
    {
        unsigned long idx = c;
        if (idx < colNames.size())
            return colNames[idx];
        static SString dummy;
        return dummy;
    }

    int ColumnCount() const { return iNumColumns; }

    int RowCount() const { return iNumRows; }

    void AddColumn(const SString& strColumnName)
    {
        colNames.push_back(strColumnName);
        iNumColumns++;
    }

    SString* AddRow()
    {
        iNumRows++;
        cellList.insert(cellList.end(), ColumnCount(), SString());
        return &cellList[cellList.size() - ColumnCount()];
    }

    SString& Data(unsigned long c, unsigned long r)
    {
        unsigned long idx = c + r * ColumnCount();
        if (idx < cellList.size())
            return cellList[idx];
        static SString cellDummy;
        return cellDummy;
    }

    void Clear()
    {
        colNames.clear();
        cellList.clear();
        iNumColumns = 0;
        iNumRows = 0;
    }
};

//
// CClientPerfStatModule
//
class CClientPerfStatModule
{
public:
    virtual ~CClientPerfStatModule() {}

    virtual const SString& GetCategoryName() = 0;
    virtual void           DoPulse() = 0;
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) = 0;
};

//
// CTiming and timing structures for high-performance profiling
//
struct CTiming
{
    unsigned long calls{0};
    TIMEUS        total_us{0};
    TIMEUS        max_us{0};

    CTiming() = default;
    CTiming& operator+=(const CTiming& other)
    {
        calls += other.calls;
        total_us += other.total_us;
        max_us = std::max(max_us, other.max_us);
        return *this;
    }
};

struct CTimingPair
{
    CTiming acc;   // Accumulator for current period
    CTiming prev;  // Result for previous period

    void Pulse(CTimingPair* above)
    {
        if (above)
            above->acc += prev;
        prev = acc;
        acc = CTiming();
    }
};

class CTimingBlock
{
public:
    CTimingPair s5;   // 5 second period
    CTimingPair s60;  // 60
    CTimingPair m5;   // 300
    CTimingPair m60;  // 3600

    void Pulse1s(int flags)
    {
        if (flags & 1)
            s5.Pulse(&s60);
        if (flags & 2)
            s60.Pulse(&m5);
        if (flags & 4)
            m5.Pulse(&m60);
        if (flags & 8)
            m60.Pulse(nullptr);
    }
};

//
// CClientPerfStatLuaTiming
//
class CClientPerfStatLuaTiming : public CClientPerfStatModule
{
public:
    // CClientPerfStatModule
    virtual const SString& GetCategoryName() = 0;
    virtual void           DoPulse() = 0;
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) = 0;

    // CClientPerfStatLuaTiming
    virtual void OnLuaMainCreate(CLuaMain* pLuaMain) = 0;
    virtual void OnLuaMainDestroy(CLuaMain* pLuaMain) = 0;
    virtual void UpdateLuaTiming(CLuaMain* pLuaMain, const char* szEventName, TIMEUS timeUs) = 0;

    // Modern High-Performance Profiling API (O(1) pre-cached accumulation & active gating)
    virtual bool          IsActive() const noexcept = 0;
    virtual CTimingBlock* GetTimingBlock(CLuaMain* luaMain, const char* eventName, bool createIfNotFound = true) = 0;
    virtual CTimingBlock* GetResourceTimingBlock(CLuaMain* luaMain) = 0;
    virtual void          UpdateTimingFast(CTimingBlock* eventTiming, CTimingBlock* resourceTiming, TIMEUS timeUs) noexcept = 0;

    static CClientPerfStatLuaTiming* GetSingleton();
};

//
// CClientPerfStatLuaMemory
//
class CClientPerfStatLuaMemory : public CClientPerfStatModule
{
public:
    // CClientPerfStatModule
    virtual const SString& GetCategoryName() = 0;
    virtual void           DoPulse() = 0;
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) = 0;

    // CClientPerfStatLuaMemory
    virtual void OnLuaMainCreate(CLuaMain* pLuaMain) = 0;
    virtual void OnLuaMainDestroy(CLuaMain* pLuaMain) = 0;

    static CClientPerfStatLuaMemory* GetSingleton();
};

//
// CClientPerfStatLibMemory
//
class CClientPerfStatLibMemory : public CClientPerfStatModule
{
public:
    // CClientPerfStatModule
    virtual const SString& GetCategoryName() = 0;
    virtual void           DoPulse() = 0;
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) = 0;

    // CClientPerfStatLibMemory

    static CClientPerfStatLibMemory* GetSingleton();
};

//
// CClientPerfStatPacketUsage
//
class CClientPerfStatPacketUsage : public CClientPerfStatModule
{
public:
    // CClientPerfStatModule
    virtual const SString& GetCategoryName() = 0;
    virtual void           DoPulse() = 0;
    virtual void           GetStats(CClientPerfStatResult* pOutResult, const std::map<SString, int>& optionMap, const SString& strFilter) = 0;

    // CClientPerfStatPacketUsage

    static CClientPerfStatPacketUsage* GetSingleton();
};
