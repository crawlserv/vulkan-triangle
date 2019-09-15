/*
 * WinProp.hpp
 *
 * Properties of a window (title, width and height).
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_WINPROP_HPP_
#define SRC_STRUCT_WINPROP_HPP_

#include <string>	// std::string

namespace spacelite::Struct {

	struct WinProp {
		std::string title;

		unsigned int width;
		unsigned int height;

		WinProp(
				const std::string& setTitle,
				unsigned int setWidth,
				unsigned int setHeight
		) : title(setTitle), width(setWidth), height(setHeight) {}
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_WINPROP_HPP_ */
