/*
 * VulkanCommandBuffer.hpp
 *
 * Wrapping a command buffer for the Vulkan API.
 *
 * Contains only a reference to an already allocated instance (because instances are allocated en bloc).
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANCOMMANDBUFFER_HPP_
#define SRC_WRAPPER_VULKANCOMMANDBUFFER_HPP_

#include "VulkanError.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanVertexBuffer.hpp"

#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanCommandBuffer {
	public:
		VulkanCommandBuffer(
				VkCommandBuffer& dataRef,
				VulkanRenderPass& renderPass,
				VulkanFrameBuffer& frameBuffer,
				const VkExtent2D& extent,
				VulkanPipeline& pipeline,
				VulkanVertexBuffer& vertexBuffer,
				unsigned long numberOfVertices
		);

		// getters
		VkCommandBuffer& get();
		const VkCommandBuffer& get() const;

		// not copyable, only moveable
		VulkanCommandBuffer(const VulkanCommandBuffer&) = delete;
		VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept;
		VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
		VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept;

	private:
		VkCommandBuffer& instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline VulkanCommandBuffer::VulkanCommandBuffer(
			VkCommandBuffer& dataRef,
			VulkanRenderPass& renderPass,
			VulkanFrameBuffer& frameBuffer,
			const VkExtent2D& extent,
			VulkanPipeline& pipeline,
			VulkanVertexBuffer& vertexBuffer,
			unsigned long numberOfVertices
	) : instance(dataRef) {
		VkCommandBufferBeginInfo beginInfo = {};

		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult vulkanResult = vkBeginCommandBuffer(this->instance, &beginInfo);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not begin command buffer: " + Wrapper::VulkanError(vulkanResult).str());

		// start render pass
		VkRenderPassBeginInfo renderPassInfo = {};

		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.get();
		renderPassInfo.framebuffer = frameBuffer.get();
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = extent;

		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(this->instance, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		{
			// bind graphics pipeline
			vkCmdBindPipeline(this->instance, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());

			// bind vertex buffer
			VkBuffer vulkanVertexBuffers[] = { vertexBuffer.get() };
			VkDeviceSize vulkanOffsets[] = { 0 };

			vkCmdBindVertexBuffers(this->instance, 0, 1, vulkanVertexBuffers, vulkanOffsets);

			// draw vertices
			vkCmdDraw(this->instance, static_cast<unsigned int>(numberOfVertices), 1, 0, 0);
		}

		// finish render pass
		vkCmdEndRenderPass(this->instance);

		// finish command buffer
		vulkanResult = vkEndCommandBuffer(this->instance);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not finish command buffer: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// get reference to the instance of the command buffer
	inline VkCommandBuffer& VulkanCommandBuffer::get() {
		return this->instance;
	}

	// get const reference to the instance of the command buffer
	inline const VkCommandBuffer& VulkanCommandBuffer::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept
			:	instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanCommandBuffer& VulkanCommandBuffer::operator=(VulkanCommandBuffer&& other) noexcept {
		using std::swap;

		swap(this->instance, other.instance);

		return *this;
	}
}

#endif /* SRC_WRAPPER_VULKANCOMMANDBUFFER_HPP_ */
