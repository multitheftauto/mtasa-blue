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
#include <dpp/misc-enum.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <functional>
#include <cstddef>
#include <variant>
#include <memory>
#include <string_view>

/**
 * @brief The main namespace for D++ functions, classes and types
 */
namespace dpp {

enum sticker_format : uint8_t;

/**
 * @brief Macro that expands to static_asserts checking sizeof and alignof are equal between two types.
 */
#define DPP_CHECK_ABI_COMPAT(a, b) \
static_assert(sizeof(a) == sizeof(b), #a " and " #b " must be the same size for ABI compatibility"); \
static_assert(alignof(a) == alignof(b), #a " and " #b " must be the same alignment for ABI compatibility"); \

/**
 * @brief Utility helper functions, generally for logging, running programs, time/date manipulation, etc
 */
namespace utility {

/**
 * @brief Helper function to easily create discord's cdn endpoint urls.
 * @warning **For internal use only!**
 *
 * @see https://discord.com/developers/docs/reference#image-formatting-cdn-endpoints
 * @param allowed_formats A vector of supported formats for the endpoint from the discord docs
 * @param path_without_extension The path for the endpoint (without the extension e.g. `.png`)
 * @param format the wished format to return. Must be one of the formats passed in `allowed_formats`, otherwise it returns an empty string
 * @param size the image size which will be appended as a querystring to the url.
 * It must be any power of two between 16 and 4096, otherwise no querystring will be appended (discord then returns the image as their default size)
 * @param prefer_animated Whether the user prefers gif format. If true, it'll return gif format whenever the image is available as animated.
 * In this case, the `format`-parameter is only used for non-animated images.
 * @param is_animated Whether the image is actually animated or not
 * @return std::string endpoint url or empty string
 */
std::string DPP_EXPORT cdn_endpoint_url(const std::vector<image_type> &allowed_formats, const std::string &path_without_extension, const dpp::image_type format, uint16_t size, bool prefer_animated = false, bool is_animated = false);

/**
 * @brief Helper function to easily create discord's cdn endpoint urls.
 * @warning **For internal use only!**
 *
 * @see https://discord.com/developers/docs/reference#image-formatting-cdn-endpoints
 * @param allowed_formats A vector of supported formats for the endpoint from the discord docs
 * @param path_without_extension The path for the endpoint (without the extension e.g. `.png`)
 * @param hash The hash (optional). If not empty, it will be prefixed with `a_` for animated images (`is_animated`=true)
 * @param format the wished format to return. Must be one of the formats passed in `allowed_formats`, otherwise it returns an empty string
 * @param size the image size which will be appended as a querystring to the url.
 * It must be any power of two between 16 and 4096, otherwise no querystring will be appended (discord then returns the image as their default size)
 * @param prefer_animated Whether the user prefers gif format. If true, it'll return gif format whenever the image is available as animated.
 * In this case, the `format`-parameter is only used for non-animated images.
 * @param is_animated Whether the image is actually animated or not
 * @return std::string endpoint url or empty string
 */
std::string DPP_EXPORT cdn_endpoint_url_hash(const std::vector<image_type> &allowed_formats, const std::string &path_without_extension, const std::string &hash, const dpp::image_type format, uint16_t size, bool prefer_animated = false, bool is_animated = false);

/**
 * @brief Helper function to easily create discord's cdn endpoint urls (specialised for stickers)
 * @warning **For internal use only!**
 *
 * @see https://discord.com/developers/docs/reference#image-formatting-cdn-endpoints
 * @param sticker_id The sticker ID. Returns empty string if 0
 * @param format The format type
 * @return std::string endpoint url or empty string
 */
std::string DPP_EXPORT cdn_endpoint_url_sticker(snowflake sticker_id, sticker_format format);

/**
 * @brief Supported AVX instruction set type for audio mixing
 */
enum avx_type_t : uint8_t {
	/**
	 * @brief No AVX Support
	 */
	avx_none,

	/**
	 * @brief AVX support
	 */
	avx_1,

	/**
	 * @brief AVX2 support
	 */
	avx_2,

	/**
	 * @brief AVX512 support
	 */
	avx_512,
};

/**
 * @brief Timestamp formats for dpp::utility::timestamp()
 * 
 * @note These values are the actual character values specified by the Discord API
 * and should not be changed unless the Discord API changes the specification!
 * They have been sorted into numerical order of their ASCII value to keep C++ happy.
 */
enum time_format : uint8_t {
	/**
	 * @brief "20 April 2021" - Long Date
	 */
	tf_long_date = 'D',

	/**
	 * @brief "Tuesday, 20 April 2021 16:20" - Long Date/Time
	 */
	tf_long_datetime = 'F',

	/**
	 * @brief "2 months ago" - Relative Time
	 */
	tf_relative_time = 'R',

	/**
	 * @brief "16:20:30" - Long Time
	 */
	tf_long_time = 'T',

	/**
	 * @brief "20/04/2021" - Short Date
	 */
	tf_short_date =	'd',

	/**
	 * @brief "20 April 2021 16:20" - Short Date/Time
	 */
	tf_short_datetime = 'f',

	/**
	 * @brief "16:20" - Short Time
	 */
	tf_short_time =	't',
};

/**
 * @brief Guild navigation types for dpp::utility::guild_navigation()
 */
enum guild_navigation_type {
	/**
	 * @brief _Customize_ tab with the server's dpp::onboarding_prompt
	 */
	gnt_customize,

	/**
	 * @brief "16:20" _Browse Channels_ tab
	 */
	gnt_browse,

	/**
	 * @brief Server Guide
	 */
	gnt_guide,
};

/**
 * @brief The base URL for CDN content such as profile pictures and guild icons.
 */
inline const std::string cdn_host = "https://cdn.discordapp.com"; 

/**
 * @brief The base URL for message/user/channel links.
 */
inline const std::string url_host = "https://discord.com"; 

/**
 * @brief Callback for the results of a command executed via dpp::utility::exec
 */
typedef std::function<void(const std::string& output)> cmd_result_t;

/**
 * @brief Run a commandline program asynchronously. The command line program
 * is spawned in a separate std::thread, and when complete, its output from
 * stdout is passed to the callback function in its string parameter. For example:
 * ```cpp
 * dpp::utility::exec("/bin/ls", {"-al"}, [](const std::string& output) {
 *     std::cout << "Output of 'ls -al': " << output << "\n";
 * });
 * ```
 * 
 * @param cmd The command to run.
 * @param parameters Command line parameters. Each will be escaped using `std::quoted`.
 * @param callback The callback to call on completion.
 */
void DPP_EXPORT exec(const std::string& cmd, std::vector<std::string> parameters = {}, cmd_result_t callback = {});

/**
 * @brief Return a mentionable timestamp (used in a message). These timestamps will display the given timestamp in the user's timezone and locale.
 * 
 * @param ts Time stamp to convert
 * @param tf Format of timestamp using dpp::utility::time_format
 * @return std::string The formatted timestamp
 */
std::string DPP_EXPORT timestamp(time_t ts, time_format tf = tf_short_datetime);

/**
 * @brief Create a mentionable guild navigation (used in a message).
 *
 * @param guild_id The guild ID
 * @param gnt Guild navigation type using dpp::utility::guild_navigation_type
 * @return std::string The formatted timestamp
 */
std::string DPP_EXPORT guild_navigation(const snowflake guild_id, guild_navigation_type gnt);

/**
 * @brief Returns current date and time
 * 
 * @return std::string Current date and time in "Y-m-d H:M:S" format
 */
std::string DPP_EXPORT current_date_time();
/**
 * @brief Convert a dpp::loglevel enum value to a string
 * 
 * @param in log level to convert
 * @return std::string string form of log level
 */
std::string DPP_EXPORT loglevel(dpp::loglevel in);

/**
 * @brief Store a 128 bit icon hash (profile picture, server icon etc)
 * as a 128 bit binary value made of two uint64_t.
 * Has a constructor to build one from a string, and a method to fetch
 * the value back in string form.
 */
struct DPP_EXPORT iconhash {
	/**
	 * @brief High 64 bits.
	 */
	uint64_t first;

	/**
	 * @brief Low 64 bits.
	 */
	uint64_t second;

	/**
	 * @brief Construct a new iconhash object
	 * @param _first Leftmost portion of the hash value
	 * @param _second Rightmost portion of the hash value
	 */
	iconhash(uint64_t _first = 0, uint64_t _second = 0) noexcept;

	/**
	 * @brief Construct a new iconhash object
	 *
	 * @param hash String hash to construct from.
	 * Must contain a 32 character hex string.
	 *
	 * @throws std::length_error if the provided
	 * string is not exactly 32 characters long.
	 */
	iconhash(const std::string &hash);

	/**
	 * @brief Assign from std::string
	 *
	 * @param assignment string to assign from.
	 *
	 * @throws std::length_error if the provided
	 * string is not exactly 32 characters long.
	 */
	iconhash& operator=(const std::string &assignment);

	/**
	 * @brief Check if one iconhash is equal to another
	 *
	 * @param other other iconhash to compare
	 * @return True if the iconhash objects match
	 */
	bool operator==(const iconhash& other) const noexcept;

	/**
	 * @brief Change value of iconhash object
	 *
	 * @param hash String hash to change to.
	 * Must contain a 32 character hex string.
	 *
	 * @throws std::length_error if the provided
	 * string is not exactly 32 characters long.
	 */
	void set(const std::string &hash);

	/**
	 * @brief Convert iconhash back to 32 character
	 * string value.
	 *
	 * @return std::string Hash value
	 */
	std::string to_string() const;
};

/**
 * @brief Image to be received or sent to API calls.
 *
 * This class is carefully crafted to be 16 bytes,
 * this is why we use a ptr + 4 byte size instead of a vector.
 * We want this class to be substitutable with iconhash in data structures.
 */
struct DPP_EXPORT image_data {
	/**
	 * @brief Data in bytes of the image.
	 */
	std::unique_ptr<std::byte[]> data = nullptr;

	/**
	 * @brief Size of the data in bytes.
	 */
	uint32_t size = 0;

	/**
	 * @brief Type of the image.
	 *
	 * @see image_type
	 */
	image_type type = {};

	/**
	 * @brief Construct an empty image.
	 */
	image_data() = default;

	/**
	 * @brief Copy an image.
	 *
	 * @param rhs Image to copy
	 */
	image_data(const image_data& rhs);

	/**
	 * @brief Move an image.
	 *
	 * @param rhs Image to copy
	 */
	image_data(image_data&& rhs) noexcept = default;

	/**
	 * @brief Construct from string buffer
	 *
	 * @param format Image format
	 * @param bytes Data in a string
	 * @see image_type
	 */
	image_data(image_type format, std::string_view bytes);

	/**
	 * @brief Construct from byte buffer
	 *
	 * @param format Image format
	 * @param bytes Data of the image
	 * @param byte_size Image size in bytes
	 * @see image_type
	 */
	image_data(image_type format, const std::byte* bytes, uint32_t byte_size);

	/**
	 * @brief Copy an image data.
	 *
	 * @param rhs Image to copy
	 * @return self for chaining
	 */
	image_data& operator=(const image_data& rhs);

	/**
	 * @brief Move an image data.
	 *
	 * @param rhs Image to move from
	 * @return self for chaining
	 */
	image_data& operator=(image_data&& rhs) noexcept = default;

	/**
	 * @brief Set image data.
	 *
	 * @param format Image format
	 * @param bytes Data of the image
	 */
	void set(image_type format, std::string_view bytes);

	/**
	 * @brief Set image data.
	 *
	 * @param format Format of the image
	 * @param bytes Data of the image
	 * @param byte_size Image size in bytes
	 */
	void set(image_type format, const std::byte* bytes, uint32_t byte_size);

	/**
	 * @brief Encode to base64.
	 *
	 * @return std::string New string with the image data encoded in base64
	 */
	std::string base64_encode() const;

	/**
	 * @brief Get the file extension.
	 *
	 * Alias for \ref file_extension
	 * @return std::string File extension e.g. `.png`
	 */
	std::string get_file_extension() const;

	/**
	 * @brief Get the mime type.
	 *
	 * Alias for \ref mime_type
	 * @return std::string File mime type e.g. "image/png"
	 */
	std::string get_mime_type() const;

	/**
	 * @brief Check if this is an empty image.
	 *
	 * @return bool Whether the image is empty or not
	 */
	bool empty() const noexcept;

	/**
	 * @brief Build a data URI scheme suitable for sending to Discord
	 *
	 * @see https://discord.com/developers/docs/reference#image-data
	 * @return The data URI scheme as a json or null if empty
	 */
	json to_nullable_json() const;
};

/**
 * @brief Wrapper class around a variant for either iconhash or image,
 * for API objects that have one or the other (generally iconhash when receiving,
 * image when uploading an image)
 */
struct icon {
	/**
	 * @brief Iconhash received or image data for upload.
	 */
	std::variant<std::monostate, iconhash, image_data> hash_or_data;

	/**
	 * @brief Assign to iconhash.
	 *
	 * @param hash Iconhash
	 */
	icon& operator=(const iconhash& hash);

	/**
	 * @brief Assign to iconhash.
	 *
	 * @param hash Iconhash
	 */
	icon& operator=(iconhash&& hash) noexcept;

	/**
	 * @brief Assign to image.
	 *
	 * @param img Image
	 */
	icon& operator=(const image_data& img);

	/**
	 * @brief Assign to image.
	 *
	 * @param img Image
	 */
	icon& operator=(image_data&& img) noexcept;

	/**
	 * @brief Check whether this icon is stored as an iconhash
	 *
	 * @see iconhash
	 * @return bool Whether this icon is stored as an iconhash
	 */
	bool is_iconhash() const;

	/**
	 * @brief Get as icon hash.
	 *
	 * @warning The behavior is undefined if `is_iconhash() == false`
	 * @return iconhash& This iconhash
	 */
	iconhash& as_iconhash() &;

	/**
	 * @brief Get as icon hash.
	 *
	 * @warning The behavior is undefined if `is_iconhash() == false`
	 * @return iconhash& This iconhash
	 */
	const iconhash& as_iconhash() const&;

	/**
	 * @brief Get as icon hash.
	 *
	 * @warning The behavior is undefined if `is_iconhash() == false`
	 * @return iconhash& This iconhash
	 */
	iconhash&& as_iconhash() &&;

	/**
	 * @brief Check whether this icon is stored as an image
	 *
	 * @see image_data
	 * @return bool Whether this icon is stored as an image
	 */
	bool is_image_data() const;

	/**
	 * @brief Get as image data.
	 *
	 * @warning The behavior is undefined if `is_image_data() == false`
	 * @return image_data& This image
	 */
	image_data& as_image_data() &;

	/**
	 * @brief Get as image.
	 *
	 * @warning The behavior is undefined if `is_image_data() == false`
	 * @return image_data& This image
	 */
	const image_data& as_image_data() const&;

	/**
	 * @brief Get as image.
	 *
	 * @warning The behavior is undefined if `is_image_data() == false`
	 * @return image_data& This image
	 */
	image_data&& as_image_data() &&;
};

/**
 * @brief Return the current time with fractions of seconds.
 * This is a unix epoch time with the fractional seconds part
 * after the decimal place.
 * 
 * @return double time with fractional seconds
 */
double DPP_EXPORT time_f();

/**
 * @brief Returns true if D++ was built with voice support
 * 
 * @return bool True if voice support is compiled in (libsodium/libopus) 
 */
bool DPP_EXPORT has_voice();

/**
 * @brief Returns an enum value indicating which AVX instruction
 * set is used for mixing received voice data, if any
 * 
 * @return avx_type_t AVX type
 */
avx_type_t DPP_EXPORT voice_avx();

/**
 * @brief Returns true if D++ was built with coroutine support
 * 
 * @return bool True if coroutines are supported 
 */
bool DPP_EXPORT is_coro_enabled();

/**
 * @brief Convert a byte count to display value
 * 
 * @param c number of bytes
 * @return std::string display value suffixed with M, G, T where necessary
 */
std::string DPP_EXPORT bytes(uint64_t c);

/**
 * @brief A class used to represent an uptime in hours, minutes,
 * seconds and days, with helper functions to convert from time_t
 * and display as a string.
 */
struct DPP_EXPORT uptime {
	/**
	 * @brief Number of days.
	 */
	uint16_t days;

	/**
	 * @brief Number of hours.
	 */
	uint8_t hours;

	/**
	 * @brief Number of minutes.
	 */
	uint8_t mins;

	/**
	 * @brief Number of seconds.
	 */
	uint8_t secs;

	/**
	 * @brief Construct a new uptime object
	 */
	uptime();

	/**
	 * @brief Construct a new uptime object
	 * 
	 * @param diff A time_t to initialise the object from
	 */
	uptime(time_t diff);

	/**
	 * @brief Construct a new uptime object
	 * 
	 * @param diff A time_t to initialise the object from
	 */
	uptime(double diff);

	/**
	 * @brief Get uptime as string
	 * 
	 * @return std::string Uptime as string
	 */
	std::string to_string() const;

	/**
	 * @brief Get uptime as seconds
	 * 
	 * @return uint64_t Uptime as seconds
	 */
	uint64_t to_secs() const;

	/**
	 * @brief Get uptime as milliseconds
	 * 
	 * @return uint64_t Uptime as milliseconds
	 */
	uint64_t to_msecs() const;
};

/**
 * @brief Convert doubles to RGB for sending in embeds
 * 
 * @param red red value, between 0 and 1 inclusive
 * @param green green value, between 0 and 1 inclusive
 * @param blue blue value, between 0 and 1 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT rgb(double red, double green, double blue);

/**
 * @brief Convert ints to RGB for sending in embeds
 *
 * @param red red value, between 0 and 255 inclusive
 * @param green green value, between 0 and 255 inclusive
 * @param blue blue value, between 0 and 255 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT rgb(int red, int green, int blue);

/**
 * @brief Convert doubles to CMYK for sending in embeds
 *
 * @param c cyan value, between 0 and 1 inclusive
 * @param m magenta value, between 0 and 1 inclusive
 * @param y yellow value, between 0 and 1 inclusive
 * @param k key (black) value, between 0 and 1 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT cmyk(double c, double m, double y, double k);

/**
 * @brief Convert ints to CMYK for sending in embeds
 *
 * @param c cyan value, between 0 and 255 inclusive
 * @param m magenta value, between 0 and 255 inclusive
 * @param y yellow value, between 0 and 255 inclusive
 * @param k key (black) value, between 0 and 255 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT cmyk(int c, int m, int y, int k);

/**
 * @brief Convert doubles to HSL for sending in embeds
 *
 * @param h hue value, between 0 and 1 inclusive
 * @param s saturation value in percentage, between 0 and 1 inclusive
 * @param l lightness value in percentage, between 0 and 1 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT hsl(double h, double s, double l);

/**
 * @brief Convert ints to HSL for sending in embeds
 *
 * @param h hue value, between 0 and 360 inclusive
 * @param s saturation value in percentage, between 0 and 100 inclusive
 * @param l lightness value in percentage, between 0 and 100 inclusive
 * @return uint32_t returned integer colour value
 */
uint32_t DPP_EXPORT hsl(int h, int s, int l);

/**
 * @brief Output hex values of a section of memory for debugging
 * 
 * @param data The start of the data to display
 * @param length The length of data to display
 */
std::string DPP_EXPORT debug_dump(uint8_t* data, size_t length);

/**
 * @brief Returns the length of a UTF-8 string in codepoints.
 * @note Result is unspecified for strings that are not valid UTF-8.
 *
 * @param str string to count length of
 * @return size_t Length of string
 */
size_t DPP_EXPORT utf8len(std::string_view str);

/**
 * @brief Return subview of a UTF-8 encoded string in codepoints.
 * @note You must ensure that the resulting view is not used after the lifetime of the viewed string has ended.
 * @note Result is unspecified for strings that are not valid UTF-8.
 *
 * @param str string to return substring from
 * @param start start codepoint offset
 * @param length length in codepoints
 * @return std::string_view The requested subview
 */
std::string_view DPP_EXPORT utf8subview(std::string_view str, size_t start, size_t length);

/**
 * @brief Return substring of a UTF-8 encoded string in codepoints.
 * @note Result is unspecified for strings that are not valid UTF-8.
 *
 * @param str string to return substring from
 * @param start start codepoint offset
 * @param length length in codepoints
 * @return std::string The requested substring
 */
std::string DPP_EXPORT utf8substr(std::string_view str, size_t start, size_t length);

/**
 * @brief Read a whole file into a std::string.
 * @note This function can take a while if this is a large file, causing events to not reply and also taking up a lot of memory. Make sure you have enough memory, and use dpp::interaction_create_t::thinking in events where you call this function on big files.
 * @warning Be aware this function can block! If you are regularly reading large files, consider caching them.
 * @param filename The path to the file to read
 * @return std::string The file contents
 * @throw dpp::file_exception on failure to read the entire file
 */
std::string DPP_EXPORT read_file(const std::string& filename);

/**
 * @brief Validate a string value
 * In the event the length of the string is less than _min, then an exception of type dpp:length_exception
 * will be thrown. If the string is longer than _max UTF8 codepoints it will be truncated to fit.
 * 
 * @param value The value to validate
 * @param _min Minimum length
 * @param _max Maximum length
 * @param exception_message Exception message to throw if value length < _min
 * @return std::string Validated string, truncated if necessary.
 * @throw dpp::length_exception if value UTF8 length < _min
 */
std::string DPP_EXPORT validate(const std::string& value, size_t _min, size_t _max, const std::string& exception_message);

/**
 * @brief Get the url query parameter for the cdn endpoint. Internally used to build url getters.
 * 
 * @param size size to generate url parameter for. Must be any power of two between 16 and 4096 (inclusive) or it'll return an empty string.
 * @return std::string url query parameter e.g. `?size=128`, or an empty string
 */
std::string DPP_EXPORT avatar_size(uint32_t size);

/**
 * @brief Split (tokenize) a string into a vector, using the given separators
 * 
 * @param in Input string
 * @param sep Separator characters
 * @return std::vector<std::string> Tokenized strings 
 */
std::vector<std::string> DPP_EXPORT tokenize(std::string const &in, const char* sep = "\r\n");

/**
 * @brief Create a bot invite
 * 
 * @param bot_id Bot ID
 * @param permissions Permission bitmask of the bot to invite
 * @param scopes Scopes to use
 * @return Invite URL
 */
std::string DPP_EXPORT bot_invite_url(const snowflake bot_id, const uint64_t permissions = 0, const std::vector<std::string>& scopes = {"bot", "applications.commands"});

/**
 * @brief Escapes Discord's markdown sequences in a string
 * 
 * @param text Text to escape
 * @param escape_code_blocks If set to false, then code blocks are not escaped.
 * This means that you can still use a code block, and the text within will be left as-is.
 * If set to true, code blocks will also be escaped so that ` symbol may be used as a normal
 * character.
 * @return std::string The text with the markdown special characters escaped with a backslash
 */
std::string DPP_EXPORT markdown_escape(const std::string& text, bool escape_code_blocks = false);

/**
 * @brief Encodes a url parameter similar to [php urlencode()](https://www.php.net/manual/en/function.urlencode.php)
 * 
 * @param value String to encode
 * @return std::string URL encoded string
 */
std::string DPP_EXPORT url_encode(const std::string &value);

/**
 * @brief Create a mentionable slashcommand (used in a message).
 * @param command_id The ID of the slashcommand
 * @param command_name The command name
 * @param subcommand Optional: The subcommand name (for mentioning a subcommand)
 * @return std::string The formatted mention
 */
std::string DPP_EXPORT slashcommand_mention(snowflake command_id, const std::string &command_name, const std::string &subcommand = "");

/**
 * @brief Create a mentionable slashcommand (used in a message).
 * @param command_id The ID of the slashcommand
 * @param command_name The command name
 * @param subcommand_group The subcommand group name
 * @param subcommand The subcommand name
 * @return std::string The formatted mention of the slashcommand with its subcommand
 */
std::string DPP_EXPORT slashcommand_mention(snowflake command_id, const std::string &command_name, const std::string &subcommand_group, const std::string &subcommand);

/**
 * @brief Create a mentionable user.
 * @param id The ID of the user.
 * @return std::string The formatted mention of the user.
 */
std::string DPP_EXPORT user_mention(const snowflake& id);

/**
* @brief Create a mentionable channel.
* @param id The ID of the channel.
* @return std::string The formatted mention of the channel.
*/
std::string DPP_EXPORT channel_mention(const snowflake& id);

/**
* @brief Create a mentionable emoji
* @param name The name of the emoji.
* @param id The ID of the emoji.
* @param is_animated is emoji animated.
* @return std::string The formatted mention of the emoji.
*/
std::string DPP_EXPORT emoji_mention(std::string_view name, snowflake id, bool is_animated = false);

/**
* @brief Create a mentionable role.
* @param id The ID of the role.
* @return std::string The formatted mention of the role.
*/
std::string DPP_EXPORT role_mention(const snowflake& id);

/**
* @brief Create a URL for message.
* @param guild_id The ID of the guild where message is written.
* @param channel_id The ID of the channel where message is written.
* @param message_id The ID of the message.
* @return std::string The URL to message or empty string if any of ids is 0.
*/
std::string DPP_EXPORT message_url(const snowflake& guild_id, const snowflake& channel_id, const snowflake& message_id);

/**
* @brief Create a URL for message.
* @param guild_id The ID of the guild where channel is located.
* @param channel_id The ID of the channel.
* @return std::string The URL to message or empty string if any of ids is 0.
*/
std::string DPP_EXPORT channel_url(const snowflake& guild_id, const snowflake& channel_id);

/**
* @brief Create a URL for message.
* @param guild_id The ID of the guild where thread is located.
* @param thread_id The ID of the thread.
* @return std::string The URL to message or empty string if any of ids is 0.
*/
std::string DPP_EXPORT thread_url(const snowflake& guild_id, const snowflake& thread_id);

/**
* @brief Create a URL for message.
* @param user_id The ID of the guild where thread is located.
* @return std::string The URL to message or empty string if id is 0.
*/
std::string DPP_EXPORT user_url(const snowflake& user_id);



#ifdef _DOXYGEN_
/**
 * @brief Get the mime type for an image type.
 * @param type Image type
 * @return std::string The mime type for this image type
 */
std::string DPP_EXPORT mime_type(image_type type);

/**
 * @brief Get the mime type for a sticker format.
 * @param format Sticker format
 * @return std::string The mime type for this sticker format
 */
std::string DPP_EXPORT mime_type(sticker_format format);

/**
 * @brief Get the file extension for an image type.
 * @param type Image type
 * @return std::string The file extension (e.g. ".png") for this image type
 */
std::string DPP_EXPORT file_extension(image_type type);

/**
 * @brief Get the file extension for a sticker format.
 * @param format Sticker format
 * @return std::string The file extension (e.g. ".png") for this sticker format
 */
std::string DPP_EXPORT file_extension(sticker_format format);
#else
/**
 * @brief Get the mime type for an image type.
 * @param type Image type
 * @return std::string The mime type for this image type
 */
template <typename T>
extern std::enable_if_t<std::is_same_v<T, image_type>, std::string> DPP_EXPORT mime_type(T type);

/**
 * @brief Get the mime type for a sticker format.
 * @param format Sticker format
 * @return std::string The mime type for this sticker format
 */
template <typename T>
extern std::enable_if_t<std::is_same_v<T, sticker_format>, std::string> DPP_EXPORT mime_type(T format);

/**
 * @brief Get the file extension for an image type.
 * @param type Image type
 * @return std::string The file extension (e.g. ".png") for this image type
 */
template <typename T>
extern std::enable_if_t<std::is_same_v<T, image_type>, std::string> DPP_EXPORT file_extension(T type);

/**
 * @brief Get the file extension for a sticker format.
 * @param format Sticker format
 * @return std::string The file extension (e.g. ".png") for this sticker format
 */
template <typename T>
extern std::enable_if_t<std::is_same_v<T, sticker_format>, std::string> DPP_EXPORT file_extension(T format);
#endif

/**
 * @brief Returns the library's version string
 * 
 * @return std::string version
 */
std::string DPP_EXPORT version();

/**
 * @brief Build a URL parameter string e.g. "?a=b&c=d&e=f" from a map of key/value pairs.
 * Entries with empty key names or values are omitted.
 * 
 * @param parameters parameters to create a url query string for
 * @return std::string A correctly encoded url query string
 */
std::string DPP_EXPORT make_url_parameters(const std::map<std::string, std::string>& parameters);

/**
 * @brief Build a URL parameter string e.g. "?a=b&c=d&e=f" from a map of key/value pairs.
 * Entries with empty key names or zero values are omitted.
 * 
 * @param parameters parameters to create a url query string for
 * @return std::string A correctly encoded url query string
 */
std::string DPP_EXPORT make_url_parameters(const std::map<std::string, uint64_t>& parameters);

/**
 * @brief Set the name of the current thread for debugging and statistical reporting
 * 
 * @param name New name to set
 */
void DPP_EXPORT set_thread_name(const std::string& name);

#ifdef __cpp_concepts // if c++20
/**
 * @brief Concept satisfied if a callable F can be called using the arguments Args, and that its return value is convertible to R.
 *
 * @tparam F Callable object
 * @tparam R Return type to check for convertibility to
 * @tparam Args... Arguments to use to resolve the overload
 * @return Whether the expression `F(Args...)` is convertible to R
 */
template <typename F, typename R, typename... Args>
concept callable_returns = std::convertible_to<std::invoke_result_t<F, Args...>, R>;

/**
 * @brief Type trait to check if a callable F can be called using the arguments Args, and that its return value is convertible to R.
 *
 * @deprecated In C++20 mode, prefer using the concept `callable_returns`.
 * @tparam F Callable object
 * @tparam R Return type to check for convertibility to
 * @tparam Args... Arguments to use to resolve the overload
 * @return Whether the expression `F(Args...)` is convertible to R
 */
template <typename F, typename R, typename... Args>
inline constexpr bool callable_returns_v = callable_returns<F, R, Args...>;
#else
/**
 * @brief Type trait to check if a callable F can be called using the arguments Args, and that its return value is convertible to R.
 *
 * @tparam F Callable object
 * @tparam R Return type to check for convertibility to
 * @tparam Args... Arguments to use to resolve the overload
 * @return Whether the expression `F(Args...)` is convertible to R
 */
template <typename F, typename R, typename... Args>
inline constexpr bool callable_returns_v = std::is_convertible_v<std::invoke_result_t<F, Args...>, R>;
#endif

/**
 * @brief Utility struct that has the same size and alignment as another but does nothing. Useful for ABI compatibility.
 *
 * @tparam T Type to mimic
 */
template <typename T>
struct alignas(T) dummy {
	/** @brief Array of bytes with a size mimicking T */
	std::array<std::byte, sizeof(T)> data;
};

} // namespace utility
} // namespace dpp
