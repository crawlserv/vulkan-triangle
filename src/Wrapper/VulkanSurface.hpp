/*
 * VulkanSurface.hpp
 *
 * Wraps the render surface of a window used by the Vulkan API.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANSURFACE_HPP_
#define SRC_WRAPPER_VULKANSURFACE_HPP_

#include "VulkanError.hpp"
#include "VulkanInstance.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Main/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanSurface {
	public:
		VulkanSurface(
				VulkanInstance& vInstance,
				Main::Window& window
		);
		virtual ~VulkanSurface();

		// getters
		VkSurfaceKHR& get();
		const VkSurfaceKHR& get() const;

		// not copyable, only moveable
		VulkanSurface(const VulkanSurface&) = delete;
		VulkanSurface(VulkanSurface&& other) noexcept;
		VulkanSurface& operator=(const VulkanSurface&) = delete;
		VulkanSurface& operator=(VulkanSurface&& other) noexcept;

	private:
		VulkanInstance& parent;
		VkSurfaceKHR instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the surface
	inline VulkanSurface::VulkanSurface(
			VulkanInstance& vInstance,
			Main::Window& window
	) : parent(vInstance), instance(VK_NULL_HANDLE) {
		VkResult vulkanResult = glfwCreateWindowSurface(
				this->parent.get(),
				window.getPointer(),
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create surface: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy the surface
	inline VulkanSurface::~VulkanSurface() {
		if(this->instance)
			vkDestroySurfaceKHR(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the surface
	inline VkSurfaceKHR& VulkanSurface::get() {
		return this->instance;
	}

	// get const reference to the instance of the surface
	inline const VkSurfaceKHR& VulkanSurface::get() const {
		return this->instance;
	}

}

#endif /* SRC_WRAPPER_VULKANSURFACE_HPP_ */
