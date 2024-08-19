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
#include <string>
#include <exception>
#include <algorithm>

namespace dpp {

/**
 * @brief Exception error codes possible for dpp::exception::code()
 * 
 * This list is a combined list of Discord's error codes, HTTP error codes, 
 * zlib, opus, sodium and C library codes (e.g. DNS, socket etc). You may 
 * use these to easily identify a type of exception without having to resort
 * to string comparison against dpp::exception::what()
 * 
 * For detailed descriptions of each error code, see the text description
 * returned in `what()`.
 * 
 * @note Some exceptions MAY have error codes which are NOT in this list
 * in the event a C library is updated and adds new codes we did not document
 * here. In this case, or where the code is not specific, refer to `what()`.
 */
enum exception_error_code {
	err_no_code_specified = 0,
	err_zlib_see_errno = -1,
	err_zlib_init_stream = -2,
	err_zlib_init_data = -3,
	err_zlib_init_mem = -4,
	err_zlib_init_buffer = -5,
	err_zlib_init_version = -6,
	err_opus_bad_arg = -11,
	err_opus_buffer_too_small = -12,
	err_opus_internal_error = -13,
	err_opus_invalid_packet = -14,
	err_opus_unimplemented = -15,
	err_opus_invalid_state = -16, 
	err_opus_alloc_fail = -17,
	err_dns_bad_flags = -21,
	err_name_or_service_unknown = -22,
	err_dns_again = -23,
	err_dns_fail = -24,
	err_dns_family = -26,
	err_dns_socket_type = -27,
	err_dns_service = -28,
	err_dns_memory = -30,
	err_dns_system_error = -31,
	err_dns_overflow = -32,
	err_ssl_new = 1,
	err_ssl_connect = 2,
	err_write = 3,
	err_ssl_write = 4,
	err_no_sessions_left = 5,
	err_auto_shard = 6,
	err_reconnection = 7,
	err_bind_failure = 8,
	err_nonblocking_failure = 9,
	err_voice_terminating = 10,
	err_connect_failure = 11,
	err_ssl_context = 12,
	err_ssl_version = 13,
	err_invalid_socket = 14,
	err_socket_error = 15,
	err_websocket_proto_already_set = 16,
	err_command_handler_not_ready = 17,
	err_no_owning_message = 18,
	err_cancelled_event = 19,
	err_event_status = 20,
	err_event_start_time = 21,
	err_event_end_time = 22,
	err_command_has_caps = 23,
	err_choice_autocomplete = 24,
	err_interaction = 25,
	err_too_many_component_rows = 26,
	err_invalid_webhook = 27,
	err_voice_state_timestamp = 28,
	err_no_voice_support = 29,
	err_invalid_voice_packet_length = 30,
	err_opus = 31,
	err_sodium = 32,
	err_etf = 33,
	err_cache = 34,
	err_icon_size = 35,
	err_massive_audio = 36,
	err_unknown = 37,
	err_bad_request = 400,
	err_unauthorized = 401,
	err_payment_required = 402,
	err_forbidden = 403,
	err_not_found = 404,
	err_method_not_allowed = 405,
	err_not_acceptable = 406,
	err_proxy_auth_required = 407,
	err_request_timeout = 408,
	err_conflict = 409,
	err_gone = 410,
	err_length_required = 411,
	err_precondition_failed = 412,
	err_payload_too_large = 413,
	err_uri_too_long = 414,
	err_unsupported_media_type = 415,
	err_range_not_satisfiable = 416,
	err_expectation_failed = 417,
	err_im_a_teapot = 418,
	err_page_expired = 419,
	err_twitter_rate_limited = 420,
	err_misdirected_request = 421,
	err_unprocessable_content = 422,
	err_webdav_locked = 423,
	err_webdav_failed_dependency = 424,
	err_too_early = 425,
	err_upgrade_required = 426,
	err_precondition_required = 428,
	err_rate_limited = 429,
	err_request_headers_too_large = 431,
	err_page_blocked = 450,
	err_unavailable_for_legal_reasons = 451,
	err_http_request_on_https_port = 497,
	err_internal_server_error = 500,
	err_not_implemented = 501,
	err_bad_gateway = 502,
	err_service_unavailable = 503,
	err_gateway_timeout = 504,
	err_http_version_not_supported = 505,
	err_variant_also_negotiates = 506,
	err_webdav_insufficient_storage = 507,
	err_webdav_loop_detected = 508,
	err_bandwidth_limit_exceeded = 509,
	err_not_extended = 510,
	err_network_auth_required = 511,
	err_web_server_down = 521,
	err_connection_timed_out = 522,
	err_origin_unreachable = 523,
	err_timeout = 524,
	err_ssl_handshake_failed = 525,
	err_invalid_ssl_certificate = 526,
	err_railgun = 527,
	err_cloudflare = 530,	
	err_websocket_unknown = 4000,
	err_websocket_bad_opcode= 4001,
	err_websocket_decode = 4002,
	err_websocket_not_authenticated = 4003,
	err_websocket_authentication_failed = 4004,
	err_websocket_already_authenticated = 4005,
	err_websocket_invalid_seq_number = 4007,
	err_websocket_rate_limited = 4008,
	err_websocket_session_timeout = 4009,
	err_websocket_invalid_shard = 4010,
	err_websocket_sharding_required = 4011,
	err_websocket_invalid_api_version = 4012,
	err_websocket_invalid_intents = 4013,
	err_websocket_disallowed_intents = 4014,
	err_websocket_voice_disconnected = 4014,
	err_websocket_voice_server_crashed = 4015,
	err_websocket_voice_unknown_encryption = 4016,
	err_compression_stream = 6000,
	err_compression_data = 6001,
	err_compression_memory = 6002,
	err_unknown_account = 10001,
	err_unknown_application = 10002,
	err_unknown_channel = 10003,
	err_unknown_guild = 10004,
	err_unknown_integration = 10005,
	err_unknown_invite = 10006,
	err_unknown_member = 10007,
	err_unknown_message = 10008,
	err_unknown_permission_overwrite = 10009,
	err_unknown_provider = 10010,
	err_unknown_role = 10011,
	err_unknown_token = 10012,
	err_unknown_user = 10013,
	err_unknown_emoji = 10014,
	err_unknown_webhook = 10015,
	err_unknown_webhook_service = 10016,
	err_unknown_session = 10020,
	err_unknown_ban = 10026,
	err_unknown_sku = 10027,
	err_unknown_store_listing = 10028,
	err_unknown_entitlement = 10029,
	err_unknown_build = 10030,
	err_unknown_lobby = 10031,
	err_unknown_branch = 10032,
	err_unknown_store_directory_layout = 10033,
	err_unknown_redistributable = 10036,
	err_unknown_gift_code = 10038,
	err_unknown_stream = 10049,
	err_unknown_premium_server_subscribe_cooldown = 10050,
	err_unknown_guild_template = 10057,
	err_unknown_discoverable_server_category = 10059,
	err_unknown_sticker = 10060,
	err_unknown_interaction = 10062,
	err_unknown_application_command = 10063,
	err_unknown_voice_state = 10065,
	err_unknown_application_command_permissions = 10066,
	err_unknown_stage_instance = 10067,
	err_unknown_guild_member_verification_form = 10068,
	err_unknown_guild_welcome_screen = 10069,
	err_unknown_guild_scheduled_event = 10070,
	err_unknown_guild_scheduled_event_user = 10071,
	err_unknown_tag = 10087,
	err_bots_cannot_use_this_endpoint = 20001,
	err_only_bots_can_use_this_endpoint = 20002,
	err_explicit_content = 20009,
	err_unauthorized_for_application = 20012,
	err_slowmode_rate_limit = 20016,
	err_owner_only = 20018,
	err_announcement_rate_limit = 20022,
	err_under_minimum_age = 20024,
	err_write_rate_limit = 20029,
	err_stage_banned_words = 20031,
	err_guild_premium_subscription_level_too_low = 20035,
	err_guilds = 30001,
	err_friends = 30002,
	err_pins_for_the_channel = 30003,
	err_recipients = 30004,
	err_guild_roles = 30005,
	err_webhooks = 30007,
	err_emojis = 30008,
	err_reactions = 30010,
	err_group_dms = 30011,
	err_guild_channels = 30013,
	err_attachments_in_a_message = 30015,
	err_invites = 30016,
	err_animated_emojis = 30018,
	err_server_members = 30019,
	err_server_categories = 30030,
	err_guild_already_has_a_template = 30031,
	err_application_commands = 30032,
	err_thread_participants = 30033,
	err_daily_application_command_creates = 30034,
	err_bans_for_non_guild_members_have_been_exceeded = 30035,
	err_bans_fetches = 30037,
	err_uncompleted_guild_scheduled_events = 30038,
	err_stickers = 30039,
	err_prune_requests = 30040,
	err_guild_widget_settings_updates = 30042,
	err_edits_to_messages_older_than_1_hour = 30046,
	err_pinned_threads_in_a_forum_channel = 30047,
	err_tags_in_a_forum_channel = 30048,
	err_bitrate_is_too_high_for_channel_of_this_type = 30052,
	err_premium_emojis = 30056,
	err_webhooks_per_guild = 30058,
	err_channel_permission_overwrites = 30060,
	err_the_channels_for_this_guild_are_too_large = 30061,
	err_unauthorized_invalid_token = 40001,
	err_verify_your_account = 40002,
	err_you_are_opening_direct_messages_too_fast = 40003,
	err_send_messages_has_been_temporarily_disabled = 40004,
	err_request_entity_too_large = 40005,
	err_this_feature_has_been_temporarily_disabled_server_side = 40006,
	err_the_user_is_banned_from_this_guild = 40007,
	err_connection_has_been_revoked = 40012,
	err_target_user_is_not_connected_to_voice = 40032,
	err_this_message_has_already_been_crossposted = 40033,
	err_an_application_command_with_that_name_already_exists = 40041,
	err_application_interaction_failed_to_send = 40043,
	err_cannot_send_a_message_in_a_forum_channel = 40058,
	err_interaction_has_already_been_acknowledged = 40060,
	err_tag_names_must_be_unique = 40061,
	err_service_resource_is_being_rate_limited = 40062,
	err_no_tags_available = 40066,
	err_tag_required = 40067,
	err_entitlement_already_granted = 40074,
	err_missing_access = 50001,
	err_invalid_account_type = 50002,
	err_cannot_execute_action_on_a_dm_channel = 50003,
	err_guild_widget_disabled = 50004,
	err_cannot_edit_a_message_by_other_user = 50005,
	err_cannot_send_empty_message = 50006,
	err_cannot_send_messages_to_this_user = 50007,
	err_cannot_send_messages_in_a_non_text_channel = 50008,
	err_channel_verification_level_too_high = 50009,
	err_oauth2_application_does_not_have_a_bot = 50010,
	err_oauth2_application_limit = 50011,
	err_invalid_oauth2_state = 50012,
	err_permissions = 50013,
	err_invalid_authentication_token = 50014,
	err_note_was_too_long = 50015,
	err_too_few_or_too_many_messages = 50016,
	err_invalid_mfa_level = 50017,
	err_invalid_pin = 50019,
	err_invite_code_invalid = 50020,
	err_system_message = 50021,
	err_channel_type = 50024,
	err_invalid_oauth2_access_token = 50025,
	err_missing_required_oauth2_scope = 50026,
	err_invalid_webhook_token = 50027,
	err_invalid_role = 50028,
	err_invalid_recipients = 50033,
	err_too_old_to_bulk_delete = 50034,
	err_invalid_form_body = 50035,
	err_invite_error = 50036,
	err_invalid_activity_action = 50039,
	err_invalid_api_version_provided = 50041,
	err_file_uploaded_exceeds_the_maximum_size = 50045,
	err_invalid_file_uploaded = 50046,
	err_cannot_self_redeem_this_gift = 50054,
	err_invalid_guild = 50055,
	err_invalid_sku = 50057,
	err_invalid_request_origin = 50067,
	err_invalid_message_type = 50068,
	err_payment_source_required = 50070,
	err_cannot_modify_a_system_webhook = 50073,
	err_cannot_delete_a_channel_required_for_community_guilds = 50074,
	err_cannot_edit_stickers_within_a_message = 50080,
	err_invalid_sticker_sent = 50081,
	err_tried_to_perform_an_operation_on_an_archived_thread = 50083,
	err_invalid_thread_notification_settings = 50084,
	err_before_value_is_earlier_than_the_thread_creation_date = 50085,
	err_community_server_channels_must_be_text_channels = 50086,
	err_bad_event_entity_type = 50091,
	err_this_server_is_not_available_in_your_location = 50095,
	err_monetization_enabled_in_order_to_perform_this_action = 50097,
	err_more_boosts_to_perform_this_action = 50101,
	err_the_request_body_contains_invalid_json = 50109,
	err_owner_cannot_be_pending_member = 50131,
	err_ownership_cannot_be_transferred_to_a_bot_user = 50132,
	err_failed_to_resize_asset_below_the_maximum_size = 50138,
	err_cannot_mix_subscription_and_non_subscription_roles_for_an_emoji = 50144,
	err_cannot_convert_between_premium_emoji_and_normal_emoji = 50145,
	err_uploaded_file_not_found = 50146,
	err_voice_messages_do_not_support_additional_content = 50159,
	err_voice_messages_must_have_a_single_audio_attachment = 50160,
	err_voice_messages_must_have_supporting_metadata = 50161,
	err_voice_messages_cannot_be_edited = 50162,
	err_cannot_delete_guild_subscription_integration = 50163,
	err_you_cannot_send_voice_messages_in_this_channel = 50173,
	err_the_user_account_must_first_be_verified = 50178,
	err_you_do_not_have_permission_to_send_this_sticker = 50600,
	err_two_factor_is_required_for_this_operation = 60003,
	err_no_users_with_discordtag_exist = 80004,
	err_reaction_was_blocked = 90001,
	err_user_cannot_use_burst_reactions = 90002,
	err_application_not_yet_available = 110001,
	err_api_resource_is_currently_overloaded = 130000,
	err_the_stage_is_already_open = 150006,
	err_cannot_reply_without_permission_to_read_message_history = 160002,
	err_a_thread_has_already_been_created_for_this_message = 160004,
	err_thread_is_locked = 160005,
	err_active_threads = 160006,
	err_active_announcement_threads = 160007,
	err_invalid_json_for_uploaded_lottie_file = 170001,
	err_uploaded_lotties_cannot_contain_rasterized_images = 170002,
	err_sticker_maximum_framerate = 170003,
	err_sticker_frame_count = 170004,
	err_lottie_animation_dimensions = 170005,
	err_sticker_frame_rate = 170006,
	err_sticker_animation_duration = 170007,
	err_cannot_update_a_finished_event = 180000,
	err_failed_to_create_stage_needed_for_stage_event = 180002,
	err_message_was_blocked_by_automatic_moderation = 200000,
	err_title_was_blocked_by_automatic_moderation = 200001,
	err_webhooks_posted_to_forum_channels_must_have_a_thread_name_or_thread_id = 220001,
	err_webhooks_posted_to_forum_channels_cannot_have_both_a_thread_name_and_thread_id = 220002,
	err_webhooks_can_only_create_threads_in_forum_channels = 220003,
	err_webhook_services_cannot_be_used_in_forum_channels = 220004,
	err_message_blocked_links = 240000,
	err_cannot_enable_onboarding_requirements_are_not_met = 350000,
	err_cannot_update_onboarding_below_requirements = 350001,
};

/**
 * @brief The dpp::exception class derives from std::exception and supports some other
 * ways of passing in error details such as via std::string.
 */
class exception : public std::exception
{
protected:
	/**
	 * @brief Exception message
	 */
	std::string msg;

	/**
	 * @brief Exception error code
	 */
	exception_error_code error_code;

public:

	using std::exception::exception;

	/**
	 * @brief Construct a new exception object
	 */
	exception() = default;

	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 */
	explicit exception(const char* what) : msg(what), error_code(err_no_code_specified) { }

	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 * @param code Exception code
	 */
	explicit exception(exception_error_code code, const char* what) : msg(what), error_code(code) { }

	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 * @param len length of reason message
	 */
	exception(const char* what, size_t len) : msg(what, len), error_code(err_no_code_specified) { }

	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 */
	explicit exception(const std::string& what) : msg(what), error_code(err_no_code_specified) { }
	
	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 * @param code Exception code
	 */
	explicit exception(exception_error_code code, const std::string& what) : msg(what), error_code(code) { }
	
	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 */
	explicit exception(std::string&& what) : msg(std::move(what)) { }

	/**
	 * @brief Construct a new exception object
	 * 
	 * @param what reason message
	 * @param code Exception code
	 */
	explicit exception(exception_error_code code, std::string&& what) : msg(std::move(what)), error_code(code) { }

	/**
	 * @brief Construct a new exception object (copy constructor)
	 */
	exception(const exception&) = default;

	/**
	 * @brief Construct a new exception object (move constructor)
	 */
	exception(exception&&) = default;

	/**
	 * @brief Destroy the exception object
	 */
	~exception() override = default;

	/**
	 * @brief Copy assignment operator
	 * 
	 * @return exception& reference to self
	 */
	exception & operator = (const exception &) = default;

	/**
	 * @brief Move assignment operator
	 * 
	 * @return exception& reference to self
	 */
	exception & operator = (exception&&) = default;

	/**
	 * @brief Get exception message
	 * 
	 * @return const char* error message
	 */
	[[nodiscard]] const char* what() const noexcept override { return msg.c_str(); };

	/**
	 * @brief Get exception code
	 * 
	 * @return exception_error_code error code
	 */
	[[nodiscard]] exception_error_code code() const noexcept { return error_code; };

};

#ifndef _DOXYGEN_
	#define derived_exception(name, ancestor) class name : public dpp::ancestor { \
	public: \
		using dpp::ancestor::ancestor; \
		name() = default; \
		explicit name(const char* what) : ancestor(what) { } \
		explicit name(exception_error_code code, const char* what) : ancestor(code, what) { } \
		name(const char* what, size_t len) : ancestor(what, len) { } \
		explicit name(const std::string& what) : ancestor(what) { } \
		explicit name(exception_error_code code, const std::string& what) : ancestor(code, what) { } \
		explicit name(std::string&& what) : ancestor(what) { } \
		explicit name(exception_error_code code, std::string&& what) : ancestor(code, what) { } \
		name(const name&) = default; \
		name(name&&) = default; \
		~name() override = default; \
		name & operator = (const name &) = default; \
		name & operator = (name&&) = default; \
		[[nodiscard]] const char* what() const noexcept override { return msg.c_str(); }; \
		[[nodiscard]] exception_error_code code() const noexcept { return error_code; }; \
	};
#endif

#ifdef _DOXYGEN_
	/*
	 * THESE DEFINITIONS ARE NOT THE REAL DEFINITIONS USED BY PROGRAM CODE.
	 *
	 * They exist only to cause Doxygen to emit proper documentation for the derived exception types.
	 * Proper definitions are emitted by the `derived_exception` macro in the "else" section.
	 */

	/**
	 * @brief Represents an error in logic, e.g. you asked the library to do something the Discord API does not support
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class logic_exception : public dpp::exception { };
	/**
	 * @brief Represents an error reading or writing to a file
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class file_exception : public dpp::exception { };
	/**
	 * @brief Represents an error establishing or maintaining a connection
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class connection_exception : public dpp::exception { };
	/**
	 * @brief Represents an error with voice processing
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class voice_exception : public dpp::exception { };
	/**
	 * @brief Represents an error on a REST API call, e.g. a HTTPS request
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class rest_exception : public dpp::exception { };
	/**
	 * @brief Represents invalid length of argument being passed to a function
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class length_exception : public dpp::exception { };
	/**
	 * @brief Represents inability to parse data, usually caused by malformed JSON or ETF
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class parse_exception : public dpp::exception { };
	/**
	 * @brief Represents invalid access to dpp's cache or its members, which may or may not exist. 
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class cache_exception : public dpp::exception { };
	/**
	 * @brief Represents an attempt to construct a cluster with an invalid bot token.
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class invalid_token_exception : public dpp::rest_exception { };
#ifdef DPP_CORO
	/**
	 * @brief Represents the cancellation of a task. Will be thrown to the awaiter of a cancelled task.
	 * @note This is a stub for documentation purposes. For full information on supported methods please see dpp::exception.
	 */
	class task_cancelled_exception : public dpp::exception { };
#endif /* DPP_CORO */
#else
	derived_exception(logic_exception, exception);
	derived_exception(file_exception, exception);
	derived_exception(connection_exception, exception);
	derived_exception(voice_exception, exception);
	derived_exception(rest_exception, exception);
	derived_exception(invalid_token_exception, rest_exception);
	derived_exception(length_exception, exception);
	derived_exception(parse_exception, exception);
	derived_exception(cache_exception, exception);
#  ifdef DPP_CORO
	derived_exception(task_cancelled_exception, exception);
#  endif /* DPP_CORO */
#endif

} // namespace dpp

