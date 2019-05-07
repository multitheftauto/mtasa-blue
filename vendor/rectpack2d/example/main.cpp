#include <iostream>
#include "../src/finders_interface.h"

/* For description of the algorithm, please see the README.md */

using namespace rectpack2D;

int main() {
	constexpr bool allow_flip = true;
	const auto runtime_flipping_mode = flipping_option::ENABLED;

	/* 
		Here, we choose the "empty_spaces" class that the algorithm will use from now on. 
	
		The first template argument is a bool which determines
		if the algorithm will try to flip rectangles to better fit them.

		The second argument is optional and specifies an allocator for the empty spaces.
		The default one just uses a vector to store the spaces.
		You can also pass a "static_empty_spaces<10000>" which will allocate 10000 spaces on the stack,
		possibly improving performance.
	*/

	using spaces_type = rectpack2D::empty_spaces<allow_flip, default_empty_spaces>;

	/* 
		rect_xywh or rect_xywhf (see src/rect_structs.h), 
		depending on the value of allow_flip.
	*/

	using rect_type = output_rect_t<spaces_type>;

	/*
		Note: 

		The multiple-bin functionality was removed. 
		This means that it is now up to you what is to be done with unsuccessful insertions.
		You may initialize another search when this happens.
	*/

	auto report_successful = [](rect_type&) {
		return callback_result::CONTINUE_PACKING;
	};

	auto report_unsuccessful = [](rect_type&) {
		return callback_result::ABORT_PACKING;
	};

	/*
		Initial size for the bin, from which the search begins.
		The result can only be smaller - if it cannot, the algorithm will gracefully fail.
	*/

	const auto max_side = 1000;

	/*
		The search stops when the bin was successfully inserted into,
		AND the next candidate bin size differs from the last successful one by *less* then discard_step.

		See the algorithm section of README for more information.
	*/

	const auto discard_step = 1;

	/* 
		Create some arbitrary rectangles.
		Every subsequent call to the packer library will only read the widths and heights that we now specify,
		and always overwrite the x and y coordinates with calculated results.
	*/

	std::vector<rect_type> rectangles;

	rectangles.emplace_back(rect_xywh(0, 0, 20, 40));
	rectangles.emplace_back(rect_xywh(0, 0, 120, 40));
	rectangles.emplace_back(rect_xywh(0, 0, 85, 59));
	rectangles.emplace_back(rect_xywh(0, 0, 199, 380));
	rectangles.emplace_back(rect_xywh(0, 0, 85, 875));
	
	auto report_result = [&rectangles](const rect_wh& result_size) {
		std::cout << "Resultant bin: " << result_size.w << " " << result_size.h << std::endl;

		for (const auto& r : rectangles) {
			std::cout << r.x << " " << r.y << " " << r.w << " " << r.h << std::endl;
		}
	};

	{
		/*
			Example 1: Find best packing with default orders. 

			If you pass no comparators whatsoever, 
			the standard collection of 6 orders:
		   	by area, by perimeter, by bigger side, by width, by height and by "pathological multiplier"
			- will be passed by default.
		*/

		const auto result_size = find_best_packing<spaces_type>(
			rectangles,
			make_finder_input(
				max_side,
				discard_step,
				report_successful,
				report_unsuccessful,
				runtime_flipping_mode
			)
		);

		report_result(result_size);
	}

	{
		/* Example 2: Find best packing using all provided custom rectangle orders. */

		using rect_ptr = rect_type*;

		auto my_custom_order_1 = [](const rect_ptr a, const rect_ptr b) {
			return a->get_wh().pathological_mult() > b->get_wh().pathological_mult();
		};

		auto my_custom_order_2 = [](const rect_ptr a, const rect_ptr b) {
			return a->get_wh().pathological_mult() < b->get_wh().pathological_mult();
		};

		const auto result_size = find_best_packing<spaces_type>(
			rectangles,
			make_finder_input(
				max_side,
				discard_step,
				report_successful,
				report_unsuccessful,
				runtime_flipping_mode
			),

			my_custom_order_1,
			my_custom_order_2
		);

		report_result(result_size);
	}

	{
		/* Example 3: Find best packing exactly in the order of provided input. */

		const auto result_size = find_best_packing_dont_sort<spaces_type>(
			rectangles,
			make_finder_input(
				max_side,
				discard_step,
				report_successful,
				report_unsuccessful,
				runtime_flipping_mode
			)
		);

		report_result(result_size);
	}

	{
		/* Example 4: Manually perform insertions. This way you can try your own best-bin finding logic. */

		auto packing_root = spaces_type({ max_side, max_side });
		packing_root.flipping_mode = runtime_flipping_mode;

		for (auto& r : rectangles) {
			if (const auto inserted_rectangle = packing_root.insert(std::as_const(r).get_wh())) {
				r = *inserted_rectangle;
			}
			else {
				std::cout << "Failed to insert a rectangle." << std::endl;
				break;
			}
		}

		const auto result_size = packing_root.get_rects_aabb();

		report_result(result_size);
	}

	return 0;
}
