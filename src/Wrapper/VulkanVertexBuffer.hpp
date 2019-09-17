/*
 * VulkanVertexBuffer.hpp
 *
 * Wraps a vertex buffer for the Vulkan API by using the base class for general buffers.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANVERTEXBUFFER_HPP_
#define SRC_WRAPPER_VULKANVERTEXBUFFER_HPP_

#include "VulkanBuffer.hpp"

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanVertexBuffer : public VulkanBuffer {
	public:
		VulkanVertexBuffer(
				VulkanDevice& device,
				VulkanPhysicalDevice& physicalDevice,
				VulkanCommandPool& commandPool,
				bool isExclusive,
				unsigned long size,
				const void * in
		);
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create a staging buffer first, fill it and swap it with the vertex buffer on the device
	inline VulkanVertexBuffer::VulkanVertexBuffer(
			VulkanDevice& device,
			VulkanPhysicalDevice& physicalDevice,
			VulkanCommandPool& commandPool,
			bool isExclusive,
			unsigned long size,
			const void * in
	) : VulkanBuffer(
			device,
			physicalDevice,
			commandPool,
			size,
			isExclusive,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	) {
		// fill staging buffer
		this->fill(in);

		// create on-device buffer
		VulkanBuffer onDevice(
				*this,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		// copy data to the on-device buffer
		onDevice.copyFrom(*this, size);

		// swap buffers (staging buffer will be destroyed after the end of construction)
		using std::swap;

		swap((VulkanBuffer&) *this, onDevice);
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANVERTEXBUFFER_HPP_ */
