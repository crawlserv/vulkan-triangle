/*
 * VulkanError.hpp
 *
 * Wraps an error that occured using the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANERROR_HPP_
#define SRC_WRAPPER_VULKANERROR_HPP_

#include <string>	// std::string

#include <vulkan/vulkan_core.h>

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanError {
	public:
		VulkanError(VkResult vulkanResult);

		std::string str();

	protected:
		VkResult errCode;
		std::string errDescription;
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: parse error by Vulkan API
	inline VulkanError::VulkanError(VkResult vulkanResult) : errCode(vulkanResult) {
		switch(this->errCode) {
#define STR(r) case VK_ ##r: this->errDescription = #r; break
			STR(NOT_READY);
			STR(TIMEOUT);
			STR(EVENT_SET);
			STR(EVENT_RESET);
			STR(INCOMPLETE);
			STR(ERROR_OUT_OF_HOST_MEMORY);
			STR(ERROR_OUT_OF_DEVICE_MEMORY);
			STR(ERROR_INITIALIZATION_FAILED);
			STR(ERROR_DEVICE_LOST);
			STR(ERROR_MEMORY_MAP_FAILED);
			STR(ERROR_LAYER_NOT_PRESENT);
			STR(ERROR_EXTENSION_NOT_PRESENT);
			STR(ERROR_FEATURE_NOT_PRESENT);
			STR(ERROR_INCOMPATIBLE_DRIVER);
			STR(ERROR_TOO_MANY_OBJECTS);
			STR(ERROR_FORMAT_NOT_SUPPORTED);
			STR(ERROR_SURFACE_LOST_KHR);
			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
			STR(SUBOPTIMAL_KHR);
			STR(ERROR_OUT_OF_DATE_KHR);
			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
			STR(ERROR_VALIDATION_FAILED_EXT);
			STR(ERROR_INVALID_SHADER_NV);
			STR(ERROR_NOT_PERMITTED_EXT);
			STR(ERROR_OUT_OF_POOL_MEMORY_KHR);
			STR(ERROR_INVALID_EXTERNAL_HANDLE_KHR);
			STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
			STR(ERROR_FRAGMENTATION_EXT);
			STR(ERROR_INVALID_DEVICE_ADDRESS_EXT);
			STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
			STR(RESULT_BEGIN_RANGE);
			STR(RESULT_RANGE_SIZE);
			STR(RESULT_MAX_ENUM);
			STR(SUCCESS);
#undef STR
		}
	}

	// get stringified version of error by Vulkan API
	inline std::string VulkanError::str() {
		return "[" + std::to_string(this->errCode) + "] " + this->errDescription;
	}

} // spacelite::Wrapper

#endif /* SRC_WRAPPER_VULKANERROR_HPP_ */
