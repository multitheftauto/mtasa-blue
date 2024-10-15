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
#include <dpp/etf.h>
#include <dpp/sysdep.h>
#include <dpp/discordevents.h>
#include <dpp/exception.h>
#include <dpp/json.h>
#include <zlib.h>
#include <iostream>

namespace dpp {

using json = nlohmann::json;


void etf_parser::buffer_write(etf_buffer *pk, const char *bytes, size_t l) {

	if (pk->length + l > pk->buf.size()) {
		// Grow buffer 2x to avoid excessive re-allocations.
		pk->buf.resize((pk->length + l) * 2);
	}

	memcpy(pk->buf.data() + pk->length, bytes, l);
	pk->length += l;
}

void etf_parser::append_version(etf_buffer *b) {
	static unsigned char buf[1] = {FORMAT_VERSION};
	buffer_write(b, (const char *)buf, 1);
}

void etf_parser::append_nil(etf_buffer *b) {
	static unsigned char buf[5] = {ett_atom_small, 3, 'n', 'i', 'l'};
	buffer_write(b, (const char *)buf, 5);
}

void etf_parser::append_false(etf_buffer *b) {
	static unsigned char buf[7] = {ett_atom_small, 5, 'f', 'a', 'l', 's', 'e'};
	buffer_write(b, (const char *)buf, 7);
}

void etf_parser::append_true(etf_buffer *b) {
	static unsigned char buf[6] = {ett_atom_small, 4, 't', 'r', 'u', 'e'};
	buffer_write(b, (const char *)buf, 6);
}

void etf_parser::append_small_integer(etf_buffer *b, unsigned char d) {
	unsigned char buf[2] = {ett_smallint, d};
	buffer_write(b, (const char *)buf, 2);
}

void etf_parser::append_integer(etf_buffer *b, int32_t d) {
	unsigned char buf[5];
	buf[0] = ett_integer;
	store_32_bits(buf + 1, d);
	buffer_write(b, (const char *)buf, 5);
}

void etf_parser::append_unsigned_long_long(etf_buffer *b, unsigned long long d) {
	unsigned char buf[1 + 2 + sizeof(unsigned long long)];
	buf[0] = ett_bigint_small;

	unsigned char bytes_enc = 0;
	while (d > 0) {
		buf[3 + bytes_enc] = d & 0xFF;
		d >>= 8;
		bytes_enc++;
	}
	buf[1] = bytes_enc;
	buf[2] = 0;

	buffer_write(b, (const char *)buf, 1 + 2 + bytes_enc);
}

void etf_parser::append_long_long(etf_buffer *b, long long d) {
	unsigned char buf[1 + 2 + sizeof(unsigned long long)];
	buf[0] = ett_bigint_small;
	buf[2] = d < 0 ? 1 : 0;
	unsigned long long ull = d < 0 ? -d : d;
	unsigned char bytes_enc = 0;
	while (ull > 0) {
		buf[3 + bytes_enc] = ull & 0xFF;
		ull >>= 8;
		bytes_enc++;
	}
	buf[1] = bytes_enc;
	buffer_write(b, (const char *)buf, 1 + 2 + bytes_enc);
}

void etf_parser::append_double(etf_buffer *b, double f) {
	unsigned char buf[1 + 8] = {0};
	uint64_t val_64_out{0};
	buf[0] = ett_new_float;
	memcpy(&val_64_out, &f, sizeof(double));
	store_64_bits(buf + 1, val_64_out);
	buffer_write(b, (const char *)buf, 1 + 8);
}

void etf_parser::append_atom(etf_buffer *b, const char *bytes, size_t size) {
	if (size < 255) {
		unsigned char buf[2] = {ett_atom_small, (unsigned char)size};
		buffer_write(b, (const char *)buf, 2);
		buffer_write(b, (const char *)bytes, size);
	} else {
		unsigned char buf[3];
		buf[0] = ett_atom;

		if (size > 0xFFFF) {
			throw dpp::parse_exception(err_etf, "ETF: Atom too large");
		}

		store_16_bits(buf + 1, size);
		buffer_write(b, (const char *)buf, 3);
		buffer_write(b, (const char *)bytes, size);
	}
}

void etf_parser::append_atom_utf8(etf_buffer *b, const char *bytes, size_t size) {
	if (size < 255) {
		unsigned char buf[2] = {ett_atom_utf8_small, (unsigned char)size};
		buffer_write(b, (const char *)buf, 2);
		buffer_write(b, (const char *)bytes, size);
	} else {
		unsigned char buf[3];
		buf[0] = ett_atom_utf8;

		if (size > 0xFFFF) {
			throw dpp::parse_exception(err_etf, "ETF: Atom too large");
		}

		store_16_bits(buf + 1, size);
		buffer_write(b, (const char *)buf, 3);
		buffer_write(b, (const char *)bytes, size);
	}
}

void etf_parser::append_binary(etf_buffer *b, const char *bytes, size_t size) {
	unsigned char buf[5];
	buf[0] = ett_binary;

	store_32_bits(buf + 1, size);
	buffer_write(b, (const char *)buf, 5);
	buffer_write(b, (const char *)bytes, size);
}

void etf_parser::append_string(etf_buffer *b, const char *bytes, size_t size) {
	unsigned char buf[3];
	buf[0] = ett_string;

	store_16_bits(buf + 1, size);
	buffer_write(b, (const char *)buf, 3);
	buffer_write(b, (const char *)bytes, size);
}

void etf_parser::append_tuple_header(etf_buffer *b, size_t size) {
	if (size < 256) {
		unsigned char buf[2];
		buf[0] = ett_small_tuple;
		buf[1] = (unsigned char)size;
		buffer_write(b, (const char *)buf, 2);
	} else {
		unsigned char buf[5];
		buf[0] = ett_large_tuple;
		store_32_bits(buf + 1, size);
		buffer_write(b, (const char *)buf, 5);
	}
}

void etf_parser::append_nil_ext(etf_buffer *b) {
	static unsigned char buf[1] = {ett_nil};
	buffer_write(b, (const char *)buf, 1);
}

void etf_parser::append_list_header(etf_buffer *b, size_t size) {
	unsigned char buf[5];
	buf[0] = ett_list;
	store_32_bits(buf + 1, size);
	buffer_write(b, (const char *)buf, 5);
}

void etf_parser::append_map_header(etf_buffer *b, size_t size) {
	unsigned char buf[5];
	buf[0] = ett_map;
	store_32_bits(buf + 1, size);
	buffer_write(b, (const char *)buf, 5);
}

etf_parser::etf_parser()
{
}

etf_parser::~etf_parser() = default;


uint8_t etf_parser::read_8_bits() {
	if (offset + sizeof(uint8_t) > size) {
		throw dpp::parse_exception(err_etf, "ETF: read_8_bits() past end of buffer");
	}
	auto val = *reinterpret_cast<const uint8_t*>(data + offset);
	offset += sizeof(uint8_t);
	return val;
}

uint16_t etf_parser::read_16_bits() {
	if (offset + sizeof(uint16_t) > size) {
		throw dpp::parse_exception(err_etf, "ETF: read_16_bits() past end of buffer");
	}
	uint16_t val = etf_byte_order_16(*reinterpret_cast<const uint16_t*>(data + offset));
	offset += sizeof(uint16_t);
	return val;
}

uint32_t etf_parser::read_32_bits() {
	if (offset + sizeof(uint32_t) > size) {
		throw dpp::parse_exception(err_etf, "ETF: read_32_bits() past end of buffer");
	}
	uint32_t val = etf_byte_order_32(*reinterpret_cast<const uint32_t*>(data + offset));
	offset += sizeof(uint32_t);
	return val;
}

uint64_t etf_parser::read_64_bits() {
	if (offset + sizeof(uint64_t) > size) {
		throw dpp::parse_exception(err_etf, "ETF: read_64_bits() past end of buffer");
	}
	uint64_t val = etf_byte_order_64(*reinterpret_cast<const uint64_t*>(data + offset));
	offset += sizeof(val);
	return val;
}

const char* etf_parser::read_string(uint32_t length) {
	if (offset + length > size) {
		return nullptr;
	}

	const uint8_t* str = data + offset;
	offset += length;
	return (const char*)str;
}

json etf_parser::process_atom(const char* atom, uint16_t length) {
	json j;

	if (atom == NULL) {
		return j;
	}

	/**
	 * WARNING: PERFORMANCE HOTSPOT
	 * Valgrind cachegrind identified this as a performance hotspot in DPP when using ETF. This gets called a LOT,
	 * and the difference here between comparing the simple constant types here where the lengths are known in this
	 * way, compared to using memcpy or strncpy four times, is absolutely huge (factors of ten). Considering this
	 * can be called hundreds of times a second on a busy bot, the performance of this function should not be
	 * underestimated. Think carefully before 'tidying' this function further!
	 * 
	 */
	if (length >= 3 && length <= 5) {
		if (length == 4 && atom[0] == 'n' && atom[1] == 'u' && atom[2] == 'l' && atom[3] == 'l') { // "null"
			return j;
		}
		else if (length == 4 && atom[0] == 't' && atom[1] == 'r' && atom[2] == 'u' && atom[3] == 'e') { // "true"
			return true;
		}
		else if (length == 3 && atom[0] == 'n' && atom[1] == 'i' && atom[2] == 'l') { // "nil"
			return j;
		}
		else if (length == 5 && atom[0] == 'f' && atom[1] == 'a' && atom[2] == 'l' && atom[3] == 's' && atom[4] == 'e') { // "fals","e"
			return false;
		}
	}

	j = std::string(atom, length);
	return j;
}

json etf_parser::decode_atom() {
	auto length = read_16_bits();
	const char* atom = read_string(length);
	return process_atom(atom, length);
}

json etf_parser::decode_small_atom() {
	auto length = read_8_bits();
	const char* atom = read_string(length);
	return process_atom(atom, length);
}

json etf_parser::decode_small_integer() {
	json j;
	j = (uint8_t)read_8_bits();
	return j;
}

json etf_parser::decode_integer() {
	json j;
	j = (int32_t)read_32_bits();
	return j;
}

json etf_parser::decode_array(uint32_t length) {
	json array = json::array();
	for(uint32_t i = 0; i < length; ++i) {
		array.emplace_back(inner_parse());
	}
	return array;
}

json etf_parser::decode_list() {
	const uint32_t length = read_32_bits();
	auto array = decode_array(length);

	const auto tailMarker = read_8_bits();
	if (tailMarker != ett_nil) {
		return json();
	}

	return array;
}

json etf_parser::decode_tuple(uint32_t length) {
	return decode_array(length);
}

json etf_parser::decode_nil() {
	return json::array();
}

json etf_parser::decode_map() {
	const uint32_t length = read_32_bits();
	json map;
	for(uint32_t i = 0; i < length; ++i) {
		auto key = inner_parse();
		if (key.is_number()) {
			map.emplace(std::to_string(key.get<uint64_t>()), inner_parse());
		} else {
			map.emplace(key.get<std::string>(), inner_parse());
		}
	}
	return map;
}

json etf_parser::decode_float() {

	const uint8_t FLOAT_LENGTH = 31;
	const char* floatStr = read_string(FLOAT_LENGTH);

	if (floatStr == NULL) {
		return json();
	}

	double number;
	char null_terminated[FLOAT_LENGTH + 1] = {0};

	memcpy(null_terminated, floatStr, FLOAT_LENGTH);

	auto count = sscanf(null_terminated, "%lf", &number);

	if (count != 1) {
		return json();
	}

	json j = number;
	return j;
}

json etf_parser::decode_new_float() {
	union {
		uint64_t ui64;
		double df;
	} val;
	val.ui64 = read_64_bits();
	json j = val.df;
	return j;
}

json etf_parser::decode_bigint(uint32_t digits) {
	const uint8_t sign = read_8_bits();

	if (digits > 8) {
		throw dpp::parse_exception(err_etf, "ETF: big integer larger than 8 bytes unsupported");
	}

	uint64_t value = 0;
	uint64_t b = 1;
	for(uint32_t i = 0; i < digits; ++i) {
		uint64_t digit = read_8_bits();
		value += digit * b;
		b <<= 8;
	}

	if (digits <= 4) {
		if (sign == 0) {
			json j = std::to_string(static_cast<uint32_t>(value));
			return j;
		}

		const bool isSignBitAvailable = (value & (1 << 31)) == 0;
		if (isSignBitAvailable) {
			int32_t negativeValue = -static_cast<int32_t>(value);
			json j = std::to_string(negativeValue);
			return j;
		}
	}

	if (sign == 0) {
		json j = std::to_string(value);
		return j;
	} else {
		json j = std::to_string(-((int64_t)value));
		return j;
	}
}

json etf_parser::decode_bigint_small() {
	const auto bytes = read_8_bits();
	return decode_bigint(bytes);
}

json etf_parser::decode_bigint_large() {
	const auto bytes = read_32_bits();
	return decode_bigint(bytes);
}

json etf_parser::decode_binary() {
	const auto length = read_32_bits();
	const char* str = read_string(length);
	if (str == NULL) {
		return json();
	}
	json j = std::string(str, length);
	return j;
}

json etf_parser::decode_string() {
	const auto length = read_16_bits();
	const char* str = read_string(length);
	if (str == NULL) {
		return json();
	}
	json j = std::string(str, length);
	return j;
}

json etf_parser::decode_string_as_list() {
	const auto length = read_16_bits();
	json array = json::array();
	if (offset + length > size) {
		throw dpp::parse_exception(err_etf, "String list past end of buffer");
	}
	for(uint16_t i = 0; i < length; ++i) {
		array.emplace_back(decode_small_integer());
	}
	return array;
}

json etf_parser::decode_tuple_small() {
	return decode_tuple(read_8_bits());
}

json etf_parser::decode_tuple_large() {
	return decode_tuple(read_32_bits());
}

json etf_parser::decode_compressed() {
	const uint32_t uncompressedSize = read_32_bits();
	unsigned long sourceSize = uncompressedSize;
	std::vector<uint8_t> outBuffer;
	outBuffer.reserve(uncompressedSize);
	const int ret = uncompress((Bytef*)outBuffer.data(), &sourceSize, (const unsigned char*)(data + offset), (uLong)(size - offset));

	offset += sourceSize;
	if (ret != Z_OK) {
		throw dpp::parse_exception(err_etf, "ETF compressed value: decompresson error");
	}

	uint8_t* old_data = data;
	size_t old_size = size;
	size_t old_offset = offset;
	data = outBuffer.data();
	size = uncompressedSize;
	offset = 0;
	json j = inner_parse();
	data = old_data;
	size = old_size;
	offset = old_offset;
	return j;
}

json etf_parser::decode_reference() {
	json reference;

	reference["node"] = inner_parse();

	std::vector<int32_t> ids;
	ids.emplace_back(read_32_bits());
	reference["id"] = ids;

	reference["creation"] = read_8_bits();

	return reference;
}

json etf_parser::decode_new_reference() {
	json reference;

	uint16_t len = read_16_bits();
	reference["node"] = inner_parse();
	reference["creation"] = read_8_bits();

	std::vector<int32_t> ids;
	for(uint16_t i = 0; i < len; ++i) {
		ids.emplace_back(read_32_bits());
	}
	reference["id"] = ids;

	return reference;
}

json etf_parser::decode_port() {
	json port;
	port["node"] = inner_parse();
	port["id"] = read_32_bits();
	port["creation"] = read_8_bits();
	return port;
}

json etf_parser::decode_pid() {
	json pid;
	pid["node"] = inner_parse();
	pid["id"] = read_32_bits();
	pid["serial"] = read_32_bits();
	pid["creation"] = read_8_bits();
	return pid;
}

json etf_parser::decode_export() {
	json exp;
	exp["mod"] = inner_parse();
	exp["fun"] = inner_parse();
	exp["arity"] = inner_parse();
	return exp;
}

json etf_parser::inner_parse() {
	/* Decode one value into json from ETF */
	if (offset >= size) {
		throw dpp::parse_exception(err_etf, "Read past end of ETF buffer");
	}

	const uint8_t type = read_8_bits();

	switch(type) {
		case ett_distribution:
			throw dpp::parse_exception(err_etf, "Distribution headers are not supported");
		case ett_smallint:
			return decode_small_integer();
		case ett_integer:
			return decode_integer();
		case ett_float:
			return decode_float();
		case ett_new_float:
			return decode_new_float();
		case ett_atom:
			return decode_atom();
		case ett_atom_small:
			return decode_small_atom();
		case ett_small_tuple:
			return decode_tuple_small();
		case ett_large_tuple:
			return decode_tuple_large();
		case ett_nil:
			return decode_nil();
		case ett_string:
			return decode_string_as_list();
		case ett_list:
			return decode_list();
		case ett_map:
			return decode_map();
		case ett_binary:
			return decode_binary();
		case ett_bigint_small:
			return decode_bigint_small();
		case ett_bigint_large:
			return decode_bigint_large();
		case ett_reference:
			return decode_reference();
		case ett_new_reference:
			return decode_new_reference();
		case ett_port:
			return decode_port();
		case ett_pid:
			return decode_pid();
		case ett_export:
			return decode_export();
		case ett_compressed:
			return decode_compressed();
		default:
			throw dpp::parse_exception(err_etf, "Unknown data type in ETF");
	}
}

json etf_parser::parse(const std::string& in) {
	/* Recursively decode multiple values from ETF to JSON */
	offset = 0;
	size = in.size();
	data = (uint8_t*)in.data();
	const uint8_t version = read_8_bits();
	if (version == FORMAT_VERSION) {
		return inner_parse();
	} else {
		throw dpp::parse_exception(err_etf, "Incorrect ETF version");
	}
}

void etf_parser::inner_build(const json* i, etf_buffer* b)
{
	if (i->is_number_integer()) {
		/* Numeric integer types by size */
		int64_t number = i->get<int64_t>();
		if (number >= 0 && number <= 127) {
			unsigned char num = (unsigned char)number;
			append_small_integer(b, num);
		}
		else if (number >= std::numeric_limits<uint32_t>::max() - 1) {
			append_unsigned_long_long(b, number);
		}
		else {
			int32_t num32 = (int32_t)number;
			append_integer(b, num32);
		}
	}
	else if (i->is_number_float()) {
		/* Floating point (double) types */
		double decimal = i->get<double>();
		append_double(b, decimal);
	}
	else if (i->is_null()) {
		/* NULL type */
		append_nil(b);
	}
	else if (i->is_boolean()) {
		/* Boolean types */
		bool truthy = i->get<bool>();
		if (truthy) {
			append_true(b);
		} else {
			append_false(b);
		}
	}
	else if (i->is_string()) {
		/* String types */
		const std::string s = i->get<std::string>();
		append_binary(b, s.c_str(), s.length());
	}
	else if (i->is_array()) {
		/* Array types (can contain any other type, recursively) */
		const size_t length = i->size();
		if (length == 0) {
			append_nil_ext(b);
		} else {
			if (length > std::numeric_limits<uint32_t>::max() - 1) {
				throw dpp::parse_exception(err_etf, "ETF encode: List too large for ETF");
			}
		}

		append_list_header(b, length);
		for(size_t index = 0; index < length; ++index) {
			inner_build(&((*i)[index]), b);
		}
		append_nil_ext(b);
	}
	else if (i->is_object()) {
		/* Object types (can contain any other type, recursively, but nlohmann::json only supports string keys) */
		const size_t length = i->size();
		if (length > std::numeric_limits<uint32_t>::max() - 1) {
			throw dpp::parse_exception(err_etf, "ETF encode: Map too large for ETF");
		}
		append_map_header(b, length);
		for (auto n = i->begin(); n != i->end(); ++n) {
			json jstr = n.key();
			inner_build(&jstr, b);
			inner_build(&(n.value()), b);
		}
	}
}

std::string etf_parser::build(const json& j) {
	etf_buffer pk(1024 * 1024);
	append_version(&pk);
	inner_build(&j, &pk);
	return std::string(pk.buf.data(), pk.length);
}

etf_buffer::etf_buffer(size_t initial) {
	buf.resize(initial);
	length = 0;
}

etf_buffer::~etf_buffer() = default;

} // namespace dpp

