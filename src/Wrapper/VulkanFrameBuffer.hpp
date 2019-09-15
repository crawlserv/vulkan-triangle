/*
 * VulkanFrameBuffers.hpp
 *
 * Wraps a frame buffer for the Vulkan API.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANFRAMEBUFFER_HPP_
#define SRC_WRAPPER_VULKANFRAMEBUFFER_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"
#include "VulkanImageView.hpp"
#include "VulkanRenderPass.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanFrameBuffer {
	public:
		VulkanFrameBuffer(
				VulkanDevice& device,
				const VulkanImageView& imageView,
				const VulkanRenderPass& renderPass,
				const VkExtent2D& extent
		);
		virtual ~VulkanFrameBuffer();

		// creation and destruction
		void create();
		void destroy() noexcept;

		// getters
		VkFramebuffer& get();
		const VkFramebuffer& get() const;
		unsigned int getNumber() const;

		// not copyable, only moveable
		VulkanFrameBuffer(const VulkanFrameBuffer&) = delete;
		VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept;
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer&) = delete;
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkFramebuffer instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create frame buffers
	inline VulkanFrameBuffer::VulkanFrameBuffer(
			VulkanDevice& device,
			const VulkanImageView& imageView,
			const VulkanRenderPass& renderPass,
			const VkExtent2D& extent
	) : parent(device), instance(VK_NULL_HANDLE) {
		VkImageView attachments[] = {
			imageView.get()
		};

		VkFramebufferCreateInfo vulkanFrameBufferInfo = {};

		vulkanFrameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vulkanFrameBufferInfo.renderPass = renderPass.get();
		vulkanFrameBufferInfo.attachmentCount = 1;
		vulkanFrameBufferInfo.pAttachments = attachments;
		vulkanFrameBufferInfo.width = extent.width;
		vulkanFrameBufferInfo.height = extent.height;
		vulkanFrameBufferInfo.layers = 1;

		VkResult vulkanResult = vkCreateFramebuffer(
				this->parent.get(),
				&vulkanFrameBufferInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create frame buffer: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: outsourced to destroy()
	inline VulkanFrameBuffer::~VulkanFrameBuffer() {
		if(this->instance)
			vkDestroyFramebuffer(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the frame buffer
	inline VkFramebuffer& VulkanFrameBuffer::get() {
		return this->instance;
	}

	// get const reference to the instance of the frame buffer
	inline const VkFramebuffer& VulkanFrameBuffer::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanFrameBuffer::VulkanFrameBuffer(VulkanFrameBuffer&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanFrameBuffer& VulkanFrameBuffer::operator=(VulkanFrameBuffer&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANFRAMEBUFFER_HPP_ */
