/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Misc.h
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <cassert>
#include <cmath>
#include <deque>
#include <list>
#include <map>
#include <set>

#include "SString.h"
#include "WString.h"
#include "SharedUtil.Defines.h"
#include "SharedUtil.Map.h"

namespace SharedUtil
{
    class CArgMap;
#ifdef WIN32

    SString GetMajorVersionString();

    // Get a system registry value
    SString GetSystemRegistryValue(uint hKey, const SString& strPath, const SString& strName);

    // Get/set registry values for the current version
    void    SetRegistryValue(const SString& strPath, const SString& strName, const SString& strValue, bool bFlush = false);
    SString GetRegistryValue(const SString& strPath, const SString& strName);
    bool    RemoveRegistryKey(const SString& strPath);

    // Get/set registry values for a particular version
    void    SetVersionRegistryValue(const SString& strVersion, const SString& strPath, const SString& strName, const SString& strValue);
    SString GetVersionRegistryValue(const SString& strVersion, const SString& strPath, const SString& strName);

    // Get/set registry values for all versions (Common)
    void    SetCommonRegistryValue(const SString& strPath, const SString& strName, const SString& strValue);
    SString GetCommonRegistryValue(const SString& strPath, const SString& strName);

    bool ShellExecuteBlocking(const SString& strAction, const SString& strFile, const SString& strParameters = "", const SString& strDirectory = "",
                              int nShowCmd = 1);
    bool ShellExecuteNonBlocking(const SString& strAction, const SString& strFile, const SString& strParameters = "", const SString& strDirectory = "",
                                 int nShowCmd = 1);

    //
    // Output a UTF8 encoded messagebox
    // Used in the Win32 Client only
    //
    #ifdef _WINDOWS_
    int MessageBoxUTF8(HWND hWnd, SString lpText, SString lpCaption, UINT uType);
    #endif

    //
    // Return full path and filename of parent exe
    //
    SString GetParentProcessPathFilename(int pid);

    //
    // Get startup directory as saved in the registry by the launcher
    // Used in the Win32 Client only
    //
    SString GetMTASABaseDir();

    //
    // Turns a relative MTASA path i.e. "MTA\file.dat"
    // into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
    //
    SString CalcMTASAPath(const SString& strPath);

    // Returns true if current process is GTA (i.e not MTA process)
    bool IsGTAProcess();

    //
    // Run ShellExecute with these parameters after exit
    //
    void SetOnQuitCommand(const SString& strOperation, const SString& strFile = "", const SString& strParameters = "", const SString& strDirectory = "",
                          const SString& strShowCmd = "");

    //
    // What to do on next restart
    //
    void SetOnRestartCommand(const SString& strOperation, const SString& strFile = "", const SString& strParameters = "", const SString& strDirectory = "",
                             const SString& strShowCmd = "");
    bool GetOnRestartCommand(SString& strOperation, SString& strFile, SString& strParameters, SString& strDirectory, SString& strShowCmd);

    //
    // What server to connect to after update
    //
    void    SetPostUpdateConnect(const SString& strHost);
    SString GetPostUpdateConnect();

    //
    // For tracking results of new features
    //
    void    AddReportLog(uint uiId, const SString& strText, uint uiAmountLimit = 0);
    void    AddExceptionReportLog(uint uiId, const char* szExceptionName, const char* szExceptionText);
    void    SetReportLogContents(const SString& strText);
    SString GetReportLogContents();
    SString GetReportLogProcessTag();
    void    WriteDebugEvent(const SString& strText);
    void    WriteErrorEvent(const SString& strText);
    void    BeginEventLog();
    void    CycleEventLog();

    void    SetApplicationSetting(const SString& strPath, const SString& strName, const SString& strValue);
    SString GetApplicationSetting(const SString& strPath, const SString& strName);
    bool    RemoveApplicationSettingKey(const SString& strPath);
    void    SetApplicationSettingInt(const SString& strPath, const SString& strName, int iValue);
    int     GetApplicationSettingInt(const SString& strPath, const SString& strName);
    int     IncApplicationSettingInt(const SString& strPath, const SString& strName);

    void    SetApplicationSetting(const SString& strName, const SString& strValue);
    SString GetApplicationSetting(const SString& strName);
    void    SetApplicationSettingInt(const SString& strName, int iValue);
    int     GetApplicationSettingInt(const SString& strName);
    int     IncApplicationSettingInt(const SString& strName);

    void WatchDogReset();
    bool WatchDogIsSectionOpen(const SString& str);
    void WatchDogIncCounter(const SString& str);
    int  WatchDogGetCounter(const SString& str);
    void WatchDogClearCounter(const SString& str);
    void WatchDogBeginSection(const SString& str);
    void WatchDogCompletedSection(const SString& str);
    bool WatchDogWasUncleanStop();
    void WatchDogSetUncleanStop(bool bOn);
    bool WatchDogWasLastRunCrash();
    void WatchDogSetLastRunCrash(bool bOn);
    void WatchDogUserDidInteractWithMenu();

    void           SetProductRegistryPath(const SString& strRegistryPath);
    const SString& GetProductRegistryPath();
    void           SetProductCommonDataDir(const SString& strCommonDataDir);
    const SString& GetProductCommonDataDir();
    void           SetProductVersion(const SString& strVersion);
    const SString& GetProductVersion();

    // BrowseToSolution flags
    enum
    {
        EXIT_GAME_FIRST = 1,             // Exit from game before showing message - Useful only if game has started and has control of the screen
        ASK_GO_ONLINE = 2,               // Ask user if he wants to go online (otherwise, always go online)
        TERMINATE_IF_YES = 4,            // What to do at the end. Only relevant if EXIT_GAME_FIRST is not used
        TERMINATE_IF_NO = 8,             //    ''
        TERMINATE_IF_YES_OR_NO = TERMINATE_IF_YES | TERMINATE_IF_NO,
        TERMINATE_PROCESS = TERMINATE_IF_YES_OR_NO,
        ICON_ERROR = 0x10,               // MB_ICONERROR
        ICON_QUESTION = 0x20,            // MB_ICONQUESTION
        ICON_WARNING = 0x30,             // MB_ICONWARNING
        ICON_INFO = 0x40,                // MB_ICONINFORMATION
        ICON_MASK_VALUE = ICON_ERROR | ICON_QUESTION | ICON_WARNING | ICON_INFO,
        SHOW_MESSAGE_ONLY = 0x80,            // Just show message without going online
    };
    void BrowseToSolution(const SString& strType, int uiFlags = 0, const SString& strMessageBoxMessage = "", const SString& strErrorCode = "");
    bool ProcessPendingBrowseToSolution();
    void ClearPendingBrowseToSolution();

    SString GetSystemErrorMessage(uint uiErrorCode, bool bRemoveNewlines = true, bool bPrependCode = true);
    void    SetClipboardText(const SString& strText);
    SString GetClipboardText();

    // Version checks
    bool IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor);
    bool IsWindowsXPSP3OrGreater();
    bool IsWindowsVistaOrGreater();
    bool IsWindows7OrGreater();
    bool IsWindows8OrGreater();

    bool QueryThreadEntryPointAddress(void* thread, DWORD* entryPointAddress);

    DWORD GetMainThreadId();

#endif

    // Ensure rand() seed gets set to a new unique value
    void RandomizeRandomSeed();

    //
    // Return true if currently executing the main thread
    // See implementation for details
    bool IsMainThread();

    // CPU stats
    struct SThreadCPUTimes
    {
        uint  uiProcessorNumber = 0;
        float fUserPercent = 0;
        float fKernelPercent = 0;
        float fTotalCPUPercent = 0;
        float fUserPercentAvg = 0;
        float fKernelPercentAvg = 0;
        float fTotalCPUPercentAvg = 0;
    };
    struct SThreadCPUTimesStore : SThreadCPUTimes
    {
        SThreadCPUTimesStore() {}
        uint64 ullPrevCPUMeasureTimeMs = 0;
        uint64 ullPrevUserTimeUs = 0;
        uint64 ullPrevKernelTimeUs = 0;
        float  fAvgTimeSeconds = 5.0f;
    };
    DWORD _GetCurrentProcessorNumber();
    void  GetThreadCPUTimes(uint64& outUserTime, uint64& outKernelTime);
    void  UpdateThreadCPUTimes(SThreadCPUTimesStore& store, long long* pllTickCount = NULL);

    SString EscapeString(const SString& strText, const SString& strDisallowedChars, char cSpecialChar = '#', uchar ucLowerLimit = 0, uchar ucUpperLimit = 255);
    SString UnescapeString(const SString& strText, char cSpecialChar = '#');
    SString EscapeURLArgument(const SString& strText);

    SString ExpandEnvString(const SString& strInput);

    // Version string things
    bool    IsValidVersionString(const SString& strVersion);
    SString ExtractVersionStringBuildNumber(const SString& strVersion);

    //
    // Try to make a path relative to the 'resources/' directory
    //
    SString ConformResourcePath(const char* szRes, bool bConvertToUnixPathSep = false);

    //
    // string stuff
    //

    std::wstring MbUTF8ToUTF16(const SString& s);

    std::string UTF16ToMbUTF8(const std::wstring& ws);
    std::string UTF16ToMbUTF8(const wchar_t* ws);
    std::string UTF16ToMbUTF8(const char16_t* ws);

    std::wstring ANSIToUTF16(const SString& s);

    int GetUTF8Confidence(const unsigned char* input, int len);

    bool IsUTF8BOM(const void* pData, uint uiLength);

    // Buffer identification
    bool IsLuaCompiledScript(const void* pData, uint uiLength);
    bool IsLuaObfuscatedScript(const void* pData, uint uiLength);

    // Return a pointer to the (shifted) trimmed string
    // @ref https://stackoverflow.com/a/26984026
    char* Trim(char* szText);

    //
    // Some templates
    //
    // Clamps a value between two other values ( min < a < max )
    template <class T>
    T Clamp(const T& min, const T& a, const T& max)
    {
        return a < min ? min : a > max ? max : a;
    }

    // Checks whether a value is between two other values ( min <= a <= max )
    template <class T>
    bool Between(const T& min, const T& a, const T& max)
    {
        return a >= min && a <= max;
    }

    // Lerps between two values depending on the weight
    template <class T>
    T Lerp(const T& from, float fAlpha, const T& to)
    {
        return (T)((to - from) * fAlpha + from);
    }

    // Find the relative position of Pos between From and To
    inline const float Unlerp(const double dFrom, const double dPos, const double dTo)
    {
        // Avoid dividing by 0 (results in INF values)
        if (dFrom == dTo)
            return 1.0f;

        return static_cast<float>((dPos - dFrom) / (dTo - dFrom));
    }

    // Unlerp avoiding extrapolation
    inline const float UnlerpClamped(const double dFrom, const double dPos, const double dTo) { return Clamp(0.0f, Unlerp(dFrom, dPos, dTo), 1.0f); }

    template <class T>
    int Round(T value)
    {
        return static_cast<int>(std::floor(value + 0.5f));
    }

    template <class T>
    T WrapAround(T fLow, T fValue, T fHigh)
    {
        const T fSize = fHigh - fLow;
        return fValue - (fSize * std::floor((fValue - fLow) / fSize));
    }

    //
    // Evaluate a function that is described by a set of points
    //
    template <class T>
    struct SSamplePoint
    {
        T in, out;
    };

    template <class T>
    T EvalSamplePosition(const SSamplePoint<T>* pPoints, uint uiNumPoints, const T& samplePosition)
    {
        // Before first point
        if (samplePosition < pPoints[0].in)
            return pPoints[0].out;

        // Between points
        for (uint i = 1; i < uiNumPoints; i++)
        {
            if (samplePosition < pPoints[i].in)
            {
                // Find position between input points
                T pos = UnlerpClamped(pPoints[i - 1].in, samplePosition, pPoints[i].in);
                // Map to output points
                return Lerp(pPoints[i - 1].out, pos, pPoints[i].out);
            }
        }

        // After last point
        return pPoints[uiNumPoints - 1].out;
    }

    //
    // Container helpers for std::list/vector/map
    //

    //
    // std:: container helpers
    //

    // Returns true if the item is in the itemList
    template <class TL, class T>
    bool ListContains(const TL& itemList, const T& item)
    {
        if (itemList.empty())
            return false;
        typename TL ::const_iterator it = itemList.begin();
        for (; it != itemList.end(); ++it)
            if (item == *it)
                return true;
        return false;
    }
    // Add item if it does not aleady exist in itemList
    template <class TL, class T>
    void ListAddUnique(TL& itemList, const T& item)
    {
        if (!ListContains(itemList, item))
            itemList.push_back(item);
    }

    //
    // std::list helpers
    //

    // Remove first occurrence of item from itemList
    template <class T>
    void ListRemoveFirst(std::list<T>& itemList, const T& item)
    {
        typename std::list<T>::iterator it = itemList.begin();
        for (; it != itemList.end(); ++it)
            if (item == *it)
            {
                itemList.erase(it);
                break;
            }
    }

    // Remove all occurrences of item from itemList
    template <class T>
    void ListRemove(std::list<T>& itemList, const T& item)
    {
        itemList.remove(item);
    }

    //
    // std::vector helpers
    //

    // Remove first occurrence of item from itemList
    template <class T>
    void ListRemoveFirst(std::vector<T>& itemList, const T& item)
    {
        typename std::vector<T>::iterator it = itemList.begin();
        for (; it != itemList.end(); ++it)
            if (item == *it)
            {
                itemList.erase(it);
                break;
            }
    }

    // Remove all occurrences of item from itemList
    template <class T>
    void ListRemove(std::vector<T>& itemList, const T& item)
    {
        typename std::vector<T>::iterator it = itemList.begin();
        while (it != itemList.end())
        {
            if (item == *it)
                it = itemList.erase(it);
            else
                ++it;
        }
    }

    // Remove item at index from itemList
    template <class T>
    void ListRemoveIndex(std::vector<T>& itemList, uint index)
    {
        if (index < itemList.size())
            itemList.erase(itemList.begin() + index);
    }

    // Append one list onto another
    template <class T>
    void ListAppend(std::vector<T>& itemList, const std::vector<T>& other)
    {
        itemList.insert(itemList.end(), other.begin(), other.end());
    }

    // Clear and reserve memory for the same size
    template <class T>
    void ListClearAndReserve(std::vector<T>& itemList)
    {
        size_t prevSize = itemList.size();
        itemList.clear();
        itemList.reserve(prevSize);
    }

    //
    // std::deque helpers
    //

    // Remove all occurrences of item from itemList
    template <class T>
    void ListRemove(std::deque<T>& itemList, const T& item)
    {
        typename std::deque<T>::iterator it = itemList.begin();
        while (it != itemList.end())
        {
            if (item == *it)
                it = itemList.erase(it);
            else
                ++it;
        }
    }

    //
    // SColor
    //
    // Encapsulates the most common usage of 4 byte color storage.
    // Casts to and from a DWORD as 0xAARRGGBB
    //
    class SColor
    {
    public:
        union
        {
            struct
            {
                unsigned char B, G, R, A;
            };
            unsigned long ulARGB;
        };

        SColor() : ulARGB(0) {}

        SColor(unsigned long ulValue) { ulARGB = ulValue; }

        operator unsigned long() const { return ulARGB; }
    };

    //
    // SColorARGB
    //
    // Make an SColor from A,R,G,B
    //
    class SColorARGB : public SColor
    {
    public:
        SColorARGB(unsigned char ucA, unsigned char ucR, unsigned char ucG, unsigned char ucB)
        {
            A = ucA;
            R = ucR;
            G = ucG;
            B = ucB;
        }

        template <class T, class U, class V, class W>
        SColorARGB(T a, U r, V g, W b)
        {
            A = Clamp<unsigned char>(0, static_cast<unsigned char>(a), 255);
            R = Clamp<unsigned char>(0, static_cast<unsigned char>(r), 255);
            G = Clamp<unsigned char>(0, static_cast<unsigned char>(g), 255);
            B = Clamp<unsigned char>(0, static_cast<unsigned char>(b), 255);
        }
    };

    //
    // SColorRGBA
    //
    // Make an SColor from R,G,B,A
    //
    class SColorRGBA : public SColor
    {
    public:
        SColorRGBA(unsigned char ucR, unsigned char ucG, unsigned char ucB, unsigned char ucA)
        {
            A = ucA;
            R = ucR;
            G = ucG;
            B = ucB;
        }

        template <class T, class U, class V, class W>
        SColorRGBA(T r, U g, V b, W a)
        {
            A = Clamp<unsigned char>(0, static_cast<unsigned char>(a), 255);
            R = Clamp<unsigned char>(0, static_cast<unsigned char>(r), 255);
            G = Clamp<unsigned char>(0, static_cast<unsigned char>(g), 255);
            B = Clamp<unsigned char>(0, static_cast<unsigned char>(b), 255);
        }
    };

    //
    // Things to make it simpler to use SColor with the source code as it stands
    //
    typedef SColor RGBA;

    inline unsigned char COLOR_RGBA_R(SColor color) { return color.R; }
    inline unsigned char COLOR_RGBA_G(SColor color) { return color.G; }
    inline unsigned char COLOR_RGBA_B(SColor color) { return color.B; }
    inline unsigned char COLOR_RGBA_A(SColor color) { return color.A; }
    inline unsigned char COLOR_ARGB_A(SColor color) { return color.A; }

    inline SColor COLOR_RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A) { return SColorRGBA(R, G, B, A); }
    inline SColor COLOR_ARGB(unsigned char A, unsigned char R, unsigned char G, unsigned char B) { return SColorRGBA(R, G, B, A); }
    inline SColor COLOR_ABGR(unsigned char A, unsigned char B, unsigned char G, unsigned char R) { return SColorRGBA(R, G, B, A); }

    //
    // Cross platform critical section
    //
    class CCriticalSection
    {
    public:
        CCriticalSection();
        ~CCriticalSection();
        void Lock();
        void Unlock();

    private:
        void* m_pCriticalSection;
    };

    //
    // Class to automatically local/unlock a critical section for the current scope
    //
    class CAutoCSLock
    {
        CAutoCSLock(const CAutoCSLock&);
        CAutoCSLock& operator=(const CAutoCSLock&);

    public:
        CAutoCSLock(CCriticalSection& criticalSection) : m_CS(criticalSection) { m_CS.Lock(); }

        ~CAutoCSLock() { m_CS.Unlock(); }

    protected:
        CCriticalSection& m_CS;
    };

    // Macro for instantiating automatic critical section locking procedure
    #define LOCK_SCOPE( cs ) \
                volatile CAutoCSLock _lock ( cs )

    //
    // Expiry stuff
    //
    int GetBuildAge();
    int GetDaysUntilExpire();

    //
    // string stuff
    //
    SString RemoveColorCodes(const char* szString);
    void    RemoveColorCodesInPlaceW(WString& strText);
    bool    IsColorCode(const char* szColorCode);
    bool    IsColorCodeW(const wchar_t* wszColorCode);

    //
    // ID 'stack'
    //
    // Note: IDs run from 1 to Capacity
    //
    template <typename T, unsigned long INITIAL_MAX_STACK_SIZE>
    class CStack
    {
    public:
        CStack()
        {
            m_ulCapacity = 0;
            ExpandBy(INITIAL_MAX_STACK_SIZE - 1);
        }

        unsigned long GetCapacity() const { return m_ulCapacity; }

        unsigned long GetUnusedAmount() const { return m_Queue.size(); }

        void ExpandBy(unsigned long ulAmount)
        {
            const unsigned long ulOldSize = m_ulCapacity;
            const unsigned long ulNewSize = m_ulCapacity + ulAmount;

            // Add ID's for new items
            for (T ID = ulOldSize + 1; ID <= ulNewSize; ++ID)
            {
                m_Queue.push_front(ID);
            }
            m_ulCapacity = ulNewSize;
        }

        bool Pop(T& dest)
        {
            // Got any items? Pop from the back
            if (m_Queue.size() > 0)
            {
                T ID = m_Queue.back();
                dest = ID;
                m_Queue.pop_back();
                return true;
            }

            // No IDs left
            return false;
        }

        void Push(T ID)
        {
            assert(m_Queue.size() < m_ulCapacity);
            // Push to the front
            m_Queue.push_front(ID);
        }

    private:
        unsigned long m_ulCapacity;
        std::deque<T> m_Queue;
    };

    //
    // Fixed sized string buffer
    //
    template <size_t MAX_LENGTH>
    class SFixedString
    {
        char szData[MAX_LENGTH + 1];

    public:
        constexpr SFixedString() { szData[0] = 0; }

        // In
        constexpr SFixedString& Assign(const char* szOther, size_t len)
        {
            STRNCPY(szData, szOther, len + 1);
            return *this;
        }

        constexpr SFixedString& operator=(const char* szOther)
        {
            Assign(szOther, MAX_LENGTH + 1);
            return *this;
        }
#ifdef __cpp_lib_string_view
        constexpr SFixedString& operator=(std::string_view other)
        {
            Assign(other.data(), other.length() + 1);
            return *this;
        }
#endif

#ifdef __cpp_lib_string_view
        // Out
        constexpr operator std::string_view() const { return {szData}; }
#endif
        constexpr       operator const char*() const { return szData; }
        constexpr char* Data() { return &szData[0]; }

        constexpr size_t GetMaxLength() const { return MAX_LENGTH; }
        size_t           GetLength() const { return strlen(szData); }

        // Shake it all about
        void           Encrypt();
        constexpr bool Empty() { return szData[0] == 0; }
        constexpr void Clear() const { szData[0] = 0; }

        // Returns a pointer to a null-terminated character array
        const char* c_str() const noexcept { return &szData[0]; }
    };

    ///////////////////////////////////////////////////////////////
    //
    // CArgMap
    //
    // For manipulation and packing of key/values <=> string
    // "key1=value1&key2=value2"
    //
    ///////////////////////////////////////////////////////////////
    class CArgMap
    {
        std::multimap<SString, SString> m_Map;
        SString                         m_strArgSep;
        SString                         m_strPartsSep;
        SString                         m_strDisallowedChars;
        char                            m_cEscapeCharacter;

    public:
        CArgMap(const SString& strArgSep = "=", const SString& strPartsSep = "&", const SString& strExtraDisallowedChars = "");
        void    SetEscapeCharacter(char cEscapeCharacter);
        void    Merge(const CArgMap& other, bool bAllowMultiValues = false);
        void    SetFromString(const SString& strLine, bool bAllowMultiValues = false);
        void    MergeFromString(const SString& strLine, bool bAllowMultiValues = false);
        SString ToString() const;
        bool    HasMultiValues() const;
        void    RemoveMultiValues();
        SString Escape(const SString& strIn) const;
        SString Unescape(const SString& strIn) const;
        void    Set(const SString& strInCmd, const SString& strInValue);                                    // Set a unique key string value
        void    Set(const SString& strInCmd, int iValue);                                                   // Set a unique key int value
        void    Insert(const SString& strInCmd, int iValue);                                                // Insert a key int value
        void    Insert(const SString& strInCmd, const SString& strInValue);                                 // Insert a key string value
        bool    Contains(const SString& strInCmd) const;                                                    // Test if key exists
        bool    Get(const SString& strInCmd, SString& strOut, const char* szDefault = "") const;            // First result as string
        SString Get(const SString& strInCmd) const;                                                         // First result as string
        bool    Get(const SString& strInCmd, std::vector<SString>& outList) const;                          // All results as strings
        bool    Get(const SString& strInCmd, int& iValue, int iDefault = 0) const;                          // First result as int
        void    GetKeys(std::vector<SString>& outList) const;                                               // All keys
    };

    // Some templated accessors for CArgMap derived classes
    template <class T, class U>
    void SetOption(SString& strText, const SString& strKey, const U& value)
    {
        T temp;
        temp.SetFromString(strText);
        temp.Set(strKey, value);
        strText = temp.ToString();
    }

    template <class T>
    void GetOption(const SString& strText, const SString& strKey, SString& strOutValue, const char* szDefault = "")
    {
        T temp;
        temp.SetFromString(strText);
        temp.Get(strKey, strOutValue, szDefault);
    }

    template <class T>
    void GetOption(const SString& strText, const SString& strKey, int& iOutValue, int iDefault = 0)
    {
        T temp;
        temp.SetFromString(strText);
        temp.Get(strKey, iOutValue, iDefault);
    }

    // Coerce to a bool from an int
    template <class T>
    void GetOption(const SString& strText, const SString& strKey, bool& bOutValue, int iDefault = 0)
    {
        T temp;
        temp.SetFromString(strText);
        int iOutValue;
        temp.Get(strKey, iOutValue, iDefault);
        bOutValue = (iOutValue != 0);
    }

    // Coerce to other types from an int
    template <class T, class U>
    void GetOption(const SString& strText, const SString& strKey, U& outValue, int iDefault = 0)
    {
        T temp;
        temp.SetFromString(strText);
        int iOutValue;
        temp.Get(strKey, iOutValue, iDefault);
        outValue = static_cast<U>(iOutValue);
    }

    // Comma separated set of numbers
    template <class T, class U>
    void GetOption(const SString& strText, const SString& strKey, const char* szSeperator, std::set<U>& outValues)
    {
        SString strNumbers;
        GetOption<T>(strText, strKey, strNumbers);
        std::vector<SString> numberList;
        strNumbers.Split(szSeperator, numberList);
        for (uint i = 0; i < numberList.size(); i++)
            if (!numberList[i].empty())
                MapInsert(outValues, static_cast<U>(atoi(numberList[i])));
    }

    ///////////////////////////////////////////////////////////////
    //
    // CMappedContainer
    //
    // Combination list and map - For fast list Contains() method
    //
    ///////////////////////////////////////////////////////////////
    template <class T, class LIST_TYPE>
    class CMappedContainer
    {
    public:
        // map only
        bool Contains(const T& item) const { return MapContains(m_Map, item); }

        // list only
        typename LIST_TYPE ::iterator         begin() { return m_List.begin(); }
        typename LIST_TYPE ::iterator         end() { return m_List.end(); }
        typename LIST_TYPE ::const_iterator   begin() const { return m_List.begin(); }
        typename LIST_TYPE ::const_iterator   end() const { return m_List.end(); }
        typename LIST_TYPE ::reverse_iterator rbegin() { return m_List.rbegin(); }
        typename LIST_TYPE ::reverse_iterator rend() { return m_List.rend(); }
        uint                                  size() const { return m_List.size(); }
        bool                                  empty() const { return m_List.empty(); }
        const T&                              back() const { return m_List.back(); }
        const T&                              front() const { return m_List.front(); }

        // list and map
        void push_back(const T& item)
        {
            AddMapRef(item);
            m_List.push_back(item);
        }

        void push_front(const T& item)
        {
            AddMapRef(item);
            m_List.push_front(item);
        }

        void pop_back()
        {
            RemoveMapRef(m_List.back());
            m_List.pop_back();
        }

        void pop_front()
        {
            RemoveMapRef(m_List.front());
            m_List.pop_front();
        }

        // Remove all occurrences of item
        void remove(const T& item)
        {
            if (Contains(item))
            {
                // Remove all refs from map and list
                MapRemove(m_Map, item);
                ListRemove(m_List, item);
            }
        }

        void clear()
        {
            m_Map.clear();
            m_List.clear();
        }

        typename LIST_TYPE ::iterator erase(typename LIST_TYPE ::iterator iter)
        {
            RemoveMapRef(*iter);
            return m_List.erase(iter);
        }

    protected:
        // Increment reference count for item
        void AddMapRef(const T& item)
        {
            if (int* pInt = MapFind(m_Map, item))
                (*pInt)++;
            else
                MapSet(m_Map, item, 1);
        }

        // Decrement reference count for item
        void RemoveMapRef(const T& item)
        {
            typename std::map<T, int>::iterator it = m_Map.find(item);
            if (it != m_Map.end())
                if (!--(it->second))
                    m_Map.erase(it);
        }

        // Debug
        void Validate() const
        {
            int iTotalRefs = 0;
            for (typename std::map<T, int>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
                iTotalRefs += it->second;
            assert(iTotalRefs == m_List.size());
        }

        std::map<T, int> m_Map;
        LIST_TYPE        m_List;
    };

    template <class T>
    class CMappedList : public CMappedContainer<T, std::list<T> >
    {
    };

    template <class T>
    class CMappedArray : public CMappedContainer<T, std::vector<T> >
    {
    public:
        const T& operator[](int idx) const { return CMappedContainer<T, std::vector<T> >::m_List[idx]; }
        T&       operator[](int idx) { return CMappedContainer<T, std::vector<T> >::m_List[idx]; }
    };

    // Returns true if the item is in the itemList
    template <class U, class T>
    bool ListContains(const CMappedList<U>& itemList, const T& item)
    {
        return itemList.Contains(item);
    }

    // Returns true if the item is in the itemList
    template <class U, class T>
    bool ListContains(const CMappedArray<U>& itemList, const T& item)
    {
        return itemList.Contains(item);
    }

    // Remove all occurrences of item from itemList
    template <class U, class T>
    void ListRemove(CMappedList<U>& itemList, const T& item)
    {
        itemList.remove(item);
    }

    // Remove all occurrences of item from itemList
    template <class U, class T>
    void ListRemove(CMappedArray<U>& itemList, const T& item)
    {
        itemList.remove(item);
    }

    ///////////////////////////////////////////////////////////////
    //
    // CIntrusiveListNode
    //
    // Entry in an CIntrusiveList
    //
    ///////////////////////////////////////////////////////////////
    template <class T>
    class CIntrusiveListNode
    {
    public:
        typedef CIntrusiveListNode<T> Node;

        CIntrusiveListNode(T* pOuterItem) : m_pOuterItem(pOuterItem), m_pPrev(NULL), m_pNext(NULL) {}

        T*    m_pOuterItem;            // Item this node is inside
        Node* m_pPrev;
        Node* m_pNext;
    };

    ///////////////////////////////////////////////////////////////
    //
    // CIntrusiveList
    //
    // A linked list with the links stored inside the list items
    //
    ///////////////////////////////////////////////////////////////
    template <typename T>
    class CIntrusiveList
    {
        void operator=(const CIntrusiveList& other);            // Copy will probably not work as expected
        // CIntrusiveList ( const CIntrusiveList& other );       // Default copy constructor is required by dense_hash for some reason

    public:
        class IteratorBase;

    protected:
        typedef CIntrusiveListNode<T> Node;

        size_t m_Size;
        Node*  m_pFirst;
        Node*  m_pLast;
        Node T::*                  m_pNodePtr;                   // Pointer to the CIntrusiveListNode member variable in T
        std::vector<IteratorBase*> m_ActiveIterators;            // Keep track of iterators

    public:
        //
        // CIntrusiveList < T > :: IteratorBase
        //
        class IteratorBase
        {
        protected:
            CIntrusiveList<T>* m_pList;

        public:
            Node* m_pNode;
            IteratorBase(CIntrusiveList<T>* pList, Node* pNode) : m_pList(pList), m_pNode(pNode) { m_pList->m_ActiveIterators.push_back(this); }
            ~IteratorBase() { ListRemove(m_pList->m_ActiveIterators, this); }
            T*           operator*() { return m_pNode->m_pOuterItem; }
            virtual void NotifyRemovingNode(Node* pNode) = 0;
        };

        //
        // CIntrusiveList < T > :: Iterator
        //
        class Iterator : public IteratorBase
        {
        public:
            Iterator(CIntrusiveList<T>* pList, Node* pNode) : IteratorBase(pList, pNode) {}
            bool         operator==(const Iterator& other) const { return IteratorBase::m_pNode == other.m_pNode; }
            bool         operator!=(const Iterator& other) const { return IteratorBase::m_pNode != other.m_pNode; }
            void         operator++() { IteratorBase::m_pNode = IteratorBase::m_pNode->m_pNext; }
            void         operator++(int) { IteratorBase::m_pNode = IteratorBase::m_pNode->m_pNext; }
            virtual void NotifyRemovingNode(Node* pNode)
            {
                if (IteratorBase::m_pNode == pNode)
                    IteratorBase::m_pNode = IteratorBase::m_pNode->m_pNext;
            }
        };

        //
        // CIntrusiveList < T > :: ReverseIterator
        //
        class ReverseIterator : public IteratorBase
        {
        public:
            ReverseIterator(CIntrusiveList<T>* pList, Node* pNode) : IteratorBase(pList, pNode) {}
            bool         operator==(const ReverseIterator& other) const { return IteratorBase::m_pNode == other.m_pNode; }
            bool         operator!=(const ReverseIterator& other) const { return IteratorBase::m_pNode != other.m_pNode; }
            void         operator++() { IteratorBase::m_pNode = IteratorBase::m_pNode->m_pPrev; }
            void         operator++(int) { IteratorBase::m_pNode = IteratorBase::m_pNode->m_pPrev; }
            virtual void NotifyRemovingNode(Node* pNode)
            {
                if (IteratorBase::m_pNode == pNode)
                    IteratorBase::m_pNode = IteratorBase::m_pNode->m_pPrev;
            }
        };

        //
        // Constructor
        //
        CIntrusiveList(Node T::*pNodePtr) : m_pNodePtr(pNodePtr)
        {
            assert(m_pNodePtr);            // This must be set upon construction
            m_Size = 0;
            m_pFirst = NULL;
            m_pLast = NULL;
        }

        ~CIntrusiveList() { assert(m_ActiveIterators.empty()); }

        bool empty() const { return m_Size == 0; }

        size_t size() const { return m_Size; }

        //
        // Check if list contains item
        //
        bool contains(T* pItem) const
        {
            Node* pNode = &(pItem->*m_pNodePtr);
            return m_pFirst == pNode || pNode->m_pPrev || pNode->m_pNext;
        }

        //
        // Remove item from list
        //
        void remove(T* pItem)
        {
            Node* pNode = &(pItem->*m_pNodePtr);

            if (!pNode->m_pPrev && !pNode->m_pNext && m_pFirst != pNode)
                return;            // Not in list

            // Keep active iterators valid
            for (int i = m_ActiveIterators.size() - 1; i >= 0; i--)
                if (m_ActiveIterators[i]->m_pNode == pNode)
                    m_ActiveIterators[i]->NotifyRemovingNode(pNode);

            if (m_pFirst == pNode)
            {
                if (m_pLast == pNode)
                {
                    // Only item in list
                    assert(!pNode->m_pPrev && !pNode->m_pNext);
                    m_pFirst = NULL;
                    m_pLast = NULL;
                }
                else
                {
                    // First item in list
                    assert(!pNode->m_pPrev && pNode->m_pNext && pNode->m_pNext->m_pPrev == pNode);
                    pNode->m_pNext->m_pPrev = NULL;
                    m_pFirst = pNode->m_pNext;
                }
            }
            else if (m_pLast == pNode)
            {
                // Last item in list
                assert(pNode->m_pPrev && !pNode->m_pNext && pNode->m_pPrev->m_pNext == pNode);
                pNode->m_pPrev->m_pNext = NULL;
                m_pLast = pNode->m_pPrev;
            }
            else
            {
                // Somewhere in the middle
                assert(pNode->m_pPrev && pNode->m_pNext && pNode->m_pPrev->m_pNext == pNode && pNode->m_pNext->m_pPrev == pNode);
                pNode->m_pPrev->m_pNext = pNode->m_pNext;
                pNode->m_pNext->m_pPrev = pNode->m_pPrev;
            }
            pNode->m_pNext = NULL;
            pNode->m_pPrev = NULL;
            m_Size--;
        }

        //
        // Insert item at the start of the list
        //
        void push_front(T* pItem)
        {
            Node* pNode = &(pItem->*m_pNodePtr);
            assert(!pNode->m_pPrev && !pNode->m_pNext && !m_pFirst == !m_pLast);
            if (!m_pFirst)
            {
                // Initial list item
                m_pFirst = pNode;
                m_pLast = pNode;
            }
            else
            {
                pNode->m_pNext = m_pFirst;
                pNode->m_pNext->m_pPrev = pNode;
                m_pFirst = pNode;
            }
            m_Size++;
        }

        //
        // Insert item at the end of the list
        //
        void push_back(T* pItem)
        {
            Node* pNode = &(pItem->*m_pNodePtr);
            assert(!pNode->m_pPrev && !pNode->m_pNext && !m_pFirst == !m_pLast);
            if (!m_pFirst)
            {
                // Initial list item
                m_pFirst = pNode;
                m_pLast = pNode;
            }
            else
            {
                pNode->m_pPrev = m_pLast;
                pNode->m_pPrev->m_pNext = pNode;
                m_pLast = pNode;
            }
            m_Size++;
        }

        Iterator begin() { return Iterator(this, m_pFirst); }

        Iterator end() { return Iterator(this, NULL); }

        ReverseIterator rbegin() { return ReverseIterator(this, m_pLast); }

        ReverseIterator rend() { return ReverseIterator(this, NULL); }

        // Allow use of std iterator names
        typedef Iterator        iterator;
        typedef Iterator        const_iterator;            // TODO
        typedef ReverseIterator reverse_iterator;
        typedef ReverseIterator const_reverse_iterator;            // TODO
    };

    ///////////////////////////////////////////////////////////////
    //
    // CIntrusiveListExt
    //
    // Uses the member variable pointer declared in the template
    //
    ///////////////////////////////////////////////////////////////
    template <typename T, CIntrusiveListNode<T> T::*member_ptr>
    class CIntrusiveListExt : public CIntrusiveList<T>
    {
    public:
        CIntrusiveListExt() : CIntrusiveList<T>(member_ptr) {}
    };

    //
    // tolower / toupper
    // Implemented here so it can be inlined.
    //
    static const char ms_ucTolowerTab[256] = {
        '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x12',
        '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', '\x20', '\x21', '\x22', '\x23', '\x24', '\x25',
        '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f', '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38',
        '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f', '\x40', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b',
        '\x6c', '\x6d', '\x6e', '\x6f', '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x5b', '\x5c', '\x5d', '\x5e',
        '\x5f', '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67', '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f', '\x70', '\x71',
        '\x72', '\x73', '\x74', '\x75', '\x76', '\x77', '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f', '\x80', '\x81', '\x82', '\x83', '\x84',
        '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
        '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f', '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa',
        '\xab', '\xac', '\xad', '\xae', '\xaf', '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd',
        '\xbe', '\xbf', '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', '\xd0',
        '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', '\xe0', '\xe1', '\xe2', '\xe3',
        '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6',
        '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'};
    static const char ms_ucToupperTab[256] = {
        '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07', '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f', '\x10', '\x11', '\x12',
        '\x13', '\x14', '\x15', '\x16', '\x17', '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f', '\x20', '\x21', '\x22', '\x23', '\x24', '\x25',
        '\x26', '\x27', '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f', '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37', '\x38',
        '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f', '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b',
        '\x4c', '\x4d', '\x4e', '\x4f', '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e',
        '\x5f', '\x60', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47', '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f', '\x50', '\x51',
        '\x52', '\x53', '\x54', '\x55', '\x56', '\x57', '\x58', '\x59', '\x5a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f', '\x80', '\x81', '\x82', '\x83', '\x84',
        '\x85', '\x86', '\x87', '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f', '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
        '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f', '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7', '\xa8', '\xa9', '\xaa',
        '\xab', '\xac', '\xad', '\xae', '\xaf', '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7', '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd',
        '\xbe', '\xbf', '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7', '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf', '\xd0',
        '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7', '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf', '\xe0', '\xe1', '\xe2', '\xe3',
        '\xe4', '\xe5', '\xe6', '\xe7', '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef', '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6',
        '\xf7', '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff'};
    template <typename T>
    inline T tolower(T c)
    {
        return static_cast<T>(ms_ucTolowerTab[static_cast<unsigned char>(c)]);
    }
    template <typename T>
    inline T toupper(T c)
    {
        return static_cast<T>(ms_ucToupperTab[static_cast<unsigned char>(c)]);
    }

    //
    // enum reflection shenanigans
    //
    enum eDummy
    {
    };

    template <class T>
    struct CEnumInfo
    {
        struct SEnumItem
        {
            T           iValue;
            const char* szName;
        };

        CEnumInfo(const SString& strTypeName, const SEnumItem* pItemList, uint uiAmount, eDummy defaultValue, const SString& strDefaultName)
        {
            m_strTypeName = strTypeName;
            m_strDefaultName = strDefaultName;
            m_DefaultValue = defaultValue;
            for (uint i = 0; i < uiAmount; i++)
            {
                const SEnumItem& item = pItemList[i];
                m_ValueMap[item.szName] = (eDummy)item.iValue;
                m_NameMap[(eDummy)item.iValue] = item.szName;
            }
        }

        bool ValueValid(eDummy value) const { return MapContains(m_NameMap, value); }

        const SString& FindName(eDummy value) const
        {
            if (const SString* pName = MapFind(m_NameMap, value))
                return *pName;
            return m_strDefaultName;
        }

        bool FindValue(const SString& strName, eDummy& outResult) const
        {
            const eDummy* pValue;
            if ((pValue = MapFind(m_ValueMap, strName)) || (pValue = MapFind(m_ValueMap, strName.ToLower())))
            {
                outResult = *pValue;
                return true;
            }
            outResult = m_DefaultValue;
            return false;
        }

        const SString& GetTypeName() const { return m_strTypeName; }

        SString                   m_strTypeName;
        SString                   m_strDefaultName;
        eDummy                    m_DefaultValue;
        std::map<SString, eDummy> m_ValueMap;
        std::map<eDummy, SString> m_NameMap;
    };

    #define DECLARE_ENUM2(T, U) \
        CEnumInfo<U>*          GetEnumInfo     ( const T* ); \
        inline const SString&  EnumToString    ( const T& value )                           { return GetEnumInfo ( (T*)0 )->FindName    ( (eDummy)value ); }\
        inline bool            StringToEnum    ( const SString& strName, T& outResult )     { return GetEnumInfo ( (T*)0 )->FindValue   ( strName, (eDummy&)outResult ); }\
        inline const SString&  GetEnumTypeName ( const T& )                                 { return GetEnumInfo ( (T*)0 )->GetTypeName (); }\
        inline bool            EnumValueValid  ( const T& value )                           { return GetEnumInfo ( (T*)0 )->ValueValid  ( (eDummy)value ); }\

    #define IMPLEMENT_ENUM_BEGIN2(T, U) \
        CEnumInfo<U>* GetEnumInfo( const T* ) \
        { \
            using CEnumInfo = CEnumInfo<U>; \
            static const CEnumInfo::SEnumItem items[] = {

    #define IMPLEMENT_ENUM_END(name) \
        IMPLEMENT_ENUM_END_DEFAULTS(name,0,"")

    #define IMPLEMENT_ENUM_END_DEFAULTS(name,defvalue,defname) \
                            }; \
            static CEnumInfo info( name, items, NUMELMS(items),(eDummy)(defvalue),defname ); \
            return &info; \
        }

    #define ADD_ENUM(value,name) {value, name},
    #define ADD_ENUM1(value)     {value, #value},

    // enum
    #define DECLARE_ENUM(T)                                             DECLARE_ENUM2(T, int)
    #define IMPLEMENT_ENUM_BEGIN(T)                                     IMPLEMENT_ENUM_BEGIN2(T, int)

    // enum class
    #define DECLARE_ENUM_CLASS(T)                                       DECLARE_ENUM2(T, T)
    #define IMPLEMENT_ENUM_CLASS_BEGIN(T)                               IMPLEMENT_ENUM_BEGIN2(T, T)
    #define IMPLEMENT_ENUM_CLASS_END(name)                              IMPLEMENT_ENUM_END(name)
    #define IMPLEMENT_ENUM_CLASS_END_DEFAULTS(name,defvalue,defname)    IMPLEMENT_ENUM_END_DEFAULTS(name,defvalue,defname)

    //
    // Fast wildcard matching
    //
    inline bool WildcardMatch(const char* wild, const char* string)
    {
        // Written by Jack Handy - jakkhandy@hotmail.com
        assert(wild && string);

        const char *cp = NULL, *mp = NULL;

        while ((*string) && (*wild != '*'))
        {
            if ((*wild != *string) && (*wild != '?'))
            {
                return 0;
            }
            wild++;
            string++;
        }

        while (*string)
        {
            if (*wild == '*')
            {
                if (!*++wild)
                {
                    return 1;
                }
                mp = wild;
                cp = string + 1;
            }
            else if ((*wild == *string) || (*wild == '?'))
            {
                wild++;
                string++;
            }
            else
            {
                wild = mp;
                string = cp++;
            }
        }

        while (*wild == '*')
        {
            wild++;
        }
        return !*wild;
    }

    //
    // Fast wildcard matching (case insensitive)
    //
    inline bool WildcardMatchI(const char* wild, const char* string)
    {
        // Written by Jack Handy - jakkhandy@hotmail.com
        assert(wild && string);

        const char *cp = NULL, *mp = NULL;

        while ((*string) && (*wild != '*'))
        {
            if ((SharedUtil::tolower(*wild) != SharedUtil::tolower(*string)) && (*wild != '?'))
            {
                return 0;
            }
            wild++;
            string++;
        }

        while (*string)
        {
            if (*wild == '*')
            {
                if (!*++wild)
                {
                    return 1;
                }
                mp = wild;
                cp = string + 1;
            }
            else if ((SharedUtil::tolower(*wild) == SharedUtil::tolower(*string)) || (*wild == '?'))
            {
                wild++;
                string++;
            }
            else
            {
                wild = mp;
                string = cp++;
            }
        }

        while (*wild == '*')
        {
            wild++;
        }
        return !*wild;
    }

    ///////////////////////////////////////////////////////////////
    //
    // ReadTokenSeparatedList
    //
    // Split token separated values into an array.
    // Removes leading/trailing spaces and empty items
    //
    ///////////////////////////////////////////////////////////////
    inline void ReadTokenSeparatedList(const SString& strDelim, const SString& strInput, std::vector<SString>& outList)
    {
        strInput.Split(strDelim, outList);
        // Remove surrounding spaces for each item
        for (auto iter = outList.begin(); iter != outList.end();)
        {
            SString& strItem = *iter;
            strItem = strItem.TrimEnd(" ").TrimStart(" ");
            if (strItem.empty())
                iter = outList.erase(iter);
            else
                ++iter;
        }
    }

    inline void ReadCommaSeparatedList(const SString& strInput, std::vector<SString>& outList) { return ReadTokenSeparatedList(",", strInput, outList); }

    ///////////////////////////////////////////////////////////////
    //
    // CFilterMap
    //
    // Change the string "+all,-{1000~2006},+2003,-{2050},-2611,-{3120},-{4002~4100},+{4010~4020}"
    // into a map which can be queried using IsFiltered( int )
    //
    ///////////////////////////////////////////////////////////////
    class CFilterMap
    {
    public:
        CFilterMap(const SString& strFilterDesc = "")
        {
            SetAll('+');
            AddString(strFilterDesc);
        }

        bool IsFiltered(int iValue) const
        {
            if (MapContains(idMap, iValue))
                return cDefaultType == '+';
            return cDefaultType == '-';
        }

    protected:
        void AddString(const SString& strFilterDesc)
        {
            std::vector<SString> partList;
            strFilterDesc.Split(",", partList);
            for (uint i = 0; i < partList.size(); i++)
            {
                const SString& part = partList[i];
                char           cType = part.Left(1)[0];

                SString strRest = part.Right((int)part.length() - 1);
                strRest = strRest.Replace("{", "").Replace("}", "");

                SString strFrom, strTo;
                strRest.Split("~", &strFrom, &strTo);

                if (strFrom == "all")
                    SetAll(cType);
                else if (strTo.empty())
                    AddSingle(cType, atoi(strFrom));
                else
                {
                    const int iTo = atoi(strTo);
                    for (int i = atoi(strFrom); i <= iTo; i++)
                        AddSingle(cType, i);
                }
            }
        }

        void AddSingle(char cType, int iValue)
        {
            if (cType != cDefaultType)
            {
                // Add
                MapSet(idMap, iValue, true);
            }
            else
            {
                // Remove
                MapRemove(idMap, iValue);
            }
        }

        void SetAll(char cType)
        {
            idMap.clear();
            cDefaultType = cType;
        }

        std::map<uint, bool> idMap;
        char                 cDefaultType;
    };

    ///////////////////////////////////////////////////////////////
    //
    // CRefCountable
    //
    // Thread safe reference counting base class
    // Shared critical section is used for construction/destruction speed
    //
    ///////////////////////////////////////////////////////////////
    class CRefCountable
    {
        int                     m_iRefCount;
        CCriticalSection*       m_pCS;            // Use a pointer incase the static variable exists more than once
        static CCriticalSection ms_CS;

    protected:
        virtual ~CRefCountable() {}

    public:
        CRefCountable() : m_iRefCount(1), m_pCS(&ms_CS) {}

        void AddRef()
        {
            m_pCS->Lock();
            ++m_iRefCount;
            m_pCS->Unlock();
        }

        int Release()
        {
            m_pCS->Lock();
            assert(m_iRefCount > 0);
            int iNewRefCount = --m_iRefCount;
            m_pCS->Unlock();

            if (iNewRefCount == 0)
                delete this;
            return iNewRefCount;
        }
    };

    //
    // Fixed size array
    //
    // Replacement for e.g.  int var[100]
    // Checks bounds
    //
    template <class T, int SIZE>
    struct SFixedArray
    {
        T& operator[](uint uiIndex)
        {
            assert(uiIndex < SIZE);
            return data[uiIndex];
        }

        const T& operator[](uint uiIndex) const
        {
            assert(uiIndex < SIZE);
            return data[uiIndex];
        }

        T data[SIZE];
    };

    //
    // Fixed size array with a constructer
    // so it can be used with the IMPLEMENT_FIXED_ARRAY macro
    //
    template <class T, int SIZE>
    struct SFixedArrayInit : SFixedArray<T, SIZE>
    {
        SFixedArrayInit(const T* pInitData, uint uiInitCount)
        {
            dassert(SIZE == uiInitCount);
            memcpy(SFixedArray<T, SIZE>::data, pInitData, sizeof(SFixedArray<T, SIZE>::data));
        }
    };

    // Use this macro if the size of the initializer list is unknown
    #define IMPLEMENT_FIXED_ARRAY( vartype, varname ) \
        SFixedArrayInit < vartype, NUMELMS( _##varname ) > varname ( _##varname, NUMELMS( _##varname ) )

    //
    //  Ranges of numbers. i.e. 100-4000, 5000-6999, 7000-7010
    //
    class CRanges
    {
    public:
        void SetRange(uint uiStart, uint uiLength);
        void UnsetRange(uint uiStart, uint uiLength);
        bool IsRangeSet(uint uiStart, uint uiLength);            // Returns true if any part of the range already exists in the map

    protected:
        typedef std::map<uint, uint>::iterator IterType;

        void RemoveObscuredRanges(uint uiStart, uint uiLast);
        bool GetRangeOverlappingPoint(uint uiPoint, IterType& result);

        std::map<uint, uint> m_StartLastMap;
    };

    //
    // Pointer with reference count.
    //
    template <typename T>
    class CRefedPointer : public CRefCountable
    {
    private:
        T* pData;            // Target

        virtual ~CRefedPointer() { SAFE_DELETE(pData); }
        CRefedPointer(const CRefedPointer<T>& other);
        CRefedPointer<T>& operator=(const CRefedPointer<T>& other);

    public:
        CRefedPointer() { pData = new T(); }

        T* GetData() { return pData; }
    };

    //
    // Smart pointer with reference count.
    //
    template <typename T>
    class CAutoRefedPointer
    {
    private:
        CRefedPointer<T>* pPointer;

    public:
        CAutoRefedPointer() { pPointer = new CRefedPointer<T>(); }

        CAutoRefedPointer(const CAutoRefedPointer<T>& other)
        {
            pPointer = other.pPointer;
            pPointer->AddRef();
        }

        ~CAutoRefedPointer() { pPointer->Release(); }

        CAutoRefedPointer<T>& operator=(const CAutoRefedPointer<T>& other)
        {
            // Assignment operator
            if (this != &other)            // Avoid self assignment
            {
                CRefedPointer<T>* pOldPointer = pPointer;

                // Copy the data and reference pointer
                // and increment the reference count
                pPointer = other.pPointer;
                pPointer->AddRef();

                // Decrement the old reference count
                pOldPointer->Release();
            }
            return *this;
        }

        T* operator->() { return pPointer->GetData(); }

        const T* operator->() const { return pPointer->GetData(); }
    };
};            // namespace SharedUtil

using namespace SharedUtil;

//
// For checking MTA library module versions
//
typedef void(FUNC_GetMtaVersion)(char* pBuffer, uint uiMaxSize);
MTAEXPORT void GetLibMtaVersion(char* pBuffer, uint uiMaxSize);
