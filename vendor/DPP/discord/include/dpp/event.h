/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2021 Craig Edwards and D++ contributors 
 * (https://github.com/brainboxdotcc/DPP/graphs/contributors)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************************/
#pragma once
#include <dpp/export.h>
#include <dpp/snowflake.h>
#include <dpp/json_fwd.h>

#define event_decl(x,wstype) /** @brief Internal event handler for wstype websocket events. Called for each websocket message of this type. @internal */ \
	class x : public event { public: virtual void handle(class dpp::discord_client* client, nlohmann::json &j, const std::string &raw); };

/**
 * @brief The events namespace holds the internal event handlers for each websocket event.
 * These are handled internally and also dispatched to the user code if the event is hooked.
 */
namespace dpp::events { 

/**
 * @brief An event object represents an event handled internally, passed from the websocket e.g. MESSAGE_CREATE.
 */
class DPP_EXPORT event {
public:
	/**
	 * @brief Pure virtual method for event handler code
	 * @param client The creating shard
	 * @param j The json data of the event
	 * @param raw The raw event json
	 */
	virtual void handle(class discord_client* client, nlohmann::json &j, const std::string &raw) = 0;
};

/* Internal logger */
event_decl(logger,LOG);

/* Guilds */
event_decl(guild_create,GUILD_CREATE);
event_decl(guild_update,GUILD_UPDATE);
event_decl(guild_delete,GUILD_DELETE);
event_decl(guild_ban_add,GUILD_BAN_ADD);
event_decl(guild_ban_remove,GUILD_BAN_REMOVE);
event_decl(guild_emojis_update,GUILD_EMOJIS_UPDATE);
event_decl(guild_integrations_update,GUILD_INTEGRATIONS_UPDATE);
event_decl(guild_join_request_delete,GUILD_JOIN_REQUEST_DELETE);
event_decl(guild_stickers_update,GUILD_STICKERS_UPDATE);

/* Stage channels */
event_decl(stage_instance_create,STAGE_INSTANCE_CREATE);
event_decl(stage_instance_update,STAGE_INSTANCE_UPDATE);
event_decl(stage_instance_delete,STAGE_INSTANCE_DELETE);

/* Guild members */
event_decl(guild_member_add,GUILD_MEMBER_ADD);
event_decl(guild_member_remove,GUILD_MEMBER_REMOVE);
event_decl(guild_members_chunk,GUILD_MEMBERS_CHUNK);
event_decl(guild_member_update,GUILD_MEMBERS_UPDATE);

/* Guild roles */
event_decl(guild_role_create,GUILD_ROLE_CREATE);
event_decl(guild_role_update,GUILD_ROLE_UPDATE);
event_decl(guild_role_delete,GUILD_ROLE_DELETE);

/* Session state */
event_decl(resumed,RESUMED);
event_decl(ready,READY);

/* Channels */
event_decl(channel_create,CHANNEL_CREATE);
event_decl(channel_update,CHANNEL_UPDATE);
event_decl(channel_delete,CHANNEL_DELETE);
event_decl(channel_pins_update,CHANNEL_PINS_UPDATE);

/* Threads */
event_decl(thread_create,THREAD_CREATE);
event_decl(thread_update,THREAD_UPDATE);
event_decl(thread_delete,THREAD_DELETE);
event_decl(thread_list_sync,THREAD_LIST_SYNC);
event_decl(thread_member_update,THREAD_MEMBER_UPDATE);
event_decl(thread_members_update,THREAD_MEMBERS_UPDATE);

/* Messages */
event_decl(message_create,MESSAGE_CREATE);
event_decl(message_update,MESSAGE_UPDATE);
event_decl(message_delete,MESSAGE_DELETE);
event_decl(message_delete_bulk,MESSAGE_DELETE_BULK);
event_decl(message_poll_vote_add,MESSAGE_POLL_VOTE_ADD);
event_decl(message_poll_vote_remove,MESSAGE_POLL_VOTE_REMOVE);

/* Presence/typing */
event_decl(presence_update,PRESENCE_UPDATE);
event_decl(typing_start,TYPING_START);

/* Users (outside of guild) */
event_decl(user_update,USER_UPDATE);

/* Message reactions */
event_decl(message_reaction_add,MESSAGE_REACTION_ADD);
event_decl(message_reaction_remove,MESSAGE_REACTION_REMOVE);
event_decl(message_reaction_remove_all,MESSAGE_REACTION_REMOVE_ALL);
event_decl(message_reaction_remove_emoji,MESSAGE_REACTION_REMOVE_EMOJI);

/* Invites */
event_decl(invite_create,INVITE_CREATE);
event_decl(invite_delete,INVITE_DELETE);

/* Voice */
event_decl(voice_state_update,VOICE_STATE_UPDATE);
event_decl(voice_server_update,VOICE_SERVER_UPDATE);

/* Webhooks */
event_decl(webhooks_update,WEBHOOKS_UPDATE);

/* Application commands */
event_decl(interaction_create,INTERACTION_CREATE);

/* Integrations */
event_decl(integration_create,INTEGRATION_CREATE);
event_decl(integration_update,INTEGRATION_UPDATE);
event_decl(integration_delete,INTEGRATION_DELETE);

/* Scheduled events */
event_decl(guild_scheduled_event_create,GUILD_SCHEDULED_EVENT_CREATE);
event_decl(guild_scheduled_event_update,GUILD_SCHEDULED_EVENT_UPDATE);
event_decl(guild_scheduled_event_delete,GUILD_SCHEDULED_EVENT_DELETE);
event_decl(guild_scheduled_event_user_add,GUILD_SCHEDULED_EVENT_USER_ADD);
event_decl(guild_scheduled_event_user_remove,GUILD_SCHEDULED_EVENT_USER_REMOVE);

/* Auto moderation */
event_decl(automod_rule_create, AUTO_MODERATION_RULE_CREATE);
event_decl(automod_rule_update, AUTO_MODERATION_RULE_UPDATE);
event_decl(automod_rule_delete, AUTO_MODERATION_RULE_DELETE);
event_decl(automod_rule_execute, AUTO_MODERATION_ACTION_EXECUTION);

/* Audit log */
event_decl(guild_audit_log_entry_create, GUILD_AUDIT_LOG_ENTRY_CREATE);

/* Entitlements */
event_decl(entitlement_create, ENTITLEMENT_CREATE);
event_decl(entitlement_update, ENTITLEMENT_UPDATE);
event_decl(entitlement_delete, ENTITLEMENT_DELETE);

} // namespace dpp::events
