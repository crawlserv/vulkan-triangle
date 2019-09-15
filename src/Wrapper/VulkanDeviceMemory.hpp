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

#include "VulkanBuffer.hpp"
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
		VulkanDeviceMemory(VulkanDevice& device, unsigned long size, unsigned int typeIndex);
		VulkanDeviceMemory(VulkanDevice& device, const VulkanBuffer& buffer);
		VulkanDeviceMemory(VulkanDevice& device, const VulkanBuffer& buffer, unsigned long size, const void * in);
		virtual ~VulkanDeviceMemory();

		// getters
		VkDeviceMemory& get();
		const VkDeviceMemory& get() const;

		// writer
		void write(const VulkanBuffer& buffer, unsigned long size, const void * in);

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

	// constructor (1): allocate device memory
	inline VulkanDeviceMemory::VulkanDeviceMemory(
			VulkanDevice& device,
			unsigned long size,
			unsigned int typeIndex
	) : parent(device), instance(VK_NULL_HANDLE) {
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

	// constructor (2): allocate device memory for buffer
	inline VulkanDeviceMemory::VulkanDeviceMemory(
			VulkanDevice& device,
			const VulkanBuffer& buffer
	) : VulkanDeviceMemory(device, buffer.getRequiredSize(), buffer.getRequiredTypeIndex()) {}

	// constructor (3): allocate device memory for buffer and instantly write into it
	inline VulkanDeviceMemory::VulkanDeviceMemory(
			VulkanDevice& device,
			const VulkanBuffer& buffer,
			unsigned long size,
			const void * in
	) : VulkanDeviceMemory(device, buffer) {
		this->write(buffer, size, in);
	}

	// destructor: free allocated memory
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

	// write to buffer
	inline void VulkanDeviceMemory::write(const VulkanBuffer& buffer, unsigned long size, const void * in) {
		vkBindBufferMemory(this->parent.get(), buffer.get(), this->instance, 0);

		// map GPU memory into CPU accessible memory
		void * data = nullptr;

		vkMapMemory(this->parent.get(), this->instance, 0, size, 0, &data);

		std::memcpy(data, in, (size_t) size);

		vkUnmapMemory(this->parent.get(), this->instance);
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
