/*
 * VulkanQueueFamilies.hpp
 *
 * Indices for the graphics and family queue of a device using the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_VULKANQUEUEFAMILIES_HPP_
#define SRC_STRUCT_VULKANQUEUEFAMILIES_HPP_

#include <optional>	// std::optional

namespace spacelite::Struct {

	struct VulkanQueueFamilies {
		std::optional<unsigned int> graphicsFamily;
		std::optional<unsigned int> presentFamily;

		bool isComplete() const {
			return this->graphicsFamily.has_value() && this->presentFamily.has_value();
		}
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_VULKANQUEUEFAMILIES_HPP_ */
