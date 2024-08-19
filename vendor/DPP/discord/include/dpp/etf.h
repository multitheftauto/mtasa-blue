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
 * Parts of this file inspired by, or outright copied from erlpack:
 * https://github.com/discord/erlpack/
 * 
 * Acknowledgements:
 *
 *   sysdep.h:
 *       Based on work by FURUHASHI Sadayuki in msgpack-python
 *       (https://github.com/msgpack/msgpack-python)
 *
 *       Copyright (C) 2008-2010 FURUHASHI Sadayuki
 *       Licensed under the Apache License, Version 2.0 (the "License").
 *
 ************************************************************************************/

#pragma once
#include <dpp/export.h>
#include <dpp/snowflake.h>
#include <dpp/json_fwd.h>

namespace dpp {

/**
 * @brief Current ETF format version in use
 */
const uint8_t FORMAT_VERSION = 131;

/**
 * @brief Represents a token which identifies the type of value which follows it
 * in the ETF binary structure.
 */
enum etf_token_type : uint8_t {
	/**
	 * @brief 68 [Distribution header]
	 */
	ett_distribution = 'D',

	/**
	 * @brief 70 [Float64:IEEE float]
	 */
	ett_new_float =	'F',

	/**
	 * @brief 77 [UInt32:Len, UInt8:Bits, Len:Data]
	 */
	ett_bit_binary = 'M',

	/**
	 * @brief 80 [UInt4:UncompressedSize, N:ZlibCompressedData]
	 */
	ett_compressed = 'P',

	/**
	 * @brief 97 [UInt8:Int]
	 */
	ett_smallint = 'a',

	/**
	 * @brief 98 [Int32:Int]
	 */
	ett_integer = 'b',

	/**
	 * @brief 99 [31:Float String] Float in string format (formatted "%.20e", sscanf "%lf").
	 *
	 * @note Superseded by ett_new_float.
	 */
	ett_float = 'c',

	/**
	 * @brief 100 [UInt16:Len, Len:AtomName] max Len is 255
	 */
	ett_atom = 'd',

	/**
	 * @brief 101 [atom:Node, UInt32:ID, UInt8:Creation]
	 */
	ett_reference = 'e',

	/**
	 * @brief 102 [atom:Node, UInt32:ID, UInt8:Creation]
	 */
	ett_port = 'f',

	/**
	 * @brief 103 [atom:Node, UInt32:ID, UInt32:Serial, UInt8:Creation]
	 */
	ett_pid = 'g',

	/**
	 * @brief 104 [UInt8:Arity, N:Elements]
	 */
	ett_small_tuple = 'h',

	/**
	 * @brief 105 [UInt32:Arity, N:Elements]
	 */
	ett_large_tuple = 'i',

	/**
	 * @brief 106 empty list
	 */
	ett_nil = 'j',

	/**
	 * @brief 107 [UInt16:Len, Len:Characters]
	 */
	ett_string = 'k',

	/**
	 * @brief 108 [UInt32:Len, Elements, Tail]
	 */
	ett_list = 'l',

	/**
	 * @brief 109 [UInt32:Len, Len:Data]
	 */
	ett_binary = 'm',

	/**
	 * @brief 110 [UInt8:n, UInt8:Sign, n:nums]
	 */
	ett_bigint_small = 'n',

	/**
	 * @brief 111 [UInt32:n, UInt8:Sign, n:nums]
	 */
	ett_bigint_large = 'o',

	/**
	 * @brief 112 [UInt32:Size, UInt8:Arity, 16*Uint6-MD5:Uniq, UInt32:Index, UInt32:NumFree, atom:Module, int:OldIndex, int:OldUniq, pid:Pid, NunFree*ext:FreeVars]
	 */
	ett_new_function = 'p',

	/**
	 * @brief 113 [atom:Module, atom:Function, smallint:Arity]
	 */
	ett_export = 'q',

	/**
	 * @brief 114 [UInt16:Len, atom:Node, UInt8:Creation, Len*UInt32:ID]
	 */
	ett_new_reference = 'r',

	/**
	 * @brief 115 [UInt8:Len, Len:AtomName]
	 */
	ett_atom_small = 's',

	/**
	 * @brief 116 [UInt32:Airty, N:Pairs]
	 */
	ett_map = 't',

	/**
	 * @brief 117 [UInt4:NumFree, pid:Pid, atom:Module, int:Index, int:Uniq, NumFree*ext:FreeVars]
	 */
	ett_function = 'u',

	/**
	 * @brief 118 [UInt16:Len, Len:AtomName] max Len is 255 characters (up to 4 bytes per)
	 */
	ett_atom_utf8 =	 'v',

	/**
	 * @brief 119 [UInt8:Len, Len:AtomName]
	 */
	ett_atom_utf8_small = 'w'
};

/**
 * @brief Represents a buffer of bytes being encoded into ETF
 */
struct DPP_EXPORT etf_buffer {
	/**
	 * @brief Raw buffer
	 */
	std::vector<char> buf;

	/**
	 * @brief Current used length of buffer
	 * (this is different from buf.size() as it is pre-allocated
	 * using resize and may not all be in use)
	 */
	size_t length;

	/**
	 * @brief Construct a new etf buffer object
	 * 
	 * @param initial initial buffer size to allocate
	 */
	etf_buffer(size_t initial);

	/**
	 * @brief Destroy the etf buffer object
	 */
	~etf_buffer();
};

/**
 * @brief The etf_parser class can serialise and deserialise ETF (Erlang Term Format)
 * into and out of an nlohmann::json object, so that layers above the websocket don't
 * have to be any different for handling ETF.
 */
class DPP_EXPORT etf_parser {
	/**
	 * @brief Current size of binary data
	 */
	size_t size;

	/**
	 * @brief Current offset into binary data
	 */
	size_t offset;

	/**
	 * @brief Pointer to binary ETF data to be decoded
	 */
	uint8_t* data;

	/**
	 * @brief Parse a single value, and if that value contains other
	 * values (e.g. an array or map) then call itself recursively.
	 * 
	 * @return nlohmann::json JSON value from the ETF
	 */
	nlohmann::json inner_parse();

	/**
	 * @brief Read 8 bits of data from the buffer
	 * 
	 * @return uint8_t data retrieved
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	uint8_t read_8_bits();

	/**
	 * @brief Read 16 bits of data from the buffer
	 * 
	 * @return uint16_t data retrieved
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	uint16_t read_16_bits();

	/**
	 * @brief Read 32 bits of data from the buffer
	 * 
	 * @return uint32_t data retrieved
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	uint32_t read_32_bits();

	/**
	 * @brief Read 64 bits of data from the buffer
	 * 
	 * @return uint64_t data retrieved
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	uint64_t read_64_bits();

	/**
	 * @brief Read string data from the buffer
	 *
	 * @param length Length of string to retrieve 
	 * @return const char* data retrieved
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	const char* read_string(uint32_t length);

	/**
	 * @brief Process an 'atom' value.
	 * An atom is a "label" or constant value within the data,
	 * such as a key name, nullptr, or false.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json process_atom(const char* atom, uint16_t length);

	/**
	 * @brief Decode an 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_atom();

	/**
	 * @brief Decode a small 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_small_atom();

	/**
	 * @brief Decode a small integer value (0-255).
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_small_integer();

	/**
	 * @brief Decode an integer value (-MAXINT -> MAXINT-1).
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_integer();

	/**
	 * @brief Decode an array of values.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_array(uint32_t length);

	/**
	 * @brief Decode a list of values.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_list();

	/**
	 * @brief Decode a 'tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_tuple(uint32_t length);

	/**
	 * @brief Decode a nil 'atom' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_nil();

	/**
	 * @brief Decode a map (object) value.
	 * Will recurse to evaluate each member variable.
	 * 
	 * @return nlohmann::json values converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_map();

	/**
	 * @brief Decode a floating point numeric value.
	 * (depreciated in erlang but still expected to be supported)
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_float();

	/**
	 * @brief Decode a floating type numeric value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_new_float();

	/**
	 * @brief Decode a 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_bigint(uint32_t digits);

	/**
	 * @brief Decode a small 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_bigint_small();

	/**
	 * @brief Decode a large 'bigint' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_bigint_large();

	/**
	 * @brief Decode a binary value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_binary();

	/**
	 * @brief Decode a string value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_string();

	/**
	 * @brief Decode a string list value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_string_as_list();

	/**
	 * @brief Decode a 'small tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_tuple_small();

	/**
	 * @brief Decode a 'large tuple' value.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_tuple_large();

	/**
	 * @brief Decode a compressed value.
	 * This is a zlib-compressed binary blob which contains another
	 * ETF object.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_compressed();

	/**
	 * @brief Decode a 'reference' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_reference();

	/**
	 * @brief Decode a 'new reference' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_new_reference();

	/**
	 * @brief Decode a 'port' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_port();

	/**
	 * @brief Decode a 'PID' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_pid();

	/**
	 * @brief Decode an 'export' value.
	 * Erlang expects this to be supported, in practice Discord doesn't send these right now.
	 * 
	 * @return nlohmann::json value converted to JSON
	 * @throw dpp::exception Data stream isn't long enough to fetch requested bits
	 */
	nlohmann::json decode_export();

	/**
	 * @brief Write to output buffer for creation of ETF from JSON
	 * 
	 * @param pk buffer struct
	 * @param bytes byte buffer to write
	 * @param l number of bytes to write
	 * @throw std::exception Buffer cannot be extended
	 */
	void buffer_write(etf_buffer *pk, const char *bytes, size_t l);

	/**
	 * @brief Append version number to ETF buffer
	 * 
	 * @param b buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_version(etf_buffer *b);

	/**
	 * @brief Append nil value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_nil(etf_buffer *b);

	/**
	 * @brief Append false value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_false(etf_buffer *b);

	/**
	 * @brief Append true value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_true(etf_buffer *b);

	/**
	 * @brief Append small integer value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param d double to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_small_integer(etf_buffer *b, unsigned char d);

	/**
	 * @brief Append integer value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param d integer to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_integer(etf_buffer *b, int32_t d);

	/**
	 * @brief Append 64 bit integer value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param d integer to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_unsigned_long_long(etf_buffer *b, unsigned long long d);

	/**
	 * @brief Append 64 bit integer value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param d integer to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_long_long(etf_buffer *b, long long d);

	/**
	 * @brief Append double value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param f double to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_double(etf_buffer *b, double f);

	/**
	 * @brief Append atom value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param bytes pointer to string to append
	 * @param size size of string to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_atom(etf_buffer *b, const char *bytes, size_t size);

	/**
	 * @brief Append utf8 atom value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param bytes pointer to string to append
	 * @param size size of string to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_atom_utf8(etf_buffer *b, const char *bytes, size_t size);

	/**
	 * @brief Append binary value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param bytes pointer to string to append
	 * @param size size of string to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_binary(etf_buffer *b, const char *bytes, size_t size);

	/**
	 * @brief Append string value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param bytes pointer to string to append
	 * @param size size of string to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_string(etf_buffer *b, const char *bytes, size_t size);

	/**
	 * @brief Append tuple value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param size size of value to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_tuple_header(etf_buffer *b, size_t size);

	/**
	 * @brief Append list terminator to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_nil_ext(etf_buffer *b);

	/**
	 * @brief Append a list header value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param size size of values to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_list_header(etf_buffer *b, size_t size);

	/**
	 * @brief Append a map header value to ETF buffer
	 * 
	 * @param b  buffer to append to
	 * @param size size of values to append
	 * @throw std::exception Buffer cannot be extended
	 */
	void append_map_header(etf_buffer *b, size_t size);

	/**
	 * @brief Build ETF buffer
	 * 
	 * @param j JSON object to build from
	 * @param b Buffer to append to
	 * @throw std::exception Buffer cannot be extended
	 */
	void inner_build(const nlohmann::json* j, etf_buffer* b);

public:
	/**
	 * @brief Construct a new etf parser object
	 */
	etf_parser();

	/**
	 * @brief Destroy the etf parser object
	 */
	~etf_parser();

	/**
	 * @brief Convert ETF binary content to nlohmann::json
	 * 
	 * @param in Raw binary ETF data (generally from a websocket)
	 * @return nlohmann::json JSON data for use in the library
	 * @throw dpp::exception Malformed or otherwise invalid ETF content
	 */
	nlohmann::json parse(const std::string& in);

	/**
	 * @brief Create ETF binary data from nlohmann::json
	 * 
	 * @param j JSON value to encode to ETF
	 * @return std::string raw ETF data. Note that this can
	 * and probably will contain null values, use std::string::data()
	 * and std::string::size() to manipulate or send it.
	 * @throw std::exception Not enough memory, or invalid data types/values
	 */
	std::string build(const nlohmann::json& j);
};

} // namespace dpp
