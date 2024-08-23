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
#include <CIdArray.h>

IDiscord::IDiscord() noexcept : dpp::cluster("")
{
    on_ready.attach([this](const dpp::ready_t& event) {
        if (m_hasStarted)
            return;

        m_hasStarted = true;
    });
    on(DiscordEvent::on_ready, []() {});
}

IDiscord::~IDiscord() noexcept
{
    this->stop();
}

bool IDiscord::HasStarted() const noexcept
{
    return m_hasStarted;
}

void IDiscord::login(const std::string_view& string) noexcept
{
    this->token = string;
}

void IDiscord::start()
{
    dpp::cluster::start(dpp::st_return);
}

void IDiscord::stop()
{
    if (!m_hasStarted)
        return;
    m_hasStarted = false;
    this->shutdown();
}

template <typename F>
void IDiscord::on(DiscordEvent event, F&& func)
{

}


IDiscordGuild::IDiscordGuild()
{
    m_scriptID = CIdArray::PopUniqueId(this, EIdClass::DISCORD_GUILD);
    m_scriptID = INVALID_ARRAY_ID;
}

IDiscordGuild::~IDiscordGuild()
{
    CIdArray::PushUniqueId(this, EIdClass::DISCORD_GUILD, m_scriptID);
}

std::uint32_t IDiscordGuild::GetScriptID() const noexcept
{
    return m_scriptID;
}

IDiscordGuild* IDiscordGuild::GetFromSciptID(std::uint32_t id)
{
    return (IDiscordGuild*)CIdArray::FindEntry(id, EIdClass::DISCORD_GUILD);
}

CDiscord::CDiscord() noexcept
{
}

CDiscordGuild* CDiscord::GetGuild(dpp::snowflake id) const noexcept
{
    return dynamic_cast<CDiscordGuild*>(dpp::find_guild(id));
}

CDiscordGuild* CDiscordGuild::GetFromSciptID(std::uint32_t id)
{
    return dynamic_cast<CDiscordGuild*>(IDiscordGuild::GetFromSciptID(id));
}
