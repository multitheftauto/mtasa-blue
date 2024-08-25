/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaDatabaseDefs.h
 *  PURPOSE:     Lua function definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <CDiscord.h>

class CLuaDiscordDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static bool           DiscordStart(lua_State* luaVM);
    static void           DiscordLogin(lua_State* luaVM, std::string token) noexcept;
    static bool           DiscordOnEvent(lua_State* luaVM, IDiscord::DiscordEvent event, CLuaFunctionRef callback) noexcept;
    static CDiscordGuild* DiscordGetGuild(lua_State* luaVM, std::string id) noexcept;

    static std::vector<std::uint8_t> DiscordGetCache(lua_State* luaVM) noexcept;
    static void DiscordSetCache(lua_State* luaVM) noexcept {}

    // NOT USABLE FROM LUA
    static void DiscordOnVoiceStateUpdate(const dpp::voice_state_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceClientDisconnect(const dpp::voice_client_disconnect_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceClientSpeaking(const dpp::voice_client_speaking_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnLog(const dpp::log_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildJoinRequestDelete(const dpp::guild_join_request_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnInteractionCreate(const dpp::interaction_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnSlashcommand(const dpp::slashcommand_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnButtonClick(const dpp::button_click_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnAutocomplete(const dpp::autocomplete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnSelectClick(const dpp::select_click_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageContextMenu(const dpp::message_context_menu_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnUserContextMenu(const dpp::user_context_menu_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnFormSubmit(const dpp::form_submit_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildDelete(const dpp::guild_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnChannelDelete(const dpp::channel_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnChannelUpdate(const dpp::channel_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnReady(const dpp::ready_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageDelete(const dpp::message_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildMemberRemove(const dpp::guild_member_remove_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnResumed(const dpp::resumed_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildRoleCreate(const dpp::guild_role_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnTypingStart(const dpp::typing_start_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageReactionAdd(const dpp::message_reaction_add_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildMembersChunk(const dpp::guild_members_chunk_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageReactionRemove(const dpp::message_reaction_remove_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildCreate(const dpp::guild_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnChannelCreate(const dpp::channel_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageReactionRemoveEmoji(const dpp::message_reaction_remove_emoji_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageDeleteBulk(const dpp::message_delete_bulk_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildRoleUpdate(const dpp::guild_role_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildRoleDelete(const dpp::guild_role_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnChannelPinsUpdate(const dpp::channel_pins_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageReactionRemoveAll(const dpp::message_reaction_remove_all_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceServerUpdate(const dpp::voice_server_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildEmojisUpdate(const dpp::guild_emojis_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildStickersUpdate(const dpp::guild_stickers_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnPresenceUpdate(const dpp::presence_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnWebhooksUpdate(const dpp::webhooks_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnAutomodRuleCreate(const dpp::automod_rule_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnAutomodRuleUpdate(const dpp::automod_rule_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnAutomodRuleDelete(const dpp::automod_rule_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnAutomodRuleExecute(const dpp::automod_rule_execute_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildMemberAdd(const dpp::guild_member_add_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnInviteDelete(const dpp::invite_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildUpdate(const dpp::guild_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildIntegrationsUpdate(const dpp::guild_integrations_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildMemberUpdate(const dpp::guild_member_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnInviteCreate(const dpp::invite_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageUpdate(const dpp::message_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnUserUpdate(const dpp::user_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessageCreate(const dpp::message_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessagePollVoteAdd(const dpp::message_poll_vote_add_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnMessagePollVoteRemove(const dpp::message_poll_vote_remove_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildAuditLogEntryCreate(const dpp::guild_audit_log_entry_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildBanAdd(const dpp::guild_ban_add_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildBanRemove(const dpp::guild_ban_remove_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnIntegrationCreate(const dpp::integration_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnIntegrationUpdate(const dpp::integration_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnIntegrationDelete(const dpp::integration_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadCreate(const dpp::thread_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadUpdate(const dpp::thread_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadDelete(const dpp::thread_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadListSync(const dpp::thread_list_sync_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadMemberUpdate(const dpp::thread_member_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnThreadMembersUpdate(const dpp::thread_members_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildScheduledEventCreate(const dpp::guild_scheduled_event_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildScheduledEventUpdate(const dpp::guild_scheduled_event_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildScheduledEventDelete(const dpp::guild_scheduled_event_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildScheduledEventUserAdd(const dpp::guild_scheduled_event_user_add_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnGuildScheduledEventUserRemove(const dpp::guild_scheduled_event_user_remove_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceBufferSend(const dpp::voice_buffer_send_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceUserTalking(const dpp::voice_user_talking_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceReady(const dpp::voice_ready_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceReceive(const dpp::voice_receive_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnVoiceTrackMarker(const dpp::voice_track_marker_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnStageInstanceCreate(const dpp::stage_instance_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnStageInstanceUpdate(const dpp::stage_instance_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnStageInstanceDelete(const dpp::stage_instance_delete_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnEntitlementCreate(const dpp::entitlement_create_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnEntitlementUpdate(const dpp::entitlement_update_t& event, const CLuaFunctionRef& callback) noexcept;
    static void DiscordOnEntitlementDelete(const dpp::entitlement_delete_t& event, const CLuaFunctionRef& callback) noexcept;
};
