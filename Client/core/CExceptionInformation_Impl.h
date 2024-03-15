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

    std::uint32_t        GetCode() { return m_uiCode; };
    void*               GetAddress() { return m_pAddress; };
    bool                GetModule(void* pQueryAddress, char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress);
    virtual const char* GetModulePathName() { return m_szModulePathName; };
    virtual const char* GetModuleBaseName() { return m_szModuleBaseName; };
    virtual uint        GetAddressModuleOffset() { return m_uiAddressModuleOffset; };

    std::uint32_t GetEAX() { return m_ulEAX; };
    std::uint32_t GetEBX() { return m_ulEBX; };
    std::uint32_t GetECX() { return m_ulECX; };
    std::uint32_t GetEDX() { return m_ulEDX; };
    std::uint32_t GetESI() { return m_ulESI; };
    std::uint32_t GetEDI() { return m_ulEDI; };
    std::uint32_t GetEBP() { return m_ulEBP; };
    std::uint32_t GetESP() { return m_ulESP; };
    std::uint32_t GetEIP() { return m_ulEIP; };
    std::uint32_t GetCS() { return m_ulCS; };
    std::uint32_t GetDS() { return m_ulDS; };
    std::uint32_t GetES() { return m_ulES; };
    std::uint32_t GetFS() { return m_ulFS; };
    std::uint32_t GetGS() { return m_ulGS; };
    std::uint32_t GetSS() { return m_ulSS; };
    std::uint32_t GetEFlags() { return m_ulEFlags; };

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
