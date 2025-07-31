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
};
