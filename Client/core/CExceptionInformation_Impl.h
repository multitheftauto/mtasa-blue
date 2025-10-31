/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CExceptionInformation_Impl.h
 *  PURPOSE:     Header file for exception information parser class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CExceptionInformation.h>
#include <windows.h>
#include <cstdint>
#include <optional>
#include <vector>
#include <string>
#include <chrono>
#include <exception>
#include <memory>

class CExceptionInformation_Impl : public CExceptionInformation
{
public:
    CExceptionInformation_Impl() noexcept;
    ~CExceptionInformation_Impl() noexcept;

    unsigned int       GetCode() noexcept { return m_uiCode; }
    void*              GetAddress() noexcept { return m_pAddress; }
    [[nodiscard]] bool GetModule(void* pQueryAddress, char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress) noexcept;

    virtual const char* GetModulePathName() override { return m_szModulePathName.get(); }
    virtual const char* GetModuleBaseName() override { return m_szModuleBaseName; }
    virtual uint        GetAddressModuleOffset() override { return m_uiAddressModuleOffset; }

    unsigned long GetEAX() noexcept { return m_ulEAX; }
    unsigned long GetEBX() noexcept { return m_ulEBX; }
    unsigned long GetECX() noexcept { return m_ulECX; }
    unsigned long GetEDX() noexcept { return m_ulEDX; }
    unsigned long GetESI() noexcept { return m_ulESI; }
    unsigned long GetEDI() noexcept { return m_ulEDI; }
    unsigned long GetEBP() noexcept { return m_ulEBP; }
    unsigned long GetESP() noexcept { return m_ulESP; }
    unsigned long GetEIP() noexcept { return m_ulEIP; }
    unsigned long GetCS() noexcept { return m_ulCS; }
    unsigned long GetDS() noexcept { return m_ulDS; }
    unsigned long GetES() noexcept { return m_ulES; }
    unsigned long GetFS() noexcept { return m_ulFS; }
    unsigned long GetGS() noexcept { return m_ulGS; }
    unsigned long GetSS() noexcept { return m_ulSS; }
    unsigned long GetEFlags() noexcept { return m_ulEFlags; }

    void Set(std::uint32_t iCode, _EXCEPTION_POINTERS* pException) noexcept;

    [[nodiscard]] std::optional<std::vector<std::string>>              GetStackTrace() const noexcept;
    [[nodiscard]] std::optional<std::chrono::system_clock::time_point> GetTimestamp() const noexcept;
    [[nodiscard]] DWORD                                                GetThreadId() const noexcept;
    [[nodiscard]] DWORD                                                GetProcessId() const noexcept;
    [[nodiscard]] std::string                                          GetExceptionDescription() const noexcept;
    [[nodiscard]] bool                                                 HasDetailedStackTrace() const noexcept;

    [[nodiscard]] std::optional<std::exception_ptr> GetCapturedException() const noexcept;
    [[nodiscard]] int                               GetUncaughtExceptionCount() const noexcept;
    void                                            CaptureCurrentException() noexcept;

private:
    std::uint32_t           m_uiCode;
    void*                   m_pAddress;
    std::unique_ptr<char[]> m_szModulePathName;
    const char*             m_szModuleBaseName;
    std::string             m_moduleBaseNameStorage;
    uint                    m_uiAddressModuleOffset;
    unsigned long           m_ulEAX;
    unsigned long           m_ulEBX;
    unsigned long           m_ulECX;
    unsigned long           m_ulEDX;
    unsigned long           m_ulESI;
    unsigned long           m_ulEDI;
    unsigned long           m_ulEBP;
    unsigned long           m_ulESP;
    unsigned long           m_ulEIP;
    unsigned long           m_ulCS;
    unsigned long           m_ulDS;
    unsigned long           m_ulES;
    unsigned long           m_ulFS;
    unsigned long           m_ulGS;
    unsigned long           m_ulSS;
    unsigned long           m_ulEFlags;

    std::vector<std::string>              m_stackTrace;
    std::chrono::system_clock::time_point m_timestamp;
    DWORD                                 m_threadId;
    DWORD                                 m_processId;
    bool                                  m_hasDetailedStackTrace;

    std::exception_ptr m_capturedException;
    int                m_uncaughtExceptionCount;

    void UpdateModuleBaseNameFromCurrentPath() noexcept;
    void ClearModulePathState() noexcept;
};
