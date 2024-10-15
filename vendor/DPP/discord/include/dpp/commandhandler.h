/************************************************************************************
 *
 * D++, A Lightweight C++ library for Discord
 *
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
#include <dpp/misc-enum.h>
#include <dpp/user.h>
#include <dpp/guild.h>
#include <dpp/role.h>
#include <dpp/appcommand.h>
#include <dpp/dispatcher.h>
#include <dpp/utility.h>
#include <dpp/json_fwd.h>
#include <dpp/event_router.h>
#include <unordered_map>
#include <vector>
#include <functional>
#include <variant>

namespace dpp {

/**
 * @brief dpp::resolved_user contains both a dpp::guild_member and a dpp::user.
 * The user can be used to obtain in-depth user details such as if they are nitro,
 * and the guild member information to check their roles on a guild etc.
 * The Discord API provides both if a parameter is a user ping,
 * so we offer both in a combined structure.
 */
struct DPP_EXPORT resolved_user {
	/**
	 * @brief Holds user information
	 */
	dpp::user user;

	/**
	 * @brief Holds member information
	 */
	dpp::guild_member member;
};

/**
 * @brief Represents a received parameter.
 * We use variant so that multiple non-related types can be contained within.
 */
typedef std::variant<std::monostate, std::string, dpp::role, dpp::channel, dpp::resolved_user, int64_t, bool, double> command_parameter;

/**
 * @brief Parameter types when registering a command.
 * We don't pass these in when triggering the command in the handler, because it is
 * expected the developer added the command so they know what types to expect for each named
 * parameter.
 */
enum parameter_type {
	/**
	 * @brief String parameter.
	 */
	pt_string,

	/**
	 * @brief Role object parameter.
	 */
	pt_role,

	/**
	 * @brief Channel object parameter.
	 */
	pt_channel,

	/**
	 * @brief User object parameter.
	 */
	pt_user,

	/**
	 * @brief 64 bit signed integer parameter.
	 */
	pt_integer,

	/**
	 * @brief double floating point parameter.
	 */
	pt_double,

	/**
	 * @brief Boolean parameter.
	 */
	pt_boolean
};

/**
 * @brief Details of a command parameter used in registration.
 * Note that for non-slash commands optional parameters can only be at the end of
 * the list of parameters.
 */
struct DPP_EXPORT param_info {
	/**
	 * @brief Type of parameter
	 */
	parameter_type type;

	/**
	 * @brief True if the parameter is optional.
	 * For non-slash commands optional parameters may only be on the end of the list.
	 */
	bool optional;

	/**
	 * @brief Description of command. Displayed only for slash commands
	 */
	std::string description;

	/**
	 * @brief Allowed multiple choice options.
	 * The key name is the string passed to the command handler
	 * and the key value is its description displayed to the user.
	 */
	std::map<command_value, std::string> choices;

	/**
	 * @brief Construct a new param_info object
	 * 
	 * @param t Type of parameter
	 * @param o True if parameter is optional
	 * @param description The parameter description
	 * @param opts The options for a multiple choice parameter
	 */
	param_info(parameter_type t, bool o, const std::string &description, const std::map<command_value, std::string> &opts = {});
};

/**
 * @brief Parameter list used during registration.
 * Note that use of vector/pair is important here to preserve parameter order,
 * as opposed to unordered_map (which doesn't guarantee any order at all) and 
 * std::map, which reorders keys alphabetically.
 */
typedef std::vector<std::pair<std::string, param_info>> parameter_registration_t;

/**
 * @brief Parameter list for a called command.
 * See dpp::parameter_registration_t for an explanation as to why vector is used.
 */
typedef std::vector<std::pair<std::string, command_parameter>> parameter_list_t;

/**
 * @brief Represents the sending source of a command.
 * This is passed to any command handler and should be passed back to
 * commandhandler::reply(), allowing the reply method to route any replies back
 * to the origin, which may be a slash command or a message. Both require different
 * response facilities but we want this to be transparent if you use the command
 * handler class.
 * @deprecated commandhandler and message commands are deprecated and dpp::slashcommand is encouraged as a replacement.
 */
struct DPP_EXPORT command_source {
	/**
	 * @brief Sending guild id
	 */
	snowflake guild_id;

	/**
	 * @brief Source channel id
	 */
	snowflake channel_id;

	/**
	 * @brief Command ID of a slash command
	 */
	snowflake command_id;

	/**
	 * @brief Token for sending a slash command reply
	 */
	std::string command_token;

	/**
	 * @brief The user who issued the command
	 */
	user issuer;

	/**
	 * @brief Copy of the underlying message_create_t event, if it was a message create event
	 */
	std::optional<message_create_t> message_event;

	/**
	 * @brief Copy of the underlying interaction_create_t event, if it was an interaction create event
	 */
	std::optional<interaction_create_t> interaction_event;

	/**
	 * @brief Construct a command_source object from a message_create_t event
	 */
	command_source(const struct message_create_t& event);

	/**
	 * @brief Construct a command_source object from an interaction_create_t event
	 */
	command_source(const struct interaction_create_t& event);
};

/**
 * @brief The function definition for a command handler. Expects a command name string,
 * and a list of command parameters.
 * @deprecated commandhandler and message commands are deprecated and dpp::slashcommand is encouraged as a replacement.
 */
typedef std::function<void(const std::string&, const parameter_list_t&, command_source)> command_handler;

/**
 * @brief Represents the details of a command added to the command handler class.
 * @deprecated commandhandler and message commands are deprecated and dpp::slashcommand is encouraged as a replacement.
 */
struct DPP_EXPORT command_info_t {
	/**
	 * @brief Function reference for the handler. This is std::function so it can represent
	 * a class member, a lambda or a raw C function pointer.
	 */
	command_handler func;

	/**
	 * @brief Parameters requested for the command, with their types
	 */
	parameter_registration_t parameters;

	/**
	 * @brief Guild ID the command exists on, or 0 to be present on all guilds
	 */
	snowflake guild_id;
};


/**
 * @brief The commandhandler class represents a group of commands, prefixed or slash commands with handling functions.
 * 
 * It can automatically register slash commands, and handle routing of messages and interactions to separated command handler
 * functions.
 * @deprecated commandhandler and message commands are deprecated and dpp::slashcommand is encouraged as a replacement.
 */
class DPP_EXPORT commandhandler {
private:
	/**
	 * @brief List of guild commands to bulk register
	 */
	std::map<dpp::snowflake, std::vector<dpp::slashcommand>> bulk_registration_list_guild;

	/**
	 * @brief List of global commands to bulk register
	 */
	std::vector<dpp::slashcommand> bulk_registration_list_global;
public:
	/**
	 * @brief Commands in the handler
	 */
	std::unordered_map<std::string, command_info_t> commands;

	/**
	 * @brief Valid prefixes
	 */
	std::vector<std::string> prefixes;

	/**
	 * @brief Set to true automatically if one of the prefixes added is "/"
	 */
	bool slash_commands_enabled;

	/**
	 * @brief Cluster we are attached to for issuing REST calls
	 */
	class cluster* owner;

	/**
	 * @brief Application ID
	 */
	snowflake app_id;

	/**
	 * @brief Interaction event handle
	 */
	event_handle interactions;

	/**
	 * @brief Message event handle
	 */
	event_handle messages;

	/**
	 * @brief Returns true if the string has a known prefix on the start.
	 * Modifies string to remove prefix if it returns true.
	 * 
	 * @param str String to check and modify
	 * @return true string contained a prefix, prefix removed from string
	 * @return false string did not contain a prefix
	 */
	bool string_has_prefix(std::string &str);

public:

	/**
	 * @brief Construct a new commandhandler object
	 * 
	 * @param o Owning cluster to attach to
	 * @param auto_hook_events Set to true to automatically hook the on_slashcommand
	 * and on_message events. You should not need to set this to false unless you have a specific
	 * use case, as D++ supports multiple listeners to an event, so will allow the commandhandler
	 * to hook to your command events without disrupting other uses for the events you may have.
	 * @param application_id The application id of the bot. If not specified, the class will
	 * look within the cluster object and use cluster::me::id instead.
	 */
	commandhandler(class cluster* o, bool auto_hook_events = true, snowflake application_id = 0);

	/**
	 * @brief Destroy the commandhandler object
	 */
	~commandhandler();

	/**
	 * @brief Set the application id after construction
	 * 
	 * @param o Owning cluster to attach to
	 */
	commandhandler& set_owner(class cluster* o);

	/**
	 * @brief Add a prefix to the command handler
	 * 
	 * @param prefix Prefix to be handled by the command handler
	 * @return commandhandler& reference to self
	 */
	commandhandler& add_prefix(const std::string &prefix);

	/**
	 * @brief Add a command to the command handler
	 * 
	 * @param command Command to be handled.
	 * Note that if any one of your prefixes is "/" this will attempt to register
	 * a global command using the API and you will receive notification of this command
	 * via an interaction event.
	 * @param handler Handler function
	 * @param parameters Parameters to use for the command
	 * @param description The description of the command, shown for slash commands
	 * @param guild_id The guild ID to restrict the command to. For slash commands causes registration of a guild command as opposed to a global command.
	 * @return commandhandler& reference to self
	 * @throw dpp::logic_exception if application ID cannot be determined
	 */
	commandhandler& add_command(const std::string &command, const parameter_registration_t &parameters, command_handler handler, const std::string &description = "", snowflake guild_id = 0);

	/**
	 * @brief Register all slash commands with Discord
	 * This method must be called at least once  if you are using the "/" prefix to mark the
	 * end of commands being added to the handler. Note that this uses bulk registration and will replace any
	 * existing slash commands.
	 * 
	 * Note that if you have previously registered your commands and they have not changed, you do
	 * not need to call this again. Discord retains a cache of previously added commands.
	 * 
	 * @return commandhandler& Reference to self for chaining method calls
	 */
	commandhandler& register_commands();

	/**
	 * @brief Route a command from the on_message_create function.
	 * Call this method from within your on_message_create with the received
	 * dpp::message object if you have disabled automatic registration of events.
	 * 
	 * @param event message create event to parse
	 */
	void route(const struct dpp::message_create_t& event);

	/**
	 * @brief Route a command from the on_slashcommand function.
	 * Call this method from your on_slashcommand with the received
	 * dpp::interaction_create_t object if you have disabled automatic registration of events.
	 * 
	 * @param event command interaction event to parse
	 */
	void route(const struct slashcommand_t & event);

	/**
	 * @brief Reply to a command.
	 * You should use this method rather than cluster::message_create as
	 * the way you reply varies between slash commands and message commands.
	 * Note you should ALWAYS reply. Slash commands will emit an ugly error
	 * to the user if you do not emit some form of reply within 3 seconds.
	 * 
	 * @param m message to reply with.
	 * @param source source of the command
	 * @param callback User function to execute when the api call completes.
	 */
	void reply(const dpp::message &m, command_source source, command_completion_event_t callback = utility::log_error());

	/**
	 * @brief Reply to a command without a message, causing the discord client
	 * to display "Bot name is thinking...".
	 * The "thinking" message will persist for a maximum of 15 minutes.
	 * This counts as a reply for a slash command. Slash commands will emit an
	 * ugly error to the user if you do not emit some form of reply within 3
	 * seconds.
	 * 
	 * @param source source of the command
	 * @param callback User function to execute when the api call completes.
	 */
	void thinking(command_source source, command_completion_event_t callback = utility::log_error());

	/**
	 * @brief Easter egg (redefinition of dpp::commandhandler::thinking).
	 */
	void thonk(command_source source, command_completion_event_t callback = utility::log_error());

};

} // namespace dpp
