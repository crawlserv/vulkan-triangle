/*
 * GlfwError.hpp
 *
 * Wraps an error that occured using GLFW.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_GLFWERROR_HPP_
#define SRC_WRAPPER_GLFWERROR_HPP_

#include <string>	// std::string, std::to_string

#include <GLFW/glfw3.h>

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */

	class GlfwError {
	public:
		GlfwError();

		std::string str();

	protected:
		int errCode;
		std::string errDescription;
	};

	/*
	 * IMPLEMENTATION
	 */
	inline GlfwError::GlfwError() {
		const char * cErrDescription = nullptr;

		this->errCode = glfwGetError(&cErrDescription);
		this->errDescription = std::string(cErrDescription);
	}

	inline std::string GlfwError::str() {
		return "[" + std::to_string(this->errCode) + "] " + this->errDescription;
	}

} // spacelite::Wrapper

#endif /* SRC_WRAPPER_GLFWERROR_HPP_ */
