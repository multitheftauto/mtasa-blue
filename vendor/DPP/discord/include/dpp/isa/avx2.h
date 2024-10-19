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

#if defined _MSC_VER || defined __GNUC__ || defined __clang__

#include <immintrin.h>
#include <numeric>

namespace dpp {

	using avx_2_float = __m256;

	/**
	 * @brief A class for audio mixing operations using AVX2 instructions.
	 */
	class audio_mixer {
	public:

		/**
		 * @brief The number of 32-bit values per CPU register.
		 */
		inline static constexpr int32_t byte_blocks_per_register{ 8 };

		/**
		 * @brief Collect a single register worth of data from data_in, apply gain and increment, and store the result in data_out.
		 *        This version uses AVX2 instructions.
		 *
		 * @param data_in Pointer to the input array of int32_t values.
		 * @param data_out Pointer to the output array of int16_t values.
		 * @param current_gain The gain to be applied to the elements.
		 * @param increment The increment value to be added to each element.
		 */
		inline void collect_single_register(int32_t* data_in, int16_t* data_out, float current_gain, float increment) {
			avx_2_float current_samples_new{ _mm256_mul_ps(gather_values(data_in),
				_mm256_add_ps(_mm256_set1_ps(current_gain),
					_mm256_mul_ps(_mm256_set1_ps(increment), _mm256_set_ps(0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f)))) };

			current_samples_new =
				_mm256_blendv_ps(_mm256_max_ps(current_samples_new, _mm256_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::min()))),
					_mm256_min_ps(current_samples_new, _mm256_set1_ps(static_cast<float>(std::numeric_limits<int16_t>::max()))),
					_mm256_cmp_ps(current_samples_new, _mm256_set1_ps(0.0f), _CMP_GE_OQ));

			store_values(current_samples_new, data_out);
		}

		/**
		 * @brief Combine a register worth of elements from decoded_data and store the result in up_sampled_vector.
		 *        This version uses AVX2 instructions.
		 *
		 * @param up_sampled_vector Pointer to the array of int32_t values.
		 * @param decoded_data Pointer to the array of int16_t values.
		 * @param x Index to select a specific set of elements to combine.
		 */
		inline void combine_samples(int32_t* up_sampled_vector, const int16_t* decoded_data) {
			auto newValues{ _mm256_add_ps(gather_values(up_sampled_vector), gather_values(decoded_data)) };
			store_values(newValues, up_sampled_vector);
		}

	protected:
		/**
		 * @brief Array for storing the values to be loaded/stored.
		 */
		alignas(32) float values[byte_blocks_per_register]{};

		/**
		 * @brief Stores values from a 256-bit AVX2 vector to a storage location.
		 * @tparam value_type The target value type for storage.
		 * @param values_to_store The 256-bit AVX2 vector containing values to store.
		 * @param storage_location Pointer to the storage location.
		 */
		template<typename value_type> inline void store_values(const avx_2_float& values_to_store, value_type* storage_location) {
			_mm256_store_ps(values, values_to_store);
			for (int64_t x = 0; x < byte_blocks_per_register; ++x) {
				storage_location[x] = static_cast<value_type>(values[x]);
			}
		}

		/**
		 * @brief Specialization for gathering non-float values into an AVX2 register.
		 * @tparam value_type The type of values being gathered.
		 * @tparam Indices Parameter pack of indices for gathering values.
		 * @return An AVX2 register containing gathered values.
		 */
		template<typename value_type> inline avx_2_float gather_values(value_type* values_new) {
			for (uint64_t x = 0; x < byte_blocks_per_register; ++x) {
				values[x] = static_cast<float>(values_new[x]);
			}
			return _mm256_load_ps(values);
		}
	};

} // namespace dpp

#endif