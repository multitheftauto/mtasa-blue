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

#include <numeric>

namespace dpp {

	/**
	 * @brief A class for audio mixing operations using x64 instructions.
	 */
	class audio_mixer {
	public:
		/*
		* @brief The number of 32-bit values per CPU register.
		*/
		inline static constexpr int32_t byte_blocks_per_register{ 2 };

		/**
		 * @brief Collect a single register worth of data from data_in, apply gain and increment, and store the result in data_out.
		 *        This version uses x64 instructions.
		 *
		 * @param data_in Pointer to the input array of int32_t values.
		 * @param data_out Pointer to the output array of int16_t values.
		 * @param current_gain The gain to be applied to the elements.
		 * @param increment The increment value to be added to each element.
		 */
		inline static void collect_single_register(int32_t* data_in, int16_t* data_out, float current_gain, float increment) {
			for (uint64_t x = 0; x < byte_blocks_per_register; ++x) {
				auto increment_new = increment * x;
				auto current_gain_new = current_gain + increment_new;
				auto current_sample_new = data_in[x] * current_gain_new;
				if (current_sample_new >= std::numeric_limits<int16_t>::max()) {
					current_sample_new = std::numeric_limits<int16_t>::max();
				}
				else if (current_sample_new <= std::numeric_limits<int16_t>::min()) {
					current_sample_new = std::numeric_limits<int16_t>::min();
				}
				data_out[x] = static_cast<int16_t>(current_sample_new);
			}
		}

		/**
		 * @brief Combine a register worth of elements from decoded_data and store the result in up_sampled_vector.
		 *        This version uses x64 instructions.
		 *
		 * @param up_sampled_vector Pointer to the array of int32_t values.
		 * @param decoded_data Pointer to the array of int16_t values.
		 */
		inline static void combine_samples(int32_t* up_sampled_vector, const int16_t* decoded_data) {
			for (uint64_t x = 0; x < byte_blocks_per_register; ++x) {
				up_sampled_vector[x] += static_cast<int32_t>(decoded_data[x]);
			}

		}
	};

} // namespace dpp
