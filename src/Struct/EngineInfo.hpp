/*
 * EngineInfo.hpp
 *
 * General information about the engine (name, version and API version).
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_ENGINEINFO_HPP_
#define SRC_STRUCT_ENGINEINFO_HPP_

#include <string>	// std::string

namespace spacelite::Struct {

	struct EngineInfo {
		std::string name;

		unsigned short verMajor;
		unsigned short verMinor;
		unsigned short verPatch;

		unsigned int vulkanVersion;

		EngineInfo(
				const std::string& setName,
				unsigned short setMajor,
				unsigned short setMinor,
				unsigned short setPatch,
				unsigned int setVulkanVersion
		) : name(setName), verMajor(setMajor), verMinor(setMinor), verPatch(setPatch), vulkanVersion(setVulkanVersion) {};
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_ENGINEINFO_HPP_ */
