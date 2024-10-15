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
#include <dpp/utility.h>
#include <dpp/stringops.h>
#include <dpp/exception.h>
#include <dpp/version.h>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>
#include <functional>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <streambuf>
#include <array>
#include <dpp/cluster.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <dpp/discordevents.h>

#ifdef _WIN32
	#include <stdio.h>
	#include <stdlib.h>
	#define popen _popen
	#define pclose _pclose
#endif

#ifdef HAVE_PRCTL
	#include <sys/prctl.h>
#endif

using namespace std::literals;

namespace dpp::utility {

constexpr int max_cdn_image_size{4096};
constexpr int min_cdn_image_size{16};

std::string cdn_endpoint_url(const std::vector<image_type> &allowed_formats, const std::string &path_without_extension, const dpp::image_type format, uint16_t size, bool prefer_animated, bool is_animated) {
	return cdn_endpoint_url_hash(allowed_formats, path_without_extension, "", format, size, prefer_animated, is_animated);
}

std::string cdn_endpoint_url_hash(const std::vector<image_type> &allowed_formats, const std::string &path_without_extension, const std::string &hash, const dpp::image_type format, uint16_t size, bool prefer_animated, bool is_animated) {

	if (std::find(allowed_formats.begin(), allowed_formats.end(), format) == allowed_formats.end()) {
		return std::string(); // if the given format is not allowed for this endpoint
	}

	std::string extension;
	if (is_animated && (prefer_animated || format == i_gif)) {
		extension = ".gif";
	} else if (format == i_png) {
		extension = ".png";
	} else if (format == i_jpg) {
		extension = ".jpg";
	} else if (format == i_webp) {
		extension = ".webp";
	} else {
		return std::string();
	}

	std::string suffix = (hash.empty() ? "" : (is_animated ? "/a_" : "/") + hash); // > In the case of endpoints that support GIFs, the hash will begin with a_ if it is available in GIF format.

	return cdn_host + '/' + path_without_extension + suffix + extension + utility::avatar_size(size);
}

std::string cdn_endpoint_url_sticker(snowflake sticker_id, sticker_format format) {
	if (!sticker_id) {
		return std::string();
	}

	std::string extension = file_extension(format);

	return extension.empty() ? std::string{} : (utility::cdn_host + "/stickers/" + std::to_string(sticker_id) + extension);
}

double time_f()
{
	using namespace std::chrono;
	auto tp = system_clock::now() + 0ns;
	return static_cast<double>(tp.time_since_epoch().count()) / 1000000000.0;
}

bool has_voice() {
#if HAVE_VOICE
	return true;
#else
	return false;
#endif
}

avx_type_t voice_avx() {
#if AVX_TYPE == 512
	return avx_512;
#elif AVX_TYPE == 2
	return avx_2;
#elif AVX_TYPE == 1
	return avx_1;
#else
	return avx_none;
#endif

}

bool is_coro_enabled() {
#ifdef DPP_CORO
	return true;
#else
	return false;
#endif
}

std::string current_date_time() {
#ifdef _WIN32
	std::time_t curr_time = time(nullptr);
	return trim(std::ctime(&curr_time));
#else
	auto t = std::time(nullptr);
	struct tm timedata;
	localtime_r(&t, &timedata);
	std::stringstream s;
	s << std::put_time(&timedata, "%Y-%m-%d %H:%M:%S");
	return trim(s.str());
#endif
}

std::string loglevel(dpp::loglevel in) {
	switch (in) {
		case dpp::ll_trace: return "TRACE";
		case dpp::ll_debug: return "DEBUG";
		case dpp::ll_info: return "INFO";
		case dpp::ll_warning: return "WARN";
		case dpp::ll_error: return "ERROR";
		case dpp::ll_critical: return "CRIT";
		default: return "???";
	}
}

uptime::uptime() : days(0), hours(0), mins(0), secs(0) {
}

uptime::uptime(double diff) : uptime((time_t)diff) {
}

uptime::uptime(time_t diff) : uptime() {
	days = (uint16_t)(diff / (3600 * 24));
	hours = (uint8_t)(diff % (3600 * 24) / 3600);
	mins = (uint8_t)(diff % 3600 / 60);
	secs = (uint8_t)(diff % 60);
}

std::string uptime::to_string() const {
	if (hours == 0 && days == 0) {
		char print_buffer[64];
		snprintf(print_buffer, 64, "%02d:%02d", mins, secs);
		return print_buffer;
	} else {
		char print_buffer[64];
		std::string daystr;
		if (days) {
			daystr = std::to_string(days) + " day" + (days > 1 ? "s, " : ", ");
		}
		snprintf(print_buffer, 64, "%s%02d:%02d:%02d", daystr.c_str(), hours, mins, secs);
		return print_buffer;
	}
}

uint64_t uptime::to_secs() const {
	return secs + (mins * 60) + (hours * 60 * 60) + (days * 60 * 60 * 24);
}

uint64_t uptime::to_msecs() const {
	return to_secs() * 1000;
}

iconhash::iconhash(uint64_t _first, uint64_t _second) noexcept : first(_first), second(_second) {
}

void iconhash::set(const std::string &hash) {
	std::string clean_hash(hash);
	if (hash.empty()) {	// Clear values if empty hash
		first = second = 0;
		return;
	}
	if (hash.length() == 34 && hash.substr(0, 2) == "a_") {
		/* Someone passed in an animated icon. numpty.
			* Clean that mess up!
			*/
		clean_hash = hash.substr(2);
	}
	if (clean_hash.length() != 32) {
		throw std::length_error("iconhash must be exactly 32 characters in length, passed value is: '" + clean_hash + "'");
	}
	this->first = from_string<uint64_t>(clean_hash.substr(0, 16), std::hex);
	this->second = from_string<uint64_t>(clean_hash.substr(16, 16), std::hex);
}

iconhash::iconhash(const std::string &hash) {
	set(hash);
}

iconhash& iconhash::operator=(const std::string &assignment) {
	set(assignment);
	return *this;
}

bool iconhash::operator==(const iconhash& other) const noexcept {
	return other.first == first && other.second == second;
}

std::string iconhash::to_string() const {
	if (first == 0 && second == 0) {
		return "";
	} else {
		return to_hex(this->first) + to_hex(this->second);
	}
}

namespace {
	std::unique_ptr<std::byte[]> copy_data(const std::byte* data, size_t size) {
		if (!data)
			return nullptr;
		std::unique_ptr<std::byte[]> ret = std::make_unique<std::byte[]>(size);

		std::copy_n(data, size, ret.get());
		return ret;
	}

	template <typename Range>
	std::unique_ptr<std::byte[]> copy_data(Range&& range) {
		return copy_data(reinterpret_cast<const std::byte*>(std::data(range)), std::size(range));
	}
}

image_data::image_data(const image_data& rhs) : data{copy_data(rhs.data.get(), rhs.size)}, size{rhs.size}, type{rhs.type} {
}

image_data::image_data(image_type format, std::string_view str) : data{copy_data(str)}, size{static_cast<uint32_t>(str.size())}, type{format} {
}

image_data::image_data(image_type format, const std::byte* data, uint32_t byte_size) : data{copy_data(data, byte_size)}, size{byte_size}, type{format} {
}

image_data& image_data::operator=(const image_data& rhs) {
	data = copy_data(rhs.data.get(), rhs.size);
	size = rhs.size;
	type = rhs.type;
	return *this;
}

void image_data::set(image_type format, std::string_view bytes) {
	data = copy_data(bytes);
	size = static_cast<uint32_t>(bytes.size());
}

void image_data::set(image_type format, const std::byte* bytes, uint32_t byte_size) {
	data = copy_data(bytes, size);
	size = static_cast<uint32_t>(byte_size);
}

std::string image_data::base64_encode() const {
	return dpp::base64_encode(reinterpret_cast<unsigned char*>(data.get()), size);
}

std::string image_data::get_file_extension() const {
	return utility::file_extension(type);
}

std::string image_data::get_mime_type() const {
	return utility::mime_type(type);
}

bool image_data::empty() const noexcept {
	return (size == 0);
}

json image_data::to_nullable_json() const {
	if (empty()) {
		return nullptr;
	}
	else {
		return "data:" + get_mime_type() + ";base64," + base64_encode();
	}
}

bool icon::is_iconhash() const {
	return std::holds_alternative<iconhash>(hash_or_data);
}

iconhash& icon::as_iconhash() & {
	return std::get<iconhash>(hash_or_data);
}

const iconhash& icon::as_iconhash() const& {
	return std::get<iconhash>(hash_or_data);
}

iconhash&& icon::as_iconhash() && {
	return std::move(std::get<iconhash>(hash_or_data));
}

icon& icon::operator=(const iconhash& hash) {
	hash_or_data = hash;
	return *this;
}

icon& icon::operator=(iconhash&& hash) noexcept {
	hash_or_data = std::move(hash);
	return *this;
}

icon& icon::operator=(const image_data& img) {
	hash_or_data = img;
	return *this;
}

icon& icon::operator=(image_data&& img) noexcept {
	hash_or_data = std::move(img);
	return *this;
}

bool icon::is_image_data() const {
	return std::holds_alternative<image_data>(hash_or_data);
}

image_data& icon::as_image_data() & {
	return std::get<image_data>(hash_or_data);
}

const image_data& icon::as_image_data() const& {
	return std::get<image_data>(hash_or_data);
}

image_data&& icon::as_image_data() && {
	return std::move(std::get<image_data>(hash_or_data));
}

std::string debug_dump(uint8_t* data, size_t length) {
	std::ostringstream out;
	size_t addr = (size_t)data;
	size_t extra = addr % 16;
	if (extra != 0) {
		addr -= extra;
		out << to_hex(addr);
	}
	for (size_t n = 0; n < extra; ++n) {
		out << "-- ";
	}
	std::string ascii;
	for (uint8_t* ptr = data; ptr < data + length; ++ptr) {
		if (((size_t)ptr % 16) == 0) {
			out << ascii << "\n[" << to_hex((size_t)ptr) << "] : ";
			ascii.clear();
		}
		ascii.push_back(*ptr >= ' ' && *ptr <= '~' ? *ptr : '.');
		out << to_hex(*ptr);
	}
	out << "    " << ascii;
	out << "\n";
	return out.str();
}

std::string bytes(uint64_t c) {
	char print_buffer[64] = { 0 };
	if (c > 1099511627776) {	// 1TB
		snprintf(print_buffer, 64, "%.2fT", (c / 1099511627776.0));
	} else if (c > 1073741824) {	// 1GB
		snprintf(print_buffer, 64, "%.2fG", (c / 1073741824.0));
	}  else if (c > 1048576) {	// 1MB
		snprintf(print_buffer, 64, "%.2fM", (c / 1048576.0));
	}  else if (c > 1024) {		// 1KB
		snprintf(print_buffer, 64, "%.2fK", (c / 1024.0));
	} else {			// Bytes
		return std::to_string(c);
	}
	return print_buffer;
}

uint32_t rgb(double red, double green, double blue) {
	return (((uint32_t)(red * 255)) << 16) | (((uint32_t)(green * 255)) << 8) | ((uint32_t)(blue * 255));
}

/* NOTE: Parameters here are `int` instead of `uint32_t` or `uint8_t` to prevent ambiguity error with rgb(float, float, float) */
uint32_t rgb(int red, int green, int blue) {
	return ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
}

uint32_t cmyk(double c, double m, double y, double k) {
	int r = (int)(255 * (1 - c) * (1 - k));
	int g = (int)(255 * (1 - m) * (1 - k));
	int b = (int)(255 * (1 - y) * (1 - k));
	return rgb(r, g, b);
}

/* NOTE: Parameters here are `int` instead of `uint32_t` or `uint8_t` to prevent ambiguity error with cmyk(float, float, float, float) */
uint32_t cmyk(int c, int m, int y, int k) {
	return cmyk(c / 255.0, m / 255.0, y / 255.0, k / 255.0);
}

uint32_t hsl(int h, int s, int l) {
	double hue = static_cast<double>(h) / 360.0;
	double saturation = static_cast<double>(s) / 100.0;
	double lightness = static_cast<double>(l) / 100.0;
	return hsl(hue, saturation, lightness);
}

uint32_t hsl(double h, double s, double l) {
	const auto hue_to_rgb = [](double p, double q, double t){
		if (t < 0) {
			t += 1;
		} else if (t > 1) {
			t -= 1;
		} 
		if (t < 1.0 / 6.0) {
			return p + (q - p) * 6.0 * t;
		} else if (t < 0.5) {
			return q;
		} else if (t < 2.0 / 3.0) {
			return p + (q - p) * (2.0 / 3.0 - t) * 6.0;
		}
		return p;
	};

	double r, g, b;

	if (s == 0) {
		r = g = b = l; // Gray scale
	} else {
		double q = l < 0.5 ? l * (1 + s) : l + s - l * s;
		double p = 2 * l - q;
		r = hue_to_rgb(p, q, h + 1.0 / 3.0);
		g = hue_to_rgb(p, q, h);
		b = hue_to_rgb(p, q, h - 1.0 / 3.0);
	}
	return rgb(r, g, b);
}

void exec(const std::string& cmd, std::vector<std::string> parameters, cmd_result_t callback) {
	auto t = std::thread([cmd, parameters, callback]() {
		utility::set_thread_name("async_exec");
		std::array<char, 128> buffer;
		std::vector<std::string> my_parameters = parameters;
		std::string result;
		std::stringstream cmd_and_parameters;
		cmd_and_parameters << cmd;
		for (auto & parameter : my_parameters) {
			cmd_and_parameters << " " << std::quoted(parameter);
		}
		/* Capture stderr */
		cmd_and_parameters << " 2>&1";
		std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_and_parameters.str().c_str(), "r"), pclose);
		if (!pipe) {
			return;
		}
		while (fgets(buffer.data(), (int)buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		if (callback) {
			callback(result);
		}
	});
	t.detach();
}

size_t utf8len(std::string_view str) {
	const size_t raw_len = str.length();
	size_t pos = 0;
	size_t code_points = 0;

	while (pos != raw_len) {
		const unsigned char cur = str[pos];

		size_t code_point_len = 1;
		code_point_len += static_cast<size_t>(cur >= 0b11000000);
		code_point_len += static_cast<size_t>(cur >= 0b11100000);
		code_point_len += static_cast<size_t>(cur >= 0b11110000);

		if (raw_len - pos < code_point_len) {
			return 0; // invalid utf8, avoid going past the end
		}
		pos += code_point_len;

		code_points += 1;
	}

	return code_points;
}

std::string_view utf8subview(std::string_view str, size_t start, size_t length) {
	const size_t raw_len = str.length();
	size_t pos = 0;
	size_t code_points = 0;

	size_t subview_start = raw_len;
	size_t subview_len = std::string_view::npos;

	while (pos != raw_len) {
		if (code_points == start) {
			subview_start = pos;
		}
		if (code_points == start + length) {
			subview_len = pos - subview_start;
			break; // no point in traversing the remainder of the string
		}

		const unsigned char cur = str[pos];

		size_t code_point_len = 1;
		code_point_len += static_cast<size_t>(cur >= 0b11000000);
		code_point_len += static_cast<size_t>(cur >= 0b11100000);
		code_point_len += static_cast<size_t>(cur >= 0b11110000);

		if (raw_len - pos < code_point_len) {
			return ""; // invalid utf8, avoid going past the end
		}
		pos += code_point_len;

		code_points += 1;
	}

	return str.substr(subview_start, subview_len);
}

std::string utf8substr(std::string_view str, size_t start, size_t length) {
	return std::string(utf8subview(str, start, length));
}

std::string read_file(const std::string& filename)
{
	try {
		std::ifstream ifs(filename, std::ios::binary);
		return std::string((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	}
	catch (const std::exception& e) {
		/* Rethrow as dpp::file_exception */
		throw dpp::file_exception(e.what());
	}
}

std::string validate(const std::string& value, size_t _min, size_t _max, const std::string& exception_message) {
	if (utf8len(value) < _min) {
		throw dpp::length_exception(exception_message);
	} else if (utf8len(value) > _max) {
		return utf8substr(value, 0, _max);
	}
	return value;
}


std::string timestamp(time_t ts, time_format tf) {
	char format[2] = { (char)tf, 0 };
	return "<t:" + std::to_string(ts) + ":" + format + ">";
}

std::string guild_navigation(const snowflake guild_id, guild_navigation_type gnt) {
	std::string type;
	switch (gnt) {
		case gnt_customize:
			type = "customize";
			break;
		case gnt_browse:
			type = "browse";
			break;
		case gnt_guide:
			type = "guide";
			break;
		default:
			return "";
	}
	return "<" + std::to_string(guild_id) + ":" + type + ">";
}

std::string avatar_size(uint32_t size) {
	if (size) {
		if ( (size & (size - 1)) != 0 ) { // check if the size is a power of 2
			return std::string();
		}
		if (size > max_cdn_image_size || size < min_cdn_image_size) {
			return std::string();
		}
		return "?size=" + std::to_string(size);
	}
	return std::string();
}

std::vector<std::string> tokenize(std::string const &in, const char* sep) {
	std::string::size_type b = 0;
	std::vector<std::string> result;

	while ((b = in.find_first_not_of(sep, b)) != std::string::npos) {
		auto e = in.find(sep, b);
		result.push_back(in.substr(b, e-b));
		b = e;
	}
	return result;
}

std::string bot_invite_url(const snowflake bot_id, const uint64_t permissions, const std::vector<std::string>& scopes) {
	std::string scope;
	if (scopes.size()) {
		for (const auto& s : scopes) {
			scope += s + "+";
		}
		scope.pop_back();
	}
	return "https://discord.com/oauth2/authorize?client_id=" + std::to_string(bot_id) + "&permissions=" + std::to_string(permissions) + "&scope=" + scope;
}

std::function<void(const dpp::log_t&)> cout_logger() {
	return [](const dpp::log_t& event) {
		if (event.severity > dpp::ll_trace) {
			std::cout << "[" << dpp::utility::current_date_time() << "] " << dpp::utility::loglevel(event.severity) << ": " << event.message << "\n";
		}
	};
}

std::function<void(const dpp::confirmation_callback_t& detail)> log_error() {
	return [](const dpp::confirmation_callback_t& detail) {
		if (detail.is_error()) {
			if (detail.bot) {
				try {
					error_info e = detail.get_error();
					detail.bot->log(
						dpp::ll_error,
						"Error: " + e.human_readable
					);
				}
				catch (const std::exception& e) {
					detail.bot->log(
						dpp::ll_error,
						"Error: " + std::string(e.what())
					);
				}
			}
		}
	};
}

/* Hexadecimal sequence for URL encoding */
static const char* hex = "0123456789ABCDEF";

std::string url_encode(const std::string &value) {
	// Reserve worst-case encoded length of string, input length * 3
	std::string escaped(value.length() * 3, '\0');
	char* data = escaped.data();
	int len = 0;
	for (auto i = value.begin(); i != value.end(); ++i) {
		unsigned char c = (unsigned char)(*i);
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			// Keep alphanumeric and other accepted characters intact
			*data++ = c;
			len++;
		} else {
			// Any other characters are percent-encoded
			*data++ = '%';
			*data++ = hex[c >> 4];
			*data++ = hex[c & 0x0f];
			len += 3;
		}
	}
	escaped.resize(len);
	return escaped;
}

std::string slashcommand_mention(snowflake command_id, const std::string &command_name, const std::string &subcommand) {
	return "</" + command_name + (!subcommand.empty() ? (" " + subcommand) : "") + ":" + std::to_string(command_id) + ">";
}

std::string slashcommand_mention(snowflake command_id, const std::string &command_name, const std::string &subcommand_group, const std::string &subcommand) {
	return "</" + command_name + " " + subcommand_group + " " + subcommand + ":" + std::to_string(command_id) + ">";
}

std::string user_mention(const snowflake& id) {
	return "<@" + std::to_string(id) + ">";
}

std::string channel_mention(const snowflake &id) {
	return "<#" + std::to_string(id) + ">";
}

std::string emoji_mention(std::string_view name, snowflake id, bool is_animated) {
	if (id) {
		std::string s{};

		s += '<';
		s += (is_animated ? "a:" : ":");
		s += name;
		s += ':';
		s += id.str();
		s += '>';
		return s;
	} else {
		return ":" + std::string{name} + ":";
	}
}

std::string role_mention(const snowflake &id) {
	return "<@&" + std::to_string(id) + ">";
}

std::string message_url(const snowflake& guild_id, const snowflake& channel_id, const snowflake& message_id){
	if (guild_id.empty() || channel_id.empty() || message_id.empty()) {
		return "";
	}
	return url_host + "/channels/" + std::to_string(guild_id) + "/" + std::to_string(channel_id) + "/" + std::to_string(message_id);
}

std::string channel_url(const snowflake& guild_id, const snowflake& channel_id){
	if (guild_id.empty() || channel_id.empty()) {
		return "";
	}
	return url_host + "/channels/" + std::to_string(guild_id) + "/" + std::to_string(channel_id);
}

std::string thread_url(const snowflake& guild_id, const snowflake& thread_id){
	return channel_url(guild_id, thread_id);
};

std::string user_url(const snowflake& user_id){
	if (user_id.empty()) {
		return "";
	}
	return url_host + "/users/" + std::to_string(user_id);
};

template <typename T>
std::enable_if_t<std::is_same_v<T, image_type>, std::string> mime_type(T type) {
	static constexpr auto get_image_mime = [](image_type t) constexpr noexcept {
		using namespace std::string_view_literals;

		switch (t) {
			case i_png:
				return "image/png"sv;

			case i_jpg:
				return "image/jpeg"sv;

			case i_gif:
				return "image/gif"sv;

			case i_webp:
				return "image/webp"sv;
		}
		return std::string_view{}; // unknown
	};

	return std::string{get_image_mime(type)};
}

// Explicit instantiation, shoves it into the DLL
template std::string mime_type<image_type>(image_type t);

template <typename T>
std::enable_if_t<std::is_same_v<T, sticker_format>, std::string> mime_type(T format) {
	static constexpr auto get_sticker_mime = [](sticker_format f) constexpr noexcept {
		using namespace std::string_view_literals;

		switch (f) {
			case sf_png:
				return "image/png"sv;

			case sf_apng:
				return "image/apng"sv;

			case sf_lottie:
				return "application/json"sv;

			case sf_gif:
				return "image/gif"sv;
		}
		return std::string_view{}; // unknown
	};
	return std::string{get_sticker_mime(format)};
}

template std::string mime_type<sticker_format>(sticker_format t);

template <typename T>
std::enable_if_t<std::is_same_v<T, image_type>, std::string> file_extension(T type) {
	static constexpr auto get_image_ext = [](image_type t) constexpr noexcept {
		using namespace std::string_view_literals;

		switch (t) {
			case i_png:
				return ".png"sv;

			case i_jpg:
				return ".jpg"sv;

			case i_gif:
				return ".gif"sv;

			case i_webp:
				return ".webp"sv;
		}
		return std::string_view{}; // unknown
	};

	return std::string{get_image_ext(type)};
}

template std::string file_extension<image_type>(image_type t);

template <typename T>
std::enable_if_t<std::is_same_v<T, sticker_format>, std::string> file_extension(T format) {
	static constexpr auto get_sticker_ext = [](sticker_format f) constexpr noexcept {
		using namespace std::string_view_literals;

		switch (f) {
			case sf_png:
			case sf_apng:
				return ".png"sv;

			case sf_lottie:
				return ".json"sv;

			case sf_gif:
				return ".gif"sv;
		}
		return std::string_view{}; // unknown
	};
	return std::string{get_sticker_ext(format)};
}

template std::string file_extension<sticker_format>(sticker_format t);

std::string make_url_parameters(const std::map<std::string, std::string>& parameters) {
	std::string output;
	for(auto& [k, v] : parameters) {
		if (!k.empty() && !v.empty()) {
			output.append("&").append(k).append("=").append(url_encode(v));
		}
	}
	if (!output.empty()) {
		output[0] = '?';
	}
	return output;
}

std::string make_url_parameters(const std::map<std::string, uint64_t>& parameters) {
	std::map<std::string, std::string> params;
	for(auto& [k, v] : parameters) {
		if (v != 0) {
			params[k] = std::to_string(v);
		}
	}
	return make_url_parameters(params);
}

std::string markdown_escape(const std::string& text, bool escape_code_blocks) {
	/**
	 * @brief Represents the current state of the finite state machine
	 * for the markdown_escape function.
	 */
	enum md_state {
		/**
		 * @brief Normal text
		 */
		md_normal = 0,

		/**
		 * @brief A paragraph code block, represented by three backticks.
		 */
		md_big_code_block = 1,

		/**
		 * @brief An inline code block, represented by one backtick.
		 */
		md_small_code_block = 2,
	};

	md_state state = md_normal;
	std::string output;
	const std::string markdown_chars("\\*_|~[]()>");

	for (size_t n = 0; n < text.length(); ++n) {
		if (text.substr(n, 3) == "```") {
			/* Start/end a paragraph code block */
			output += (escape_code_blocks ? "\\`\\`\\`" : "```");
			n += 2;
			state = (state == md_normal) ? md_big_code_block : md_normal;
		} else if (text[n] == '`' && (escape_code_blocks || state != md_big_code_block)) {
			/* Start/end of an inline code block */
			output += (escape_code_blocks ? "\\`" : "`");
			state = (state == md_normal) ? md_small_code_block : md_normal;
		} else {
			/* Normal text */
			if (escape_code_blocks || state == md_normal) {
				/* Markdown sequence characters */
				if (markdown_chars.find(text[n]) != std::string::npos) {
					output += "\\";
				}
			}
			output += text[n];
		}
	}
	return output;
}

std::string version() {
	return DPP_VERSION_TEXT;
}

void set_thread_name(const std::string& name) {
	#ifdef HAVE_PRCTL
		prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name.substr(0, 15).c_str()), NULL, NULL, NULL);
	#else
		#if HAVE_PTHREAD_SETNAME_NP
			#if HAVE_SINGLE_PARAMETER_SETNAME_NP
				pthread_setname_np(name.substr(0, 15).c_str());
			#endif
			#if HAVE_TWO_PARAMETER_SETNAME_NP
				pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
			#endif
		#endif
	#endif
}

} // namespace dpp::utility
