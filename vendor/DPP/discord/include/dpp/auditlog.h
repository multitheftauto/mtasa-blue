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
#include <optional>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Defines types of audit log entry
 */
enum audit_type {
	/**
	 * @brief Guild update
	 */
	aut_guild_update = 1,

	/**
	 * @brief Channel create
	 */
	aut_channel_create = 10,

	/**
	 * @brief Channel update
	 */
	aut_channel_update = 11,

	/**
	 * @brief Channel delete
	 */
	aut_channel_delete = 12,

	/**
	 * @brief Channel overwrite create
	 */
	aut_channel_overwrite_create = 13,

	/**
	 * @brief Channel overwrite update
	 */
	aut_channel_overwrite_update = 14,

	/**
	 * @brief Channel overwrite delete
	 */
	aut_channel_overwrite_delete = 15,

	/**
	 * @brief Channel member kick
	 */
	aut_member_kick	= 20,

	/**
	 * @brief Channel member prune
	 */
	aut_member_prune = 21,

	/**
	 * @brief Channel member ban add
	 */
	aut_member_ban_add = 22,

	/**
	 * @brief Channel member ban remove
	 */
	aut_member_ban_remove = 23,

	/**
	 * @brief Guild member update
	 */
	aut_member_update = 24,

	/**
	 * @brief Guild member role update
	 */
	aut_member_role_update = 25,

	/**
	 * @brief Guild member move
	 */
	aut_member_move	= 26,

	/**
	 * @brief Guild member voice disconnect
	 */
	aut_member_disconnect = 27,

	/**
	 * @brief Guild bot add
	 */
	aut_bot_add = 28,

	/**
	 * @brief Guild role create
	 */
	aut_role_create = 30,

	/**
	 * @brief Guild role update
	 */
	aut_role_update	= 31,

	/**
	 * @brief Guild role delete
	 */
	aut_role_delete	= 32,

	/**
	 * @brief Guild invite create
	 */
	aut_invite_create = 40,

	/**
	 * @brief Guild invite update
	 */
	aut_invite_update = 41,

	/**
	 * @brief Guild invite delete
	 */
	aut_invite_delete = 42,

	/**
	 * @brief Guild webhook create
	 */
	aut_webhook_create = 50,

	/**
	 * @brief Guild webhook update
	 */
	aut_webhook_update = 51,

	/**
	 * @brief Guild webhook delete
	 */
	aut_webhook_delete = 52,

	/**
	 * @brief Guild emoji create
	 */
	aut_emoji_create = 60,

	/**
	 * @brief Guild emoji update
	 */
	aut_emoji_update = 61,

	/**
	 * @brief Guild emoji delete
	 */
	aut_emoji_delete = 62,

	/**
	 * @brief Guild message delete
	 */
	aut_message_delete = 72,

	/**
	 * @brief Guild message bulk delete
	 */
	aut_message_bulk_delete	= 73,

	/**
	 * @brief Guild message pin
	 */
	aut_message_pin	= 74,

	/**
	 * @brief Guild message unpin
	 */
	aut_message_unpin = 75,

	/**
	 * @brief Guild integration create
	 */
	aut_integration_create = 80,

	/**
	 * @brief Guild integration update
	 */
	aut_integration_update = 81,

	/**
	 * @brief Guild integration delete
	 */
	aut_integration_delete = 82,

	/**
	 * @brief Stage instance create
	 */
	aut_stage_instance_create = 83,

	/**
	 * @brief Stage instance update
	 */
	aut_stage_instance_update = 84,

	/**
	 * @brief stage instance delete
	 */
	aut_stage_instance_delete = 85,

	/**
	 * @brief Sticker create
	 */
	aut_sticker_create = 90,

	/**
	 * @brief Sticker update
	 */
	aut_sticker_update = 91,

	/**
	 * @brief Sticker delete
	 */
	aut_sticker_delete = 92,

	/**
	 * @brief Scheduled event creation
	 */
	aut_guild_scheduled_event_create = 100,

	/**
	 * @brief Scheduled event update
	 */
	aut_guild_scheduled_event_update = 101,

	/**
	 * @brief Scheduled event deletion
	 */
	aut_guild_scheduled_event_delete = 102,

	/**
	 * @brief Thread create
	 */
	aut_thread_create = 110,

	/**
	 * @brief Thread update
	 */
	aut_thread_update = 111,

	/**
	 * @brief Thread delete
	 */
	aut_thread_delete = 112,

	/**
	 * @brief Application command permissions update
	 */
	aut_appcommand_permission_update = 121,

	/**
	 * @brief Auto moderation rule creation
	 */
	aut_automod_rule_create	= 140,

	/**
	 * @brief Auto moderation rule update
	 */
	aut_automod_rule_update	= 141,

	/**
	 * @brief Auto moderation rule deletion
	 */
	aut_automod_rule_delete	= 142,

	/**
	 * @brief Message was blocked by Auto Moderation
	 */
	aut_automod_block_message = 143,

	/**
	 * @brief Message was flagged by Auto Moderation
	 */
	aut_automod_flag_to_channel = 144,

	/**
	 * @brief Member was timed out by Auto Moderation
	 */
	aut_automod_user_communication_disabled = 145,

	/**
	 * @brief Creator monetization request was created
	 */
	aut_creator_monetization_request_created = 150,

	/**
	 * @brief Creator monetization terms were accepted
	 */
	aut_creator_monetization_terms_accepted = 151,
};

/**
 * @brief Defines audit log changes
 */
struct DPP_EXPORT audit_change {
	/**
	 * @brief Optional: Serialised new value of the change, e.g. for nicknames, the new nickname.
	 */
	std::string new_value;

	/**
	 * @brief Optional: Serialised old value of the change, e.g. for nicknames, the old nickname.
	 */
	std::string old_value;

	/**
	 * @brief The property name that was changed (e.g. `nick` for nickname changes).
	 * @note For dpp::aut_appcommand_permission_update updates the key is the id of the user, channel, role, or a permission constant that was updated instead of an actual property name.
	 */
	std::string key;
};

/**
 * @brief Extra information for an audit log entry
 */
struct DPP_EXPORT audit_extra {
	/**
	 * @brief Name of the Auto Moderation rule that was triggered.
	 */
	std::string automod_rule_name;

	/**
	 * @brief Trigger type of the Auto Moderation rule that was triggered.
	 */
	std::string automod_rule_trigger_type;

	/**
	 * @brief Number of days after which inactive members were kicked.
	 */
	std::string delete_member_days;

	/**
	 * @brief Number of members removed by the prune.
	 */
	std::string members_removed;

	/**
	 * @brief Channel in which the entities were targeted.
	 */
	snowflake channel_id;

	/**
	 * @brief ID of the message that was targeted.
	 */
	snowflake message_id;

	/**
	 * @brief Number of entities that were targeted.
	 */
	std::string count;

	/**
	 * @brief ID of the overwritten entity.
	 */
	snowflake id;

	/**
	 * @brief Type of overwritten entity - "0" for "role" or "1" for "member"
	 */
	std::string type;

	/**
	 * @brief Name of the role if type is "0" (not present if type is "1").
	 */
	std::string role_name;

	/**
	 * @brief ID of the app whose permissions were targeted
	 */
	snowflake application_id;
};

/**
 * @brief An individual audit log entry
 */
struct DPP_EXPORT audit_entry : public json_interface<audit_entry> {
protected:
	friend struct json_interface<audit_entry>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	audit_entry& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief ID of the entry.
	 */
	snowflake id;

	/**
	 * ID of the affected entity (webhook, user, role, etc.) (may be empty)
	 * @note For dpp::audit_type::aut_appcommand_permission_update updates, it's the command ID or the app ID
	 */
	snowflake target_id;

	/**
	 * @brief Optional: changes made to the target_id.
	 */
	std::vector<audit_change> changes;

	/**
	 * @brief The user or app that made the changes (may be empty).
	 */
	snowflake user_id;

	/**
	 * @brief Type of action that occurred.
	 */
	audit_type type;

	/**
	 * @brief Optional: additional info for certain action types.
	 */
	std::optional<audit_extra> extra;

	/**
	 * @brief Optional: the reason for the change (1-512 characters).
	 */
	std::string reason;

	/** Constructor */
	audit_entry();

	/** Destructor */
	virtual ~audit_entry() = default;
};

/**
 * @brief The auditlog class represents the audit log entries of a guild.
 */
class DPP_EXPORT auditlog : public json_interface<auditlog> {
protected:
	friend struct json_interface<auditlog>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	auditlog& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief Audit log entries.
	 */
	std::vector<audit_entry> entries;

	/** Constructor */
	auditlog() = default;

	/** Destructor */
	virtual ~auditlog() = default;
};

} // namespace dpp
