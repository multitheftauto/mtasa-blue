/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CExceptionInformation.h
 *  PURPOSE:     Exception information interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CExceptionInformation
{
public:
    virtual ~CExceptionInformation() {}
    virtual unsigned int GetCode() = 0;
    virtual const char*  GetModulePathName() = 0;
    virtual const char*  GetModuleBaseName() = 0;
    virtual uint         GetAddressModuleOffset() = 0;

    virtual unsigned long GetEAX() = 0;
    virtual unsigned long GetEBX() = 0;
    virtual unsigned long GetECX() = 0;
    virtual unsigned long GetEDX() = 0;
    virtual unsigned long GetESI() = 0;
    virtual unsigned long GetEDI() = 0;
    virtual unsigned long GetEBP() = 0;
    virtual unsigned long GetESP() = 0;
    virtual unsigned long GetEIP() = 0;
    virtual unsigned long GetCS() = 0;
    virtual unsigned long GetDS() = 0;
    virtual unsigned long GetES() = 0;
    virtual unsigned long GetFS() = 0;
    virtual unsigned long GetGS() = 0;
    virtual unsigned long GetSS() = 0;
    virtual unsigned long GetEFlags() = 0;

#if defined(_M_X64) || defined(__x86_64__) || defined(_M_AMD64) || defined(__amd64__)
    virtual unsigned long long GetRAX() = 0;
    virtual unsigned long long GetRBX() = 0;
    virtual unsigned long long GetRCX() = 0;
    virtual unsigned long long GetRDX() = 0;
    virtual unsigned long long GetRSI() = 0;
    virtual unsigned long long GetRDI() = 0;
    virtual unsigned long long GetRBP() = 0;
    virtual unsigned long long GetRSP() = 0;
    virtual unsigned long long GetRIP() = 0;
    virtual unsigned long long GetR8() = 0;
    virtual unsigned long long GetR9() = 0;
    virtual unsigned long long GetR10() = 0;
    virtual unsigned long long GetR11() = 0;
    virtual unsigned long long GetR12() = 0;
    virtual unsigned long long GetR13() = 0;
    virtual unsigned long long GetR14() = 0;
    virtual unsigned long long GetR15() = 0;
#endif
};
