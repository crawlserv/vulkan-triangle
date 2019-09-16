/*
 * VulkanDeviceMemory.hpp
 *
 * Wraps device memory for the Vulkan API.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANDEVICEMEMORY_HPP_
#define SRC_WRAPPER_VULKANDEVICEMEMORY_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>		// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanDeviceMemory {
	public:
		VulkanDeviceMemory(VulkanDevice& device);
		virtual ~VulkanDeviceMemory();

		// allocator
		void allocate(unsigned long size, unsigned int typeIndex);

		// getters
		VkDeviceMemory& get();
		const VkDeviceMemory& get() const;

		// not copyable, only moveable
		VulkanDeviceMemory(const VulkanDeviceMemory&) = delete;
		VulkanDeviceMemory(VulkanDeviceMemory&& other) noexcept;
		VulkanDeviceMemory& operator=(const VulkanDeviceMemory&) = delete;
		VulkanDeviceMemory& operator=(VulkanDeviceMemory&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkDeviceMemory instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: save device instance
	inline VulkanDeviceMemory::VulkanDeviceMemory(VulkanDevice& device) : parent(device), instance(VK_NULL_HANDLE) {}

	// allocate memory
	inline void VulkanDeviceMemory::allocate(unsigned long size, unsigned int typeIndex) {
		VkMemoryAllocateInfo vulkanAllocInfo = {};

		vulkanAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		vulkanAllocInfo.allocationSize = size;
		vulkanAllocInfo.memoryTypeIndex = typeIndex;

		VkResult vulkanResult = vkAllocateMemory(
				this->parent.get(),
				&vulkanAllocInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not allocate memory for vertex buffer: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: free allocated memory if necessary
	inline VulkanDeviceMemory::~VulkanDeviceMemory() {
		if(this->instance)
			vkFreeMemory(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the device memory
	inline VkDeviceMemory& VulkanDeviceMemory::get() {
		return this->instance;
	}

	// get const reference to the instance of the device memory
	inline const VkDeviceMemory& VulkanDeviceMemory::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanDeviceMemory::VulkanDeviceMemory(VulkanDeviceMemory&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanDeviceMemory& VulkanDeviceMemory::operator=(VulkanDeviceMemory&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANDEVICEMEMORY_HPP_ */
