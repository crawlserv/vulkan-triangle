/*
 * CLocale.hpp
 *
 * Simple sets the locale for stdout to the C locale on its construction.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_CLOCALE_HPP_
#define SRC_WRAPPER_CLOCALE_HPP_

#include <iostream>	// std::cout
#include <locale>	// std::locale

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class CLocale {
	public:
		CLocale();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline CLocale::CLocale() {
		std::cout.imbue(std::locale(""));
	}

} // spacelite::Wrapper

#endif /* SRC_WRAPPER_CLOCALE_HPP_ */
