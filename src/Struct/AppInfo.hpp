/*
 * AppInfo.hpp
 *
 * General information about the application (name and version).
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_APPINFO_HPP_
#define SRC_STRUCT_APPINFO_HPP_

#include <string>	// std::string

namespace spacelite::Struct {

	struct AppInfo {
		std::string name;

		unsigned short verMajor;
		unsigned short verMinor;
		unsigned short verPatch;

		AppInfo(
				const std::string& setName,
				unsigned short setMajor,
				unsigned short setMinor,
				unsigned short setPatch
		) : name(setName), verMajor(setMajor), verMinor(setMinor), verPatch(setPatch) {};
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_APPINFO_HPP_ */
