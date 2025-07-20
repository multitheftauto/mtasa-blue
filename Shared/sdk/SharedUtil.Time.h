/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Time.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once
#include <chrono>
#include "SharedUtil.IntTypes.h"
#include "SharedUtil.Misc.h"
#include "SString.h"

namespace SharedUtil
{
    //
    // Retrieves the number of milliseconds that have elapsed since the function was first called (plus a little bit to make it look good).
    // This keeps the counter as low as possible to delay any precision or wrap around issues.
    // Note: Return value is module dependent
    //
    uint GetTickCount32();

    // Forbid use of GetTickCount
    #define GetTickCount GetTickCount_has_been_replaced_with_GetTickCount32
    #define GetTickCount64 GetTickCount64_wont_work_on_XP_you_IDIOT

    //
    // Retrieves the number of milliseconds that have elapsed since some arbitrary point in time.
    //
    // GetTickCount64() exists on Vista and up and is like GetTickCount() except it returns
    // an __int64 and will effectively never wrap. This is an emulated version for XP and down.
    // Note: Wrap around issue is only defeated if the gap between calls is less than 24 days.
    //
    long long GetTickCount64_();

    //
    // Retrieves the number of seconds that have elapsed since some arbitrary point in time.
    //
    double GetSecondCount();

    //
    // Get the time as a sortable string.
    // Set bDate to include the date, bMs to include milliseconds
    // Return example: "2010-09-01 14:54:31.091"
    SString GetTimeString(bool bDate = false, bool bMilliseconds = false, bool bLocal = false);

    // Get the local time as a sortable string.
    SString GetLocalTimeString(bool bDate = false, bool bMilliseconds = false);

    // Get time in microseconds
    typedef ulong TIMEUS;
    TIMEUS        GetTimeUs();

    // Get tick count cached per module
    long long GetModuleTickCount64();
    void      UpdateModuleTickCount64();

    // Debugging
    void AddTickCount(long long llTickCountAdd);

    //
    // Encapsulate a tick count value
    //
    class CTickCount
    {
        long long m_llTicks;

    public:
        // Constructors
        CTickCount() : m_llTicks(0) {}
        explicit CTickCount(long long llTicks) : m_llTicks(llTicks) {}
        explicit CTickCount(double dTicks) : m_llTicks(static_cast<long long>(dTicks)) {}

        // Operators
        CTickCount  operator+(const CTickCount& other) const { return CTickCount(m_llTicks + other.m_llTicks); }
        CTickCount  operator-(const CTickCount& other) const { return CTickCount(m_llTicks - other.m_llTicks); }
        CTickCount& operator+=(const CTickCount& other)
        {
            m_llTicks += other.m_llTicks;
            return *this;
        }
        CTickCount& operator-=(const CTickCount& other)
        {
            m_llTicks -= other.m_llTicks;
            return *this;
        }

        // Comparison
        bool operator<(const CTickCount& other) const { return m_llTicks < other.m_llTicks; }
        bool operator>(const CTickCount& other) const { return m_llTicks > other.m_llTicks; }
        bool operator<=(const CTickCount& other) const { return m_llTicks <= other.m_llTicks; }
        bool operator>=(const CTickCount& other) const { return m_llTicks >= other.m_llTicks; }
        bool operator==(const CTickCount& other) const { return m_llTicks == other.m_llTicks; }
        bool operator!=(const CTickCount& other) const { return m_llTicks != other.m_llTicks; }

        // Conversion
        double    ToDouble() const { return static_cast<double>(m_llTicks); }
        long long ToLongLong() const { return m_llTicks; }
        int       ToInt() const { return static_cast<int>(m_llTicks); }

        // Static functions
        static CTickCount Now(bool bUseModuleTickCount = false) { return CTickCount(bUseModuleTickCount ? GetModuleTickCount64() : GetTickCount64_()); }
    };

    //
    // Simple class to measure time passing
    // Main feature is the limit on how much time is allowed to pass between each Get()
    //
    class CElapsedTime
    {
    public:
        // MaxIncrement should be set higher than the expected tick interval between Get() calls
        CElapsedTime()
        {
            m_llMaxIncrement = INT_MAX;
            m_bUseModuleTickCount = false;
            Reset();
        }

        void SetMaxIncrement(long lMaxIncrement, bool bUseModuleTickCount = false)
        {
            m_llMaxIncrement = lMaxIncrement;
            m_bUseModuleTickCount = bUseModuleTickCount;
        }

        void SetUseModuleTickCount(bool bUseModuleTickCount) { m_bUseModuleTickCount = bUseModuleTickCount; }

        void Reset()
        {
            m_llUpdateTime = DoGetTickCount();
            m_ullElapsedTime = 0;
        }

        unsigned long long Get()
        {
            long long llTime = DoGetTickCount();
            m_ullElapsedTime += Clamp(0LL, llTime - m_llUpdateTime, m_llMaxIncrement);
            m_llUpdateTime = llTime;
            return m_ullElapsedTime;
        }

    protected:
        long long DoGetTickCount() { return m_bUseModuleTickCount ? GetModuleTickCount64() : GetTickCount64_(); }

        long long          m_llUpdateTime;
        unsigned long long m_ullElapsedTime;
        long long          m_llMaxIncrement;
        bool               m_bUseModuleTickCount;
    };

    //
    // Like CElapsedTime except it is not as accurate.
    // Has a lot better Get() performance than CElapsedTime as counting is done in another thread.
    //
    class CElapsedTimeApprox
    {
    public:
        CElapsedTimeApprox()
        {
            m_bInitialized = false;
            m_uiMaxIncrement = INT_MAX;
            m_pucCounterValue = NULL;
            m_ppIntervalCounter = NULL;
            m_ucUpdateCount = 0;
            m_uiElapsedTime = 0;
    #ifndef SHARED_UTIL_MANUAL_TIMER_INITIALIZATION
            StaticInitialize(this);
    #endif
        }

        ~CElapsedTimeApprox()
        {
            if (m_ppIntervalCounter && *m_ppIntervalCounter)
                if ((*m_ppIntervalCounter)->Release() == 0)
                    *m_ppIntervalCounter = NULL;
        }

        void SetMaxIncrement(uint uiMaxIncrement) { m_uiMaxIncrement = uiMaxIncrement; }

        void Reset()
        {
            m_ucUpdateCount = DoGetCount();
            m_uiElapsedTime = 0;
        }

        // This will wrap if gap between calls is over 25.5 seconds
        uint Get()
        {
            uchar ucCount = DoGetCount();
            uint  uiTimeDelta = (ucCount - m_ucUpdateCount) * 100U;
            m_ucUpdateCount = ucCount;
            m_uiElapsedTime += std::min(uiTimeDelta, m_uiMaxIncrement);
            return m_uiElapsedTime;
        }

        static void StaticInitialize(CElapsedTimeApprox* pTimer);

    protected:
        uchar DoGetCount() { return *m_pucCounterValue; }

        bool            m_bInitialized;
        uchar           m_ucUpdateCount;
        uint            m_uiMaxIncrement;
        uint            m_uiElapsedTime;
        uchar*          m_pucCounterValue;
        CRefCountable** m_ppIntervalCounter;
    };

    //
    // Measure time passing with microsecond resolution
    //
    class CElapsedTimeHD
    {
    public:
        CElapsedTimeHD() { Reset(); }

        void Reset() { m_resetTime = DoGetTickCount(); }

        // Returns time in milliseconds since last reset
        double Get()
        {
            std::chrono::duration<double, std::micro> elapsedTime = DoGetTickCount() - m_resetTime;
            return elapsedTime.count() / 1000.0;
        }

    protected:
        std::chrono::high_resolution_clock::time_point DoGetTickCount() { return std::chrono::high_resolution_clock::now(); }

        std::chrono::high_resolution_clock::time_point m_resetTime;
    };

    //
    // Timing sections of code
    //
    template <int RESERVE_NUM_ITEMS = 20>
    class CTimeUsMarker
    {
    public:
        struct SItem
        {
            const char* szDesc;
            TIMEUS      timeUs;
        };

        CTimeUsMarker() { itemList.reserve(RESERVE_NUM_ITEMS); }

        void Set(const char* szDesc)
        {
            itemList.push_back(SItem());
            SItem& item = itemList.back();
            item.timeUs = GetTimeUs();
            item.szDesc = szDesc;
        }

        void SetAndStoreString(const SString& strDesc)
        {
            stringStoreList.push_back(strDesc);
            Set(stringStoreList.back());
        }

        SString GetString() const
        {
            SString strStatus;
            for (uint i = 1; i < itemList.size(); i++)
            {
                const SItem& itemPrev = itemList[i - 1];
                const SItem& item = itemList[i];
                strStatus += SString("[%0.2fms %s] ", (item.timeUs - itemPrev.timeUs) / 1000.f, item.szDesc);
            }
            return strStatus;
        }

    protected:
        std::list<SString> stringStoreList;
        std::vector<SItem> itemList;
    };

}            // namespace SharedUtil
