/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CDiscord.cpp
 *  PURPOSE:     Discord bot manager
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CDiscord.h"

IDiscord::IDiscord() noexcept : dpp::cluster("")
{
}

CDiscord::CDiscord() noexcept
{

}

CDiscord::~CDiscord() noexcept
{
}

void CDiscord::login(const std::string_view& string) noexcept
{
    this->token = string;
}

void CDiscord::start() noexcept
{
    dynamic_cast<dpp::cluster*>(this)->start(dpp::st_return);
}
