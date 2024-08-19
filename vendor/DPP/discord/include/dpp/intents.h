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

namespace dpp {

/**
 * @brief intents are a bitmask of allowed events on your websocket.
 * 
 * Some of these are known as Privileged intents (GUILD_MEMBERS and GUILD_PRESENCES)
 * and require verification of a bot over 100 servers by discord via submission of
 * your real life ID.
 */
enum intents {
	/**
	 * @brief Intent for receipt of guild information.
	 */
	i_guilds			= (1 << 0),

	/**
	 * @brief Intent for receipt of guild members.
	 */
	i_guild_members			= (1 << 1),

	/**
	 * @brief Intent for receipt of guild bans.
	 */
	i_guild_bans			= (1 << 2),

	/**
	 * @brief Intent for receipt of guild emojis.
	 */
	i_guild_emojis			= (1 << 3),

	/**
	 * @brief Intent for receipt of guild integrations.
	 */
	i_guild_integrations		= (1 << 4),

	/**
	 * @brief Intent for receipt of guild webhooks.
	 */
	i_guild_webhooks		= (1 << 5),

	/**
	 * @brief Intent for receipt of guild invites.
	 */
	i_guild_invites			= (1 << 6),

	/**
	 * @brief Intent for receipt of guild voice states.
	 */
	i_guild_voice_states		= (1 << 7),

	/**
	 * @brief Intent for receipt of guild presences.
	 */
	i_guild_presences		= (1 << 8),

	/**
	 * @brief Intent for receipt of guild messages.
	 */
	i_guild_messages		= (1 << 9),

	/**
	 * @brief Intent for receipt of guild message reactions.
	 */
	i_guild_message_reactions	= (1 << 10),

	/**
	 * @brief Intent for receipt of guild message typing notifications.
	 */
	i_guild_message_typing		= (1 << 11),

	/**
	 * @brief Intent for receipt of direct messages (DMs).
	 */
	i_direct_messages		= (1 << 12),

	/**
	 * @brief Intent for receipt of direct message reactions.
	 */
	i_direct_message_reactions	= (1 << 13),

	/**
	 * @brief Intent for receipt of direct message typing notifications.
	 */
	i_direct_message_typing		= (1 << 14),

	/**
	 * @brief Intent for receipt of message content.
	 */
	i_message_content		= (1 << 15),

	/**
	 * @brief Scheduled events.
	 */
	i_guild_scheduled_events	= (1 << 16),

	/**
	 * @brief Auto moderation configuration.
	 */
	i_auto_moderation_configuration	= (1 << 20),

	/**
	 * @brief Auto moderation configuration.
	 */
	i_auto_moderation_execution	= (1 << 21),

	/**
	 * @brief Default D++ intents (all non-privileged intents).
	 */
	i_default_intents		= dpp::i_guilds | dpp::i_guild_bans | dpp::i_guild_emojis | dpp::i_guild_integrations |
					dpp::i_guild_webhooks | dpp::i_guild_invites | dpp::i_guild_voice_states |
					dpp::i_guild_messages | dpp::i_guild_message_reactions | dpp::i_guild_message_typing |
					dpp::i_direct_messages | dpp::i_direct_message_typing | dpp::i_direct_message_reactions |
					dpp::i_guild_scheduled_events | dpp::i_auto_moderation_configuration |
					dpp::i_auto_moderation_execution,

	/**
	 * @brief Privileged intents requiring ID.
	 */
	i_privileged_intents		= dpp::i_guild_members | dpp::i_guild_presences | dpp::i_message_content,

	/**
	 * @brief Every single intent (dpp::i_default_intents and dpp::i_privileged_intents).
	 */
	i_all_intents			= dpp::i_default_intents | dpp::i_privileged_intents,

	/**
	 * @brief Unverified bots default intents.
	 */
        i_unverified_default_intents    = dpp::i_default_intents | dpp::i_message_content
};

} // namespace dpp
