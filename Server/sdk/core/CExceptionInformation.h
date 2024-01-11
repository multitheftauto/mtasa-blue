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

    virtual std::uint32_t GetEAX() const noexcept = 0;
    virtual std::uint32_t GetEBX() const noexcept = 0;
    virtual std::uint32_t GetECX() const noexcept = 0;
    virtual std::uint32_t GetEDX() const noexcept = 0;
    virtual std::uint32_t GetESI() const noexcept = 0;
    virtual std::uint32_t GetEDI() const noexcept = 0;
    virtual std::uint32_t GetEBP() const noexcept = 0;
    virtual std::uint32_t GetESP() const noexcept = 0;
    virtual std::uint32_t GetEIP() const noexcept = 0;
    virtual std::uint32_t GetCS() const noexcept = 0;
    virtual std::uint32_t GetDS() const noexcept = 0;
    virtual std::uint32_t GetES() const noexcept = 0;
    virtual std::uint32_t GetFS() const noexcept = 0;
    virtual std::uint32_t GetGS() const noexcept = 0;
    virtual std::uint32_t GetSS() const noexcept = 0;
    virtual std::uint32_t GetEFlags() const noexcept = 0;
};
