/*
 * VulkanBuffer.hpp
 *
 * Wrapping a buffer for the Vulkan API.
 *
 * Derived classes can specify the flags for different buffer types.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANBUFFER_HPP_
#define SRC_WRAPPER_VULKANBUFFER_HPP_

#include "VulkanDevice.hpp"
#include "VulkanDeviceMemory.hpp"
#include "VulkanError.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>		// std::move, std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanBuffer {
	public:
		VulkanBuffer(
				VulkanDevice& device,
				const VulkanPhysicalDevice& physicalDevice,
				unsigned long size,
				bool isExclusive,
				VkBufferUsageFlags usageFlags,
				VkMemoryPropertyFlags memoryFlags
		);
		virtual ~VulkanBuffer();

		// getters
		VkBuffer& get();
		const VkBuffer& get() const;

		// writers
		void fill(const void * in);
		void write(unsigned long offset, unsigned long size, const void * in);

		// not copyable, only moveable
		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer(VulkanBuffer&& other) noexcept;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkBuffer instance;
		VulkanDeviceMemory memory;

		unsigned long maxContentSize;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline VulkanBuffer::VulkanBuffer(
			VulkanDevice& device,
			const VulkanPhysicalDevice& physicalDevice,
			unsigned long size,
			bool isExclusive,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags
	) :	parent(device), instance(VK_NULL_HANDLE), memory(parent), maxContentSize(size) {
		// create buffer
		VkBufferCreateInfo vulkanBufferInfo = {};

		vulkanBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vulkanBufferInfo.size = size;
		vulkanBufferInfo.usage = usageFlags;
		vulkanBufferInfo.sharingMode = isExclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

		VkResult vulkanResult = vkCreateBuffer(
				this->parent.get(),
				&vulkanBufferInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create vertex buffer: " + Wrapper::VulkanError(vulkanResult).str());

		// allocate GPU memory
		VkMemoryRequirements memRequirements;

		vkGetBufferMemoryRequirements(
				this->parent.get(),
				this->instance,
				&memRequirements
		);

		this->memory.allocate(
				memRequirements.size,
				physicalDevice.findMemoryType(memRequirements.memoryTypeBits, memoryFlags)
		);

		// bind memory to buffer
		vkBindBufferMemory(this->parent.get(), this->instance, this->memory.get(), 0);
	}

	inline VulkanBuffer::~VulkanBuffer() {
		if(this->instance)
			vkDestroyBuffer(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the buffer
	inline VkBuffer& VulkanBuffer::get() {
		return this->instance;
	}

	// get const reference to the instance of the buffer
	inline const VkBuffer& VulkanBuffer::get() const {
		return this->instance;
	}

	// fill the whole buffer
	inline void VulkanBuffer::fill(const void * in) {
		this->write(0, this->maxContentSize, in);
	}

	// write into the buffer
	inline void VulkanBuffer::write(unsigned long offset, unsigned long size, const void * in) {
		void * data;

		vkMapMemory(this->parent.get(), this->memory.get(), offset, size, 0, &data);

		memcpy(data, in, size);

		vkUnmapMemory(this->parent.get(), this->memory.get());
	}

	// move constructor
	inline VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				memory(std::move(other.memory)),
				maxContentSize(other.maxContentSize) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
		this->instance = other.instance;
		this->maxContentSize = other.maxContentSize;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->memory, other.memory);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANBUFFER_HPP_ */
