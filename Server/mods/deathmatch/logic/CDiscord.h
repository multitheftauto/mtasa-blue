/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDiscord.h
 *  PURPOSE:     Discord bot manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "dpp/dpp.h"

class IDiscord : public dpp::cluster
{
public:
    IDiscord() noexcept;
    virtual ~IDiscord() noexcept = 0;

    virtual void login(const std::string_view& token) noexcept = 0;
    virtual void start() noexcept = 0;
    void         start(bool) = delete;
};

class CDiscord : public IDiscord
{
public:
    CDiscord() noexcept;
    ~CDiscord() noexcept override;

    void login(const std::string_view& token) noexcept override;
    void start() noexcept override;

protected:
};
