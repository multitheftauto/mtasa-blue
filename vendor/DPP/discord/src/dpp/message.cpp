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
#include <algorithm>
#include <dpp/message.h>
#include <dpp/user.h>
#include <dpp/channel.h>
#include <dpp/guild.h>
#include <dpp/cache.h>
#include <dpp/json.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/exception.h>
#include <dpp/cluster.h>



namespace dpp {

using json = nlohmann::json;

component::component() :
	type(cot_action_row), label(""), style(cos_primary), custom_id(""),
	min_values(-1), max_values(-1), min_length(0), max_length(0), disabled(false), required(false)
{
	emoji.animated = false;
	emoji.id = 0;
	emoji.name = "";
}


component& component::fill_from_json_impl(nlohmann::json* j) {
	type = static_cast<component_type>(int8_not_null(j, "type"));
	label = string_not_null(j, "label");
	custom_id = string_not_null(j, "custom_id");
	disabled = bool_not_null(j, "disabled");
	placeholder = string_not_null(j, "placeholder");
	if (j->contains("min_values") && j->at("min_values").is_number_integer()) {
		min_values = j->at("min_values").get<int32_t>();
	}
	if (j->contains("max_values") && j->at("max_values").is_number_integer()) {
		max_values = j->at("max_values").get<int32_t>();
	}
	if (j->contains("default_values") && !j->at("default_values").is_null()) {
		for (auto const &v : j->at("default_values")) {
			component_default_value d;
			d.id = snowflake_not_null(&v, "id");
			d.type = static_cast<component_default_value_type>(int8_not_null(&v, "type"));
			default_values.push_back(d);
		}
	}
	if (type == cot_action_row) {
		set_object_array_not_null<component>(j, "components", components);
	} else if (type == cot_button) { // button specific fields
		style = static_cast<component_style>(int8_not_null(j, "style"));
		url = string_not_null(j, "url");
		if (j->contains("emoji")) {
			json emo = (*j)["emoji"];
			emoji.id = snowflake_not_null(&emo, "id");
			emoji.name = string_not_null(&emo, "name");
			emoji.animated = bool_not_null(&emo, "animated");
		}
	} else if (type == cot_selectmenu) { // string select menu specific fields
		set_object_array_not_null<select_option>(j, "options", options);
	} else if (type == cot_channel_selectmenu) { // channel select menu specific fields
		if (j->contains("channel_types")) {
			for (json &ct : (*j)["channel_types"]) {
				if (ct.is_number_integer()) {
					channel_types.push_back(ct.get<dpp::channel_type>());
				}
			}
		}
	} else if (type == cot_text) { // text inputs (modal) specific fields
		text_style = static_cast<text_style_type>(int8_not_null(j, "style"));
		if (j->contains("min_length") && j->at("min_length").is_number_integer()) {
			min_length = j->at("min_length").get<int32_t>();
		}
		if (j->contains("max_length") && j->at("max_length").is_number_integer()) {
			max_length = j->at("max_length").get<int32_t>();
		}
		required = bool_not_null(j, "required");
		json v = (*j)["value"];
		if (!v.is_null() && v.is_number_integer()) {
			value = v.get<int64_t>();
		} else if (!v.is_null() && v.is_number_float()) {
			value = v.get<double>();
		} else if (!v.is_null() && v.is_string()) {
			value = v.get<std::string>();
		}
	}
	return *this;
}

component& component::add_component(const component& c)
{
	set_type(cot_action_row);
	components.emplace_back(c);
	return *this;
}

component& component::add_channel_type(uint8_t ct) {
	if (type == cot_action_row) {
		set_type(cot_channel_selectmenu);
	}
	channel_types.push_back(ct);
	return *this;
}

component& component::set_type(component_type ct)
{
	type = ct;
	if (type == cot_text || type == cot_button) {
		label = dpp::utility::utf8substr(label, 0, 80);
	} else if (type == cot_selectmenu) {
		label = dpp::utility::utf8substr(label, 0, 100);
	}
	if (type == cot_text) {
		placeholder = dpp::utility::utf8substr(placeholder, 0, 100);
	} else if (type == cot_selectmenu || type == cot_user_selectmenu || type == cot_role_selectmenu || type == cot_mentionable_selectmenu || type == cot_channel_selectmenu) {
		placeholder = dpp::utility::utf8substr(placeholder, 0, 150);
	}
	return *this;
}

component& component::set_label(const std::string &l)
{
	if (type == cot_action_row) {
		set_type(cot_button);
	}
	if (type == cot_text || type == cot_button) {
		label = dpp::utility::utf8substr(l, 0, 80);
	} else if (type == cot_selectmenu) {
		label = dpp::utility::utf8substr(l, 0, 100);
	} else {
		label = l;
	}
	return *this;
}

component& component::set_default_value(const std::string &val)
{
	if (type == cot_action_row) {
		set_type(cot_text);
	}
	value = dpp::utility::utf8substr(val, 0, 4000);
	return *this;
}

component& component::set_style(component_style cs)
{
	set_type(cot_button);
	style = cs;
	return *this;
}

component& component::set_text_style(text_style_type ts)
{
	set_type(cot_text);
	text_style = ts;
	return *this;
}

component& component::set_url(const std::string& u)
{
	set_type(cot_button);
	set_style(cos_link);
	url = utility::utf8substr(u, 0, 512);
	return *this;
}

component& component::set_id(const std::string &id)
{
	if (type == cot_action_row) {
		set_type(cot_button);
	}
	custom_id = utility::utf8substr(id, 0, 100);
	return *this;
}

component& component::set_disabled(bool disable)
{
	if (type == cot_action_row) {
		set_type(cot_button);
	}
	disabled = disable;
	return *this;
}

component& component::set_required(bool require)
{
	if (type == cot_action_row) {
		set_type(cot_button);
	}
	required = require;
	return *this;
}

component& component::set_emoji(const std::string& name, dpp::snowflake id, bool animated)
{
	if (type == cot_action_row) {
		set_type(cot_button);
	}
	this->emoji.id = id;
	this->emoji.name = name;
	this->emoji.animated = animated;
	return *this;
}

component& component::set_min_length(uint32_t min_l)
{
	min_length = static_cast<int32_t>(min_l);
	return *this;
}

component& component::set_max_length(uint32_t max_l)
{
	max_length = static_cast<int32_t>(max_l);
	return *this;
}

void to_json(json& j, const attachment& a) {
	if (a.id) {
		j["id"] = a.id;
	}
	if (a.size) {
		j["size"] = a.size;
	}
	if (!a.filename.empty()) {
		j["filename"] = a.filename;
	}
	if (!a.url.empty()) {
		j["url"] = a.url;
	}
	j["ephemeral"] = a.ephemeral;
}

void to_json(json& j, const component& cp) {
	j["type"] = cp.type;
	if (cp.type == cot_text) {
		j["label"] = cp.label;
		j["required"] = cp.required;
		j["style"] = int(cp.text_style);
		if (std::holds_alternative<std::string>(cp.value) && !std::get<std::string>(cp.value).empty()) {
			j["value"] = std::get<std::string>(cp.value);
		}
		if (!cp.custom_id.empty()) {
			j["custom_id"] = cp.custom_id;
		}
		if (!cp.placeholder.empty()) {
			j["placeholder"] = cp.placeholder;
		}
		if (cp.min_length > 0) {
			j["min_length"] = cp.min_length;
		}
		if (cp.max_length > 0) {
			j["max_length"] = cp.max_length;
		}
	}
	if (cp.type == cot_button) {
		j["label"] = cp.label;
		j["style"] = int(cp.style);
		if (cp.type == cot_button && cp.style != cos_link && !cp.custom_id.empty()) {
			/* Links cannot have a custom id */
			j["custom_id"] = cp.custom_id;
		}
		if (cp.type == cot_button && cp.style == cos_link && !cp.url.empty()) {
			j["url"] = cp.url;
		}
		j["disabled"] = cp.disabled;

		if (cp.emoji.id || !cp.emoji.name.empty()) {
			j["emoji"] = {};
			j["emoji"]["animated"] = cp.emoji.animated;
		}
		if (cp.emoji.id) {
			j["emoji"]["id"] = std::to_string(cp.emoji.id);
		}
		if (!cp.emoji.name.empty()) {
			j["emoji"]["name"] = cp.emoji.name;
		}
	} else if (cp.type == cot_selectmenu) {
		j["custom_id"] = cp.custom_id;
		j["disabled"] = cp.disabled;
		if (!cp.placeholder.empty()) {
			j["placeholder"] = cp.placeholder;
		}
		if (cp.min_values >= 0) {
			j["min_values"] = cp.min_values;
		}
		if (cp.max_values >= 0) {
			j["max_values"] = cp.max_values;
		}
		j["options"] = json::array();
		for (auto opt : cp.options) {
			json o;
			if (!opt.description.empty()) {
				o["description"] = opt.description;
			}
			if (!opt.label.empty()) {
				o["label"] = opt.label;
			}
			if (!opt.value.empty()) {
				o["value"] = opt.value;
			}
			if (opt.is_default) {
				o["default"] = true;
			}
			if (!opt.emoji.name.empty()) {
				o["emoji"] = json::object();
				o["emoji"]["name"] = opt.emoji.name;
				if (opt.emoji.id) {
					o["emoji"]["id"] = std::to_string(opt.emoji.id);
				}
				if (opt.emoji.animated) {
					o["emoji"]["animated"] = true;
				}
			}
			j["options"].push_back(o);
		}
	} else if (cp.type == cot_user_selectmenu || cp.type == cot_role_selectmenu || cp.type == cot_mentionable_selectmenu) {
		j["custom_id"] = cp.custom_id;
		j["disabled"] = cp.disabled;
		if (!cp.placeholder.empty()) {
			j["placeholder"] = cp.placeholder;
		}
		if (cp.min_values >= 0) {
			j["min_values"] = cp.min_values;
		}
		if (cp.max_values >= 0) {
			j["max_values"] = cp.max_values;
		}
		if (!cp.default_values.empty()) {
			j["default_values"] = json::array();
			for (auto const &v : cp.default_values) {
				json o;
				o["id"] = v.id;
				if (v.type == dpp::cdt_role) {
					o["type"] = "role";
				} else if (v.type == dpp::cdt_channel) {
					o["type"] = "channel";
				} else if (v.type == dpp::cdt_user) {
					o["type"] = "user";
				}
				j["default_values"].push_back(o);
			}
		}
	} else if (cp.type == cot_channel_selectmenu) {
		j["custom_id"] = cp.custom_id;
		j["disabled"] = cp.disabled;
		if (!cp.placeholder.empty()) {
			j["placeholder"] = cp.placeholder;
		}
		if (cp.min_values >= 0) {
			j["min_values"] = cp.min_values;
		}
		if (cp.max_values >= 0) {
			j["max_values"] = cp.max_values;
		}
		if (!cp.channel_types.empty()) {
			j["channel_types"] = json::array();
			for (auto &type : cp.channel_types) {
				j["channel_types"].push_back(type);
			}
		}
		if (!cp.default_values.empty()) {
			j["default_values"] = json::array();
			for (auto const &v : cp.default_values) {
				json o;
				o["id"] = v.id;
				if (v.type == dpp::cdt_role) {
					o["type"] = "role";
				} else if (v.type == dpp::cdt_channel) {
					o["type"] = "channel";
				} else if (v.type == dpp::cdt_user) {
					o["type"] = "user";
				}
				j["default_values"].push_back(o);
			}
		}
	}
}

select_option::select_option() : is_default(false) {
}

select_option::select_option(const std::string &_label, const std::string &_value, const std::string &_description) : label(_label), value(_value), description(_description), is_default(false) {
}

select_option& select_option::set_label(const std::string &l) {
	label = dpp::utility::utf8substr(l, 0, 100);
	return *this;
}

select_option& select_option::set_default(bool def) {
	is_default = def;
	return *this;
}

select_option& select_option::set_value(const std::string &v) {
	value = dpp::utility::utf8substr(v, 0, 100);
	return *this;
}

select_option& select_option::set_description(const std::string &d) {
	description = dpp::utility::utf8substr(d, 0, 100);
	return *this;
}

select_option& select_option::set_emoji(const std::string &n, dpp::snowflake id, bool animated) {
	emoji.name = n;
	emoji.id = id;
	emoji.animated = animated;
	return *this;
}

select_option& select_option::set_animated(bool anim) {
	emoji.animated = anim;
	return *this;
}

select_option& select_option::fill_from_json_impl(nlohmann::json* j) {
	label = string_not_null(j, "label");
	value = string_not_null(j, "value");
	description = string_not_null(j, "description");
	if (j->contains("emoji")) {
		const json& emoj = (*j)["emoji"];
		emoji.animated = bool_not_null(&emoj, "animated");
		emoji.name = string_not_null(&emoj, "name");
		emoji.id = snowflake_not_null(&emoj, "id");
	}
	is_default = bool_not_null(j, "default");
	return *this;
}

component& component::set_placeholder(const std::string &_placeholder) {
	if (type == cot_text) {
		placeholder = dpp::utility::utf8substr(_placeholder, 0, 100);
	} else if (type == cot_selectmenu || type == cot_user_selectmenu || type == cot_role_selectmenu || type == cot_mentionable_selectmenu || type == cot_channel_selectmenu) {
		placeholder = dpp::utility::utf8substr(_placeholder, 0, 150);
	} else {
		placeholder = _placeholder;
	}
	return *this;
}

component& component::set_min_values(uint32_t _min_values) {
	min_values = static_cast<int32_t>(_min_values);
	return *this;
}

component& component::set_max_values(uint32_t _max_values) {
	max_values = static_cast<int32_t>(_max_values);
	return *this;
}

component& component::add_select_option(const select_option &option) {
	if (options.size() <= 25) {
		options.emplace_back(option);
	}
	return *this;
}

component &component::add_default_value(const snowflake id, const component_default_value_type type) {
	component_default_value default_value;
	default_value.id = id;
	default_value.type = type;
	this->default_values.push_back(default_value);
	return *this;
}

namespace {

poll_media get_poll_media(const nlohmann::json& obj, std::string_view key) {
	poll_media retval{};

	if (auto it = obj.find(key); it != obj.end()) {
		const json& media_json = *it;

		retval.text = string_not_null(&media_json, "text");
		if (it = media_json.find("emoji"); it != media_json.end()) {
			const json& emoji_json = *it;

			retval.emoji.animated = bool_not_null(&emoji_json, "animated");
			retval.emoji.name = string_not_null(&emoji_json, "name");
			retval.emoji.id = snowflake_not_null(&emoji_json, "id");
		}
	}
	return retval;
};

json make_json(const poll_media &media) {
	json retval{};

	if (media.emoji.id != 0) {
		json& emoji_json = retval["emoji"];
		emoji_json["id"] = media.emoji.id;
		emoji_json["animated"] = media.emoji.animated;
	} else if (!media.emoji.name.empty()) {
		json& emoji_json = retval["emoji"];
		emoji_json["name"] = media.emoji.name;
		emoji_json["animated"] = media.emoji.animated;
	}
	retval["text"] = media.text;
	return retval;
}

}

void from_json(const nlohmann::json& j, poll& p) {
	p.question = get_poll_media(j, "question");
	if (auto it = j.find("answers"); it != j.end() && it->is_array()) {
		for (const json& element : *it) {
			auto id = int32_not_null(&element, "answer_id");
			p.answers.emplace(id, poll_answer{
				id,
				get_poll_media(element, "poll_media")
			});
		}
	}
	p.expiry = double_not_null(&j, "expiry");
	p.allow_multiselect = bool_not_null(&j, "allow_multiselect");
	p.layout_type = static_cast<poll_layout_type>(int32_not_null(&j, "layout_type"));
	if (auto it = j.find("results"); it != j.end()) {
		const json& results_json = *it;
		poll_results p_results{};

		p_results.is_finalized = bool_not_null(&results_json, "is_finalized");
		if (it = results_json.find("answer_counts"); it != results_json.end() && it->is_array()) {
			for (const json& answer_count_json : *it) {
				auto id = int32_not_null(&answer_count_json, "id");
				p_results.answer_counts.emplace(id, poll_results::answer_count{
					id,
					int32_not_null(&answer_count_json, "count"),
					bool_not_null(&answer_count_json, "me_voted")
				});
			}
		}
		p.results = std::move(p_results);
	}
}

void to_json(json& j, const poll &p) {
	j["question"] = make_json(p.question);

	json& answers_json = j["answers"];
	for (const auto& [_, answer] : p.answers) {
		answers_json.emplace_back()["poll_media"] = make_json(answer.media);
	}
	/* When sending a poll object expiry is a duration in hours so we clamp it to positive and round */
	j["duration"] = (p.expiry < 0.0 ? uint32_t{0} : static_cast<uint32_t>(p.expiry + 0.5));
	j["allow_multiselect"] = p.allow_multiselect;
	j["layout_type"] = static_cast<uint32_t>(p.layout_type);
}

poll& poll::set_question(const std::string& text) {
	question.text = text;
	return *this;
}

poll& poll::set_duration(uint32_t hours) noexcept {
	expiry = static_cast<double>(hours);
	return *this;
}

poll& poll::set_allow_multiselect(bool allow) noexcept {
	allow_multiselect = allow;
	return *this;
}

poll& poll::add_answer(const poll_media& media) {
	uint32_t max = 0;
	for (const auto &pair : answers) {
		if (pair.first > max) {
			max = pair.first;
		}
	}
	answers.emplace(max + 1, poll_answer{max + 1, media});
	return *this;
}

poll& poll::add_answer(const std::string& text, snowflake emoji_id, bool is_animated) {
	return add_answer(poll_media{text, partial_emoji{{}, emoji_id, is_animated}});
}

poll& poll::add_answer(const std::string& text, const std::string& emoji) {
	return add_answer(poll_media{text, partial_emoji{emoji, {}, false}});
}

poll& poll::add_answer(const std::string& text, const emoji& e) {
	return add_answer(poll_media{text, partial_emoji{e.name, e.id, e.is_animated()}});
}

const std::string& poll::get_question_text() const noexcept {
	return question.text;
}

const poll_media *poll::find_answer(uint32_t id) const noexcept {
	if (auto it = answers.find(id); it != answers.end()) {
		return &it->second.media;
	}
	return nullptr;
}

std::optional<uint32_t> poll::get_vote_count(uint32_t answer_id) const noexcept {
	if (!results.has_value()) {
		return std::nullopt;
	}
	if (auto it = results->answer_counts.find(answer_id); it != results->answer_counts.end()) {
		return it->second.count;
	}
	/* Answers not present can mean 0 */
	if (find_answer(answer_id) == nullptr) {
		return std::nullopt;
	}
	return 0;
}



embed::~embed() = default;

embed::embed() : timestamp(0) {
}

message::message() : managed(0), channel_id(0), guild_id(0), sent(0), edited(0), webhook_id(0),
	owner(nullptr), type(mt_default), flags(0), pinned(false), tts(false), mention_everyone(false)
{
	message_reference.channel_id = 0;
	message_reference.guild_id = 0;
	message_reference.message_id = 0;
	message_reference.fail_if_not_exists = false;
	interaction.id = 0;
	interaction.type = interaction_type::it_ping;
	interaction.usr.id = 0;
	allowed_mentions.parse_users = false;
	allowed_mentions.parse_everyone = false;
	allowed_mentions.parse_roles = false;
	allowed_mentions.replied_user = false;

}

message::message(class cluster* o) : message() {
	owner = o;
}

message& message::set_reference(snowflake _message_id, snowflake _guild_id, snowflake _channel_id, bool fail_if_not_exists) {
	message_reference.channel_id = _channel_id;
	message_reference.guild_id = _guild_id;
	message_reference.message_id = _message_id;
	message_reference.fail_if_not_exists = fail_if_not_exists;
	return *this;
}

message& message::set_allowed_mentions(bool _parse_users, bool _parse_roles, bool _parse_everyone, bool _replied_user, const std::vector<snowflake> &users, const std::vector<snowflake> &roles) {
	allowed_mentions.parse_users = _parse_users;
	allowed_mentions.parse_everyone = _parse_everyone;
	allowed_mentions.parse_roles = _parse_roles;
	allowed_mentions.replied_user = _replied_user;
	allowed_mentions.users = users;
	allowed_mentions.roles = roles;
	return *this;
}

message::message(snowflake _channel_id, const std::string &_content, message_type t) : message() {
	channel_id = _channel_id;
	content = utility::utf8substr(_content, 0, 4000);
	type = t;
}

message& message::add_component(const component& c) {
	components.emplace_back(c);
	return *this;
}

message& message::add_embed(const embed& e) {
	embeds.emplace_back(e);
	return *this;
}

message& message::add_sticker(const sticker& s) {
	stickers.emplace_back(s);
	return *this;
}

message& message::add_sticker(const snowflake& id) {
	stickers.emplace_back().id = id;
	return *this;
}

message& message::set_flags(uint16_t f) {
	flags = f;
	return *this;
}

message& message::set_type(message_type t) {
	type = t;
	return *this;
}

message& message::set_filename(const std::string &fn) {
	if (file_data.empty()) {
		message_file_data data;
		data.name = fn;

		file_data.push_back(data);
	} else {
		file_data[file_data.size() - 1].name = fn;
	}

	return *this;
}

message& message::set_file_content(const std::string &fc) {
	if (file_data.empty()) {
		message_file_data data;
		data.content = fc;

		file_data.push_back(data);
	} else {
		file_data[file_data.size() - 1].content = fc;
	}

	return *this;
}

message& message::add_file(const std::string &fn, const std::string &fc, const std::string &fm) {
	message_file_data data;
	data.name = fn;
	data.content = fc;
	data.mimetype = fm;

	file_data.push_back(data);
	return *this;
}

message& message::set_content(const std::string &c)
{
	content = utility::utf8substr(c, 0, 4000);
	return *this;
}

message& message::set_channel_id(snowflake _channel_id) {
	channel_id = _channel_id;
	return *this;
}

message& message::set_guild_id(snowflake _guild_id) {
	guild_id = _guild_id;
	return *this;
}

message& message::set_poll(const poll& p) {
	attached_poll = p;
	return *this;
}

const poll &message::get_poll() const {
	return attached_poll.value();
}

bool message::has_poll() const noexcept {
	return attached_poll.has_value();
}

message::message(const std::string &_content, message_type t) : message() {
	content = utility::utf8substr(_content, 0, 4000);
	type = t;
}

message::message(const embed& _embed) : message() {
	embeds.emplace_back(_embed);
}

message::message(snowflake _channel_id, const embed& _embed) : message(_embed) {
	channel_id = _channel_id;
}

embed::embed(json* j) : embed() {
	title = string_not_null(j, "title");
	type = string_not_null(j, "type");
	description = string_not_null(j, "description");
	url = string_not_null(j, "url");
	timestamp = ts_not_null(j, "timestamp");
	if (j->contains("color")) {
		color = int32_not_null(j, "color");
	}
	if (j->contains("footer")) {
		dpp::embed_footer f;
		json& fj = (*j)["footer"];
		f.text = string_not_null(&fj, "text");
		f.icon_url = string_not_null(&fj, "icon_url");
		f.proxy_url = string_not_null(&fj, "proxy_url");
		footer = f;
	}
	std::vector<std::string> type_list = { "image", "video", "thumbnail" };
	for (auto& s : type_list) {
		if (j->contains(s)) {
			embed_image curr;
			json& fi = (*j)[s];
			curr.url = string_not_null(&fi, "url");
			curr.height = string_not_null(&fi, "height");
			curr.width = string_not_null(&fi, "width");
			curr.proxy_url = string_not_null(&fi, "proxy_url");
			if (s == "image") {
				image = curr;
			} else if (s == "video") {
				video = curr;
			} else if (s == "thumbnail") {
				thumbnail = curr;
			}
		}
	}
	if (j->contains("provider")) {
		json &p = (*j)["provider"];
		dpp::embed_provider pr;
		pr.name = string_not_null(&p, "name");
		pr.url = string_not_null(&p, "url");
		provider = pr;
	}
	if (j->contains("author")) {
		json &a = (*j)["author"];
		dpp::embed_author au;
		au.name = string_not_null(&a, "name");
		au.url = string_not_null(&a, "url");
		au.icon_url = string_not_null(&a, "icon_url");
		au.proxy_icon_url = string_not_null(&a, "proxy_icon_url");
		author = au;
	}
	if (j->contains("fields")) {
		json &fl = (*j)["fields"];
		for (auto & field : fl) {
			embed_field f;
			f.name = string_not_null(&field, "name");
			f.value = string_not_null(&field, "value");
			f.is_inline = bool_not_null(&field, "inline");
			fields.push_back(f);
		}
	}
}

embed& embed::add_field(const std::string& name, const std::string &value, bool is_inline) {
	if (fields.size() < 25) {
		embed_field f;
		f.name = utility::utf8substr(name, 0, 256);
		f.value = utility::utf8substr(value, 0, 1024);
		f.is_inline = is_inline;
		fields.push_back(f);
	}
	return *this;
}

embed& embed::set_author(const embed_author& a)
{
	author = a;
	return *this;
}

embed& embed::set_timestamp(time_t tstamp)
{
	timestamp = tstamp;
	return *this;
}

embed& embed::set_author(const std::string& name, const std::string& url, const std::string& icon_url) {
	dpp::embed_author a;
	a.name = utility::utf8substr(name, 0, 256);
	a.url = url;
	a.icon_url = icon_url;
	author = a;
	return *this;
}

embed& embed::set_footer(const embed_footer& f) {
	footer = f;
	return *this;
}

embed& embed::set_footer(const std::string& text, const std::string& icon_url) {
	dpp::embed_footer f;
	f.set_text(text);
	f.set_icon(icon_url);
	footer = f;
	return *this;
}

embed& embed::set_provider(const std::string& name, const std::string& url) {
	dpp::embed_provider p;
	p.name = utility::utf8substr(name, 0, 256);
	p.url = url;
	provider = p;
	return *this;
}

embed& embed::set_image(const std::string& url) {
	dpp::embed_image i;
	i.url = url;
	image = i;
	return *this;
}

embed& embed::set_video(const std::string& url) {
	dpp::embed_image v;
	v.url = url;
	video = v;
	return *this;
}

embed& embed::set_thumbnail(const std::string& url) {
	dpp::embed_image t;
	t.url = url;
	thumbnail = t;
	return *this;
}

embed& embed::set_title(const std::string &text) {
	title = utility::utf8substr(text, 0, 256);
	return *this;
}

embed& embed::set_description(const std::string &text) {
	description = utility::utf8substr(text, 0, 4096);
	return *this;
}

embed& embed::set_color(uint32_t col) {
	// Mask off alpha, as discord doesn't use it
	color = col & 0x00FFFFFF;
	return *this;
}

embed& embed::set_colour(uint32_t col) {
	return this->set_color(col);
}

embed& embed::set_url(const std::string &u) {
	url = u;
	return *this;
}

embed_footer& embed_footer::set_text(const std::string& t){
	text = utility::utf8substr(t, 0, 2048);
	return *this;
}

embed_footer& embed_footer::set_icon(const std::string& i){
	icon_url = i;
	return *this;
}

embed_footer& embed_footer::set_proxy(const std::string& p){
	proxy_url = p;
	return *this;
}

reaction::reaction() {
	count = 0;
	count_burst = 0;
	count_normal = 0;
	me = false;
	me_burst = false;
	emoji_id = 0;
}

reaction::reaction(json* j) {
	count = int32_not_null(j, "count");
	if (j->contains("count_details")) {
		json details = (*j)["count_details"];
		count_burst = int32_not_null(&details, "burst");
		count_normal = int32_not_null(&details, "normal");
	}
	me = bool_not_null(j, "me");
	me_burst = bool_not_null(j, "me_burst");
	if (j->contains("emoji")) {
		json emoji = (*j)["emoji"];
		emoji_id = snowflake_not_null(&emoji, "id");
		emoji_name = string_not_null(&emoji, "name");
	}
	if (j->contains("burst_colors") && !j->at("burst_colors").is_null()) {
		/* for some silly reason discord send the hex code as strings with a preceded hashtag. Sadly it's up to us to parse this crap */
		for (std::string hex_string : j->at("burst_colors")) {
			if (!hex_string.empty()) {
				if (hex_string.substr(0, 1) == "#") {
					hex_string = hex_string.substr(1); // remove the #
				}
				// convert the hex string to base 10 integer and append it
				burst_colors.push_back(std::stoul(hex_string, nullptr, 16));
			}
		}
	}
}

attachment::attachment(struct message* o)
	: id(0)
	, size(0)
	, width(0)
	, height(0)
	, ephemeral(false)
	, flags(0)
	, owner(o)
{
}

attachment::attachment(struct message* o, json *j) : attachment(o) {
	this->id = snowflake_not_null(j, "id");
	this->size = (*j)["size"];
	this->filename = (*j)["filename"].get<std::string>();;
	this->description = string_not_null(j, "description");
	this->url = (*j)["url"].get<std::string>();;
	this->proxy_url = (*j)["proxy_url"].get<std::string>();;
	this->width = int32_not_null(j, "width");
	this->height = int32_not_null(j, "height");
	this->content_type = string_not_null(j, "content_type");
	this->ephemeral = bool_not_null(j, "ephemeral");
	this->duration_secs = double_not_null(j, "duration_secs");
	this->waveform = string_not_null(j, "waveform");
	this->flags = int8_not_null(j, "flags");
}

void attachment::download(http_completion_event callback) const {
	/* Download attachment if there is one attached to this object */
	if (owner == nullptr || owner->owner == nullptr) {
		throw dpp::logic_exception(err_no_owning_message, "attachment has no owning message/cluster");
	}
	if (callback && this->id && !this->url.empty()) {
		owner->owner->request(this->url, dpp::m_get, callback);
	}
}

bool attachment::is_remix() const {
	return flags & a_is_remix;
}

time_t attachment::get_expire_time() const {
	size_t attr_position = url.find('?');
	/* If no attributes were sent in url, we do not need to parse more */
	if(url.npos == attr_position){
		return 0;
	}
	std::string attributes = url.substr(attr_position + 1);
	std::vector<std::string> attr_list = utility::tokenize(attributes, "&");
	auto ex_attr = std::find_if(attr_list.begin(), attr_list.end(), [](const std::string& s){return s.substr(0, 3) == "ex=";});
	if(attr_list.end() == ex_attr){
		return 0;
	}
	/* Erase 'ex=' prefix before parsing */
	return std::stol(ex_attr->substr(3), nullptr, 16);
}

time_t attachment::get_issued_time() const {
	size_t attr_position = url.find('?');
	/* No attributes were sent in url, so we do not need to parse more */
	if(url.npos == attr_position){
		return 0;
	}
	std::string attributes = url.substr(attr_position + 1);
	std::vector<std::string> attr_list = utility::tokenize(attributes, "&");
	auto is_attr = std::find_if(attr_list.begin(), attr_list.end(), [](const std::string& s){return s.substr(0, 3) == "is=";});
	if(attr_list.end() == is_attr){
		return 0;
	}
	/* Erase 'is=' prefix before parsing */
	return std::stol(is_attr->substr(3), nullptr, 16);
}

json message::to_json(bool with_id, bool is_interaction_response) const {
	/* This is the basics. once it works, expand on it. */
	json j({
		{"channel_id", channel_id},
		{"tts", tts},
		{"nonce", nonce},
		{"flags", flags},
		{"type", type},
		{"content", content}
	});

	if (with_id) {
		j["id"] = std::to_string(id);
	}

	if (!author.username.empty()) {
		/* Used for webhooks */
		j["username"] = author.username;
	}

	/* Populate message reference */
	if (message_reference.channel_id || message_reference.guild_id || message_reference.message_id) {
		j["message_reference"] = json::object();
		if (message_reference.channel_id) {
			j["message_reference"]["channel_id"] = std::to_string(message_reference.channel_id);
		}
		if (message_reference.guild_id) {
			j["message_reference"]["guild_id"] = std::to_string(message_reference.guild_id);
		}
		if (message_reference.message_id) {
			j["message_reference"]["message_id"] = std::to_string(message_reference.message_id);
		}
		j["message_reference"]["fail_if_not_exists"] = message_reference.fail_if_not_exists;
	}

	j["allowed_mentions"] = json::object();
	j["allowed_mentions"]["parse"] = json::array();
	if (allowed_mentions.parse_everyone || allowed_mentions.parse_roles || allowed_mentions.parse_users || allowed_mentions.replied_user || allowed_mentions.users.size() || allowed_mentions.roles.size()) {
		if (allowed_mentions.parse_everyone) {
			j["allowed_mentions"]["parse"].push_back("everyone");
		}
		if (allowed_mentions.parse_roles) {
			j["allowed_mentions"]["parse"].push_back("roles");
		}
		if (allowed_mentions.parse_users) {
			j["allowed_mentions"]["parse"].push_back("users");
		}
		if (!allowed_mentions.replied_user) {
			j["allowed_mentions"]["replied_user"] = false;
		} else {
			j["allowed_mentions"]["replied_user"] = true;
		}
		if (allowed_mentions.users.size()) {
			j["allowed_mentions"]["users"] = json::array();
			for (auto& user : allowed_mentions.users) {
				j["allowed_mentions"]["users"].push_back(std::to_string(user));
			}
		}
		if (allowed_mentions.roles.size()) {
			j["allowed_mentions"]["roles"] = json::array();
			for (auto& role : allowed_mentions.roles) {
				j["allowed_mentions"]["roles"].push_back(std::to_string(role));
			}
		}
	}

	if (stickers.size()) {
		auto [it, _] = j.emplace("sticker_ids", json::array());
		for (const sticker &s : stickers) {
			it->push_back(std::to_string(s.id));
		}
	}

	j["components"] = json::array();
	for (auto & component : components) {
		json n;
		n["type"] = cot_action_row;
		n["components"] = {};
		for (auto & subcomponent  : component.components) {
			json sn = subcomponent;
			n["components"].push_back(sn);
		}
		j["components"].push_back(n);
	}

	j["attachments"] = json::array();
	for (auto& attachment : attachments) {
		json a = attachment;
		j["attachments"].push_back(a);
	}

	j["embeds"] = json::array();
	for (auto& embed : embeds) {
		json e;
		if (!embed.description.empty()) {
			e["description"] = embed.description;
		}
		if (!embed.title.empty()) {
			e["title"] = embed.title;
		}
		if (!embed.url.empty()) {
			e["url"] = embed.url;
		}
		if (embed.color.has_value()) {
			e["color"] = embed.color.value();
		}
		if (embed.footer.has_value()) {
			e["footer"]["text"] = embed.footer->text;
			e["footer"]["icon_url"] = embed.footer->icon_url;
		}
		if (embed.image.has_value()) {
			e["image"]["url"] = embed.image->url;
		}
		if (embed.thumbnail.has_value()) {
			e["thumbnail"]["url"] = embed.thumbnail->url;
		}
		if (embed.author.has_value()) {
			e["author"]["name"] = embed.author->name;
			e["author"]["url"] = embed.author->url;
			e["author"]["icon_url"] = embed.author->icon_url;
		}
		if (embed.fields.size()) {
			e["fields"] = json();
			for (auto& field : embed.fields) {
				json f({ {"name", field.name}, {"value", field.value}, {"inline", field.is_inline} });
				e["fields"].push_back(f);
			}
		}
		if (embed.timestamp) {
			e["timestamp"] = ts_to_string(embed.timestamp);
		}

		j["embeds"].push_back(e);
	}

	if (attached_poll.has_value()) {
		dpp::to_json(j["poll"], *attached_poll);
	}

	return j;
}

bool message::is_crossposted() const {
	return flags & m_crossposted;
}

bool message::is_crosspost() const {
	return flags & m_is_crosspost;
}

bool message::is_dm() const {
	return guild_id.empty();
}

bool message::suppress_embeds() const {
	return flags & m_suppress_embeds;
}

message& message::suppress_embeds(bool suppress) {
	if (suppress) {
		flags |= m_suppress_embeds;
	} else {
		flags &= ~m_suppress_embeds;
	}
	return *this;
}

bool message::is_source_message_deleted() const {
	return flags & m_source_message_deleted;
}

bool message::is_urgent() const {
	return flags & m_urgent;
}

bool message::has_thread() const {
	return flags & m_has_thread;
}

bool message::is_ephemeral() const {
	return flags & m_ephemeral;
}

bool message::is_loading() const {
	return flags & m_loading;
}

bool message::is_thread_mention_failed() const {
	return flags & m_thread_mention_failed;
}

bool message::suppress_notifications() const {
	return flags & m_suppress_notifications;
}

bool message::is_voice_message() const {
	return flags & m_is_voice_message;
}


message& message::fill_from_json(json* d, cache_policy_t cp) {
	this->id = snowflake_not_null(d, "id");
	this->channel_id = snowflake_not_null(d, "channel_id");
	this->guild_id = snowflake_not_null(d, "guild_id");
	/* We didn't get a guild id. See if we can find one in the channel */
	if (guild_id.empty() && !channel_id.empty()) {
		dpp::channel* c = dpp::find_channel(this->channel_id);
		if (c) {
			this->guild_id = c->guild_id;
		}
	}
	this->flags = int16_not_null(d, "flags");
	this->type = static_cast<message_type>(int8_not_null(d, "type"));
	this->author = user();
	/* May be null, if its null cache it from the partial */
	if (d->find("author") != d->end()) {
		json &j_author = (*d)["author"];
		if (cp.user_policy == dpp::cp_none) {
			/* User caching off! Allocate a temp user to be deleted in destructor */
			this->author.fill_from_json(&j_author);
		} else {
			/* User caching on - aggressive or lazy - create a cached user entry */
			user* authoruser = find_user(snowflake_not_null(&j_author, "id"));
			if (!authoruser) {
				/* User does not exist yet, cache the partial as a user record */
				authoruser = new user();
				authoruser->fill_from_json(&j_author);
				get_user_cache()->store(authoruser);
			}
			this->author = *authoruser;
		}
	}
	if (d->find("interaction") != d->end()) {
		json& inter = (*d)["interaction"];
		interaction.id = snowflake_not_null(&inter, "id");
		interaction.name = string_not_null(&inter, "name");
		interaction.type = int8_not_null(&inter, "type");
		if (inter.contains("user") && !inter["user"].is_null()) from_json(inter["user"], interaction.usr);
	}
	set_object_array_not_null<sticker>(d, "sticker_items", stickers);
	if (d->find("mentions") != d->end()) {
		json &sub = (*d)["mentions"];
		for (auto & m : sub) {
			dpp::user u = dpp::user().fill_from_json(&m);
			dpp::guild_member gm = dpp::guild_member().fill_from_json(static_cast<json*>(&m["member"]), this->guild_id, u.id);
			mentions.push_back({u, gm});
		}
	}
	set_snowflake_array_not_null(d, "mention_roles", mention_roles);
	set_object_array_not_null<channel>(d, "mention_channels", mention_channels);
	/* Fill in member record, cache uncached ones */
	guild* g = find_guild(this->guild_id);
	this->member = {};
	if (guild_id && d->find("member") != d->end()) {
		json& mi = (*d)["member"];
		snowflake uid = snowflake_not_null(&(mi["user"]), "id");
		if (!uid && author.id) {
			uid = author.id;
		}
		if (cp.user_policy == dpp::cp_none) {
			/* User caching off! Just fill in directly but dont store member to guild */
			this->member.fill_from_json(&mi, this->guild_id, uid);
		} else if (g) {
			/* User caching on, lazy or aggressive - cache the member information */
			auto thismember = g->members.find(uid);
			if (thismember == g->members.end()) {
				if (!uid.empty() && author.id) {
					guild_member gm;
					gm.fill_from_json(&mi, this->guild_id, uid);
					g->members[author.id] = gm;
					this->member = gm;
				}
			} else {
				/* Update roles etc */
				this->member = thismember->second;
				if (author.id) {
					this->member.fill_from_json(&mi, this->guild_id, author.id);
					g->members[author.id] = this->member;
				}
			}
		}
	}
	if (d->find("embeds") != d->end()) {
		json & el = (*d)["embeds"];
		for (auto& e : el) {
			this->embeds.emplace_back(embed(&e));
		}
	}
	set_object_array_not_null<component>(d, "components", this->components);
	this->content = string_not_null(d, "content");
	this->sent = ts_not_null(d, "timestamp");
	this->edited = ts_not_null(d, "edited_timestamp");
	this->tts = bool_not_null(d, "tts");
	this->mention_everyone = bool_not_null(d, "mention_everyone");
	if (d->find("reactions") != d->end()) {
		json & el = (*d)["reactions"];
		for (auto& e : el) {
			this->reactions.emplace_back(reaction(&e));
		}
	}
	if (((*d)["nonce"]).is_string()) {
		this->nonce = string_not_null(d, "nonce");
	} else {
		this->nonce = std::to_string(snowflake_not_null(d, "nonce"));
	}
	this->pinned = bool_not_null(d, "pinned");
	this->webhook_id = snowflake_not_null(d, "webhook_id");
	for (auto& e : (*d)["attachments"]) {
		this->attachments.emplace_back(attachment(this, &e));
	}
	if (d->find("message_reference") != d->end()) {
		json& mr = (*d)["message_reference"];
		message_reference.channel_id = snowflake_not_null(&mr, "channel_id");
		message_reference.guild_id = snowflake_not_null(&mr, "guild_id");
		message_reference.message_id = snowflake_not_null(&mr, "message_id");
		message_reference.fail_if_not_exists = bool_not_null(&mr, "fail_if_not_exists");
	}
	if (auto it = d->find("poll"); it != d->end()) {
		from_json(*it, attached_poll.emplace());
	}
	return *this;
}

bool message::has_remix_attachment() const {
	return std::any_of(
		attachments.begin(),
		attachments.end(),
		[](const auto& a) -> bool {
			return a.is_remix();
		});
}

std::string message::get_url() const {
	return utility::message_url(guild_id, channel_id, id);
}

sticker::sticker() : managed(0), pack_id(0), type(st_standard), format_type(sf_png), available(true), guild_id(0), sort_value(0) {
}

sticker& sticker::fill_from_json_impl(nlohmann::json* j) {
	this->id = snowflake_not_null(j, "id");
	this->pack_id = snowflake_not_null(j, "pack_id");
	this->name = string_not_null(j, "name");
	this->description = string_not_null(j, "description");
	this->tags = string_not_null(j, "tags");
	this->asset = string_not_null(j, "asset");
	this->guild_id = snowflake_not_null(j, "guild_id");
	this->type = static_cast<sticker_type>(int8_not_null(j, "type"));
	this->format_type = static_cast<sticker_format>(int8_not_null(j, "format_type"));
	this->available = bool_not_null(j, "available");
	this->sort_value = int8_not_null(j, "sort_value");
	if (j->contains("user")) {
		sticker_user.fill_from_json(&((*j)["user"]));
	}

	return *this;
}

json sticker::to_json_impl(bool with_id) const {
	json j;

	if (with_id) {
		j["id"] = std::to_string(this->id);
	}
	j["pack_id"] = std::to_string(this->pack_id);
	if (this->guild_id) {
		j["guild_id"] = std::to_string(this->guild_id);
	}
	j["name"] = this->name;
	j["description"] = this->description;
	if (!this->tags.empty()) {
		j["tags"] = this->tags;
	}
	if (!this->asset.empty()) {
		j["asset"] = this->asset;
	}
	j["type"] = this->type;
	j["format_type"] = this->format_type;
	j["available"] = this->available;
	j["sort_value"] = this->sort_value;

	return j;
}

sticker_pack& sticker_pack::fill_from_json_impl(nlohmann::json* j) {
	this->id = snowflake_not_null(j, "id");
	this->sku_id = snowflake_not_null(j, "sku_id");
	this->cover_sticker_id = snowflake_not_null(j, "cover_sticker_id");
	this->banner_asset_id = snowflake_not_null(j, "banner_asset_id");
	this->name = string_not_null(j, "name");
	this->description = string_not_null(j, "description");
	if (j->contains("stickers")) {
		json & sl = (*j)["stickers"];
		for (auto& s : sl) {
			this->stickers[snowflake_not_null(&s, "id")] = sticker().fill_from_json(&s);
		}
	}
	return *this;
}

json sticker_pack::to_json_impl(bool with_id) const {
	json j;
	if (with_id) {
		j["id"] = std::to_string(this->id);
	}
	if (sku_id) {
		j["sku_id"] = std::to_string(sku_id);
	}
	if (cover_sticker_id) {
		j["cover_sticker_id"] = std::to_string(cover_sticker_id);
	}
	if (banner_asset_id) {
		j["banner_asset_id"] = std::to_string(banner_asset_id);
	}
	j["name"] = name;
	j["description"] = description;
	j["stickers"] = json::array();
	for (auto& s : stickers) {
		try {
			j["stickers"].push_back(json::parse(s.second.build_json(with_id)));
		}
		catch (const std::exception&) {
			/* Protection against malformed json in sticker */
		}
	}
	return j;
}

std::string sticker::get_url() const {
	return utility::cdn_endpoint_url_sticker(this->id, this->format_type);
}

sticker& sticker::set_filename(const std::string &fn) {
	filename = fn;
	return *this;
}

sticker& sticker::set_file_content(const std::string &fc) {
	filecontent = fc;
	return *this;
}


} // namespace dpp
