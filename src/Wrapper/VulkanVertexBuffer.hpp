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

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanVertexBuffer : public VulkanBuffer {
	public:
		VulkanVertexBuffer(
				VulkanDevice& device,
				const VulkanPhysicalDevice& physicalDevice,
				bool isExclusive,
				unsigned long size,
				const void * in
		);
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: set specifics for the vertex buffer and delegate creation to the base constructor (that's all!)
	inline VulkanVertexBuffer::VulkanVertexBuffer(
			VulkanDevice& device,
			const VulkanPhysicalDevice& physicalDevice,
			bool isExclusive,
			unsigned long size,
			const void * in
	) : VulkanBuffer(
			device,
			physicalDevice,
			size,
			isExclusive,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	) {
		this->fill(in);
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANVERTEXBUFFER_HPP_ */
