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
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <climits>
#include <dpp/httpsclient.h>
#include <dpp/utility.h>
#include <dpp/exception.h>
#include <dpp/stringops.h>

namespace dpp {

https_client::https_client(const std::string &hostname, uint16_t port,  const std::string &urlpath, const std::string &verb, const std::string &req_body, const http_headers& extra_headers, bool plaintext_connection, uint16_t request_timeout, const std::string &protocol)
	: ssl_client(hostname, std::to_string(port), plaintext_connection, false),
	state(HTTPS_HEADERS),
	request_type(verb),
	path(urlpath),
	request_body(req_body),
	content_length(0),
	request_headers(extra_headers),
	status(0),
	http_protocol(protocol),
	timeout(request_timeout)
{
	nonblocking = false;
	timeout = time(nullptr) + request_timeout;
	https_client::connect();
}

void https_client::connect()
{
	state = HTTPS_HEADERS;
	std::string map_headers;
	for (auto& [k,v] : request_headers) {
		map_headers += k + ": " + v + "\r\n";
	}
	if (this->sfd != SOCKET_ERROR) {
		this->write(
			this->request_type + " " + this->path + " HTTP/" + http_protocol + "\r\n"
			"Host: " + this->hostname + "\r\n"
			"pragma: no-cache\r\n"
			"Connection: keep-alive\r\n"
			"Content-Length: " +
			std::to_string(this->request_body.length()) +
			"\r\n" +
			map_headers +
			"\r\n" +
			this->request_body
		);
		read_loop();
	}
}

multipart_content https_client::build_multipart(const std::string &json, const std::vector<std::string>& filenames, const std::vector<std::string>& contents, const std::vector<std::string>& mimetypes) {
	if (filenames.empty() && contents.empty()) {
		if (!json.empty()) {
			return { json, "application/json" };
		} else {
			return {json, ""};
		}
	} else {
		/* Note: loss of upper 32 bits on this value is INTENTIONAL */
		uint32_t dummy1 = (uint32_t)time(nullptr) + (uint32_t)time(nullptr);
		time_t dummy2 = time(nullptr) * time(nullptr);
		const std::string two_cr("\r\n\r\n");
		const std::string boundary("-------------" + to_hex(dummy1) + to_hex(dummy2));
		const std::string part_start("--" + boundary + "\r\nContent-Disposition: form-data; ");
		const std::string mime_type_start("\r\nContent-Type: ");
		const std::string default_mime_type("application/octet-stream");
		
		std::string content("--" + boundary);

		/* Special case, single file */
		content += "\r\nContent-Type: application/json\r\nContent-Disposition: form-data; name=\"payload_json\"" + two_cr;
		content += json + "\r\n";
		if (filenames.size() == 1 && contents.size() == 1) {
			content += part_start + "name=\"file\"; filename=\"" + filenames[0] + "\"";
			content += mime_type_start + (mimetypes.empty() || mimetypes[0].empty() ? default_mime_type : mimetypes[0]) + two_cr;
			content += contents[0];
		} else {
			/* Multiple files */
			for (size_t i = 0; i < filenames.size(); ++i) {
				content += part_start + "name=\"files[" + std::to_string(i) + "]\"; filename=\"" + filenames[i] + "\"";
				content += "\r\nContent-Type: " + (mimetypes.size() <= i || mimetypes[i].empty() ? default_mime_type : mimetypes[i]) + two_cr;
				content += contents[i];
				content += "\r\n";
			}
		}
		content += "\r\n--" + boundary + "--";
		return { content, "multipart/form-data; boundary=" + boundary };
	}
}

const std::string https_client::get_header(std::string header_name) const {
	std::transform(header_name.begin(), header_name.end(), header_name.begin(), [](unsigned char c){
		return std::tolower(c);
	});
	auto hdrs = response_headers.find(header_name);
	if (hdrs != response_headers.end()) {
		return hdrs->second;
	}
	return std::string();
}

size_t https_client::get_header_count(std::string header_name) const {
	std::transform(header_name.begin(), header_name.end(), header_name.begin(), [](unsigned char c){
		return std::tolower(c);
	});
	return response_headers.count(header_name);
}

const std::list<std::string> https_client::get_header_list(std::string header_name) const {
	std::transform(header_name.begin(), header_name.end(), header_name.begin(), [](unsigned char c){
		return std::tolower(c);
	});
	auto hdrs = response_headers.equal_range(header_name);
	if (hdrs.first != response_headers.end()) {
		std::list<std::string> data;
		for ( auto i = hdrs.first; i != hdrs.second; ++i ) {
			data.emplace_back(i->second);
		}
		return data;
	}
	return std::list<std::string>();
}

const std::multimap<std::string, std::string> https_client::get_headers() const {
	return response_headers;
}

bool https_client::handle_buffer(std::string &buffer)
{
	bool state_changed = false;
	do {
		state_changed = false;
		switch (state) {
			case HTTPS_HEADERS:
				if (buffer.find("\r\n\r\n") != std::string::npos) {
					/* Got all headers, proceed to new state */

					std::string unparsed = buffer;

					/* Get headers string */
					std::string headers = buffer.substr(0, buffer.find("\r\n\r\n"));

					/* Modify buffer, remove headers section */
					buffer.erase(0, buffer.find("\r\n\r\n") + 4);

					/* Process headers into map */
					std::vector<std::string> h = utility::tokenize(headers);
					if (h.size()) {
						std::string status_line = h[0];
						h.erase(h.begin());
						/* HTTP/1.1 200 OK */
						std::vector<std::string> req_status = utility::tokenize(status_line, " ");
						if (req_status.size() >= 2 && (req_status[0] == "HTTP/1.1" || req_status[0] == "HTTP/1.0") && atoi(req_status[1].c_str())) {
							for(auto &hd : h) {
								std::string::size_type sep = hd.find(": ");
								if (sep != std::string::npos) {
									std::string key = hd.substr(0, sep);
									std::string value = hd.substr(sep + 2, hd.length());
									std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c){
										return std::tolower(c);
									});
									response_headers.emplace(key, value);
								}
							}
							auto it_cl = response_headers.find("content-length");
							if ( it_cl != response_headers.end()) {
								content_length = std::stoull(it_cl->second);
							} else {
								content_length = ULLONG_MAX;
							}
							auto it_conn = response_headers.find("connection");
							if (it_conn != response_headers.end() && it_conn->second == "close") {
								keepalive = false;
							}
							chunked = false;
							auto it_txenc = response_headers.find("transfer-encoding");
							if (it_txenc != response_headers.end()) {
								if (it_txenc->second.find("chunked") != std::string::npos) {
									chunked = true;
									chunk_size = 0;
									chunk_receive = 0;
									state = HTTPS_CHUNK_LEN;
									state_changed = true;
								}
							}
							status = atoi(req_status[1].c_str());
							if (status == 204  || status < 200 || status == 304 || content_length == 0) {
								return false;
							} else if (!chunked) {
								state = HTTPS_CONTENT;
								state_changed = true;
								continue;
							}
							return true;
						} else {
							/* Non-HTTP-like response with invalid headers. Go no further. */
							keepalive = false;
							return false;
						}
					} else {
						keepalive = false;
						return false;
					}

				}
			break;
			case HTTPS_CHUNK_CONTENT: {
				size_t to_read = buffer.size();
				if (chunk_receive + buffer.size() > chunk_size) {
					to_read = chunk_size - chunk_receive;
				}
				body += buffer.substr(0, to_read);
				chunk_receive += to_read;
				buffer.erase(0, to_read);
				if (chunk_receive >= chunk_size) {
					state = HTTPS_CHUNK_TRAILER;
					state_changed = true;
				} else {
					return true;
				}
			}
			break;
			case HTTPS_CHUNK_LAST:
			case HTTPS_CHUNK_TRAILER:
				if (buffer.length() >= 2 && buffer.substr(0, 2) == "\r\n") {
					if (state == HTTPS_CHUNK_LAST) {
						state = HTTPS_DONE;
						this->close();
						return false;
					} else {
						state = HTTPS_CHUNK_LEN;
						buffer.erase(0, 2);
					}
					state_changed = true;
				}
			break;
			case HTTPS_CHUNK_LEN:
				if (buffer.find("\r\n") != std::string::npos) {
					chunk_receive = 0;
					std::string chunk_length_str = buffer.substr(0, buffer.find("\r\n"));
					buffer.erase(0, buffer.find("\r\n") + 2);
					try {
						size_t index = 0;
						chunk_size = std::stoi(chunk_length_str, &index, 16);
					}
					catch (const std::exception&) {
						keepalive = false;
						return false;
					}
					state = HTTPS_CHUNK_CONTENT;
					if (chunk_size == 0) {
						state = HTTPS_CHUNK_LAST;
						chunk_size = 2;
					}
					state_changed = true;
				}
			break;
			case HTTPS_CONTENT:
				body += buffer;
				buffer.clear();
				if (body.length() >= content_length) {
					state = HTTPS_DONE;
					this->close();
					return false;
				}
			break;
			case HTTPS_DONE:
				this->close();
				return false;
			break;
		}
	} while (state_changed);
	return true;
}


uint16_t https_client::get_status() const {
	return status;
}

const std::string https_client::get_content() const {
	return body;
}

http_state https_client::get_state() {
	return this->state;
}

void https_client::one_second_timer() {
	if ((this->sfd == SOCKET_ERROR || time(nullptr) >= timeout) && this->state != HTTPS_DONE) {
		keepalive = false;
		this->close();
	}
}

void https_client::close() {
	if (state != HTTPS_DONE) {
		state = HTTPS_DONE;
		ssl_client::close();
	}
}

http_connect_info https_client::get_host_info(std::string url) {
	http_connect_info hci = { false, "http", "", 80};
	if (url.substr(0, 8) == "https://") {
		hci.port = 443;
		hci.is_ssl = true;
		hci.scheme = url.substr(0, 5);
		url = url.substr(8, url.length());
	} else if (url.substr(0, 7) == "http://") {
		hci.scheme = url.substr(0, 4);
		url = url.substr(7, url.length());
	} else if (url.substr(0, 11) == "discord.com") {
		hci.scheme = "https";
		hci.is_ssl = true;
		hci.port = 443;
	}
	size_t colon_pos = url.find(':');
	if (colon_pos != std::string::npos) {
		hci.hostname = url.substr(0, colon_pos);
		hci.port = atoi(url.substr(colon_pos + 1, url.length()).c_str());
		if (hci.port == 0) {
			hci.port = 80;
		}
	} else {
		hci.hostname = url;
	}
	return hci;
}

} // namespace dpp
