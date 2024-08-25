/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaDiscordDefs.h"
#include <CGame.h>
#include <lua/CLuaFunctionParser.h>
#include <lua/CLuaShared.h>

void CLuaDiscordDefs::LoadFunctions()
{
    // Backwards compatibility functions
    constexpr static const std::pair<const char*, lua_CFunction> functions[]
    {
        {"discordStart", ArgumentParser<DiscordStart>},
        {"discordLogin", ArgumentParser<DiscordLogin>},
        {"discordOnEvent", ArgumentParser<DiscordOnEvent>},

        {"discordGetCache", ArgumentParser<DiscordGetCache>},
        {"discordSetCache", ArgumentParser<DiscordSetCache>},

        {"discordGetGuild", ArgumentParser<DiscordGetGuild>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaDiscordDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "start", "discordStart");
    lua_classfunction(luaVM, "login", "discordLogin");
    lua_classfunction(luaVM, "on", "discordOnEvent");

    lua_classvariable(luaVM, "cache", "discordSetCache", "discordGetCache");

    lua_registerclass(luaVM, "Discord");
}

bool CLuaDiscordDefs::DiscordStart(lua_State* luaVM)
{
    static const auto discord = (&lua_getownerresource(luaVM))->GetDiscordManager();

    try
    {
        discord->start();
    }
    catch (dpp::invalid_token_exception& exc)
    {
        throw LuaFunctionError("Invalid token", false);
    }
    catch (dpp::rest_exception& exc)
    {
        throw LuaFunctionError(exc.what(), false);
    }
    return true;
}

void CLuaDiscordDefs::DiscordLogin(lua_State* luaVM, std::string token) noexcept
{
    static const auto discord = (&lua_getownerresource(luaVM))->GetDiscordManager();

    discord->login(token);
}

std::vector<std::uint8_t> CLuaDiscordDefs::DiscordGetCache(lua_State* luaVM) noexcept
{
    std::vector<std::uint8_t> policies;

    static const auto discord = (&lua_getownerresource(luaVM))->GetDiscordManager();

    policies.push_back(discord->cache_policy.channel_policy);
    policies.push_back(discord->cache_policy.emoji_policy);
    policies.push_back(discord->cache_policy.guild_policy);
    policies.push_back(discord->cache_policy.role_policy);
    policies.push_back(discord->cache_policy.user_policy);

    return policies;
}

CDiscordGuild* CLuaDiscordDefs::DiscordGetGuild(lua_State* luaVM, std::string id) noexcept
{
    static const auto discord = (&lua_getownerresource(luaVM))->GetDiscordManager();
    auto guild = discord->GetGuild(id);
    return guild;
}

// ...

bool CLuaDiscordDefs::DiscordOnEvent(lua_State* luaVM, IDiscord::DiscordEvent event, CLuaFunctionRef callback) noexcept
{
    static const auto discord = (&lua_getownerresource(luaVM))->GetDiscordManager();
    switch (event)
    {
        case IDiscord::DiscordEvent::on_voice_state_update:
            discord->on_voice_state_update.attach([callback](const auto& ev) {
                DiscordOnVoiceStateUpdate(ev, callback);
            });
            break;
        case IDiscord::DiscordEvent::on_voice_client_disconnect:
			discord->on_voice_client_disconnect.attach([callback](const auto& ev) {
				DiscordOnVoiceClientDisconnect(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_client_speaking:
			discord->on_voice_client_speaking.attach([callback](const auto& ev) {
				DiscordOnVoiceClientSpeaking(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_log:
			discord->on_log.attach([callback](const auto& ev) {
				DiscordOnLog(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_join_request_delete:
			discord->on_guild_join_request_delete.attach([callback](const auto& ev) {
				DiscordOnGuildJoinRequestDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_interaction_create:
			discord->on_interaction_create.attach([callback](const auto& ev) {
				DiscordOnInteractionCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_slashcommand:
			discord->on_slashcommand.attach([callback](const auto& ev) {
				DiscordOnSlashcommand(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_button_click:
			discord->on_button_click.attach([callback](const auto& ev) {
				DiscordOnButtonClick(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_autocomplete:
			discord->on_autocomplete.attach([callback](const auto& ev) {
				DiscordOnAutocomplete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_select_click:
			discord->on_select_click.attach([callback](const auto& ev) {
				DiscordOnSelectClick(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_context_menu:
			discord->on_message_context_menu.attach([callback](const auto& ev) {
				DiscordOnMessageContextMenu(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_user_context_menu:
			discord->on_user_context_menu.attach([callback](const auto& ev) {
				DiscordOnUserContextMenu(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_form_submit:
			discord->on_form_submit.attach([callback](const auto& ev) {
				DiscordOnFormSubmit(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_delete:
			discord->on_guild_delete.attach([callback](const auto& ev) {
				DiscordOnGuildDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_channel_delete:
			discord->on_channel_delete.attach([callback](const auto& ev) {
				DiscordOnChannelDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_channel_update:
			discord->on_channel_update.attach([callback](const auto& ev) {
				DiscordOnChannelUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_ready:
			discord->on_ready.attach([callback](const auto& ev) {
				DiscordOnReady(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_delete:
			discord->on_message_delete.attach([callback](const auto& ev) {
				DiscordOnMessageDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_member_remove:
			discord->on_guild_member_remove.attach([callback](const auto& ev) {
				DiscordOnGuildMemberRemove(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_resumed:
			discord->on_resumed.attach([callback](const auto& ev) {
				DiscordOnResumed(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_role_create:
			discord->on_guild_role_create.attach([callback](const auto& ev) {
				DiscordOnGuildRoleCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_typing_start:
			discord->on_typing_start.attach([callback](const auto& ev) {
				DiscordOnTypingStart(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_reaction_add:
			discord->on_message_reaction_add.attach([callback](const auto& ev) {
				DiscordOnMessageReactionAdd(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_members_chunk:
			discord->on_guild_members_chunk.attach([callback](const auto& ev) {
				DiscordOnGuildMembersChunk(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_reaction_remove:
			discord->on_message_reaction_remove.attach([callback](const auto& ev) {
				DiscordOnMessageReactionRemove(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_create:
			discord->on_guild_create.attach([callback](const auto& ev) {
				DiscordOnGuildCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_channel_create:
			discord->on_channel_create.attach([callback](const auto& ev) {
				DiscordOnChannelCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_reaction_remove_emoji:
			discord->on_message_reaction_remove_emoji.attach([callback](const auto& ev) {
				DiscordOnMessageReactionRemoveEmoji(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_delete_bulk:
			discord->on_message_delete_bulk.attach([callback](const auto& ev) {
				DiscordOnMessageDeleteBulk(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_role_update:
			discord->on_guild_role_update.attach([callback](const auto& ev) {
				DiscordOnGuildRoleUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_role_delete:
			discord->on_guild_role_delete.attach([callback](const auto& ev) {
				DiscordOnGuildRoleDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_channel_pins_update:
			discord->on_channel_pins_update.attach([callback](const auto& ev) {
				DiscordOnChannelPinsUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_reaction_remove_all:
			discord->on_message_reaction_remove_all.attach([callback](const auto& ev) {
				DiscordOnMessageReactionRemoveAll(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_server_update:
			discord->on_voice_server_update.attach([callback](const auto& ev) {
				DiscordOnVoiceServerUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_emojis_update:
			discord->on_guild_emojis_update.attach([callback](const auto& ev) {
				DiscordOnGuildEmojisUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_stickers_update:
			discord->on_guild_stickers_update.attach([callback](const auto& ev) {
				DiscordOnGuildStickersUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_presence_update:
			discord->on_presence_update.attach([callback](const auto& ev) {
				DiscordOnPresenceUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_webhooks_update:
			discord->on_webhooks_update.attach([callback](const auto& ev) {
				DiscordOnWebhooksUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_automod_rule_create:
			discord->on_automod_rule_create.attach([callback](const auto& ev) {
				DiscordOnAutomodRuleCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_automod_rule_update:
			discord->on_automod_rule_update.attach([callback](const auto& ev) {
				DiscordOnAutomodRuleUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_automod_rule_delete:
			discord->on_automod_rule_delete.attach([callback](const auto& ev) {
				DiscordOnAutomodRuleDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_automod_rule_execute:
			discord->on_automod_rule_execute.attach([callback](const auto& ev) {
				DiscordOnAutomodRuleExecute(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_member_add:
			discord->on_guild_member_add.attach([callback](const auto& ev) {
				DiscordOnGuildMemberAdd(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_invite_delete:
			discord->on_invite_delete.attach([callback](const auto& ev) {
				DiscordOnInviteDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_update:
			discord->on_guild_update.attach([callback](const auto& ev) {
				DiscordOnGuildUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_integrations_update:
			discord->on_guild_integrations_update.attach([callback](const auto& ev) {
				DiscordOnGuildIntegrationsUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_member_update:
			discord->on_guild_member_update.attach([callback](const auto& ev) {
				DiscordOnGuildMemberUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_invite_create:
			discord->on_invite_create.attach([callback](const auto& ev) {
                DiscordOnInviteCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_update:
			discord->on_message_update.attach([callback](const auto& ev) {
                DiscordOnMessageUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_user_update:
			discord->on_user_update.attach([callback](const auto& ev) {
				DiscordOnUserUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_create:
			discord->on_message_create.attach([callback](const auto& ev) {
				DiscordOnMessageCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_poll_vote_add:
			discord->on_message_poll_vote_add.attach([callback](const auto& ev) {
				DiscordOnMessagePollVoteAdd(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_message_poll_vote_remove:
			discord->on_message_poll_vote_remove.attach([callback](const auto& ev) {
				DiscordOnMessagePollVoteRemove(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_audit_log_entry_create:
			discord->on_guild_audit_log_entry_create.attach([callback](const auto& ev) {
				DiscordOnGuildAuditLogEntryCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_ban_add:
			discord->on_guild_ban_add.attach([callback](const auto& ev) {
				DiscordOnGuildBanAdd(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_ban_remove:
			discord->on_guild_ban_remove.attach([callback](const auto& ev) {
				DiscordOnGuildBanRemove(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_integration_create:
			discord->on_integration_create.attach([callback](const auto& ev) {
				DiscordOnIntegrationCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_integration_update:
			discord->on_integration_update.attach([callback](const auto& ev) {
				DiscordOnIntegrationUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_integration_delete:
			discord->on_integration_delete.attach([callback](const auto& ev) {
				DiscordOnIntegrationDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_create:
			discord->on_thread_create.attach([callback](const auto& ev) {
				DiscordOnThreadCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_update:
			discord->on_thread_update.attach([callback](const auto& ev) {
				DiscordOnThreadUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_delete:
			discord->on_thread_delete.attach([callback](const auto& ev) {
				DiscordOnThreadDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_list_sync:
			discord->on_thread_list_sync.attach([callback](const auto& ev) {
				DiscordOnThreadListSync(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_member_update:
			discord->on_thread_member_update.attach([callback](const auto& ev) {
				DiscordOnThreadMemberUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_thread_members_update:
			discord->on_thread_members_update.attach([callback](const auto& ev) {
				DiscordOnThreadMembersUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_scheduled_event_create:
			discord->on_guild_scheduled_event_create.attach([callback](const auto& ev) {
				DiscordOnGuildScheduledEventCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_scheduled_event_update:
			discord->on_guild_scheduled_event_update.attach([callback](const auto& ev) {
				DiscordOnGuildScheduledEventUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_scheduled_event_delete:
			discord->on_guild_scheduled_event_delete.attach([callback](const auto& ev) {
				DiscordOnGuildScheduledEventDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_scheduled_event_user_add:
			discord->on_guild_scheduled_event_user_add.attach([callback](const auto& ev) {
				DiscordOnGuildScheduledEventUserAdd(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_guild_scheduled_event_user_remove:
			discord->on_guild_scheduled_event_user_remove.attach([callback](const auto& ev) {
				DiscordOnGuildScheduledEventUserRemove(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_buffer_send:
			discord->on_voice_buffer_send.attach([callback](const auto& ev) {
				DiscordOnVoiceBufferSend(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_user_talking:
			discord->on_voice_user_talking.attach([callback](const auto& ev) {
				DiscordOnVoiceUserTalking(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_ready:
			discord->on_voice_ready.attach([callback](const auto& ev) {
				DiscordOnVoiceReady(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_receive:
			discord->on_voice_receive.attach([callback](const auto& ev) {
				DiscordOnVoiceReceive(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_receive_combined:
			discord->on_voice_receive_combined.attach([callback](const auto& ev) {
                DiscordOnVoiceReceive(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_voice_track_marker:
			discord->on_voice_track_marker.attach([callback](const auto& ev) {
				DiscordOnVoiceTrackMarker(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_stage_instance_create:
			discord->on_stage_instance_create.attach([callback](const auto& ev) {
				DiscordOnStageInstanceCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_stage_instance_update:
			discord->on_stage_instance_update.attach([callback](const auto& ev) {
				DiscordOnStageInstanceUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_stage_instance_delete:
			discord->on_stage_instance_delete.attach([callback](const auto& ev) {
				DiscordOnStageInstanceDelete(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_entitlement_create:
			discord->on_entitlement_create.attach([callback](const auto& ev) {
				DiscordOnEntitlementCreate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_entitlement_update:
			discord->on_entitlement_update.attach([callback](const auto& ev) {
				DiscordOnEntitlementUpdate(ev, callback);
			});
            break;
        case IDiscord::DiscordEvent::on_entitlement_delete:
			discord->on_entitlement_delete.attach([callback](const auto& ev) {
				DiscordOnEntitlementDelete(ev, callback);
			});
            break;
    }
    return true;
}

void CLuaDiscordDefs::DiscordOnVoiceStateUpdate(const dpp::voice_state_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}

void CLuaDiscordDefs::DiscordOnVoiceClientDisconnect(const dpp::voice_client_disconnect_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceClientSpeaking(const dpp::voice_client_speaking_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnLog(const dpp::log_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.PushString(event.message);
        arguments.PushNumber(event.severity);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildJoinRequestDelete(const dpp::guild_join_request_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnInteractionCreate(const dpp::interaction_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnSlashcommand(const dpp::slashcommand_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnButtonClick(const dpp::button_click_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnAutocomplete(const dpp::autocomplete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnSelectClick(const dpp::select_click_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageContextMenu(const dpp::message_context_menu_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnUserContextMenu(const dpp::user_context_menu_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnFormSubmit(const dpp::form_submit_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildDelete(const dpp::guild_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnChannelDelete(const dpp::channel_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnChannelUpdate(const dpp::channel_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnReady(const dpp::ready_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.PushString(event.session_id);
        arguments.PushString(std::to_string(event.shard_id));
        {
            // surround with {} to deallocate CLuaArguments
            // by going out of scope
            CLuaArguments guildsArgs;
            std::uint32_t i = 0;

            for (const auto& guild : event.guilds)
            {
                guildsArgs.PushNumber(++i);
                guildsArgs.PushString(guild.str());
            }
            arguments.PushTable(&guildsArgs);
        }
        arguments.PushNumber(event.guild_count);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageDelete(const dpp::message_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildMemberRemove(const dpp::guild_member_remove_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnResumed(const dpp::resumed_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildRoleCreate(const dpp::guild_role_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnTypingStart(const dpp::typing_start_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageReactionAdd(const dpp::message_reaction_add_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildMembersChunk(const dpp::guild_members_chunk_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageReactionRemove(const dpp::message_reaction_remove_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildCreate(const dpp::guild_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnChannelCreate(const dpp::channel_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageReactionRemoveEmoji(const dpp::message_reaction_remove_emoji_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageDeleteBulk(const dpp::message_delete_bulk_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildRoleUpdate(const dpp::guild_role_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildRoleDelete(const dpp::guild_role_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnChannelPinsUpdate(const dpp::channel_pins_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageReactionRemoveAll(const dpp::message_reaction_remove_all_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceServerUpdate(const dpp::voice_server_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildEmojisUpdate(const dpp::guild_emojis_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildStickersUpdate(const dpp::guild_stickers_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnPresenceUpdate(const dpp::presence_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnWebhooksUpdate(const dpp::webhooks_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnAutomodRuleCreate(const dpp::automod_rule_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnAutomodRuleUpdate(const dpp::automod_rule_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnAutomodRuleDelete(const dpp::automod_rule_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnAutomodRuleExecute(const dpp::automod_rule_execute_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildMemberAdd(const dpp::guild_member_add_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnInviteDelete(const dpp::invite_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildUpdate(const dpp::guild_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildIntegrationsUpdate(const dpp::guild_integrations_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildMemberUpdate(const dpp::guild_member_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnInviteCreate(const dpp::invite_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageUpdate(const dpp::message_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnUserUpdate(const dpp::user_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessageCreate(const dpp::message_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessagePollVoteAdd(const dpp::message_poll_vote_add_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnMessagePollVoteRemove(const dpp::message_poll_vote_remove_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildAuditLogEntryCreate(const dpp::guild_audit_log_entry_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildBanAdd(const dpp::guild_ban_add_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildBanRemove(const dpp::guild_ban_remove_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnIntegrationCreate(const dpp::integration_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnIntegrationUpdate(const dpp::integration_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnIntegrationDelete(const dpp::integration_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}

void PushGuild(CLuaArguments& arguments, const dpp::guild& thread)
{
}

void PushThread(CLuaArguments& arguments, const dpp::thread& thread)
{
    thread.member;
    thread.metadata;
    thread.msg;
    thread.applied_tags;
    thread.total_messages_sent;
    thread.message_count;
    thread.member_count;
    thread.newly_created;
}

void CLuaDiscordDefs::DiscordOnThreadCreate(const dpp::thread_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushGuild(arguments, *event.creating_guild);
        PushThread(arguments, event.created);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnThreadUpdate(const dpp::thread_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushGuild(arguments, *event.updating_guild);
        PushThread(arguments, event.updated);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnThreadDelete(const dpp::thread_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushGuild(arguments, *event.deleting_guild);
        PushThread(arguments, event.deleted);
        arguments.Call(&luaMain, callback);
    });
}

void CLuaDiscordDefs::DiscordOnThreadListSync(const dpp::thread_list_sync_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnThreadMemberUpdate(const dpp::thread_member_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnThreadMembersUpdate(const dpp::thread_members_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}

void CLuaDiscordDefs::DiscordOnGuildScheduledEventCreate(const dpp::guild_scheduled_event_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildScheduledEventUpdate(const dpp::guild_scheduled_event_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildScheduledEventDelete(const dpp::guild_scheduled_event_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildScheduledEventUserAdd(const dpp::guild_scheduled_event_user_add_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnGuildScheduledEventUserRemove(const dpp::guild_scheduled_event_user_remove_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceBufferSend(const dpp::voice_buffer_send_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceUserTalking(const dpp::voice_user_talking_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceReady(const dpp::voice_ready_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceReceive(const dpp::voice_receive_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnVoiceTrackMarker(const dpp::voice_track_marker_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        arguments.Call(&luaMain, callback);
    });
}

void PushStageInstance(CLuaArguments& arguments, const dpp::stage_instance& stage)
{
    arguments.PushString(stage.guild_id.str());
    arguments.PushString(stage.channel_id.str());
    arguments.PushString(stage.topic);
    const char* level = "unknown";
    switch (stage.privacy_level)
    {
        case dpp::stage_privacy_level::sp_guild_only:
            level = "guild";
            break;
        case dpp::stage_privacy_level::sp_public:
            level = "public";
            break;
    }
    arguments.PushString(level);
    arguments.PushBoolean(!stage.discoverable_disabled);
}

void CLuaDiscordDefs::DiscordOnStageInstanceCreate(const dpp::stage_instance_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushStageInstance(arguments, event.created);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnStageInstanceUpdate(const dpp::stage_instance_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushStageInstance(arguments, event.updated);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnStageInstanceDelete(const dpp::stage_instance_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushStageInstance(arguments, event.deleted);
        arguments.Call(&luaMain, callback);
    });
}

void PushEntitlement(CLuaArguments& arguments, const dpp::entitlement& entitlement)
{
    arguments.PushString(entitlement.sku_id.str());
    arguments.PushString(entitlement.application_id.str());
    arguments.PushString(entitlement.owner_id.str());
    const char* type = "unknown";
    switch (entitlement.type)
    {
        case dpp::entitlement_type::GUILD_SUBSCRIPTION:
            type = "guild";
            break;
        case dpp::entitlement_type::USER_SUBSCRIPTION:
            type = "user";
            break;
        case dpp::entitlement_type::APPLICATION_SUBSCRIPTION:
            type = "application";
            break;
    }
    arguments.PushString(type);
    arguments.PushString(std::to_string(entitlement.starts_at));
    arguments.PushString(std::to_string(entitlement.ends_at));
    arguments.PushNumber(entitlement.flags);
}

void CLuaDiscordDefs::DiscordOnEntitlementCreate(const dpp::entitlement_create_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushEntitlement(arguments, event.created);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnEntitlementUpdate(const dpp::entitlement_update_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushEntitlement(arguments, event.updating_entitlement);
        arguments.Call(&luaMain, callback);
    });
}
void CLuaDiscordDefs::DiscordOnEntitlementDelete(const dpp::entitlement_delete_t& event, const CLuaFunctionRef& callback) noexcept
{
    CLuaShared::GetAsyncTaskScheduler()->PushTask([] { return true; }, [event, callback](bool)
    {
        CLuaMain& luaMain = lua_getownercluamain(callback.GetLuaVM());

        CLuaArguments arguments;
        PushEntitlement(arguments, event.deleted);
        arguments.Call(&luaMain, callback);
    });
}
