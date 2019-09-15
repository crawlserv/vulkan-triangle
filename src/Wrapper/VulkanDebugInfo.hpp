/*
 * VulkanDebugInfo.hpp
 *
 * Wraps the information structure for a Vulkan API debugger.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANDEBUGINFO_HPP_
#define SRC_WRAPPER_VULKANDEBUGINFO_HPP_

#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <iostream>	// std::cout, std::endl

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanDebugInfo {
	public:
		VulkanDebugInfo();

		// getters
		VkDebugUtilsMessengerCreateInfoEXT * getPtr();
		const VkDebugUtilsMessengerCreateInfoEXT * getPtr() const;

	private:
		VkDebugUtilsMessengerCreateInfoEXT info;

		static VKAPI_ATTR VkBool32 VKAPI_CALL callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
			void * pUserData
		);

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline VulkanDebugInfo::VulkanDebugInfo() : info({}) {
		this->info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		this->info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		this->info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		this->info.pfnUserCallback = VulkanDebugInfo::callback;
	}

	inline VkDebugUtilsMessengerCreateInfoEXT * VulkanDebugInfo::getPtr() {
		return &(this->info);
	}

	inline const VkDebugUtilsMessengerCreateInfoEXT * VulkanDebugInfo::getPtr() const {
		return &(this->info);
	}

	inline VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugInfo::callback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType __attribute__ ((unused)),
			const VkDebugUtilsMessengerCallbackDataEXT * pCallbackData,
			void * pUserData __attribute__ ((unused))
	) {
		switch(messageSeverity) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			std::cout << "[dbg] WARNING: " << pCallbackData->pMessage << std::endl;

			break;

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			throw Exception(pCallbackData->pMessage);

		default:
			std::cout << "[dbg] " << pCallbackData->pMessage << std::endl;

			break;
		}

		return VK_FALSE;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANDEBUGINFO_HPP_ */
