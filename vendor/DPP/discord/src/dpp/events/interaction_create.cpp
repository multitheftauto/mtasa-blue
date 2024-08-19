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
#include <dpp/discordevents.h>
#include <dpp/cluster.h>
#include <dpp/appcommand.h>
#include <dpp/stringops.h>
#include <dpp/json.h>


namespace dpp::events {

namespace {

void fill_options(dpp::json option_json, std::vector<dpp::command_option>& options) {
	for (auto & o : option_json) {
		dpp::command_option opt;
		opt.name = string_not_null(&o, "name");
		opt.type = (dpp::command_option_type)int8_not_null(&o, "type");
		switch (opt.type) {
			case co_boolean:
				if(o.at("value").is_boolean()) {
					opt.value = o.at("value").get<bool>();
				}
				break;
			case co_channel:
			case co_role:
			case co_attachment:
			case co_user:
			case co_mentionable:
				opt.value = dpp::snowflake(snowflake_not_null(&o, "value"));
				break;
			case co_integer:
				if(o.at("value").is_number_integer()) {
					opt.value = o.at("value").get<int64_t>();
				}
				break;
			case co_string:
				if(o.at("value").is_string()) {
					opt.value = o.at("value").get<std::string>();
				}
				break;
			case co_number:
				if(o.at("value").is_number_float()) {
					opt.value = o.at("value").get<double>();
				}
				break;
			case co_sub_command:
			case co_sub_command_group:
				fill_options(o["options"], opt.options);
				break;
		}
		opt.focused = bool_not_null(&o, "focused");
		options.emplace_back(opt);
	}
}

}

/**
 * @brief Handle event
 * 
 * @param client Websocket client (current shard)
 * @param j JSON data for the event
 * @param raw Raw JSON string
 */
void interaction_create::handle(discord_client* client, json &j, const std::string &raw) {
	json& d = j["d"];
	dpp::interaction i;
	/* We must set here because we cant pass it through the nlohmann from_json() */
	i.cache_policy = client->creator->cache_policy;
	i.fill_from_json(&d);
	/* There are several types of interactions, component interactions,
	 * auto complete interactions, dialog interactions and slash command
	 * interactions. Both fire different library events so ensure they are
	 * dispatched properly.
	 */
	if (i.type == it_application_command) {
		/* Slash command is split again into chat input, and the two context menu types */
		command_interaction cmd_data = i.get_command_interaction();
		if (cmd_data.type == ctxm_message && !client->creator->on_message_context_menu.empty()) {
			if (i.resolved.messages.size()) {
				/* Message right-click context menu */
				message_context_menu_t mcm(client, raw);
				mcm.command = i;
				mcm.set_message(i.resolved.messages.begin()->second);
				client->creator->on_message_context_menu.call(mcm);
			}
		} else if (cmd_data.type == ctxm_user && !client->creator->on_user_context_menu.empty()) {
			if (i.resolved.users.size()) {
				/* User right-click context menu */
				user_context_menu_t ucm(client, raw);
				ucm.command = i;
				ucm.set_user(i.resolved.users.begin()->second);
				client->creator->on_user_context_menu.call(ucm);
			}
		} else if (cmd_data.type == ctxm_chat_input && !client->creator->on_slashcommand.empty()) {
			dpp::slashcommand_t sc(client, raw);
			sc.command = i;
			client->creator->on_slashcommand.call(sc);
		}
		if (!client->creator->on_interaction_create.empty()) {
			/* Standard chat input. Note that for backwards compatibility, context menu
			 * events still find their way here. At some point in the future, receiving
			 * ctxm_user and ctxm_message inputs to this event will be depreciated.
			 */
			dpp::interaction_create_t ic(client, raw);
			ic.command = i;
			client->creator->on_interaction_create.call(ic);
		}
	} else if (i.type == it_modal_submit) {
		if (!client->creator->on_form_submit.empty()) {
			dpp::form_submit_t fs(client, raw);
			fs.custom_id = string_not_null(&(d["data"]), "custom_id");
			fs.command = i;
			for (auto & c : d["data"]["components"]) {
				fs.components.push_back(dpp::component().fill_from_json(&c));
			}
			client->creator->on_form_submit.call(fs);
		}
	} else if (i.type == it_autocomplete) {
		// "data":{"id":"903319628816728104","name":"blep","options":[{"focused":true,"name":"animal","type":3,"value":"a"}],"type":1}
		if (!client->creator->on_autocomplete.empty()) {
			dpp::autocomplete_t ac(client, raw);
			ac.id = snowflake_not_null(&(d["data"]), "id");
			ac.name = string_not_null(&(d["data"]), "name");
			fill_options(d["data"]["options"], ac.options);
			ac.command = i;
			client->creator->on_autocomplete.call(ac);
		}
	} else if (i.type == it_component_button) {
		dpp::component_interaction bi = std::get<component_interaction>(i.data);
		if (bi.component_type == cot_button) {
			if (!client->creator->on_button_click.empty()) {
				dpp::button_click_t ic(client, raw);
				ic.command = i;
				ic.custom_id = bi.custom_id;
				ic.component_type = bi.component_type;
				client->creator->on_button_click.call(ic);
			}
		} else if (bi.component_type == cot_selectmenu || bi.component_type == cot_user_selectmenu ||
				   bi.component_type == cot_role_selectmenu || bi.component_type == cot_mentionable_selectmenu ||
				   bi.component_type == cot_channel_selectmenu) {
			if (!client->creator->on_select_click.empty()) {
				dpp::select_click_t ic(client, raw);
				ic.command = i;
				ic.custom_id = bi.custom_id;
				ic.component_type = bi.component_type;
				ic.values = bi.values;
				client->creator->on_select_click.call(ic);
			}
		}
	}
}

};
