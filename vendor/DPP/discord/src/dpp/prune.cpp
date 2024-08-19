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
#include <dpp/prune.h>
#include <dpp/discordevents.h>
#include <dpp/stringops.h>
#include <dpp/json.h>

namespace dpp {

using json = nlohmann::json;

prune& prune::fill_from_json_impl(nlohmann::json* j) {
	days = int32_not_null(j, "days");
	compute_prune_count = bool_not_null(j, "compute_prune_count");
	set_snowflake_array_not_null(j, "include_roles", include_roles);
	return *this;
}

json prune::to_json_impl(bool with_prune_count) const {
	json j;
	for (auto & r : include_roles) {
		j["include_roles"].push_back(std::to_string(r));
	}
	if (with_prune_count) {
		j["compute_prune_count"] = compute_prune_count;
	}
	j["days"] = days;
	return j;
}

json prune::to_json(bool with_id) const {
	return to_json_impl(with_id);
}

} // namespace dpp
