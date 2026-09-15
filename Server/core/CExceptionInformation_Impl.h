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

class CExceptionInformation_Impl : public CExceptionInformation
{
public:
    CExceptionInformation_Impl();
    ~CExceptionInformation_Impl();  //            {};

    unsigned int        GetCode() { return m_uiCode; };
    void*               GetAddress() { return m_pAddress; };
    bool                GetModule(char* szModuleName, int nOutputNameLength, void** ppModuleBaseAddress);
    virtual const char* GetModulePathName() { return m_szModulePathName; };
    virtual const char* GetModuleBaseName() { return m_szModuleBaseName; };
    virtual uint        GetAddressModuleOffset() { return m_uiAddressModuleOffset; };

    unsigned long GetEAX() { return m_ulEAX; };
    unsigned long GetEBX() { return m_ulEBX; };
    unsigned long GetECX() { return m_ulECX; };
    unsigned long GetEDX() { return m_ulEDX; };
    unsigned long GetESI() { return m_ulESI; };
    unsigned long GetEDI() { return m_ulEDI; };
    unsigned long GetEBP() { return m_ulEBP; };
    unsigned long GetESP() { return m_ulESP; };
    unsigned long GetEIP() { return m_ulEIP; };
    unsigned long GetCS() { return m_ulCS; };
    unsigned long GetDS() { return m_ulDS; };
    unsigned long GetES() { return m_ulES; };
    unsigned long GetFS() { return m_ulFS; };
    unsigned long GetGS() { return m_ulGS; };
    unsigned long GetSS() { return m_ulSS; };
    unsigned long GetEFlags() { return m_ulEFlags; };

#ifdef WIN_x64
    unsigned long long GetRAX() { return m_ullRAX; };
    unsigned long long GetRBX() { return m_ullRBX; };
    unsigned long long GetRCX() { return m_ullRCX; };
    unsigned long long GetRDX() { return m_ullRDX; };
    unsigned long long GetRSI() { return m_ullRSI; };
    unsigned long long GetRDI() { return m_ullRDI; };
    unsigned long long GetRBP() { return m_ullRBP; };
    unsigned long long GetRSP() { return m_ullRSP; };
    unsigned long long GetRIP() { return m_ullRIP; };
    unsigned long long GetR8() { return m_ullR8; };
    unsigned long long GetR9() { return m_ullR9; };
    unsigned long long GetR10() { return m_ullR10; };
    unsigned long long GetR11() { return m_ullR11; };
    unsigned long long GetR12() { return m_ullR12; };
    unsigned long long GetR13() { return m_ullR13; };
    unsigned long long GetR14() { return m_ullR14; };
    unsigned long long GetR15() { return m_ullR15; };
#endif

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
#ifdef WIN_x64
    unsigned long long m_ullRAX;
    unsigned long long m_ullRBX;
    unsigned long long m_ullRCX;
    unsigned long long m_ullRDX;
    unsigned long long m_ullRSI;
    unsigned long long m_ullRDI;
    unsigned long long m_ullRBP;
    unsigned long long m_ullRSP;
    unsigned long long m_ullRIP;
    unsigned long long m_ullR8;
    unsigned long long m_ullR9;
    unsigned long long m_ullR10;
    unsigned long long m_ullR11;
    unsigned long long m_ullR12;
    unsigned long long m_ullR13;
    unsigned long long m_ullR14;
    unsigned long long m_ullR15;
#endif
};
