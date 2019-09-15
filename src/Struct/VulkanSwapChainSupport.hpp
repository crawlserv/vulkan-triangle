/*
 * VulkanQueueFamily.hpp
 *
 * Supported swap chain properties of a device using the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_STRUCT_VULKANSWAPCHAINSUPPORT_HPP_
#define SRC_STRUCT_VULKANSWAPCHAINSUPPORT_HPP_

#include <vulkan/vulkan_core.h>

#include <vector>	// std::vector

namespace spacelite::Struct {

	struct VulkanSwapChainSupport {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

} /* spacelite::Struct */

#endif /* SRC_STRUCT_VULKANSWAPCHAINSUPPORT_HPP_ */
