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

#include "VulkanCommandPool.hpp"
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
		// constructors and destructor
		VulkanBuffer(
				VulkanDevice& device,
				VulkanPhysicalDevice& physicalDevice,
				VulkanCommandPool& commandPool,
				unsigned long size,
				bool isExclusive,
				VkBufferUsageFlags usageFlags,
				VkMemoryPropertyFlags memoryFlags
		);
		VulkanBuffer(
				const VulkanBuffer& buffer,
				VkBufferUsageFlags usageFlags,
				VkMemoryPropertyFlags memoryFlags
		);

		virtual ~VulkanBuffer();

		// destruction
		void destroy();

		// getters
		VkBuffer& get();
		const VkBuffer& get() const;

		// writers
		void fill(const void * in);
		void write(unsigned long offset, unsigned long size, const void * in);
		void copyFrom(const VulkanBuffer& other, VkDeviceSize size);

		// not (simply) copyable, only moveable
		VulkanBuffer(const VulkanBuffer&) = delete;
		VulkanBuffer(VulkanBuffer&& other) noexcept;
		VulkanBuffer& operator=(const VulkanBuffer&) = delete;
		VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

	protected:
		VulkanDevice& parent;
		VulkanPhysicalDevice& physical;
		VulkanCommandPool& pool;
		VkBuffer instance;
		VulkanDeviceMemory memory;

		unsigned long maxContentSize;
		bool exclusive;

	private:
		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor (1): create a new buffer
	inline VulkanBuffer::VulkanBuffer(
			VulkanDevice& device,
			VulkanPhysicalDevice& physicalDevice,
			VulkanCommandPool& commandPool,
			unsigned long size,
			bool isExclusive,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags
	) :			parent(device),
				physical(physicalDevice),
				pool(commandPool),
				instance(VK_NULL_HANDLE),
				memory(parent),
				maxContentSize(size),
				exclusive(isExclusive) {
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
				this->physical.findMemoryType(memRequirements.memoryTypeBits, memoryFlags)
		);

		// bind memory to buffer
		vkBindBufferMemory(this->parent.get(), this->instance, this->memory.get(), 0);
	}

	// constructor (2): create a buffer from an existing buffer (but possible changing usage and memor)
	inline VulkanBuffer::VulkanBuffer(
			const VulkanBuffer& buffer,
			VkBufferUsageFlags usageFlags,
			VkMemoryPropertyFlags memoryFlags
	) : VulkanBuffer(
			buffer.parent,
			buffer.physical,
			buffer.pool,
			buffer.maxContentSize,
			buffer.exclusive,
			usageFlags,
			memoryFlags
	) {}

	// destructor: outsourced to destroy()
	inline VulkanBuffer::~VulkanBuffer() {
		this->destroy();
	}

	// destroy the buffer
	inline void VulkanBuffer::destroy() {
		if(this->instance) {
			vkDestroyBuffer(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);

			this->instance = VK_NULL_HANDLE;
		}
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

	// copy into the buffer using the command pool for the copy command
	inline void VulkanBuffer::copyFrom(const VulkanBuffer& other, VkDeviceSize size) {
		VkCommandBufferAllocateInfo allocInfo = {};

		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = this->pool.get();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;

		vkAllocateCommandBuffers(this->parent.get(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};

		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferCopy copyRegion = {};

			copyRegion.size = size;

			vkCmdCopyBuffer(commandBuffer, other.get(), this->instance, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(this->parent.getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

		vkQueueWaitIdle(this->parent.getGraphicsQueue());

		vkFreeCommandBuffers(this->parent.get(), this->pool.get(), 1, &commandBuffer);
	}

	// move constructor
	inline VulkanBuffer::VulkanBuffer(VulkanBuffer&& other) noexcept
			:	parent(other.parent),
				physical(other.physical),
				pool(other.pool),
				instance(other.instance),
				memory(std::move(other.memory)),
				maxContentSize(other.maxContentSize),
				exclusive(other.exclusive) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanBuffer& VulkanBuffer::operator=(VulkanBuffer&& other) noexcept {
		this->instance = other.instance;
		this->maxContentSize = other.maxContentSize;
		this->exclusive = other.exclusive;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->physical, other.physical);
		swap(this->pool, other.pool);
		swap(this->memory, other.memory);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANBUFFER_HPP_ */
