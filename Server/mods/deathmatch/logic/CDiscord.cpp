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

std::unordered_map<dpp::snowflake, std::unique_ptr<CDiscordGuild>> CDiscord::ms_guilds = {};

IDiscord::IDiscord() noexcept : dpp::cluster("")
{
}

CDiscord::CDiscord() noexcept
{
    auto handle = on_ready.attach(
        [this](const dpp::ready_t& event)
        {
            if (m_hasStarted)
                return;

            m_hasStarted = true;
        });
    m_eventHandlers[DiscordEvent::on_ready].push_back(handle);
}

CDiscord::~CDiscord() noexcept
{
    this->stop();
}

bool CDiscord::HasStarted() const noexcept
{
    return m_hasStarted;
}

void CDiscord::login(const std::string_view& string) noexcept
{
    this->token = string;
}

void CDiscord::start()
{
    dpp::cluster::start(dpp::st_return);
}

void CDiscord::stop()
{
    if (!m_hasStarted)
        return;
    m_hasStarted = false;
    this->shutdown();
}

CDiscordGuild::CDiscordGuild()
{
    m_scriptID = CIdArray::PopUniqueId(this, EIdClass::DISCORD_GUILD);
    m_scriptID = INVALID_ARRAY_ID;
}

CDiscordGuild::CDiscordGuild(dpp::guild guild) : CDiscordGuild()
{
    *this = guild;
}

CDiscordGuild::CDiscordGuild(const dpp::guild* guild) : CDiscordGuild(guild ? *guild : dpp::guild())
{
}

CDiscordGuild::~CDiscordGuild()
{
    CIdArray::PushUniqueId(this, EIdClass::DISCORD_GUILD, m_scriptID);
}

std::uint32_t CDiscordGuild::GetScriptID() const noexcept
{
    return m_scriptID;
}

CDiscordGuild* CDiscordGuild::GetFromSciptID(std::uint32_t id)
{
    return static_cast<CDiscordGuild*>(CIdArray::FindEntry(id, EIdClass::DISCORD_GUILD));
}

IDiscordGuild* IDiscordGuild::GetFromSciptID(std::uint32_t id)
{
    return CDiscordGuild::GetFromSciptID(id);
}

CDiscordGuild* CDiscord::GetGuild(dpp::snowflake id) noexcept
{
    if (SharedUtil::MapContains(ms_guilds, id))
        return ms_guilds[id].get();

    auto guild = dpp::find_guild(id);
    if (!guild)
        return nullptr;

    ms_guilds[id] = std::make_unique<CDiscordGuild>(*guild);
    return ms_guilds[id].get();
}

IDiscordGuild* IDiscord::GetGuild(dpp::snowflake id) noexcept
{
    return dynamic_cast<IDiscordGuild*>(CDiscord::GetGuild(id));
}
