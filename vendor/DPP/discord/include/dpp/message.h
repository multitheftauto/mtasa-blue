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
#include <dpp/queues.h>
#include <dpp/snowflake.h>
#include <dpp/managed.h>
#include <dpp/user.h>
#include <dpp/channel.h>
#include <dpp/guild.h>
#include <optional>
#include <variant>
#include <dpp/json_fwd.h>
#include <dpp/json_interface.h>

namespace dpp {

/**
 * @brief Represents the type of a component
 */
enum component_type : uint8_t {
	/**
	 * @brief Action row, a container for other components.
	 */
	cot_action_row = 1,

	/**
	 * @brief Clickable button.
	 */
	cot_button = 2,

	/**
	 * @brief Select menu for picking from defined text options.
	 */
	cot_selectmenu = 3,

	/**
	 * @brief Text input.
	 */
	cot_text = 4,

	/**
	 * @brief Select menu for users.
	 */
	cot_user_selectmenu = 5,

	/**
	 * @brief Select menu for roles.
	 */
	cot_role_selectmenu = 6,

	/**
	 * @brief Select menu for mentionables (users and roles).
	 */
	cot_mentionable_selectmenu = 7,

	/**
	 * @brief Select menu for channels.
	 */
	cot_channel_selectmenu = 8,
};

/**
 * @brief An emoji reference for a component (select menus included) or a poll.
 *
 * To set an emoji on your button or poll answer, you must set one of either the name or id fields.
 * The easiest way for buttons is to use the dpp::component::set_emoji method.
 *
 * @note This is a **very** scaled down version of dpp::emoji, we advise that you refrain from using this.
 */
struct partial_emoji {
	/**
	 * @brief The name of the emoji.
	 *
	 * For built in unicode emojis, set this to the
	 * actual unicode value of the emoji e.g. "😄"
	 * and not for example ":smile:"
	 */
	std::string name{};

	/**
	 * @brief The emoji ID value for emojis that are custom
	 * ones belonging to a guild.
	 *
	 * The same rules apply as with other emojis,
	 * that the bot must be on the guild where the emoji resides
	 * and it must be available for use
	 * (e.g. not disabled due to lack of boosts, etc)
	 */
	dpp::snowflake id{0};

	/**
	 * @brief Is the emoji animated?
	 *
	 * @note Only applies to custom emojis.
	 */
	bool animated{false};
};

/**
 * @brief An emoji for a component. Alias to partial_emoji, for backwards compatibility.
 *
 * @see partial_emoji
 */
using component_emoji = partial_emoji;

/**
 * @brief The data for a file attached to a message.
 *
 * @todo Change the naming of this and make stickers (and potentially anything else that has data like this) use this.
 */
struct message_file_data {
	/**
	 * @brief Name of file to upload (for use server-side in discord's url).
	 */
	std::string name{};

	/**
	 * @brief File content to upload (raw binary)
	 */
	std::string content{};

	/**
	 * @brief Mime type of files to upload.
	 *
	 * @todo Look at turning this into an enum? This would allow people to easily compare mimetypes if they happen to change.
	 */
	std::string mimetype{};
};

/**
 * @brief Types of text input
 */
enum text_style_type : uint8_t {
	/**
	 * @brief Intended for short single-line text.
	 */
	text_short = 1,

	/**
	 * @brief Intended for much longer inputs.
	 */
	text_paragraph = 2,
};

/**
 * @brief Represents the style of a button
 */
enum component_style : uint8_t {
	/**
	 * @brief Blurple
	 */
	cos_primary = 1,

	/**
	 * @brief Grey
	 */
	cos_secondary,

	/**
	 * @brief Green
	 */
	cos_success,

	/**
	 * @brief Red
	 */
	cos_danger,

	/**
	 * @brief An external hyperlink to a website
	 */
	cos_link
};

/**
 * Represents the type of a dpp::component_default_value
 *
 * @note They're different to discord's value types
 */
enum component_default_value_type: uint8_t {
	cdt_user = 0,
	cdt_role = 1,
	cdt_channel = 2,
};

/**
 * @brief A Default value structure for components
 */
struct DPP_EXPORT component_default_value {
	/**
	 * @brief The type this default value represents
	 */
	component_default_value_type type;

	/**
	 * @brief Default value. ID of a user, role, or channel
	 */
	dpp::snowflake id;
};

/**
 * @brief An option for a select component
 */
struct DPP_EXPORT select_option : public json_interface<select_option> {
protected:
	friend struct json_interface<select_option>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	select_option& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief User-facing name of the option
	 */
	std::string label;

	/**
	 * @brief Dev-defined value of the option
	 */
	std::string value;

	/**
	 * @brief Additional description of the option
	 */
	std::string description;

	/**
	 * @brief True if option is the default option
	 */
	bool is_default;

	/**
	 * @brief The emoji for the select option.
	 */
	partial_emoji emoji;

	/**
	 * @brief Construct a new select option object
	 */
	select_option();

	/**
	 * @brief Destructs the select option object.
	 */
	virtual ~select_option() = default;

	/**
	 * @brief Construct a new select option object
	 * 
	 * @param label Label of option
	 * @param value Value of option
	 * @param description Description of option
	 */
	select_option(const std::string &label, const std::string &value, const std::string &description = "");

	/**
	 * @brief Set the label
	 * 
	 * @param l the user-facing name of the option. It will be truncated to the maximum length of 100 UTF-8 characters.
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_label(const std::string &l);

	/**
	 * @brief Set the value
	 * 
	 * @param v value to set. It will be truncated to the maximum length of 100 UTF-8 characters.
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_value(const std::string &v);

	/**
	 * @brief Set the description
	 * 
	 * @param d description to set. It will be truncated to the maximum length of 100 UTF-8 characters.
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_description(const std::string &d);

	/**
	 * @brief Set the emoji
	 * 
	 * @param n emoji name
	 * @param id emoji id for custom emojis
	 * @param animated true if animated emoji
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_emoji(const std::string &n, dpp::snowflake id = 0, bool animated = false);

	/**
	 * @brief Set the option as default
	 * 
	 * @param def true to set the option as default
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_default(bool def);

	/**
	 * @brief Set the emoji as animated
	 * 
	 * @param anim true if animated
	 * @return select_option& reference to self for chaining
	 */
	select_option& set_animated(bool anim);
};

/**
 * @brief Represents the component object.
 * A component is a clickable button or drop down list
 * within a discord message, where the buttons emit
 * on_button_click events when the user interacts with
 * them.
 *
 * You should generally define one component object and
 * then insert one or more additional components into it
 * using component::add_component(), so that the parent
 * object is an action row and the child objects are buttons.
 */
class DPP_EXPORT component : public json_interface<component> {
protected:
	friend struct json_interface<component>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	component& fill_from_json_impl(nlohmann::json* j);

public:
	/**
	 * @brief Component type, either a button or action row
	 */
	component_type type;

	/**
	 * @brief Sub components, buttons on an action row
	 */
	std::vector<component> components;

	/**
	 * @brief Component label (for buttons, text inputs).
	 * Maximum of 80 characters.
	 */
	std::string label;

	/**
	 * @brief Component style (for buttons).
	 */
	component_style style;

	/**
	 * @brief Text style (for text inputs).
	 */
	text_style_type text_style;

	/**
	 * @brief Component id (for buttons, menus, text inputs).
	 * Maximum of 100 characters.
	 */
	std::string custom_id;

	/**
	 * @brief URL for link types (dpp::cos_link).
	 * Maximum of 512 characters.
	 */
	std::string url;

	/**
	 * @brief Placeholder text for select menus and text inputs (max 150 characters)
	 */
	std::string placeholder;

	/**
	 * @brief Minimum number of items that must be chosen for a select menu (0-25).
	 *
	 * @note Use -1 to not set this. This is done by default.
	 */
	int32_t min_values;

	/**
	 * @brief Maximum number of items that can be chosen for a select menu (0-25).
	 *
	 * @note Use -1 to not set this. This is done by default.
	 */
	int32_t max_values;

	/**
	 * @brief Minimum length for text input (0-4000)
	 */
	int32_t min_length;

	/**
	 * @brief Maximum length for text input (1-4000)
	 */
	int32_t max_length;

	/**
	 * @brief Select options for select menus.
	 *
	 * @warning Only required and available for select menus of type dpp::cot_selectmenu
	 */
	std::vector<select_option> options;

	/**
	 * @brief List of channel types (dpp::channel_type) to include in the channel select component (dpp::cot_channel_selectmenu)
	 */
	std::vector<uint8_t> channel_types;

	/**
	 * @brief List of default values for auto-populated select menu components.
	 *
	 * @note The amount of default values must be in the range defined by dpp::component::min_values and dpp::component::max_values.
	 *
	 * @warning Only available for auto-populated select menu components, which include dpp::cot_user_selectmenu, dpp::cot_role_selectmenu, dpp::cot_mentionable_selectmenu, and dpp::cot_channel_selectmenu components.
	 */
	std::vector<component_default_value> default_values;

	/**
	 * @brief Disabled flag (for buttons)
	 */
	bool disabled;

	/**
	 * @brief Whether the text input is required to be filled
	 */
	bool required;

	/**
	 * @brief Value of the modal.
	 * Filled or valid when populated from an on_form_submit event, or from the set_value function.
	 */
	std::variant<std::monostate, std::string, int64_t, double> value;

	/**
	 * @brief The emoji for this component.
	 */
	partial_emoji emoji;

	/**
	 * @brief Constructor
	 */
	component();

	/**
	 * @brief Destructor
	 */
	virtual ~component() = default;

	/**
	 * @brief Add a channel type to include in the channel select component (dpp::cot_channel_selectmenu)
	 *
	 * @param ct The dpp::channel_type
	 * @return component& reference to self
	 */
	component& add_channel_type(uint8_t ct);

	/**
	 * @brief Set the type of the component. Button components
	 * (type dpp::cot_button) should always be contained within
	 * an action row (type dpp::cot_action_row). As described
	 * below, many of the other methods automatically set this
	 * to the correct type so usually you should not need to
	 * manually  call component::set_type().
	 *
	 * @param ct The component type
	 * @return component& reference to self
	 */
	component& set_type(component_type ct);

	/**
	 * @brief Set the text style of a text component
	 * @note Sets type to `cot_text`
	 * 
	 * @param ts Text style type to set
	 * @return component& reference to self
	 */
	component& set_text_style(text_style_type ts);

	/**
	 * @brief Set the label of the component, e.g. button text.
	 * For action rows, this field is ignored. Setting the
	 * label will auto-set the type to dpp::cot_button.
	 *
	 * @param label Label text to set. It will be truncated to the maximum length of 80 UTF-8 characters.
	 * @return component& Reference to self
	 */
	component& set_label(const std::string &label);

	/**
	 * @brief Set the default value of the text input component.
	 * For action rows, this field is ignored. Setting the
	 * value will auto-set the type to dpp::cot_text.
	 *
	 * @param val Value text to set. It will be truncated to the maximum length of 4000 UTF-8 characters.
	 * @return component& Reference to self
	 */
	component& set_default_value(const std::string &val);

	/**
	 * @brief Set the url for dpp::cos_link types.
	 * Calling this function sets the style to dpp::cos_link
	 * and the type to dpp::cot_button.
	 *
	 * @param url URL to set. It will be truncated to the maximum length of 512 UTF-8 characters.
	 * @return component& reference to self.
	 */
	component& set_url(const std::string &url);

	/**
	 * @brief Set the style of the component, e.g. button colour.
	 * For action rows, this field is ignored. Setting the
	 * style will auto-set the type to dpp::cot_button.
	 *
	 * @param cs Component style to set
	 * @return component& reference to self
	 */
	component& set_style(component_style cs);

	/**
	 * @brief Set the id of the component.
	 * For action rows, this field is ignored. Setting the
	 * id will auto-set the type to dpp::cot_button.
	 *
	 * @param id Custom ID string to set. This ID will be sent
	 * for any on_button_click events related to the button.
	 * @note The maximum length of the Custom ID is 100 UTF-8 codepoints.
	 * If your Custom ID is longer than this, it will be truncated.
	 * @return component& Reference to self
	 */
	component& set_id(const std::string &id);

	/**
	 * @brief Set the component to disabled.
	 * Defaults to false on all created components.
	 *
	 * @param disable True to disable, false to disable.
	 * @return component& Reference to self
	 */
	component& set_disabled(bool disable);

	/**
	 * @brief Set if this component is required.
	 * Defaults to false on all created components.
	 *
	 * @param require True to require this, false to make it optional.
	 * @return component& Reference to self
	 */
	component& set_required(bool require);

	/**
	 * @brief Set the placeholder
	 * 
	 * @param placeholder placeholder string. It will be truncated to the
	 * maximum length of 150 UTF-8 characters for select menus, and 100 UTF-8
	 * characters for modals.
	 * @return component& Reference to self
	 */
	component& set_placeholder(const std::string &placeholder);

	/**
	 * @brief Set the minimum number of items that must be chosen for a select menu
	 * 
	 * @param min_values min value to set (0-25)
	 * @return component& Reference to self
	 */
	component& set_min_values(uint32_t min_values);

	/**
	 * @brief Set the maximum number of items that can be chosen for a select menu
	 * 
	 * @param max_values max value to set (0-25)
	 * @return component& Reference to self
	 */
	component& set_max_values(uint32_t max_values);

	/**
	 * @brief Set the minimum input length for a text input
	 * 
	 * @param min_l min length to set (0-4000)
	 * @return component& Reference to self
	 */
	component& set_min_length(uint32_t min_l);

	/**
	 * @brief Set the maximum input length for a text input
	 * 
	 * @param max_l max length to set (1-4000)
	 * @return component& Reference to self
	 */
	component& set_max_length(uint32_t max_l);

	/**
	 * @brief Add a select option
	 * 
	 * @param option option to add
	 * @return component& Reference to self
	 */
	component& add_select_option(const select_option &option);

	/**
	 * @brief Add a sub-component, only valid for action rows.
	 * Adding subcomponents to a component will automatically
	 * set this component's type to dpp::cot_action_row.
	 *
	 * @param c The sub-component to add
	 * @return component& reference to self
	 */
	component& add_component(const component& c);

	/**
	 * @brief Add a default value.
	 *
	 * @param id Default value. ID of a user, role, or channel
	 * @param type The type this default value represents
	 */
	component& add_default_value(const snowflake id, const component_default_value_type type);

	/**
	 * @brief Set the emoji of the current sub-component.
	 * Only valid for buttons. Adding an emoji to a component
	 * will automatically set this components type to
	 * dpp::cot_button. One or both of name and id must be set.
	 * For a built in unicode emoji, you only need set name,
	 * and should set it to a unicode character e.g. "😄".
	 * For custom emojis, set the name to the name of the emoji
	 * on the guild, and the id to the emoji's ID.
	 * Setting the animated boolean is only valid for custom
	 * emojis.
	 *
	 * @param name Emoji name, or unicode character to use
	 * @param id Emoji id, for custom emojis only.
	 * @param animated True if the custom emoji is animated.
	 * @return component& Reference to self
	 */
	component& set_emoji(const std::string& name, dpp::snowflake id = 0, bool animated = false);
};

/**
 * @brief A footer in a dpp::embed
 */
struct DPP_EXPORT embed_footer {
	/**
	 * @brief Footer text
	 */
	std::string text;

	/**
	 * @brief Footer icon url.
	 *
	 * @warning Only supports http(s) and attachments.
	 */
	std::string icon_url;

	/**
	 * @brief Proxied icon url.
	 */
	std::string proxy_url;

	/**
	 * @brief Set footer's text.
	 * @param t string to set as footer text. It will be truncated to the maximum length of 2048 UTF-8 characters.
	 * @return A reference to self so this method may be "chained".
	 */
	embed_footer& set_text(const std::string& t);

	/**
	 * @brief Set footer's icon url.
	 * @param i url to set as footer icon url
	 * @return A reference to self so this method may be "chained".
	 */
	embed_footer& set_icon(const std::string& i);

	/**
	 * @brief Set footer's proxied icon url.
	 * @param p url to set as footer proxied icon url
	 * @return A reference to self so this method may be "chained".
	 */
	embed_footer& set_proxy(const std::string& p);
};

/**
 * @brief An video, image or thumbnail in a dpp::embed
 */
struct DPP_EXPORT embed_image {
	/**
	 * @brief URL to image or video.
	 */
	std::string url;

	/**
	 * @brief Proxied image url.
	 */
	std::string proxy_url;

	/**
	 * @brief Height (calculated by discord).
	 */
	std::string height;

	/**
	 * @brief Width (calculated by discord).
	 */
	std::string width;
};

/**
 * @brief Embed provider in a dpp::embed. Received from discord but cannot be sent
 */
struct DPP_EXPORT embed_provider {
	/**
	 * @brief Provider name.
	 */
	std::string name;

	/**
	 * @brief Provider URL.
	 */
	std::string url;
};

/**
 * @brief Author within a dpp::embed object
 */
struct DPP_EXPORT embed_author {
	/**
	 * @brief Author name.
	 */
	std::string name;

	/**
	 * @brief Author url.
	 *
	 * @warning Only supports http(s).
	 */
	std::string url;

	/**
	 * @brief Author icon url.
	 *
	 * @warning Only supports http(s) and attachments.
	 */
	std::string icon_url;

	/**
	 * @brief Proxied icon url.
	 */
	std::string proxy_icon_url;
};

/**
 * @brief A dpp::embed may contain zero or more fields.
 */
struct DPP_EXPORT embed_field {
	/**
	 * @brief Name of field (max length 256).
	 */
	std::string name;

	/**
	 * @brief Value of field (max length 1024).
	 */
	std::string value;

	/**
	 * @brief True if the field is to be displayed inline.
	 */
	bool is_inline;
};

/**
 * @brief A rich embed for display within a dpp::message.
 */
struct DPP_EXPORT embed {
	/**
	 * @brief Optional: Title of embed.
	 */
	std::string title;

	/**
	 * @brief Optional: Type of embed.
	 *
	 * @note Always "rich" for webhook embeds.
	 */
	std::string type;

	/**
	 * @brief Optional: Description of embed.
	 */
	std::string description;

	/**
	 * @brief Optional: URL of embed.
	 */
	std::string url;

	/**
	 * @brief Optional: Timestamp of embed content.
	 */
	time_t timestamp;

	/**
	 * @brief Optional: Color code of the embed.
	 */
	std::optional<uint32_t>	color;

	/**
	 * @brief Optional: Footer information.
	 */
	std::optional<embed_footer> footer;

	/**
	 * @brief Optional: Image information.
	 */
	std::optional<embed_image> image;

	/**
	 * @brief Optional: Thumbnail information.
	 */
	std::optional<embed_image> thumbnail;

	/**
	 * @brief Optional: Video information
	 *
	 * @warning Can't send this.
	 */
	std::optional<embed_image> video;

	/**
	 * @brief Optional: Provider information.
	 *
	 * @warning Can't send this.
	 */
	std::optional<embed_provider> provider;

	/**
	 * @brief Optional: Author information.
	 */
	std::optional<embed_author> author;

	/**
	 * @brief Optional: Fields information.
	 */
	std::vector<embed_field> fields;

	/**
	 * @brief Constructor
	 */
	embed();

	/**
	 * @brief Constructor to build embed from json object
	 * @param j JSON to read content from
	 */
	embed(nlohmann::json* j);

	/**
	 * @brief Destructor
	 */
	~embed();

	/**
	 * @brief Set embed title.
	 * @param text The text of the title. It will be truncated to the maximum length of 256 UTF-8 characters.
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_title(const std::string &text);

	/**
	 * @brief Set embed description.
	 * @param text The text of the title. It will be truncated to the maximum length of 4096 UTF-8 characters.
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_description(const std::string &text);

	/**
	 * @brief Set the footer of the embed.
	 * @param f the footer to set
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_footer(const embed_footer& f);

	/**
	 * @brief Set the footer of the embed.
	 * @param text string to set as footer text. It will be truncated to the maximum length of 2048 UTF-8 characters.
	 * @param icon_url an url to set as footer icon url (only supports http(s) and attachments)
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_footer(const std::string& text, const std::string& icon_url);

	/**
	 * @brief Set embed colour.
	 * @param col The colour of the embed
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_color(uint32_t col);

	/**
	 * @brief Set embed colour.
	 * @param col The colour of the embed
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_colour(uint32_t col);

	/**
	 * @brief Set embed timestamp.
	 * @param tstamp The timestamp to show in the footer, should be in UTC
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_timestamp(time_t tstamp);

	/**
	 * @brief Set embed url.
	 * @param url the url of the embed
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_url(const std::string &url);

	/**
	 * @brief Add an embed field.
	 * @param name The name of the field. It will be truncated to the maximum length of 256 UTF-8 characters.
	 * @param value The value of the field. It will be truncated to the maximum length of 1024 UTF-8 characters.
	 * @param is_inline Whether or not to display the field 'inline' or on its own line
	 * @return A reference to self so this method may be "chained".
	 */
	embed& add_field(const std::string& name, const std::string &value, bool is_inline = false);

	/**
	 * @brief Set embed author.
	 * @param a The author to set
	 * @return A reference to self so this method may be "chained".
	 */ 
	embed& set_author(const dpp::embed_author& a);

	/**
	 * @brief Set embed author.
	 * @param name The name of the author. It will be truncated to the maximum length of 256 UTF-8 characters.
	 * @param url The url of the author (only supports http(s))
	 * @param icon_url The icon URL of the author (only supports http(s) and attachments)
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_author(const std::string& name, const std::string& url, const std::string& icon_url);

	/**
	 * @brief Set embed provider.
	 * @param name The provider name. It will be truncated to the maximum length of 256 UTF-8 characters.
	 * @param url The provider url
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_provider(const std::string& name, const std::string& url);

	/**
	 * @brief Set embed image.
	 * @param url The embed image URL (only supports http(s) and attachments)
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_image(const std::string& url);

	/**
	 * @brief Set embed video.
	 * @param url The embed video url
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_video(const std::string& url);

	/**
	 * @brief Set embed thumbnail.
	 * @param url The embed thumbnail url (only supports http(s) and attachments)
	 * @return A reference to self so this method may be "chained".
	 */
	embed& set_thumbnail(const std::string& url);
};

/**
 * @brief Represents a reaction to a dpp::message.
 */
struct DPP_EXPORT reaction {
	/**
	 * @brief Total number of times this emoji has been used to react (including super reacts)
	 */
	uint32_t count;

	/**
	 * @brief Count of super reactions
	 */
	uint32_t count_burst;

	/**
	 * @brief Count of normal reactions
	 */
	uint32_t count_normal;

	/**
	 * @brief ID of emoji for reaction
	 */
	snowflake emoji_id;

	/**
	 * @brief Name of emoji, if applicable
	 */
	std::string emoji_name;

	/**
	 * @brief Whether your bot reacted using this emoji
	 */
	bool me;

	/**
	 * @brief Whether your bot super-reacted using this emoji
	 */
	bool me_burst;

	/**
	 * @brief HEX colors used for super reaction.
	 *
	 * @note Stored as integers.
	 */
	std::vector<uint32_t> burst_colors;

	/**
	 * @brief Constructs a new reaction object.
	 */
	reaction();

	/**
	 * @brief Constructs a new reaction object from a JSON object.
	 * @param j The JSON to read data from
	 */
	reaction(nlohmann::json* j);

	/**
	 * @brief Destructs the reaction object.
	 */
	~reaction() = default;
};

/**
 * @brief Bitmask flags for a dpp::attachment
 */
enum attachment_flags : uint8_t {
	/**
	 * @brief This attachment has been edited using the remix feature on mobile.
	 */
	a_is_remix = 1 << 2,
};

/**
 * @brief Represents an attachment in a dpp::message
 */
struct DPP_EXPORT attachment {
	/**
	 * @brief ID of attachment.
	 */
	snowflake id;

	/**
	 * @brief Size of the attachment in bytes.
	 */
	uint32_t size;

	/**
	 * @brief File name of the attachment.
	 */
	std::string filename;

	/**
	 * @brief Optional: Description of the attachment.
	 * Max 1024 characters.
	 */
	std::string description;

	/**
	 * @brief URL which points to the attachment.
	 */
	std::string url;

	/**
	 * @brief Proxied URL which points to the attachment.
	 */
	std::string proxy_url;

	/**
	 * @brief Width of the attachment, if applicable.
	 */
	uint32_t width;

	/**
	 * @brief Height of the attachment, if applicable.
	 */
	uint32_t height;

	/**
	 * @brief MIME type of the attachment, if applicable.
	 */
	std::string content_type;

	/**
	 * @brief Whether this attachment is ephemeral, if applicable.
	 */
	bool ephemeral;

	/**
	 * @brief The duration of the audio file.
	 *
	 * @note Currently for voice messages.
	 */
	double duration_secs;

	/**
	 * @brief Base64 encoded bytearray representing a sampled waveform.
	 *
	 * @note Currently for voice messages.
	 */
	std::string waveform;

	/**
	 * @brief Flags made from dpp::attachment_flags.
	 */
	uint8_t	flags;

	/**
	 * @brief Owning message
	 */
	struct message* owner;

	/**
	 * @brief Constructs a new attachment object.
	 * @param o Owning dpp::message object
	 */
	attachment(struct message* o);

	/**
	 * @brief Constructs a new attachment object from a JSON object.
	 * @param o Owning dpp::message object
	 * @param j JSON to read information from
	 */
	attachment(struct message* o, nlohmann::json* j);

	/**
	 * @brief Destructs the attachment object.
	 */
	~attachment() = default;

	/**
	 * @brief Download this attachment
	 *
	 * @param callback A callback which is called when the download completes.
	 * @note The content of the file will be in the http_info.body parameter of the
	 * callback parameter.
	 * @throw dpp::logic_exception If there is no owner associated with this attachment that
	 * itself has an owning cluster, this method will throw a dpp::logic_exception when called.
	 */
	void download(http_completion_event callback) const;
	
	/**
	 * @brief Returns true if remixed
	 * 
	 * @return true if remixed
	 */
	bool is_remix() const;
	
	/**
	 * @brief Returns expiration timestamp for attachment's URL
	 * 
	 * @return timestamp of URL expiration
	 */
	time_t get_expire_time() const;
	
	/**
	 * @brief Returns creation timestamp for attachment's URL
	 * 
	 * @return timestamp of URL creation
	 */
	time_t get_issued_time() const;
};

/**
 * @brief Represents the type of a sticker
 */
enum sticker_type : uint8_t {
	/**
	 * @brief An official sticker in a pack.
	 */
	st_standard = 1,

	/**
	 * @brief Guild sticker.
	 */
	st_guild = 2
};

/**
 * @brief The file format (png, apng, lottie) of a sticker
 */
enum sticker_format : uint8_t {
	sf_png = 1,
	sf_apng = 2,
	sf_lottie = 3,
	sf_gif = 4,
};

/**
 * @brief Represents stickers received in messages
 */
struct DPP_EXPORT sticker : public managed, public json_interface<sticker> {
protected:
	friend struct json_interface<sticker>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	sticker& fill_from_json_impl(nlohmann::json* j);

	/** Build JSON from this object.
	 * @param with_id True if the ID is to be set in the JSON structure
	 * @return The JSON of the invite
	 */
	virtual json to_json_impl(bool with_id = true) const;

public:
	/**
	 * @brief Optional: for standard stickers, id of the pack the sticker is from
	 */
	snowflake pack_id;

	/**
	 * @brief The name of the sticker.
	 */
	std::string name;

	/**
	 * @brief Description of the sticker
	 *
	 * @note This may be empty.
	 */
	std::string description;

	/**
	 * @brief The sticker's (or related) expressions.
	 *
	 * @note If it's a guild sticker, this will be the Discord name of
	 * a unicode emoji representing the sticker's expression.
	 * Otherwise, this will be a comma-separated list of related expressions.
	 */
	std::string tags;

	/**
	 * @brief Asset ID
	 *
	 * @deprecated now an empty string but still sent by discord.
	 * While discord still send this empty string value,
	 * we will still have this field here in the library.
	 */
	std::string asset;

	/**
	 * @brief The type of sticker.
	 */
	sticker_type type;

	/**
	 * @brief type of sticker format.
	 */
	sticker_format format_type;

	/**
	 * @brief Optional: Whether this guild sticker can be used.
	 *
	 * @note May be false due to loss of Server Boosts.
	 */
	bool available;

	/**
	 * @brief Optional: ID of the guild that owns this sticker.
	 */
	snowflake guild_id;

	/**
	 * @brief Optional: The user that uploaded the guild sticker.
	 */
	user sticker_user;

	/**
	 * @brief Optional: The standard sticker's sort order within its pack.
	 */
	uint8_t	sort_value;

	/**
	 * @brief Name of file to upload (when adding or editing a sticker).
	 */
	std::string filename;

	/**
	 * @brief File content to upload (raw binary).
	 */
	std::string filecontent;

	/**
	 * @brief Construct a new sticker object
	 */
	sticker();

	virtual ~sticker() = default;

	/**
	 * @brief Get the sticker url.
	 *
	 * @return std::string The sticker url or an empty string, if the id is empty
	 */
	std::string get_url() const;

	/**
	 * @brief Set the filename
	 * 
	 * @param fn filename
	 * @return message& reference to self
	 */
	sticker& set_filename(const std::string &fn);

	/**
	 * @brief Set the file content
	 * 
	 * @param fc raw file content contained in std::string
	 * @return message& reference to self
	 */
	sticker& set_file_content(const std::string &fc);

};

/**
 * @brief Represents a sticker pack (the built in groups of stickers that all nitro users get to use)
 */
struct DPP_EXPORT sticker_pack : public managed, public json_interface<sticker_pack> {
protected:
	friend struct json_interface<sticker_pack>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	sticker_pack& fill_from_json_impl(nlohmann::json* j);

	/** Build JSON from this object.
	 * @param with_id True if the ID is to be set in the JSON structure
	 * @return The JSON of the invite
	 */
	virtual json to_json_impl(bool with_id = true) const;

public:
	/**
	 * @brief The stickers in the pack.
	 */
	std::map<snowflake, sticker> stickers{};

	/**
	 * @brief Name of the sticker pack.
	 */
	std::string name{};

	/**
	 * @brief ID of the pack's SKU.
	 */
	snowflake sku_id{0};

	/**
	 * @brief Optional: ID of a sticker in the pack which is shown as the pack's icon.
	 */
	snowflake cover_sticker_id{0};

	/**
	 * @brief Description of the sticker pack.
	 */
	std::string description{};

	/**
	 * @brief ID of the sticker pack's banner image.
	 */
	snowflake banner_asset_id{};
};

/**
 * @brief Poll layout types
 *
 * @note At the time of writing Discord only has 1, "The, uhm, default layout type."
 * @see https://discord.com/developers/docs/resources/poll#layout-type
 */
enum poll_layout_type {
	/**
	 * @brief According to Discord, quote, "The, uhm, default layout type."
	 */
	pl_default = 1
};

/**
 * @brief Structure representing a poll media, for example the poll question or a possible poll answer.
 *
 * @see https://discord.com/developers/docs/resources/poll#poll-media-object-poll-media-object-structure
 */
struct poll_media {
	/**
	 * @brief Text of the media
	 */
	std::string text{};

	/**
	 * @brief Emoji of the media.
	 */
	partial_emoji emoji{};
};

/**
 * @brief Represents an answer in a poll.
 *
 * @see https://discord.com/developers/docs/resources/poll#poll-answer-object-poll-answer-object-structure
 */
struct poll_answer {
	/**
	 * @brief ID of the answer. Only sent by the Discord API, this is a dead field when creating a poll.
	 *
	 * @warn At the time of writing the Discord API warns users not to rely on anything regarding sequence or "first value" of this field.
	 */
	uint32_t id{0};

	/**
	 * @brief Data of the answer.
	 *
	 * @see poll_media
	 */
	poll_media media{};
};

/**
 * @brief Represents the results of a poll
 *
 * @see https://discord.com/developers/docs/resources/poll#poll-results-object-poll-results-object-structure
 */
struct poll_results {
	/**
	 * @brief Represents a reference to an answer and its count of votes
	 *
	 * @see https://discord.com/developers/docs/resources/poll#poll-results-object-poll-answer-count-object-structure
	 */
	struct answer_count {
		/**
		 * @brief ID of the answer. Relates to an answer in the answers field
		 *
		 * @see poll_answer::answer_id
		 */
		uint32_t answer_id{0};

		/**
		 * @brief Number of votes for this answer
		 */
		uint32_t count{0};

		/**
		 * @brief Whether the current user voted
		 */
		bool me_voted{false};
	};

	/**
	 * @brief Whether the poll has finalized, and the answers are precisely counted
	 *
	 * @note Discord states that due to the way they count and cache answers,
	 * while a poll is running the count of answers might not be accurate.
	 */
	bool is_finalized{false};

	/**
	 * @brief Count of votes for each answer. If an answer is not present in this list,
	 * then its vote count is 0
	 */
	std::map<uint32_t, answer_count> answer_counts;
};

/**
 * @brief Represents a poll.
 *
 * @see https://discord.com/developers/docs/resources/poll
 */
struct DPP_EXPORT poll {
	/**
	 * @brief Poll question. At the time of writing only the text field is supported by Discord
	 *
	 * @see media
	 */
	poll_media question{};

	/**
	 * @brief List of answers of the poll.
	 *
	 * @note At the time of writing this can contain up to 10 answers
	 * @see answer
	 */
	std::map<uint32_t, poll_answer> answers{};

	/**
	 * @brief When retriving a poll from the API, this is the timestamp at which the poll will expire.
	 * When creating a poll, this is the number of hours the poll should be up for, up to 7 days (168 hours), and this field will be rounded.
	 */
	double expiry{24.0};

	/**
	 * @brief Whether a user can select multiple answers
	 */
	bool allow_multiselect{false};

	/**
	 * @brief Layout type of the poll. Defaults to, well, pl_default
	 *
	 * @see poll_layout_type
	 */
	poll_layout_type layout_type{pl_default};

	/**
	 * @brief The (optional) results of the poll. This field may or may not be present, and its absence means "unknown results", not "no results".
	 *
	 * @note Quote from Discord: "The results field may be not present in certain responses where, as an implementation detail,
	 * we do not fetch the poll results in our backend. This should be treated as "unknown results",
	 * as opposed to "no results". You can keep using the results if you have previously received them through other means."
	 *
	 * @see https://discord.com/developers/docs/resources/poll#poll-results-object
	 */
	std::optional<poll_results> results{std::nullopt};

	/**
	 * @brief Set the question for this poll
	 *
	 * @param text Text for the question
	 * @return self for method chaining
	 */
	poll& set_question(const std::string& text);

	/**
	 * @brief Set the duration of the poll in hours
	 *
	 * @param hours Duration of the poll in hours, max 7 days (168 hours) at the time of writing
	 * @return self for method chaining
	 */
	poll& set_duration(uint32_t hours) noexcept;

	/**
	 * @brief Set the duration of the poll in hours
	 *
	 * @param hours Duration of the poll in hours
	 * @return self for method chaining
	 */
	poll& set_allow_multiselect(bool allow) noexcept;

	/**
	 * @brief Add an answer to this poll
	 *
	 * @note At the time of writing this, a poll can have up to 10 answers
	 * @param media Data of the answer
	 * @return self for method chaining
	 */
	poll& add_answer(const poll_media& media);

	/**
	 * @brief Add an answer to this poll
	 *
	 * @note At the time of writing this, a poll can have up to 10 answers
	 * @param text Text for the answer
	 * @param emoji_id Optional emoji
	 * @param is_animated Whether the emoji is animated
	 * @return self for method chaining
	 */
	poll& add_answer(const std::string& text, snowflake emoji_id = 0, bool is_animated = false);

	/**
	 * @brief Add an answer to this poll
	 *
	 * @note At the time of writing this, a poll can have up to 10 answers
	 * @param text Text for the answer
	 * @param emoji Optional emoji
	 * @return self for method chaining
	 */
	poll& add_answer(const std::string& text, const std::string& emoji);

	/**
	 * @brief Add an answer to this poll
	 *
	 * @note At the time of writing this, a poll can have up to 10 answers
	 * @param text Text for the answer
	 * @param e Optional emoji
	 * @return self for method chaining
	 */
	poll& add_answer(const std::string& text, const emoji& e);

	/**
	 * @brief Helper to get the question text
	 *
	 * @return question.text
	 */
	[[nodiscard]] const std::string& get_question_text() const noexcept;

	/**
	 * @brief Helper to find an answer by ID
	 *
	 * @param id ID to find
	 * @return Pointer to the answer with the matching ID, or nullptr if not found
	 */
	[[nodiscard]] const poll_media* find_answer(uint32_t id) const noexcept;

	/**
	 * @brief Helper to find the vote count in the results
	 *
	 * @param answer_id ID of the answer to find
	 * @return std::optional<uint32_t> Optional count of votes. An empty optional means Discord did not send the results, it does not mean 0. It can also mean the poll does not have an answer with this ID
	 * @see https://discord.com/developers/docs/resources/poll#poll-results-object
	 */
	[[nodiscard]] std::optional<uint32_t> get_vote_count(uint32_t answer_id) const noexcept;
};

/**
 * @brief Bitmask flags for a dpp::message
 */
enum message_flags : uint16_t {
	/**
	 * @brief This message has been published to subscribed channels (via Channel Following).
	 */
	m_crossposted = 1 << 0,

	/**
	 * @brief This message originated from a message in another channel (via Channel Following).
	 */
	m_is_crosspost =  1 << 1,

	/**
	 * @brief Do not include any embeds when serializing this message.
	 */
	m_suppress_embeds = 1 << 2,

	/**
	 * @brief The source message for this crosspost has been deleted (via Channel Following).
	 */
	m_source_message_deleted = 1 << 3,

	/**
	 * @brief This message came from the urgent message system.
	 */
	m_urgent = 1 << 4,

	/**
	 * @brief This message has an associated thread, with the same id as the message.
	 */
	m_has_thread = 1 << 5,

	/**
	 * @brief This message is only visible to the user who invoked the Interaction.
	 */
	m_ephemeral = 1 << 6,

	/**
	 * @brief This message is an Interaction Response and the bot is "thinking".
	 */
	m_loading = 1 << 7,

	/**
	 * @brief This message failed to mention some roles and add their members to the thread.
	 */
	m_thread_mention_failed = 1 << 8,

	/**
	 * @brief This message will not trigger push and desktop notifications.
	 */
	m_suppress_notifications = 1 << 12,

	/**
	 * @brief This message is a voice message.
	 */
	m_is_voice_message = 1 << 13,
};

/**
 * @brief Represents possible values for the dpp::embed type field.
 * These are loosely defined by the API docs and do not influence how the client renders embeds.
 *
 * @note The only type a bot can send is dpp::embed_type::emt_rich.
 */
namespace embed_type {
	/**
	 * @brief Rich text
	 */
	const std::string emt_rich = "rich";

	/**
	 * @brief Image
	 */
	const std::string emt_image = "image";

	/**
	 * @brief Video link
	 */
	const std::string emt_video = "video";

	/**
	 * @brief Animated gif
	 */
	const std::string emt_gifv = "gifv";

	/**
	 * @brief Article
	 */
	const std::string emt_article = "article";

	/**
	 * @brief Link URL
	 */
	const std::string emt_link = "link";

	/**
	 * @brief Auto moderation filter
	 */
	const std::string emt_automod = "auto_moderation_message";
} // namespace embed_type

/**
 * @brief Message types for dpp::message::type
 */
enum message_type {
	/**
	 * @brief Default
	 */
	mt_default = 0,

	/**
	 * @brief Add recipient
	 */
	mt_recipient_add = 1,

	/**
	 * @brief Remove recipient
	 */
	mt_recipient_remove = 2,

	/**
	 * @brief Call
	 */
	mt_call	= 3,

	/**
	 * @brief Channel name change
	 */
	mt_channel_name_change = 4,

	/**
	 * @brief Channel icon change
	 */
	mt_channel_icon_change = 5,

	/**
	 * @brief Message pinned
	 */
	mt_channel_pinned_message = 6,

	/**
	 * @brief Member joined
	 */
	mt_guild_member_join = 7,

	/**
	 * @brief Boost
	 */
	mt_user_premium_guild_subscription = 8,

	/**
	 * @brief Boost level 1
	 */
	mt_user_premium_guild_subscription_tier_1 = 9,

	/**
	 * @brief Boost level 2
	 */
	mt_user_premium_guild_subscription_tier_2 = 10,

	/**
	 * @brief Boost level 3
	 */
	mt_user_premium_guild_subscription_tier_3 = 11,

	/**
	 * @brief Follow channel
	 */
	mt_channel_follow_add = 12,

	/**
	 * @brief Disqualified from discovery
	 */
	mt_guild_discovery_disqualified	= 14,

	/**
	 * @brief Re-qualified for discovery
	 */
	mt_guild_discovery_requalified = 15,

	/**
	 * @brief Discovery grace period warning 1
	 */
	mt_guild_discovery_grace_period_initial_warning	= 16,

	/**
	 * @brief Discovery grace period warning 2
	 */
	mt_guild_discovery_grace_period_final_warning = 17,

	/**
	 * @brief Thread Created
	 */
	mt_thread_created = 18,

	/**
	 * @brief Reply
	 */
	mt_reply = 19,

	/**
	 * @brief Application command
	 */
	mt_application_command = 20,

	/**
	 * @brief Thread starter message
	 */
	mt_thread_starter_message = 21,

	/**
	 * @brief Invite reminder
	 */
	mt_guild_invite_reminder = 22,

	/**
	 * @brief Context Menu Command
	 */
	mt_context_menu_command = 23,

	/**
	 * @brief Auto moderation action
	 */
	mt_auto_moderation_action = 24,

	/**
	 * @brief Role subscription purchase
	 */
	mt_role_subscription_purchase = 25,

	/**
	 * @brief Interaction premium upsell
	 */
	mt_interaction_premium_upsell = 26,

	/**
	 * @brief Stage start
	 */
	mt_stage_start = 27,

	/**
	 * @brief Stage end
	 */
	mt_stage_end = 28,

	/**
	 * @brief Stage speaker
	 */
	mt_stage_speaker = 29,

	/**
	 * @brief Stage topic
	 */
	mt_stage_topic = 31,

	/**
	 * @brief Guild application premium subscription
	 */
	mt_application_premium_subscription = 32,
};

/**
 * @brief Represents the caching policy of a cache in the library.
 */
enum cache_policy_setting_t {
	/**
	 * @brief request aggressively on seeing new guilds, and also store missing data from messages.
	 * This is the default behaviour and the least memory-efficient option. Memory usage will increase
	 * over time, initially quite rapidly, and then linearly over time. It is the least cpu-intensive
	 * setting.
	 */
	cp_aggressive = 0,

	/**
	 * @brief only cache when there is relevant activity, e.g. a message to the bot.
	 * This is a good middle-ground, memory usage will increase linearly over time.
	 */
	cp_lazy = 1,

	/**
	 * @brief Don't cache anything. Fill details when we see them.
	 * This is the most memory-efficient option but consumes more CPU time
	 */
	cp_none = 2
};

/**
 * @brief Represents the caching policy of the cluster.
 * 
 * Channels and guilds are always cached as these caches are used
 * internally by the library. The memory usage of these is minimal.
 * 
 * All default to 'aggressive' which means to actively attempt to cache,
 * going out of the way to fill the caches completely. On large bots this
 * can take a LOT of RAM.
 */
struct DPP_EXPORT cache_policy_t {
	/**
	 * @brief Caching policy for users and guild members
	 */
	cache_policy_setting_t user_policy = cp_aggressive;

	/**
	 * @brief Caching policy for emojis
	 */
	cache_policy_setting_t emoji_policy = cp_aggressive;

	/**
	 * @brief Caching policy for roles
	 */
	cache_policy_setting_t role_policy = cp_aggressive;

	/**
	 * @brief Caching policy for roles
	 */
	cache_policy_setting_t channel_policy = cp_aggressive;

	/**
	 * @brief Caching policy for roles
	 */
	cache_policy_setting_t guild_policy = cp_aggressive;
};

/**
 * @brief Contains a set of predefined cache policies for use when constructing a dpp::cluster
 */
namespace cache_policy {

	/**
	 * @brief A shortcut constant for all caching enabled for use in dpp::cluster constructor
	 */
	inline constexpr cache_policy_t cpol_default = { cp_aggressive, cp_aggressive, cp_aggressive, cp_aggressive, cp_aggressive };

	/**
	 * @brief A shortcut constant for a more balanced caching policy for use in dpp::cluster constructor
	 */
	inline constexpr cache_policy_t cpol_balanced = { cp_lazy, cp_lazy, cp_lazy, cp_aggressive, cp_aggressive };

	/**
	 * @brief A shortcut constant for all caching disabled for use in dpp::cluster constructor
	 */
	inline constexpr cache_policy_t cpol_none = { cp_none, cp_none, cp_none, cp_none, cp_none };

};

/**
 * @brief Represents messages sent and received on Discord
 */
struct DPP_EXPORT message : public managed, json_interface<message> {
protected:
	friend struct json_interface<message>;

	/** Read class values from json object
	 * @param j A json object to read from
	 * @return A reference to self
	 */
	inline message& fill_from_json_impl(nlohmann::json *j) {
		return fill_from_json(j, {cp_aggressive, cp_aggressive, cp_aggressive});
	}

	/** Build a JSON from this object.
	 * @param with_id True if an ID is to be included in the JSON
	 * @return JSON
	 */
	inline json to_json_impl(bool with_id = false) const {
		return to_json(with_id, false);
	}
public:
	/**
	 * @brief ID of the channel the message was sent in.
	 */
	snowflake channel_id;

	/**
	 * @brief Optional: ID of the guild the message was sent in.
	 */
	snowflake guild_id;

	/**
	 * @brief The author of this message.
	 *
	 * @warning This is not guaranteed to be a valid user.
	 */
	user author;

	/**
	 * @brief Optional: member properties for this message's author
	 */
	guild_member member;

	/**
	 * @brief Contents of the message.
	 */
	std::string content;

	/**
	 * @brief Message components.
	 */
	std::vector<dpp::component> components;

	/**
	 * @brief When this message was sent.
	 */
	time_t sent;

	/**
	 * @brief When this message was edited.
	 *
	 * @note This may be 0 if never edited.
	 */
	time_t edited;

	/**
	 * @brief Users specifically mentioned in the message.
	 */
	std::vector<std::pair<user, guild_member>> mentions;

	/**
	 * @brief Roles specifically mentioned in this message (only IDs currently).
	 */
	std::vector<snowflake> mention_roles;

	/**
	 * @brief Channels mentioned in the message.
	 *
	 * @warning Not all types supported.
	 *
	 * @note Discord: Only textual channels that are visible to everyone in a lurkable guild will ever be included.
	 * Only crossposted messages (via Channel Following) currently include mention_channels at all. (includes ID, Guild ID, Type, Name).
	 */
	std::vector<channel> mention_channels;

	/**
	 * @brief Any attached files.
	 */
	std::vector<attachment> attachments;

	/**
	 * @brief Up to 10 dpp::embed objects.
	 */
	std::vector<embed> embeds;

	/**
	 * @brief Optional: reactions to the message.
	 */
	std::vector<reaction> reactions;

	/**
	 * @brief Optional: Used for validating a message was sent.
	 */
	std::string nonce;

	/**
	 * @brief Optional: Webhook ID.
	 *
	 * @note If the message is generated by a webhook, its ID will be here. Otherwise, the field will be 0.
	 */
	snowflake webhook_id;

	/**
	 * @brief Partial stickers. Only id, name and format_type are filled
	 */
	std::vector<sticker> stickers;

	/**
	 * @brief An array of file data to use for uploading files.
	 *
	 * @note You should use dpp::message::add_file to add data to this!
	 */
	std::vector<message_file_data> file_data;

	/**
	 * @brief Reference to another message, e.g. a reply
	 */
	struct message_ref {
		/**
		 * @brief ID of the originating message.
		 */
		snowflake message_id;

		/**
		 * @brief ID of the originating message's channel.
		 */
		snowflake channel_id;

		/**
		 * @brief ID of the originating message's guild.
		 */
		snowflake guild_id;

		/**
		 * @brief when sending, whether to error if the referenced message doesn't exist instead of sending as a normal (non-reply) message.
		 * Default true.
		 */
		bool fail_if_not_exists;
	} message_reference;

	/**
	 * @brief Reference to an interaction
	 */
	struct message_interaction_struct{
		/**
		 * @brief ID of the interaction.
		 */
		snowflake id;

		/**
		 * @brief Type of interaction.
		 */
		uint8_t type;

		/**
		 * @brief Name of the application command.
		 */
		std::string name;

		/**
		 * @brief The user who invoked the interaction.
		 */
		user usr;
	} interaction;

	/**
	 * @brief Allowed mentions details
	 */
	struct allowed_ref {
		/**
		 * @brief Set to true to parse user mentions in the text. Default is false
		 */
		bool parse_users;

		/**
		 * @brief Set to true to at-everyone and at-here mentions in the text. Default is false
		 */
		bool parse_everyone;

		/**
		 * @brief Set to true to parse role mentions in the text. Default is false
		 */
		bool parse_roles;

		/**
		 * @brief Set to true to mention the user who sent the message this one is replying to. Default is false
		 */
		bool replied_user;

		/**
		 * @brief List of users to allow pings for 
		 */
		std::vector<snowflake> users;

		/**
		 * @brief List of roles to allow pings for 
		 */
		std::vector<snowflake> roles;
	} allowed_mentions;

	/**
	 * @brief The cluster which created this message object.
	 */
	class cluster* owner;

	/**
	 * @brief Message type.
	 */
	message_type type;

	/**
	 * @brief Flags made from dpp::message_flags
	 */
	uint16_t flags;

	/**
	 * @brief Whether this message is pinned.
	 */
	bool pinned;

	/**
	 * @brief Whether this was a TTS message.
	 */
	bool tts;

	/**
	 * @brief Whether this message mentions everyone.
	 */
	bool mention_everyone;

	/**
	 * @brief Optional poll attached to this message
	 */
	std::optional<poll> attached_poll;

	/**
	 * @brief Construct a new message object
	 */
	message();

	/*
	 * @brief Construct a new message object
	 * @param m Message to copy
	 */
	message(const message& m) = default;

	/*
	 * @brief Construct a new message object
	 * @param m Message to move
	 */
	message(message&& m) = default;

	/**
	 * @brief Construct a new message object
	 * @param o Owning cluster, passed down to various things such as dpp::attachment.
	 * Owning cluster is optional (can be nullptr) and if nulled, will prevent some functions
	 * such as attachment::download from functioning (they will throw, if used)
	 */
	message(class cluster* o);

	/**
	 * @brief Construct a new message object with a channel and content
	 *
	 * @param channel_id The channel to send the message to
	 * @param content The content of the message. It will be truncated to the maximum length of 4000 UTF-8 characters.
	 * @param type The message type to create
	 */
	message(snowflake channel_id, const std::string &content, message_type type = mt_default);

	/**
	 * @brief Construct a new message object with content
	 *
	 * @param _embed An embed to send
	 */
	message(const embed& _embed);

	/**
	 * @brief Construct a new message object with a channel and content
	 *
	 * @param channel_id The channel to send the message to
	 * @param _embed An embed to send
	 */
	message(snowflake channel_id, const embed& _embed);

	/**
	 * @brief Construct a new message object with content
	 *
	 * @param content The content of the message. It will be truncated to the maximum length of 4000 UTF-8 characters.
	 * @param type The message type to create
	 */
	message(const std::string &content, message_type type = mt_default);

	/**
	 * @brief Destroy the message object
	 */
	~message() override = default;

	/**
	 * @brief Copy a message object
	 *
	 * @param m Message to copy
	 * @return message& Reference to self
	 */
	message &operator=(const message& m) = default;

	/**
	 * @brief Move a message object
	 *
	 * @param m Message to move
	 * @return message& Reference to self
	 */
	message &operator=(message&& m) = default;

	/**
	 * @brief Set the original message reference for replies/crossposts
	 * 
	 * @param _message_id message id to reply to
	 * @param _guild_id guild id to reply to (optional)
	 * @param _channel_id channel id to reply to (optional)
	 * @param fail_if_not_exists true if the message send should fail if these values are invalid (optional)
	 * @return message& reference to self
	 */
	message& set_reference(snowflake _message_id, snowflake _guild_id = 0, snowflake _channel_id = 0, bool fail_if_not_exists = false);

	/**
	 * @brief Set the allowed mentions object for pings on the message
	 * 
	 * @param _parse_users whether or not to parse users in the message content or embeds, default false
	 * @param _parse_roles whether or not to parse roles in the message content or embeds, default false
	 * @param _parse_everyone whether or not to parse everyone/here in the message content or embeds, default false
	 * @param _replied_user if set to true and this is a reply, then ping the user we reply to, default false
	 * @param users list of user ids to allow pings for, default an empty vector
	 * @param roles list of role ids to allow pings for, default an empty vector
	 * @return message& reference to self
	 */
	message& set_allowed_mentions(bool _parse_users = false, bool _parse_roles = false, bool _parse_everyone = false, bool _replied_user = false, const std::vector<snowflake> &users = {}, const std::vector<snowflake> &roles = {});

	using json_interface<message>::fill_from_json;
	using json_interface<message>::to_json;

	/** Fill this object from json.
	 * @param j JSON object to fill from
	 * @param cp Cache policy for user records, whether or not we cache users when a message is received
	 * @return A reference to self
	 */
	message& fill_from_json(nlohmann::json* j, cache_policy_t cp);

	/** Build JSON from this object.
	 * @param with_id True if the ID is to be included in the built JSON
	 * @param is_interaction_response Set to true if this message is intended to be included in an interaction response.
	 * This will exclude some fields that are not valid in interactions at this time.
	 * @return The JSON text of the message
	 */
	virtual json to_json(bool with_id, bool is_interaction_response) const;

	/**
	 * @brief Returns true if the message was crossposted to other servers
	 * 
	 * @return true if crossposted
	 */
	bool is_crossposted() const;

	/**
	 * @brief Returns true if posted from other servers announcement channel via webhook
	 * 
	 * @return true if posted from other server
	 */
	bool is_crosspost() const;

	/**
	 * @brief True if embeds have been removed
	 * 
	 * @return true if embeds removed
	 */
	bool suppress_embeds() const;

	/**
	 * @brief Set whether embeds should be suppressed
	 *
	 * @param suppress whether embeds should be suppressed
	 * @return message& reference to self
	 */
	message& suppress_embeds(bool suppress);

	/**
	 * @brief True if source message was deleted
	 * 
	 * @return true if source message deleted
	 */
	bool is_source_message_deleted() const;

	/**
	 * @brief True if urgent
	 * 
	 * @return true if urgent
	 */
	bool is_urgent() const;

	/**
	 * @brief True if has thread attached
	 *
	 * @return true if has thread attached
	 */
	bool has_thread() const;

	/**
	 * @brief True if ephemeral (visible only to issuer of a slash command)
	 * 
	 * @return true if ephemeral
	 */
	bool is_ephemeral() const;

	/**
	 * @brief True if loading
	 * 
	 * @return true if loading
	 */
	bool is_loading() const;

	/**
	 * @brief Returns true if this message failed to mention some roles and add their members to the thread
	 *
	 * @return true if this message failed to mention some roles and add their members to the thread
	 */
	bool is_thread_mention_failed() const;

	/**
	 * @brief True if the message will not trigger push and desktop notifications
	 *
	 * @return True if notifications suppressed
	 */
	bool suppress_notifications() const;

	/**
	 * @brief True if the message is a voice message
	 *
	 * @return True if voice message
	 */
	bool is_voice_message() const;

	/**
	 * @brief Add a component (button) to message
	 * 
	 * @param c component to add
	 * @return message& reference to self
	 */
	message& add_component(const component& c);

	/**
	 * @brief Add an embed to message
	 * 
	 * @param e embed to add
	 * @return message& reference to self
	 */
	message& add_embed(const embed& e);

	/**
	 * @brief Add a sticker to this message
	 *
	 * As of writing this, a message can only contain up to 3 stickers
	 * @param s sticker to add
	 * @return message& reference to self
	 */
	message& add_sticker(const sticker& s);

	/**
	 * @brief Add a sticker to this message
	 *
	 * As of writing this, a message can only contain up to 3 stickers
	 * @param id id of the sticker to add
	 * @return message& reference to self
	 */
	message& add_sticker(const snowflake& id);

	/**
	 * @brief Set the flags
	 * 
	 * @param f flags to set from dpp::message_flags
	 * @return message& reference to self
	 */
	message& set_flags(uint16_t f);

	/**
	 * @brief Set the message type
	 * 
	 * @param t type to set
	 * @return message& reference to self
	 */
	message& set_type(message_type t);

	/**
	 * @brief Set the filename of the last file in list
	 * 
	 * @param fn filename
	 * @return message& reference to self
	 * @deprecated Use message::add_file instead
	 */
	message& set_filename(const std::string &fn);

	/**
	 * @brief Set the file content of the last file in list
	 * 
	 * @param fc raw file content contained in std::string
	 * @return message& reference to self
	 * @deprecated Use message::add_file instead
	 */
	message& set_file_content(const std::string &fc);

	/**
	 * @brief Add a file to the message
	 *
	 * @param filename filename
	 * @param filecontent raw file content contained in std::string
	 * @param filemimetype optional mime type of the file
	 * @return message& reference to self
	 */
	message& add_file(const std::string &filename, const std::string &filecontent, const std::string &filemimetype = "");

	/**
	 * @brief Set the message content
	 * 
	 * @param c message content. It will be truncated to the maximum length of 4000 UTF-8 characters.
	 * @return message& reference to self
	 */
	message& set_content(const std::string &c);
	
	/**
	 * @brief Set the channel id
	 * 
	 * @param _channel_id channel id
	 * @return message& reference to self
	 */
	message& set_channel_id(snowflake _channel_id);

	/**
	 * @brief Set the channel id
	 * 
	 * @param _guild_id channel id
	 * @return message& reference to self
	 */
	message& set_guild_id(snowflake _guild_id);

	/**
	 * @brief Returns true if the message is from a DM
	 * 
	 * @return true if message is a DM
	 */
	bool is_dm() const;

	/**
	 * @brief Returns true if message has remixed attachment
	 * 
	 * @return true if message has remixed attachment
	 */
	bool has_remix_attachment() const;

	/**
	 * @brief Returns URL to message 
	 * 
	 * @return string of URL to message
	 */
	std::string get_url() const;

	/**
	 * @brief Convenience method to set the poll
	 *
	 * @return message& Self reference for method chaining
	 */
	message& set_poll(const poll& p);

	/**
	 * @brief Convenience method to get the poll attached to this message
	 *
	 * @throw std::bad_optional_access if has_poll() == false
	 * @return const poll& Poll attached to this object
	 */
	[[nodiscard]] const poll& get_poll() const;

	/**
	 * @brief Method to check if the message has a poll
	 *
	 * @return bool Whether the message has a poll
	 */
	[[nodiscard]] bool has_poll() const noexcept;
};

/**
 * @brief A group of messages
 */
typedef std::unordered_map<snowflake, message> message_map;

/**
 * @brief A group of stickers
 */
typedef std::unordered_map<snowflake, sticker> sticker_map;

/**
 * @brief A group of sticker packs
 */
typedef std::unordered_map<snowflake, sticker_pack> sticker_pack_map;

} // namespace dpp
