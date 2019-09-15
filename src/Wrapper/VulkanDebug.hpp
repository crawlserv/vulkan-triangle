/*
 * VulkanDebug.hpp
 *
 * Wraps a debug messenger for the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANDEBUG_HPP_
#define SRC_WRAPPER_VULKANDEBUG_HPP_

#include "VulkanDebugInfo.hpp"
#include "VulkanError.hpp"
#include "VulkanInstance.hpp"

#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanDebug {
	public:
		VulkanDebug(VulkanInstance& instance, const VulkanDebugInfo& debugInfo);
		virtual ~VulkanDebug();

		// not copyable, only moveable
		VulkanDebug(const VulkanDebug&) = delete;
		VulkanDebug(VulkanDebug&& other) noexcept;
		VulkanDebug& operator=(const VulkanDebug&) = delete;
		VulkanDebug& operator=(VulkanDebug&& other) noexcept;

	private:
		VulkanInstance& parent;
		VkDebugUtilsMessengerEXT instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline VulkanDebug::VulkanDebug(
			VulkanInstance& instance,
			const VulkanDebugInfo& debugInfo
#ifdef NDEBUG
			 __attribute__ ((unused))
#endif
	) : parent(instance), instance(VK_NULL_HANDLE) {
#ifndef NDEBUG
		auto creationFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
				this->parent.get(),
				"vkCreateDebugUtilsMessengerEXT"
		);

		VkResult vulkanResult = VK_NOT_READY;

		if(creationFunction)
			vulkanResult = creationFunction(
					this->parent.get(),
					debugInfo.getPtr(),
					Helper::VulkanAllocator::ptr,
					&(this->instance)
			);
		else
			vulkanResult = VK_ERROR_EXTENSION_NOT_PRESENT;

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not setup debug messenger for Vulkan API: " + Wrapper::VulkanError(vulkanResult).str());
#endif
	}

	inline VulkanDebug::~VulkanDebug() {
		if(this->instance) {
			auto destructionFunction = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
					this->parent.get(),
					"vkDestroyDebugUtilsMessengerEXT"
			);

			if(destructionFunction)
				destructionFunction(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
		}
	}

	// move constructor
	inline VulkanDebug::VulkanDebug(VulkanDebug&& other) noexcept : parent(other.parent), instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanDebug& VulkanDebug::operator=(VulkanDebug&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANDEBUG_HPP_ */
