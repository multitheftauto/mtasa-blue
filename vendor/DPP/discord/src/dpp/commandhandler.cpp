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
#include <dpp/commandhandler.h>
#include <dpp/cluster.h>
#include <dpp/exception.h>
#include <dpp/stringops.h>
#include <sstream>
#include <utility>

namespace dpp {

param_info::param_info(parameter_type t, bool o, const std::string &d, const std::map<command_value, std::string> &opts) : type(t), optional(o), description(d), choices(opts)
{
}

command_source::command_source(const struct message_create_t& event) : guild_id(event.msg.guild_id), channel_id(event.msg.channel_id), command_id(0), issuer(event.msg.author), message_event(event), interaction_event(std::nullopt)
{
}

command_source::command_source(const struct interaction_create_t& event) : guild_id(event.command.guild_id), channel_id(event.command.channel_id), command_id(event.command.id), command_token(event.command.token), issuer(event.command.usr), message_event(std::nullopt), interaction_event(event)
{
}

commandhandler::commandhandler(cluster* o, bool auto_hook_events, snowflake application_id) : slash_commands_enabled(false), owner(o), app_id(application_id)
{
	if (!application_id && o->me.id) {
		app_id = o->me.id;
	}
	if (auto_hook_events) {
		interactions = o->on_slashcommand([this](const dpp::slashcommand_t &event) {
			this->route(event);
		});
		messages = o->on_message_create([this](const dpp::message_create_t & event) {
			this->route(event);
		});
	} else {
		interactions = messages = 0;
	}

}

commandhandler& commandhandler::set_owner(cluster* o)
{
	owner = o;
	return *this;
}

commandhandler::~commandhandler()
{
	if (messages && interactions) {
		owner->on_message_create.detach(messages);
		owner->on_slashcommand.detach(interactions);
	}
}

commandhandler& commandhandler::add_prefix(const std::string &prefix)
{
	prefixes.emplace_back(prefix);
	if (prefix == "/") {
		/* Register existing slash commands */
		slash_commands_enabled = true;
	}
	return *this;
}

commandhandler& commandhandler::add_command(const std::string &command, const parameter_registration_t &parameters, command_handler handler, const std::string &description, snowflake guild_id)
{
	command_info_t i;
	i.func = std::move(handler);
	i.guild_id = guild_id;
	i.parameters = parameters;
	commands[lowercase(command)] = i;
	if (slash_commands_enabled) {
		if (this->app_id.empty()) {
			if (owner->me.id.empty()) {
				throw dpp::logic_exception(err_command_handler_not_ready, "Command handler not ready (i don't know my application ID)");
			} else {
				this->app_id = owner->me.id;
			}
		}
		dpp::slashcommand newcommand;
		/* Create a new global command on ready event */
		newcommand.set_name(lowercase(command)).set_description(description).set_application_id(this->app_id);

		for (auto& parameter : parameters) {
			command_option_type cot = co_string;
			switch (parameter.second.type) {
				case pt_boolean:
					cot = co_boolean;
				break;
				case pt_integer:
					cot = co_integer;
				break;
				case pt_string:
					cot = co_string;
				break;
				case pt_user:
					cot = co_user;
				break;
				case pt_role:
					cot = co_role;
				break;
				case pt_channel:
					cot = co_channel;
				break;
				case pt_double:
					cot = co_number;
				break;
			}

			command_option opt(cot, parameter.first, parameter.second.description, !parameter.second.optional);
			if (!parameter.second.choices.empty()) {
				for (auto& c : parameter.second.choices) {
					opt.add_choice(dpp::command_option_choice(c.second, c.first));
				}
			}
			newcommand.add_option(opt);
		}
		/* Register the command */
		if (guild_id) {
			if (bulk_registration_list_guild.find(guild_id) == bulk_registration_list_guild.end()) {
				bulk_registration_list_guild[guild_id] = {};
			}
			bulk_registration_list_guild[guild_id].emplace_back(newcommand);
		} else {
			bulk_registration_list_global.emplace_back(newcommand);
		}
	}
	return *this;
}

commandhandler& commandhandler::register_commands()
{
	for(auto & guild_commands : bulk_registration_list_guild) {
		owner->guild_bulk_command_create(guild_commands.second, guild_commands.first, [guild_commands, this](const dpp::confirmation_callback_t &callback) {
			if (callback.is_error()) {
				this->owner->log(dpp::ll_error, "Failed to register guild slash commands for guild id '" + std::to_string(guild_commands.first) + "': " + callback.http_info.body);
			}
		});
	}
	owner->global_bulk_command_create(bulk_registration_list_global, [this](const dpp::confirmation_callback_t &callback) {
		if (callback.is_error()) {
			this->owner->log(dpp::ll_error, "Failed to register global slash commands: " + callback.http_info.body);
		}
	});	
	return *this;
}

bool commandhandler::string_has_prefix(std::string &str)
{
	for (auto& p : prefixes) {
		size_t prefix_length = utility::utf8len(p);
		if (utility::utf8subview(str, 0, prefix_length) == p) {
			str.erase(str.begin(), str.begin() + prefix_length);
			return true;
		}
	}
	return false;
}

/* Note that message based command routing relies on cache to resolve ping types (e.g. user, channel ping).
 * There isn't really a way around this for many things because there is no 'resolved' member for it.
 * We only get resolved information for the user issuing the command.
 */
void commandhandler::route(const struct dpp::message_create_t& event)
{
	std::string msg_content = event.msg.content;
	if (string_has_prefix(msg_content)) {
		/* Put the string into stringstream to parse parameters at spaces.
		 * We use stringstream as it handles multiple spaces etc nicely.
		 */
		std::stringstream ss(msg_content);
		std::string command;
		ss >> command;
		/* Prefixed command, the prefix was removed */
		auto found_cmd = commands.find(lowercase(command));
		if (found_cmd != commands.end()) {
			/* Filter out guild specific commands that are not for the current guild */
			if (found_cmd->second.guild_id && found_cmd->second.guild_id != event.msg.guild_id) {
				return;
			}

			parameter_list_t call_params;

			/* Command found; parse parameters */
			for (auto& p : found_cmd->second.parameters) {
				command_parameter param;

				/* Check for end of stream */
				if (!ss) {
					/* If it's an optional param, we dont care */
					if (!p.second.optional) {
						/* Trigger missing parameter handler? */
					}
					break;
				}

				switch (p.second.type) {
					case pt_string: {
						std::string x;
						ss >> x;
						param = x;
					}
					break;
					case pt_role: {
						std::string x;
						ss >> x;
						if (x.length() > 4 && x[0] == '<' && x[1] == '&') {
							snowflake rid = from_string<uint64_t>(x.substr(2, x.length() - 1));
							role* r = dpp::find_role(rid);
							if (r) {
								param = *r;
							}
						}
					}
					break;
					case pt_channel: {
						std::string x;
						ss >> x;
						if (x.length() > 4 && x[0] == '<' && x[1] == '#') {
							snowflake cid = from_string<uint64_t>(x.substr(2, x.length() - 1));
							channel* c = dpp::find_channel(cid);
							if (c) {
								param = *c;
							}
						}
					}
					break;
					case pt_user: {
						std::string x;
						ss >> x;
						if (x.length() > 4 && x[0] == '<' && x[1] == '@') {
							snowflake uid = from_string<uint64_t>(x.substr(2, x.length() - 1));
							user* u = dpp::find_user(uid);
							if (u) {
								dpp::resolved_user m;
								m.user = *u;
								dpp::guild* g = dpp::find_guild(event.msg.guild_id);
								if (g->members.find(uid) != g->members.end()) {
									m.member = g->members[uid];
								}
								param = m;
							}

						}
					}
					break;
					case pt_integer: {
						int64_t x = 0;
						ss >> x;
						param = x;
					}
					break;
					case pt_double: {
						double x = 0;
						ss >> x;
						param = x;
					}
					break;
					case pt_boolean: {
						std::string x;
						bool y = false;
						ss >> x;
						x = lowercase(x);
						if (x == "yes" || x == "1" || x == "true") {
							y = true;
						}
						param = y;
					}
					break;
				}

				/* Add parameter to the list */
				call_params.emplace_back(p.first, param);
			}

			/* Call command handler */
			found_cmd->second.func(command, call_params, command_source(event));
		}
	}
}

void commandhandler::route(const struct slashcommand_t & event)
{
	/* We don't need to check for prefixes here, slash command interactions
	 * dont have prefixes at all.
	 */
	command_interaction cmd = std::get<command_interaction>(event.command.data);

	auto found_cmd = commands.find(lowercase(cmd.name));
	if (found_cmd != commands.end()) {
		/* Command found; parse parameters */
		parameter_list_t call_params;
		for (auto& p : found_cmd->second.parameters) {
			command_parameter param;
			const command_value& slash_parameter = event.get_parameter(p.first);
			dpp::command_resolved res = event.command.resolved;

			if (p.second.optional && slash_parameter.index() == 0 /* std::monostate */) {
				/* Missing optional parameter, skip this */
				continue;
			}
			
			switch (p.second.type) {
				case pt_string: {
					std::string s = std::get<std::string>(slash_parameter);
					param = s;
				}
				break;
				case pt_role: {
					snowflake rid = std::get<snowflake>(slash_parameter);
					role* r = dpp::find_role(rid);
					if (r) {
						/* Use cache if the role is in the cache */
						param = *r;
					} else {
						/* Otherwise use interaction resolved fields */
						if (res.roles.find(rid) != res.roles.end()) {
							param = res.roles[rid];
						}
					}
				}
				break;
				case pt_channel: {
					snowflake cid = std::get<snowflake>(slash_parameter);
					channel* c = dpp::find_channel(cid);
					if (c) {
						/* Use cache if the channel is in the cache */
						param = *c;
					} else {
						/* Otherwise use interaction resolved fields */
						if (res.channels.find(cid) != res.channels.end()) {
							param = res.channels[cid];
						}
					}
				}
				break;
				case pt_user: {
					snowflake uid = std::get<snowflake>(slash_parameter);
					/* TODO: Make this used resolved, not cache */
					user* u = dpp::find_user(uid);
					if (u) {
						/* Use the cache if the user is in the cache */
						dpp::resolved_user m;
						m.user = *u;
						dpp::guild* g = dpp::find_guild(event.command.guild_id);
						if (g->members.find(uid) != g->members.end()) {
							m.member = g->members[uid];
						}
						param = m;
					} else {
						/* Otherwise use interaction resolved fields */
						if (
							event.command.resolved.users.find(uid) != event.command.resolved.users.end()
							&&
							event.command.resolved.members.find(uid) != event.command.resolved.members.end()
						) {
							/* Fill in both member and user info */
							dpp::resolved_user m;
							m.member = res.members[uid];
							m.user = res.users[uid];
							param = m;
						}
					}
				}
				break;
				case pt_integer: {
					int64_t i = std::get<int64_t>(slash_parameter);
					param = i;
				}
				break;
				case pt_boolean: {
					bool b = std::get<bool>(slash_parameter);
					param = b;
				}
				break;
				case pt_double: {
					double b = std::get<double>(slash_parameter);
					param = b;
				}
				break;
			}

			/* Add parameter to the list */
			call_params.emplace_back(p.first, param);
		}

		/* Call command handler */
		found_cmd->second.func(cmd.name, call_params, command_source(event));
	}
}

void commandhandler::reply(const dpp::message &m, command_source source, command_completion_event_t callback)
{
	dpp::message msg = m;
	msg.owner = this->owner;
	msg.guild_id = source.guild_id;
	msg.channel_id = source.channel_id;
	if (!source.command_token.empty() && source.command_id) {
		owner->interaction_response_create(source.command_id, source.command_token, dpp::interaction_response(ir_channel_message_with_source, msg), callback);
	} else {
		owner->message_create(msg, callback);
	}
}

void commandhandler::thinking(command_source source, command_completion_event_t callback)
{
	dpp::message msg(this->owner);
	msg.content = "*";
	msg.guild_id = source.guild_id;
	msg.channel_id = source.channel_id;
	if (!source.command_token.empty() && source.command_id) {
		owner->interaction_response_create(source.command_id, source.command_token, dpp::interaction_response(ir_deferred_channel_message_with_source, msg), callback);
	}
}

void commandhandler::thonk(command_source source, command_completion_event_t callback)
{
	thinking(source, callback);
}

} // namespace dpp
