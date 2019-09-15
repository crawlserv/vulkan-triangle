/*
 * VulkanRenderPass.hpp
 *
 * Wraps a render pass used by the Vulkan API.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANRENDERPASS_HPP_
#define SRC_WRAPPER_VULKANRENDERPASS_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanRenderPass {
	public:
		VulkanRenderPass(VulkanDevice& device, VkFormat& targetFormat);
		virtual ~VulkanRenderPass();

		// creation and destruction
		void create();
		void destroy() noexcept;

		// getters
		VkRenderPass& get();
		const VkRenderPass& get() const;

		// not copyable, only moveable
		VulkanRenderPass(const VulkanRenderPass&) = delete;
		VulkanRenderPass(VulkanRenderPass&& other) noexcept;
		VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
		VulkanRenderPass& operator=(VulkanRenderPass&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkRenderPass instance;
		VkFormat& format;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: outsourced to create()
	inline VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, VkFormat& targetFormat)
			: parent(device), instance(VK_NULL_HANDLE), format(targetFormat) {
		this->create();
	}

	// destructor: outsourced to destroy()
	inline VulkanRenderPass::~VulkanRenderPass() {
		this->destroy();
	}

	// create the render pass
	inline void VulkanRenderPass::create() {
		VkAttachmentDescription vulkanColorAttachment = {};

		vulkanColorAttachment.format = this->format;
		vulkanColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		vulkanColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vulkanColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vulkanColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vulkanColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vulkanColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vulkanColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference vulkanColorAttachmentRef = {};

		vulkanColorAttachmentRef.attachment = 0;
		vulkanColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription vulkanSubpass = {};

		vulkanSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vulkanSubpass.colorAttachmentCount = 1;
		vulkanSubpass.pColorAttachments = &vulkanColorAttachmentRef;

		// subpass dependencies
		VkSubpassDependency vulkanDependency = {};

		vulkanDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		vulkanDependency.dstSubpass = 0;
		vulkanDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vulkanDependency.srcAccessMask = 0;
		vulkanDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vulkanDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo vulkanRenderPassInfo = {};

		vulkanRenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vulkanRenderPassInfo.attachmentCount = 1;
		vulkanRenderPassInfo.pAttachments = &vulkanColorAttachment;
		vulkanRenderPassInfo.subpassCount = 1;
		vulkanRenderPassInfo.pSubpasses = &vulkanSubpass;
		vulkanRenderPassInfo.dependencyCount = 1;
		vulkanRenderPassInfo.pDependencies = &vulkanDependency;

		VkResult vulkanResult = vkCreateRenderPass(
				this->parent.get(),
				&vulkanRenderPassInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create render pass: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destroy the render pass (may not throw an exception as it is used by the destructor, too)
	inline void VulkanRenderPass::destroy() noexcept {
		if(this->instance) {
			vkDestroyRenderPass(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);

			this->instance = VK_NULL_HANDLE;
		}
	}

	// get reference to the instance of the render pass
	inline VkRenderPass& VulkanRenderPass::get() {
		return this->instance;
	}

	// get const reference to the instance of the render pass
	inline const VkRenderPass& VulkanRenderPass::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanRenderPass::VulkanRenderPass(VulkanRenderPass&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				format(other.format) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanRenderPass& VulkanRenderPass::operator=(VulkanRenderPass&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->format, other.format);

		return *this;
	}

} /* spacelite::Wrapper */



#endif /* SRC_WRAPPER_VULKANRENDERPASS_HPP_ */
