/*
 * VulkanRequirements.hpp
 *
 * Validation layers and extensions required from the device using the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_VULKANREQUIREMENTS_HPP_
#define SRC_STRUCT_VULKANREQUIREMENTS_HPP_

#include <vector>	// std::vector

namespace spacelite::Struct {

	struct VulkanRequirements {
		std::vector<const char *> validationLayers;
		std::vector<const char *> deviceExtensions;

		VulkanRequirements(
				const std::vector<const char *>& requiredValidationLayers,
				const std::vector<const char *>& requiredDeviceExtensions
		) : validationLayers(requiredValidationLayers), deviceExtensions(requiredDeviceExtensions) {}
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_VULKANREQUIREMENTS_HPP_ */
