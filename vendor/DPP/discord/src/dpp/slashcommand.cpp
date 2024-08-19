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
#include <dpp/appcommand.h>
#include <dpp/discordevents.h>
#include <dpp/exception.h>
#include <dpp/json.h>
#include <dpp/stringops.h>
#include <dpp/cache.h>
#include <iostream>

namespace dpp {

using json = nlohmann::json;

slashcommand::slashcommand() : managed(), application_id(0), type(ctxm_chat_input), default_permission(true), version(1), default_member_permissions(p_use_application_commands), dm_permission(false), nsfw(false) {
}

slashcommand::slashcommand(const std::string &_name, const std::string &_description, const dpp::snowflake _application_id) : slashcommand() {
	set_name(_name);
	set_description(_description);
	set_application_id(_application_id);
}

slashcommand::slashcommand(const std::string &_name, const slashcommand_contextmenu_type _type, const dpp::snowflake _application_id): slashcommand() {
	set_name(_name);
	set_type(_type);
	set_application_id(_application_id);
}

slashcommand& slashcommand::set_dm_permission(bool dm) {
	dm_permission = dm;
	return *this;
}

slashcommand& slashcommand::set_nsfw(bool is_nsfw) {
	nsfw = is_nsfw;
	return *this;
}

slashcommand& slashcommand::set_default_permissions(uint64_t defaults) {
	default_member_permissions = defaults;
	return *this;
}

slashcommand& slashcommand::fill_from_json_impl(nlohmann::json* j) {
	id = snowflake_not_null(j, "id");
	name = string_not_null(j, "name");
	description = string_not_null(j, "description");
	version = snowflake_not_null(j, "version");
	application_id = snowflake_not_null(j, "application_id");
	// DEPRECATED
	// default_permission = bool_not_null(j, "default_permission");
	default_member_permissions = snowflake_not_null(j, "default_member_permissions");
	dm_permission = bool_not_null(j, "dm_permission");
	nsfw = bool_not_null(j, "nsfw");

	type = (slashcommand_contextmenu_type)int8_not_null(j, "type");
	set_object_array_not_null<command_option>(j, "options", options); // command_option fills recursive
	return *this;
}

void to_json(json& j, const command_option_choice& choice) {
	j["name"] = choice.name;
	if (std::holds_alternative<int64_t>(choice.value)) {
		j["value"] = std::get<int64_t>(choice.value);
	} else if (std::holds_alternative<bool>(choice.value)) {
		j["value"] = std::get<bool>(choice.value);
	} else if (std::holds_alternative<snowflake>(choice.value)) {
		j["value"] = std::to_string(std::get<snowflake>(choice.value));
	} else if (std::holds_alternative<double>(choice.value)) {
		j["value"] = std::to_string(std::get<double>(choice.value));
	} else {
		j["value"] = std::get<std::string>(choice.value);
	}
	if (choice.name_localizations.size()) {
		j["name_localizations"] = json::object();
		for(auto& loc : choice.name_localizations) {
			j["name_localizations"][loc.first] = loc.second;
		}
	}
}

command_option& command_option::set_min_value(command_option_range min_v) {
	min_value = min_v;
	return *this;
}

command_option& command_option::set_max_value(command_option_range max_v) {
	max_value = max_v;
	return *this;
}

command_option& command_option::set_min_length(command_option_range min_v) {
	min_value = min_v;
	return *this;
}

command_option& command_option::set_max_length(command_option_range max_v) {
	max_value = max_v;
	return *this;
}

void to_json(json& j, const command_option& opt) {
	j["name"] = opt.name;
	j["description"] = opt.description;
	j["type"] = opt.type;
	j["autocomplete"] = opt.autocomplete;
	j["required"] = opt.required;

	if (opt.name_localizations.size()) {
		j["name_localizations"] = json::object();
		for(auto& loc : opt.name_localizations) {
			j["name_localizations"][loc.first] = loc.second;
		}
	}
	if (opt.description_localizations.size()) {
		j["description_localizations"] = json::object();
		for(auto& loc : opt.description_localizations) {
			j["description_localizations"][loc.first] = loc.second;
		}
	}

	/* Check for minimum and maximum values */
	if (opt.type == dpp::co_string) {
		if (std::holds_alternative<int64_t>(opt.min_value)) {
			j["min_length"] = std::get<int64_t>(opt.min_value);
		}
		if (std::holds_alternative<int64_t>(opt.max_value)) {
			j["max_length"] = std::get<int64_t>(opt.max_value);
		}
	} else if (opt.type == dpp::co_integer) {
		if (std::holds_alternative<int64_t>(opt.min_value)) {
			j["min_value"] = std::get<int64_t>(opt.min_value);
		}
		if (std::holds_alternative<int64_t>(opt.max_value)) {
			j["max_value"] = std::get<int64_t>(opt.max_value);
		}
	} else if (opt.type == dpp::co_number) {
		if (std::holds_alternative<double>(opt.min_value)) {
			j["min_value"] = std::get<double>(opt.min_value);
		}
		if (std::holds_alternative<double>(opt.max_value)) {
			j["max_value"] = std::get<double>(opt.max_value);
		}
	}

	if (opt.options.size()) {
		j["options"] = json();

		for (const auto& opt : opt.options) {
			json jopt = opt;
			j["options"].push_back(jopt);
		}
	}

	if (opt.choices.size()) {
		j["choices"] = json();

		for (const auto& choice : opt.choices) {
			json jchoice = choice;
			j["choices"].push_back(jchoice);
		}
	}

	if (!opt.channel_types.empty()) {
		j["channel_types"] = json();

		for (const auto ch_type : opt.channel_types) {
			j["channel_types"].push_back(ch_type);
		}
	}
}

void to_json(nlohmann::json& j, const command_permission& cp) {
	j["id"] = std::to_string(cp.id);
	j["type"] = cp.type;
	j["permission"] =  cp.permission;
}

void to_json(nlohmann::json& j, const guild_command_permissions& gcp) {
	j["id"] = std::to_string(gcp.id);
	j["application_id"] = std::to_string(gcp.application_id);
	j["guild_id"] = std::to_string(gcp.guild_id);
	j["permissions"] =  gcp.permissions;
}

void to_json(json& j, const slashcommand& p) {
	j["name"] = p.name;

	if (p.type != ctxm_user && p.type != ctxm_message) { 
		j["description"] = p.description;
	}

	/* Only send this if set to something other than ctxm_none */
	if (p.type != ctxm_none) {
		j["type"] = p.type;
	}

	j["default_member_permissions"] = std::to_string(p.default_member_permissions);
	j["dm_permission"] = p.dm_permission;
	j["nsfw"] = p.nsfw;

	if (p.name_localizations.size()) {
		j["name_localizations"] = json::object();
		for(auto& loc : p.name_localizations) {
			j["name_localizations"][loc.first] = loc.second;
		}
	}

	if (p.type != ctxm_user && p.type != ctxm_message) {
		if (p.description_localizations.size()) {
			j["description_localizations"] = json::object();
			for(auto& loc : p.description_localizations) {
				j["description_localizations"][loc.first] = loc.second;
			}
		}

		if (p.options.size()) {
			j["options"] = json();

			for (const auto& opt : p.options) {
				json jopt = opt;
				j["options"].push_back(jopt);
			}
		}
	}

	if (p.permissions.size())  {
		j["permissions"] = json();

		for(const auto& perm : p.permissions) {
			json jperm = perm;
			j["permissions"].push_back(jperm);
		}
	}

	// DEPRECATED
	// j["default_permission"] = p.default_permission;
	j["application_id"] = std::to_string(p.application_id);
}

json slashcommand::to_json_impl(bool with_id) const {
	json j = *this;

	if (with_id) {
		j["id"] = std::to_string(id);
	}

	return j;
}

slashcommand& slashcommand::set_type(slashcommand_contextmenu_type t) {
	type = t;
	if (type == ctxm_chat_input) {
		name = lowercase(name);
	}
	return *this;
}

slashcommand& slashcommand::set_name(const std::string &n) {
	if (type == ctxm_chat_input) {
		name = lowercase(utility::utf8substr(n, 0, 32));
	} else {
		name = utility::utf8substr(n, 0, 32);
	}
	return *this;
}

slashcommand& slashcommand::set_description(const std::string &d) {
	description = utility::utf8substr(d, 0, 100);
	return *this;
}

slashcommand& slashcommand::set_application_id(snowflake i) {
	application_id = i;
	return *this;
}

slashcommand& slashcommand::add_permission(const command_permission& p) {
	this->permissions.emplace_back(p);
	return *this;
}

slashcommand& slashcommand::disable_default_permissions() {
	this->default_permission = false;
	return *this;
}

command_option_choice::command_option_choice(const std::string &n, command_value v) : name(n), value(v)
{
}

command_option_choice &command_option_choice::fill_from_json_impl(nlohmann::json *j) {
	name = string_not_null(j, "name");
	if ((*j)["value"].is_boolean()) { // is bool
		value.emplace<bool>((*j)["value"].get<bool>());
	} else if ((*j)["value"].is_number_float()) { // is double
		value.emplace<double>((*j)["value"].get<double>());
	} else if (((*j)["value"].is_string() && snowflake_not_null(j, "value") != 0)) { // is snowflake (string containing 64 bit integer)
		value.emplace<snowflake>(dpp::snowflake(snowflake_not_null(j, "value")));
	} else if ((*j)["value"].is_number_integer()) { // is int64
		value.emplace<int64_t>((*j)["value"].get<int64_t>());
	} else { // else string
		value.emplace<std::string>((*j)["value"].get<std::string>());
	}
	if (j->contains("name_localizations")) {
		for(auto loc = (*j)["name_localizations"].begin(); loc != (*j)["name_localizations"].end(); ++loc) {
			name_localizations[loc.key()] = loc.value().get<std::string>();;
		}
	}

	return *this;
}

command_option::command_option(command_option_type t, const std::string &n, const std::string &d, bool r) :
	type(t), name(n), description(d), required(r), focused(false), autocomplete(false)
{
	if (std::any_of(n.begin(), n.end(), [](unsigned char c){ return std::isupper(c); })) {
		throw dpp::logic_exception(err_command_has_caps, "Command options can not contain capital letters in the name of the option.");
	}
}

command_option& command_option::add_choice(const command_option_choice &o)
{
	if (this->autocomplete) {
		throw dpp::logic_exception(err_choice_autocomplete, "Can't set autocomplete=true if choices exist in the command_option");
	}
	choices.emplace_back(o);
	return *this;
}

command_option& command_option::add_option(const command_option &o)
{
	options.emplace_back(o);
	return *this;
}

command_option& command_option::add_channel_type(const channel_type ch)
{
	this->channel_types.emplace_back(ch);
	return *this;
}

command_option& command_option::set_auto_complete(bool autocomp)
{
	if (autocomp && !choices.empty()) {
		throw dpp::logic_exception(err_choice_autocomplete, "Can't set autocomplete=true if choices exist in the command_option");
	}
	this->autocomplete = autocomp;
	return *this;
}

command_option &command_option::fill_from_json_impl(nlohmann::json *j) {
	/*
	* Command options contains command options. Therefor the object is filled with recursion.
	*/
	std::function<void(nlohmann::json *, command_option &, uint8_t)> fill = [&fill](nlohmann::json *j, command_option &o, uint8_t depth) {
		o.type = (command_option_type)int8_not_null(j, "type");
		o.name = string_not_null(j, "name");
		o.description = string_not_null(j, "description");
		o.required = bool_not_null(j, "required");
		set_object_array_not_null<command_option_choice>(j, "choices", o.choices);

		if (j->contains("name_localizations")) {
			for(auto loc = (*j)["name_localizations"].begin(); loc != (*j)["name_localizations"].end(); ++loc) {
				o.name_localizations[loc.key()] = loc.value().get<std::string>();;
			}
		}
		if (j->contains("description_localizations")) {
			for(auto loc = (*j)["description_localizations"].begin(); loc != (*j)["description_localizations"].end(); ++loc) {
				o.description_localizations[loc.key()] = loc.value().get<std::string>();
			}
		}

		if (j->contains("options") && depth < 3) { // maximum amount of nested options. fixed to 3 levels: subcommand group -> subcommand -> its options
			for (auto &joption : (*j)["options"]) {
				command_option p;
				fill(&joption, p, depth + 1);
				o.options.push_back(p);
			}
		}

		if (j->contains("channel_types")) {
			for (auto& jtype : (*j)["channel_types"]) {
				o.channel_types.push_back( (channel_type)jtype.get<int8_t>());
			}
		}
		if (j->contains("min_value")) {
			if ((*j)["min_value"].is_number_integer()) {
				o.min_value.emplace<int64_t>(int64_not_null(j, "min_value"));
			} else if ((*j)["min_value"].is_number()) {
				o.min_value.emplace<double>(double_not_null(j, "min_value"));
			}
		}
		if (j->contains("max_value")) {
			if ((*j)["max_value"].is_number_integer()) {
				o.max_value.emplace<int64_t>(int64_not_null(j, "max_value"));
			} else if ((*j)["max_value"].is_number()) {
				o.max_value.emplace<double>(double_not_null(j, "max_value"));
			}
		}
		if (j->contains("min_length")) {
			o.min_value.emplace<int64_t>(int32_not_null(j, "min_length"));
		}
		if (j->contains("max_length")) {
			o.max_value.emplace<int64_t>(int32_not_null(j, "max_length"));
		}
		o.autocomplete = bool_not_null(j, "autocomplete");
	};

	fill(j, *this, 0);

	return *this;
}


slashcommand& slashcommand::add_option(const command_option &o)
{
	options.emplace_back(o);
	return *this;
}

interaction::interaction() : application_id(0), type(0), guild_id(0), channel_id(0), message_id(0), version(0), cache_policy(cache_policy::cpol_default) {
}

command_interaction interaction::get_command_interaction() const {
	if (std::holds_alternative<command_interaction>(data)) {
		return std::get<command_interaction>(data);
	} else {
		throw dpp::logic_exception(err_interaction, "Interaction is not for a command");
	}
}

component_interaction interaction::get_component_interaction() const {
	if (std::holds_alternative<component_interaction>(data)) {
		return std::get<component_interaction>(data);
	} else {
		throw dpp::logic_exception(err_interaction, "Interaction is not for a component");
	}
}

autocomplete_interaction interaction::get_autocomplete_interaction() const {
	if (std::holds_alternative<autocomplete_interaction>(data)) {
		return std::get<autocomplete_interaction>(data);
	} else {
		throw dpp::logic_exception(err_interaction, "Interaction is not for an autocomplete");
	}
}

std::string interaction::get_command_name() const {
	try {
		return get_command_interaction().name;
	}
	catch (dpp::logic_exception&) {
		return std::string();
	}
}

interaction& interaction::fill_from_json_impl(nlohmann::json* j) {
	j->get_to(*this);
	return *this;
}

json interaction::to_json_impl(bool with_id) const {
	/* There is no facility to build the json of an interaction as bots don't send them, only the API sends them as an event payload */
	return "";
}

slashcommand& slashcommand::add_localization(const std::string& language, const std::string& _name, const std::string& _description) {
	name_localizations[language] = _name;
	if (! _description.empty()) {
		description_localizations[language] = _description;
	}
	return *this;
}

command_option_choice& command_option_choice::add_localization(const std::string& language, const std::string& _name) {
	name_localizations[language] = _name;
	return *this;
}

command_option& command_option::add_localization(const std::string& language, const std::string& _name, const std::string& _description) {
	name_localizations[language] = _name;
	if (! _description.empty()) {
		description_localizations[language] = _description;
	}
	return *this;
}

void from_json(const nlohmann::json& j, command_data_option& cdo) {
	cdo.name = string_not_null(&j, "name");
	cdo.type = (command_option_type)int8_not_null(&j, "type");

	if (j.contains("options") && !j.at("options").is_null()) {
		j.at("options").get_to(cdo.options);
	}

	/* If there's a focused, define it */
	if (j.contains("focused") && !j.at("focused").is_null()) {
		cdo.focused = bool_not_null(&j, "focused");
	}

	if (j.contains("value") && !j.at("value").is_null()) {
		switch (cdo.type) {
			case co_boolean:
				if(j.at("value").is_boolean()) {
					cdo.value = j.at("value").get<bool>();
				}
				break;
			case co_channel:
			case co_role:
			case co_attachment:
			case co_user:
			case co_mentionable:
				cdo.value = dpp::snowflake(snowflake_not_null(&j, "value"));
				break;
			case co_integer:
				if(j.at("value").is_number_integer()) {
					cdo.value = j.at("value").get<int64_t>();
				}
				break;
			case co_string:
				if(j.at("value").is_string()) {
					cdo.value = j.at("value").get<std::string>();
				}
				break;
			case co_number:
				if(j.at("value").is_number_float()) {
					cdo.value = j.at("value").get<double>();
				}
				break;
			case co_sub_command:
			case co_sub_command_group:
			break;
		}
	}
}

void from_json(const nlohmann::json& j, command_interaction& ci) {
	ci.id = snowflake_not_null(&j, "id");
	ci.name = string_not_null(&j, "name");
	ci.type = (dpp::slashcommand_contextmenu_type)int8_not_null(&j, "type");
	ci.target_id = snowflake_not_null(&j, "target_id");

	if (j.contains("options") && !j.at("options").is_null()) {
		j.at("options").get_to(ci.options);
	}
}

void from_json(const nlohmann::json& j, component_interaction& bi) {
	bi.component_type = int8_not_null(&j, "component_type");
	bi.custom_id = string_not_null(&j, "custom_id");
	if ((bi.component_type == cot_selectmenu || bi.component_type == cot_user_selectmenu ||
		 bi.component_type == cot_role_selectmenu || bi.component_type == cot_mentionable_selectmenu ||
		 bi.component_type == cot_channel_selectmenu) &&
		j.find("values") != j.end()) {
		/* Get values */
		for (auto& entry : j["values"]) {
			bi.values.push_back(entry.get<std::string>());
		}
	}
}

void from_json(const nlohmann::json& j, autocomplete_interaction& ai) {
	ai.id = snowflake_not_null(&j, "id");
	ai.name = string_not_null(&j, "name");
	ai.type = (dpp::slashcommand_contextmenu_type)int8_not_null(&j, "type");
	ai.target_id = snowflake_not_null(&j, "target_id");

	if (j.contains("options") && !j.at("options").is_null()) {
		j.at("options").get_to(ai.options);
	}
}

void from_json(const nlohmann::json& j, interaction& i) {
	i.id = snowflake_not_null(&j, "id");
	i.locale = string_not_null(&j, "locale");
	i.guild_locale = string_not_null(&j, "guild_locale");
	i.application_id = snowflake_not_null(&j, "application_id");
	i.channel_id = snowflake_not_null(&j, "channel_id");
	i.guild_id = snowflake_not_null(&j, "guild_id");
	i.app_permissions = snowflake_not_null(&j, "app_permissions");

	if (j.contains("channel") && !j.at("channel").is_null()) {
		const json& c = j["channel"];
		i.channel = channel().fill_from_json(const_cast<json*>(&c));
	}

	if (j.contains("message") && !j.at("message").is_null()) {
		const json& m = j["message"];
		i.msg = message().fill_from_json(const_cast<json*>(&m), i.cache_policy);
		set_snowflake_not_null(&m, "id", i.message_id);
	}

	i.type = int8_not_null(&j, "type");
	i.token = string_not_null(&j, "token");
	i.version = int8_not_null(&j, "version");
	if (j.contains("member") && !j.at("member").is_null()) {
		/* Command invoked from a guild */
		if (j.at("member").contains("user") && !j.at("member").at("user").is_null()) {
			j.at("member").at("user").get_to(i.usr);
			/* Caching is on; store user if needed */
			if (i.cache_policy.user_policy != dpp::cp_none) {
				user* check = dpp::find_user(i.usr.id);
				if (!check && i.usr.id) {
					/* User does not exist yet, cache the partial as a user record */
					check = new user();
					*check = i.usr;
					dpp::get_user_cache()->store(check);
				}
			}
		}
		j.at("member").get_to(i.member);
		i.member.user_id = i.usr.id;
		i.member.guild_id = i.guild_id;
		if (i.cache_policy.user_policy != dpp::cp_none) {
			/* User caching on, lazy or aggressive - cache or update the member information */
			guild* g = dpp::find_guild(i.guild_id);
			if (g) {
				g->members[i.member.user_id] = i.member;
			}
		}
		/* store the included permissions of this member in the resolved set */
		if (j.at("member").contains("permissions") && !j.at("member").at("permissions").is_null()) {
			dpp::snowflake id(i.member.user_id);
			if (id) {
				i.resolved.member_permissions[id] = snowflake_not_null(&(j.at("member")), "permissions");
			}
		}
	} else if (j.contains("user") && !j.at("user").is_null()) {
		/* Command invoked from a DM */
		j.at("user").get_to(i.usr);
		i.member.user_id = i.usr.id;
		i.member.guild_id = 0;
	}

	if (j.contains("data") && !j.at("data").is_null()) {

		const json& data = j["data"];

		/* Deal with 'resolved' data, e.g. users, members, roles, channels */
		if (data.find("resolved") != data.end()) {
			const json& d_resolved = data["resolved"];
			/* Users */
			if (d_resolved.contains("users")) {
				for (auto v = d_resolved["users"].begin(); v != d_resolved["users"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.users[id] = dpp::user().fill_from_json(&f);
				}
			}
			/* Roles */
			if (d_resolved.contains("roles")) {
				for (auto v = d_resolved["roles"].begin(); v != d_resolved["roles"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.roles[id] = dpp::role().fill_from_json(i.guild_id, &f);
				}
			}
			/* Attachments */
			if (d_resolved.contains("attachments")) {
				for (auto v = d_resolved["attachments"].begin(); v != d_resolved["attachments"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.attachments.emplace(id, dpp::attachment(nullptr, &f));
				}
			}
			/* Channels */
			if (d_resolved.contains("channels")) {
				for (auto v = d_resolved["channels"].begin(); v != d_resolved["channels"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.channels[id] = dpp::channel().fill_from_json(&f);
				}
			}
			/* Members */
			if (d_resolved.contains("members")) {
				for (auto v = d_resolved["members"].begin(); v != d_resolved["members"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.members[id] = dpp::guild_member().fill_from_json(&f, i.guild_id, id);
					if (f.contains("permissions")) {
						i.resolved.member_permissions[id] = snowflake_not_null(&f, "permissions");
					}
				}
			}
			/* Messages */
			if (d_resolved.contains("messages")) {
				for (auto v = d_resolved["messages"].begin(); v != d_resolved["messages"].end(); ++v) {
					json f = *v;
					dpp::snowflake id(v.key());
					i.resolved.messages[id] = dpp::message().fill_from_json(&f);
				}
			}
		}


		if (i.type == it_application_command) {
			command_interaction ci;
			j.at("data").get_to(ci);
			i.data = ci;
		} else if (i.type == it_component_button) {
			component_interaction bi;
			j.at("data").get_to(bi);
			i.data = bi;
		} else if (i.type == it_autocomplete) {
			autocomplete_interaction ai;
			j.at("data").get_to(ai);
			i.data = ai;
		}
	}

	if(j.contains("entitlements")) {
		for (auto& entitle : j["entitlements"]) {
			i.entitlements.emplace_back(entitlement().fill_from_json(const_cast<json*>(&entitle)));
		}
	}
}

interaction_response& interaction_response::add_autocomplete_choice(const command_option_choice& achoice) {
	if (autocomplete_choices.size() < AUTOCOMPLETE_MAX_CHOICES) {
		this->autocomplete_choices.emplace_back(achoice);
	}
	return *this;
}


interaction_response::interaction_response(interaction_response_type t, const message& m) : type{t}, msg{m} {}

interaction_response::interaction_response(interaction_response_type t, message&& m) : type{t}, msg{m} {}

interaction_response::interaction_response(interaction_response_type t) : interaction_response() {
	type = t;
}

interaction_response& interaction_response::fill_from_json_impl(nlohmann::json* j) {
	type = (interaction_response_type)int8_not_null(j, "type");
	if (j->contains("data")) {
		msg.fill_from_json(&((*j)["data"]));
	}
	return *this;
}

interaction_modal_response& interaction_modal_response::fill_from_json_impl(nlohmann::json* j) {
	type = (interaction_response_type)int8_not_null(j, "type");
	json& d = (*j)["data"];
	custom_id = string_not_null(&d, "custom_id");
	title = string_not_null(&d, "title");
	if (d.contains("components")) {
		components.clear();
		for (auto& c : d["components"]) {
			auto row = dpp::component().fill_from_json(&c);
			if (!row.components.empty()) {
				components.push_back(row.components);
			}
		}
	}
	return *this;
}

json interaction_response::to_json_impl(bool with_id) const {
	json j;
	j["type"] = this->type;
	if (this->autocomplete_choices.empty()) {
		json msg_json = msg.to_json(false, true);
		auto cid = msg_json.find("channel_id");
		if (cid != msg_json.end()) {
			msg_json.erase(cid);
		}
		j["data"] = msg_json;
	} else {
		j["data"] = json::object();
		j["data"]["choices"] = json::array();
		for (auto & c : this->autocomplete_choices) {
			json opt = c;
			j["data"]["choices"].push_back(opt);
		}
	}
	return j;
}

/* NOTE: Forward declaration for internal function actually defined in message.cpp */
void to_json(json& j, const component& cp);

interaction_modal_response::interaction_modal_response() : interaction_response(ir_modal_dialog), current_row(0) {
	// Default to one empty row
	components.push_back({});
}

interaction_modal_response::interaction_modal_response(const std::string& _custom_id, const std::string& _title, const std::vector<component> _components) : 
	interaction_response(ir_modal_dialog),
	current_row(0), custom_id(_custom_id),
	title(dpp::utility::utf8substr(_title, 0, 45)) {
	// Default to one empty row
	components.push_back(_components);
}

json interaction_modal_response::to_json_impl(bool with_id) const {
	json j;
	j["type"] = this->type;
	j["data"] = json::object();
	j["data"]["custom_id"] = this->custom_id;
	j["data"]["title"] = this->title;
	j["data"]["components"] = json::array();
	for (auto & row : components) {
		json n;
		n["type"] = cot_action_row;
		n["components"] = json::array();
		for (auto & component : row) {
			json sn = component;
			n["components"].push_back(sn);
		}
		j["data"]["components"].push_back(n);
	}
	return j;
}

interaction_modal_response& interaction_modal_response::add_component(const component& c) {
	components[current_row].push_back(c);
	return *this;
}

interaction_modal_response& interaction_modal_response::add_row() {
	if (components.size() < 5) {
		current_row++;
		components.push_back({});
	} else {
		throw dpp::logic_exception(err_too_many_component_rows, "A modal dialog can only have a maximum of five component rows");
	}
	return *this;
}

interaction_modal_response& interaction_modal_response::set_custom_id(const std::string& _custom_id) {
	custom_id = _custom_id;
	return *this;
}

interaction_modal_response& interaction_modal_response::set_title(const std::string& _title) {
	title = _title;
	return *this;
}

command_permission::command_permission(snowflake id, const command_permission_type t, bool permission) :
	id(id), type(t), permission(permission) {
}

command_permission& command_permission::fill_from_json_impl(nlohmann::json* j) {
	id = snowflake_not_null(j, "id");
	type = (command_permission_type)int8_not_null(j, "type");
	permission = bool_not_null(j, "permission");
	return *this;
}

guild_command_permissions::guild_command_permissions() : id(0), application_id(0), guild_id(0)
{
}

guild_command_permissions &guild_command_permissions::fill_from_json_impl(nlohmann::json *j) {
	id = snowflake_not_null(j, "id");
	application_id = snowflake_not_null(j, "application_id");
	guild_id = snowflake_not_null(j, "guild_id");
	set_object_array_not_null<command_permission>(j, "permissions", permissions);

	return *this;
}

const dpp::user& interaction::get_resolved_user(snowflake id) const {
	return get_resolved<user, std::map<snowflake, user>>(id, resolved.users);
}

const dpp::role& interaction::get_resolved_role(snowflake id) const {
	return get_resolved<role, std::map<snowflake, role>>(id, resolved.roles);
}

const dpp::channel& interaction::get_resolved_channel(snowflake id) const {
	return get_resolved<dpp::channel, std::map<snowflake, dpp::channel>>(id, resolved.channels);
}

const dpp::guild_member& interaction::get_resolved_member(snowflake id) const {
	return get_resolved<guild_member, std::map<snowflake, guild_member>>(id, resolved.members);
}

const dpp::permission& interaction::get_resolved_permission(snowflake id) const {
	return get_resolved<permission, std::map<snowflake, permission>>(id, resolved.member_permissions);
}

const dpp::message& interaction::get_resolved_message(snowflake id) const {
	return get_resolved<message, std::map<snowflake, message>>(id, resolved.messages);
}

const dpp::attachment& interaction::get_resolved_attachment(snowflake id) const {
	return get_resolved<attachment, std::map<snowflake, attachment>>(id, resolved.attachments);
}

const dpp::user& interaction::get_issuing_user() const {
	return usr;
}

const dpp::message& interaction::get_context_message() const {
	return msg;
}

const dpp::channel& interaction::get_channel() const {
	auto c = find_channel(channel_id);
	if (c == nullptr) {
		throw dpp::logic_exception(err_unknown_channel, "No channel for this command interaction");
	}
	return *c;
}

const dpp::guild& interaction::get_guild() const {
	auto g = find_guild(guild_id);
	if (g == nullptr) {
		throw dpp::logic_exception(err_unknown_guild, "No guild for this command interaction");
	}
	return *g;
}

std::string command_interaction::get_mention() const {
	return dpp::utility::slashcommand_mention(id, name);
}

std::string slashcommand::get_mention() const {
	return dpp::utility::slashcommand_mention(id, name);
}
} // namespace dpp
