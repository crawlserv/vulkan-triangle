/*
 * CLocale.hpp
 *
 * Sets the locale for stdout on its construction.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_CLOCALE_HPP_
#define SRC_WRAPPER_CLOCALE_HPP_

#include <iostream>		// std::cout
#include <locale>		// std::locale
#include <stdexcept>	// std::runtime_error
#include <string>		// std::string

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class SetLocale {
	public:
		SetLocale(const std::string& locale);
	};

	/*
	 * IMPLEMENTATION
	 */
	inline SetLocale::SetLocale(const std::string& locale) {
		try {
			std::cout.imbue(std::locale(locale.c_str()));
		}
		catch(const std::runtime_error& e) {
			std::cout.imbue(std::locale());

			std::cout	<< "Could not set locale \""
						<< locale
						<< "\", using user-preferred \""
						<< std::locale("").name().c_str()
						<< "\" instead."
						<< std::endl;
		}
	}

} // spacelite::Wrapper

#endif /* SRC_WRAPPER_CLOCALE_HPP_ */
