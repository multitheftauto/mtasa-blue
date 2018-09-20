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
    CExceptionInformation_Impl(void);
    ~CExceptionInformation_Impl(void);            //            {};

    unsigned int        GetCode(void) { return m_uiCode; };
    void*               GetAddress(void) { return m_pAddress; };
    bool                GetModule(void* pQueryAddress, char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress);
    virtual const char* GetModulePathName(void) { return m_szModulePathName; };
    virtual const char* GetModuleBaseName(void) { return m_szModuleBaseName; };
    virtual uint        GetAddressModuleOffset(void) { return m_uiAddressModuleOffset; };

    unsigned long GetEAX(void) { return m_ulEAX; };
    unsigned long GetEBX(void) { return m_ulEBX; };
    unsigned long GetECX(void) { return m_ulECX; };
    unsigned long GetEDX(void) { return m_ulEDX; };
    unsigned long GetESI(void) { return m_ulESI; };
    unsigned long GetEDI(void) { return m_ulEDI; };
    unsigned long GetEBP(void) { return m_ulEBP; };
    unsigned long GetESP(void) { return m_ulESP; };
    unsigned long GetEIP(void) { return m_ulEIP; };
    unsigned long GetCS(void) { return m_ulCS; };
    unsigned long GetDS(void) { return m_ulDS; };
    unsigned long GetES(void) { return m_ulES; };
    unsigned long GetFS(void) { return m_ulFS; };
    unsigned long GetGS(void) { return m_ulGS; };
    unsigned long GetSS(void) { return m_ulSS; };
    unsigned long GetEFlags(void) { return m_ulEFlags; };

    void Set(unsigned int iCode, _EXCEPTION_POINTERS* pException);

private:
    unsigned int  m_uiCode;
    void*         m_pAddress;
    char*         m_szModulePathName;
    const char*   m_szModuleBaseName;
    uint          m_uiAddressModuleOffset;
    unsigned long m_ulEAX;
    unsigned long m_ulEBX;
    unsigned long m_ulECX;
    unsigned long m_ulEDX;
    unsigned long m_ulESI;
    unsigned long m_ulEDI;
    unsigned long m_ulEBP;
    unsigned long m_ulESP;
    unsigned long m_ulEIP;
    unsigned long m_ulCS;
    unsigned long m_ulDS;
    unsigned long m_ulES;
    unsigned long m_ulFS;
    unsigned long m_ulGS;
    unsigned long m_ulSS;
    unsigned long m_ulEFlags;
};
