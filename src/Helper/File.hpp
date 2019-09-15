/*
 * File.hpp
 *
 * Helper function for handling files.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_HELPER_FILE_HPP_
#define SRC_HELPER_FILE_HPP_

#include "../Main/Exception.hpp"

#include <fstream>	// std::ifstream
#include <string>	// std::string
#include <vector>	// std::vector

#ifndef NDEBUG
#include <iostream>	// std::cout, std::endl
#endif

namespace spacelite::Helper::File {

	/*
	 * DECLARATION
	 */

	// read binary file
	std::vector<char> readBinary(const std::string& fileName);

	// exception class
	MAIN_EXCEPTION_CLASS();

	/*
	 * IMPLEMENTATION
	 */

	// read binary file
	inline std::vector<char> readBinary(const std::string& fileName) {
		std::ifstream stream(fileName, std::ios::ate | std::ios::binary);

		if(!stream.is_open())
			throw Exception("Could not open file: " + fileName);

		// get size and reserve memory
		size_t fileSize = (size_t) stream.tellg();

		std::vector<char> buffer(fileSize);

		// write debug output to stdout (if necessary)
#ifndef NDEBUG
		std::cout << "> " << fileSize << " bytes from " << fileName << std::endl;
#endif

		// read content and close file
		stream.seekg(0);
		stream.read(buffer.data(), fileSize);
		stream.close();

		// return content
		return buffer;
	}

} /* spacelite::Helper */


#endif /* SRC_HELPER_FILE_HPP_ */
