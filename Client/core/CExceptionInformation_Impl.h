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
    CExceptionInformation_Impl();
    ~CExceptionInformation_Impl();

    unsigned int       GetCode() { return m_uiCode; }
    void*              GetAddress() { return m_pAddress; }
    [[nodiscard]] bool GetModule(void* pQueryAddress, char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress);

    virtual const char* GetModulePathName() override { return m_szModulePathName.get(); }
    virtual const char* GetModuleBaseName() override { return m_szModuleBaseName; }
    virtual uint        GetAddressModuleOffset() override { return m_uiAddressModuleOffset; }

    unsigned long GetEAX() { return m_ulEAX; }
    unsigned long GetEBX() { return m_ulEBX; }
    unsigned long GetECX() { return m_ulECX; }
    unsigned long GetEDX() { return m_ulEDX; }
    unsigned long GetESI() { return m_ulESI; }
    unsigned long GetEDI() { return m_ulEDI; }
    unsigned long GetEBP() { return m_ulEBP; }
    unsigned long GetESP() { return m_ulESP; }
    unsigned long GetEIP() { return m_ulEIP; }
    unsigned long GetCS() { return m_ulCS; }
    unsigned long GetDS() { return m_ulDS; }
    unsigned long GetES() { return m_ulES; }
    unsigned long GetFS() { return m_ulFS; }
    unsigned long GetGS() { return m_ulGS; }
    unsigned long GetSS() { return m_ulSS; }
    unsigned long GetEFlags() { return m_ulEFlags; }

    void Set(std::uint32_t iCode, _EXCEPTION_POINTERS* pException);

    [[nodiscard]] std::optional<std::vector<std::string>>              GetStackTrace() const;
    [[nodiscard]] std::optional<std::chrono::system_clock::time_point> GetTimestamp() const;
    [[nodiscard]] DWORD                                                GetThreadId() const;
    [[nodiscard]] DWORD                                                GetProcessId() const;
    [[nodiscard]] std::string                                          GetExceptionDescription() const;
    [[nodiscard]] bool                                                 HasDetailedStackTrace() const;

    [[nodiscard]] std::optional<std::exception_ptr> GetCapturedException() const;
    [[nodiscard]] int                               GetUncaughtExceptionCount() const;
    void                                            CaptureCurrentException();

    // Registry-resolved module info (independent source of truth, IDA-compatible offsets)
    [[nodiscard]] bool        HasResolvedModuleInfo() const { return m_resolvedModuleInfo.resolved; }
    [[nodiscard]] const char* GetResolvedModuleName() const { return m_resolvedModuleNameStorage.c_str(); }
    [[nodiscard]] DWORD       GetResolvedModuleBase() const { return m_resolvedModuleInfo.moduleBase; }
    [[nodiscard]] DWORD       GetResolvedRva() const { return m_resolvedModuleInfo.rva; }
    [[nodiscard]] DWORD       GetResolvedIdaAddress() const { return m_resolvedModuleInfo.idaAddress; }

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

public:
    // Registry-resolved module info (independent source of truth)
    // Public to allow SEH-protected helper functions to access
    struct ResolvedInfo
    {
        bool  resolved = false;
        DWORD moduleBase = 0;
        DWORD rva = 0;
        DWORD idaAddress = 0;
    };

private:
    ResolvedInfo m_resolvedModuleInfo;
    std::string  m_resolvedModuleNameStorage;

    void UpdateModuleBaseNameFromCurrentPath();
    void ClearModulePathState();
    void ResolveModuleFromRegistrySafe(_EXCEPTION_POINTERS* pException);
};
