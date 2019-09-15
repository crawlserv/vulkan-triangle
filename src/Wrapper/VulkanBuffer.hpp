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
#include "VulkanError.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>		// std::swap

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
		unsigned long getRequiredSize() const;
		unsigned int getRequiredTypeIndex() const;
		unsigned long getMaxContentSize() const;

		// not copyable, only moveable
		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer(VulkanBuffer&& other) noexcept;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkBuffer instance;

		unsigned long requiredSize;
		unsigned int requiredTypeIndex;
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
	) : parent(device), instance(VK_NULL_HANDLE), requiredSize(0), requiredTypeIndex(0), maxContentSize(size) {
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

		this->requiredSize = memRequirements.size;
		this->requiredTypeIndex = physicalDevice.findMemoryType(memRequirements.memoryTypeBits, memoryFlags);
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

	// get the required memory size for the buffer in bytes
	inline unsigned long VulkanBuffer::getRequiredSize() const {
		return this->requiredSize;
	}

	// get the index of the required memory type for the buffer
	inline unsigned int VulkanBuffer::getRequiredTypeIndex() const {
		return this->requiredTypeIndex;
	}

	// get the maximum size of the buffer content in bytes
	inline unsigned long VulkanBuffer::getMaxContentSize() const {
		return this->maxContentSize;
	}

	// move constructor
	inline VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				requiredSize(other.requiredSize),
				requiredTypeIndex(other.requiredTypeIndex),
				maxContentSize(other.maxContentSize) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
		this->instance = other.instance;
		this->requiredSize = other.requiredSize;
		this->requiredTypeIndex = other.requiredTypeIndex;
		this->maxContentSize = other.maxContentSize;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANBUFFER_HPP_ */
