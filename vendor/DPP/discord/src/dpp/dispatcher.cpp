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
#include <dpp/appcommand.h>
#include <dpp/message.h>
#include <dpp/discordclient.h>
#include <dpp/dispatcher.h>
#include <dpp/user.h>
#include <dpp/restresults.h>
#include <dpp/cluster.h>
#include <variant>
#include <utility>

namespace dpp {

event_dispatch_t::event_dispatch_t(discord_client* client, const std::string& raw) : raw_event(raw), from(client) {}

event_dispatch_t::event_dispatch_t(discord_client* client, std::string&& raw) : raw_event(std::move(raw)), from(client) {}

const event_dispatch_t& event_dispatch_t::cancel_event() const {
	cancelled = true;
	return *this;
}

event_dispatch_t& event_dispatch_t::cancel_event() {
	cancelled = true;
	return *this;
}

bool event_dispatch_t::is_cancelled() const {
	return cancelled;
}

const message& message_context_menu_t::get_message() const {
	return ctx_message;
}

message_context_menu_t& message_context_menu_t::set_message(const message& m) {
	ctx_message = m;
	return *this;
}

const user& user_context_menu_t::get_user() const {
	return ctx_user;
}

user_context_menu_t& user_context_menu_t::set_user(const user& u) {
	ctx_user = u;
	return *this;
}


void message_create_t::send(const std::string& m, command_completion_event_t callback) const {
	this->send(dpp::message(m), std::move(callback));
}

void message_create_t::send(const message& msg, command_completion_event_t callback) const {
	this->from->creator->message_create(std::move(message{msg}.set_channel_id(this->msg.channel_id)), std::move(callback));
}

void message_create_t::send(message&& msg, command_completion_event_t callback) const {
	msg.channel_id = this->msg.channel_id;
	this->from->creator->message_create(std::move(msg), std::move(callback));
}

void message_create_t::reply(const std::string& m, bool mention_replied_user, command_completion_event_t callback) const {
	this->reply(dpp::message{m}, mention_replied_user, std::move(callback));
}

void message_create_t::reply(const message& msg, bool mention_replied_user, command_completion_event_t callback) const {
	dpp::message msg_to_send{msg};

	msg_to_send.set_reference(this->msg.id);
	msg_to_send.channel_id = this->msg.channel_id;
	if (mention_replied_user) {
		msg_to_send.allowed_mentions.replied_user = mention_replied_user;
		msg_to_send.allowed_mentions.users.push_back(this->msg.author.id);
	}
	this->from->creator->message_create(std::move(msg_to_send), std::move(callback));
}

void message_create_t::reply(message&& msg, bool mention_replied_user, command_completion_event_t callback) const {
	msg.set_reference(this->msg.id);
	msg.channel_id = this->msg.channel_id;
	if (mention_replied_user) {
		msg.allowed_mentions.replied_user = mention_replied_user;
		msg.allowed_mentions.users.push_back(this->msg.author.id);
	}
	this->from->creator->message_create(std::move(msg), std::move(callback));
}

void interaction_create_t::reply(interaction_response_type t, const message& m, command_completion_event_t callback) const {
	from->creator->interaction_response_create(this->command.id, this->command.token, dpp::interaction_response(t, m), std::move(callback));
}

void interaction_create_t::reply(const message& m, command_completion_event_t callback) const {
	from->creator->interaction_response_create(
		this->command.id,
		this->command.token,
		dpp::interaction_response(ir_channel_message_with_source, m),
		std::move(callback)
	);
}

void interaction_create_t::thinking(bool ephemeral, command_completion_event_t callback) const {
	message msg{this->command.channel_id, std::string{"*"}};
	msg.guild_id = this->command.guild_id;
	if (ephemeral) {
		msg.set_flags(dpp::m_ephemeral);
	}
	this->reply(ir_deferred_channel_message_with_source, std::move(msg), std::move(callback));
}

void interaction_create_t::reply(command_completion_event_t callback) const {
	this->reply(ir_deferred_update_message, message{}, std::move(callback));
}

void interaction_create_t::dialog(const interaction_modal_response& mr, command_completion_event_t callback) const {
	from->creator->interaction_response_create(this->command.id, this->command.token, mr, std::move(callback));
}

void interaction_create_t::reply(interaction_response_type t, const std::string& mt, command_completion_event_t callback) const {
	this->reply(t, dpp::message(this->command.channel_id, mt, mt_application_command), std::move(callback));
}

void interaction_create_t::reply(const std::string& mt, command_completion_event_t callback) const {
	this->reply(ir_channel_message_with_source, dpp::message(this->command.channel_id, mt, mt_application_command), callback);
}

void interaction_create_t::edit_response(const message& m, command_completion_event_t callback) const {
	from->creator->interaction_response_edit(this->command.token, m, std::move(callback));
}

void interaction_create_t::edit_response(const std::string& mt, command_completion_event_t callback) const {
	this->edit_response(dpp::message(this->command.channel_id, mt, mt_application_command), std::move(callback));
}

void interaction_create_t::get_original_response(command_completion_event_t callback) const {
	from->creator->post_rest(API_PATH "/webhooks", std::to_string(command.application_id), command.token + "/messages/@original", m_get, "", [creator = this->from->creator, cb = std::move(callback)](json& j, const http_request_completion_t& http) {
		if (cb) {
			cb(confirmation_callback_t(creator, message().fill_from_json(&j), http));
		}
	});
}

void interaction_create_t::edit_original_response(const message& m, command_completion_event_t callback) const {
	std::vector<std::string> file_names{};
	std::vector<std::string> file_contents{};
	std::vector<std::string> file_mimetypes{};

	for(message_file_data data : m.file_data) {
		file_names.push_back(data.name);
		file_contents.push_back(data.content);
		file_mimetypes.push_back(data.mimetype);
	}

	from->creator->post_rest_multipart(API_PATH "/webhooks", std::to_string(command.application_id), command.token + "/messages/@original", m_patch, m.build_json(), [creator = this->from->creator, cb = std::move(callback)](json& j, const http_request_completion_t& http) {
		if (cb) {
			cb(confirmation_callback_t(creator, message().fill_from_json(&j), http));
		}
	}, m.file_data);
}

void interaction_create_t::delete_original_response(command_completion_event_t callback) const {
	from->creator->post_rest(API_PATH "/webhooks", std::to_string(command.application_id), command.token + "/messages/@original", m_delete, "", [creator = this->from->creator, cb = std::move(callback)](const json &, const http_request_completion_t& http) {
		if (cb) {
			cb(confirmation_callback_t(creator, confirmation(), http));
		}
	});
}


#ifdef DPP_CORO
async<confirmation_callback_t> interaction_create_t::co_reply() const {
	return dpp::async{[this] <typename T> (T&& cb) { this->reply(std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_reply(interaction_response_type t, const message& m) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->reply(t, m, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_reply(interaction_response_type t, const std::string& mt) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->reply(t, mt, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_reply(const message& m) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->reply(m, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_reply(const std::string& mt) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->reply(mt, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_dialog(const interaction_modal_response& mr) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->dialog(mr, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_edit_response(const message& m) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->edit_response(m, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_edit_response(const std::string& mt) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->edit_response(mt, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_thinking(bool ephemeral) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->thinking(ephemeral, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_get_original_response() const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->get_original_response(std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_edit_original_response(const message& m) const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->edit_original_response(m, std::forward<T>(cb)); }};
}

async<confirmation_callback_t> interaction_create_t::co_delete_original_response() const {
	return dpp::async{[&, this] <typename T> (T&& cb) { this->delete_original_response(std::forward<T>(cb)); }};
}
#endif /* DPP_CORO */

command_value interaction_create_t::get_parameter(const std::string& name) const {
	const command_interaction ci = command.get_command_interaction();

	for (const auto &option : ci.options) {
		if (option.type != co_sub_command && option.type != co_sub_command_group && option.name == name) {
			return option.value;
		}
	}
	/* if not found in the first level, go one level deeper */
	for (const auto &option : ci.options) { // command
		for (const auto &sub_option : option.options) { // subcommands
			if (sub_option.type != co_sub_command && sub_option.type != co_sub_command_group && sub_option.name == name) {
				return sub_option.value;
			}
		}
	}
	/* if not found in the second level, search it in the third dimension */
	for (const auto &option : ci.options) { // command
		for (const auto &sub_group_option : option.options) { // subcommand groups
			for (const auto &sub_option : sub_group_option.options) { // subcommands
				if (sub_option.type != co_sub_command && sub_option.type != co_sub_command_group && sub_option.name == name) {
					return sub_option.value;
				}
			}
		}
	}
	return {};
}

voice_receive_t::voice_receive_t(discord_client* client, const std::string& raw, discord_voice_client* vc, snowflake _user_id, const uint8_t* pcm, size_t length) : event_dispatch_t(client, raw), voice_client(vc), user_id(_user_id) {
	reassign(vc, _user_id, pcm, length);
}

voice_receive_t::voice_receive_t(discord_client* client, std::string&& raw, discord_voice_client* vc, snowflake _user_id, const uint8_t* pcm, size_t length) : event_dispatch_t(client, std::move(raw)), voice_client(vc), user_id(_user_id) {
	reassign(vc, _user_id, pcm, length);
}

void voice_receive_t::reassign(discord_voice_client* vc, snowflake _user_id, const uint8_t* pcm, size_t length) {
	voice_client = vc;
	user_id = _user_id;

	audio_data.assign(pcm, length);

	// for backwards compatibility; remove soon
	audio = audio_data.data();
	audio_size = audio_data.length();
}

} // namespace dpp
