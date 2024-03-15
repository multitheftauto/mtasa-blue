/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/core/CExceptionInformation.h
 *  PURPOSE:     Exception information interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

class CExceptionInformation
{
public:
    virtual ~CExceptionInformation() {}
    virtual std::uint32_t GetCode() = 0;
    virtual const char*  GetModulePathName() = 0;
    virtual const char*  GetModuleBaseName() = 0;
    virtual uint         GetAddressModuleOffset() = 0;

    virtual std::uint32_t GetEAX() = 0;
    virtual std::uint32_t GetEBX() = 0;
    virtual std::uint32_t GetECX() = 0;
    virtual std::uint32_t GetEDX() = 0;
    virtual std::uint32_t GetESI() = 0;
    virtual std::uint32_t GetEDI() = 0;
    virtual std::uint32_t GetEBP() = 0;
    virtual std::uint32_t GetESP() = 0;
    virtual std::uint32_t GetEIP() = 0;
    virtual std::uint32_t GetCS() = 0;
    virtual std::uint32_t GetDS() = 0;
    virtual std::uint32_t GetES() = 0;
    virtual std::uint32_t GetFS() = 0;
    virtual std::uint32_t GetGS() = 0;
    virtual std::uint32_t GetSS() = 0;
    virtual std::uint32_t GetEFlags() = 0;
};
