/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CExceptionInformation_Impl.h
 *  PURPOSE:     Header file for exception information parser class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <core/CExceptionInformation.h>
#include <windows.h>

class CExceptionInformation_Impl : public CExceptionInformation
{
public:
    CExceptionInformation_Impl();
    ~CExceptionInformation_Impl();            //            {};

    std::uint32_t           GetCode() const noexcept { return m_uiCode; };
    void*                   GetAddress() const noexcept { return m_pAddress; };
    bool                    GetModule(char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress) const noexcept;
    virtual const char*     GetModulePathName() const noexcept { return m_szModulePathName; };
    virtual const char*     GetModuleBaseName() const noexcept { return m_szModuleBaseName; };
    virtual std::uint32_t   GetAddressModuleOffset() const noexcept { return m_uiAddressModuleOffset; };

    std::uint32_t GetEAX() const noexcept { return m_ulEAX; }
    std::uint32_t GetEBX() const noexcept { return m_ulEBX; }
    std::uint32_t GetECX() const noexcept { return m_ulECX; }
    std::uint32_t GetEDX() const noexcept { return m_ulEDX; }
    std::uint32_t GetESI() const noexcept { return m_ulESI; }
    std::uint32_t GetEDI() const noexcept { return m_ulEDI; }
    std::uint32_t GetEBP() const noexcept { return m_ulEBP; }
    std::uint32_t GetESP() const noexcept { return m_ulESP; }
    std::uint32_t GetEIP() const noexcept { return m_ulEIP; }
    std::uint32_t GetCS() const noexcept { return m_ulCS; }
    std::uint32_t GetDS() const noexcept { return m_ulDS; }
    std::uint32_t GetES() const noexcept { return m_ulES; }
    std::uint32_t GetFS() const noexcept { return m_ulFS; }
    std::uint32_t GetGS() const noexcept { return m_ulGS; }
    std::uint32_t GetSS() const noexcept { return m_ulSS; }
    std::uint32_t GetEFlags() const noexcept { return m_ulEFlags; }

    void Set(std::uint32_t iCode, _EXCEPTION_POINTERS* pException);

private:
    std::uint32_t  m_uiCode;
    void*         m_pAddress;
    char*         m_szModulePathName;
    const char*   m_szModuleBaseName;
    uint          m_uiAddressModuleOffset;
    std::uint32_t m_ulEAX;
    std::uint32_t m_ulEBX;
    std::uint32_t m_ulECX;
    std::uint32_t m_ulEDX;
    std::uint32_t m_ulESI;
    std::uint32_t m_ulEDI;
    std::uint32_t m_ulEBP;
    std::uint32_t m_ulESP;
    std::uint32_t m_ulEIP;
    std::uint32_t m_ulCS;
    std::uint32_t m_ulDS;
    std::uint32_t m_ulES;
    std::uint32_t m_ulFS;
    std::uint32_t m_ulGS;
    std::uint32_t m_ulSS;
    std::uint32_t m_ulEFlags;
};
