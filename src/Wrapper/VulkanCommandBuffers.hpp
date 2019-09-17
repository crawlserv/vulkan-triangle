/*
 * VulkanCommandBuffers.hpp
 *
* Wraps the command buffers (one for each frame buffer / image view) used by the Vulkan API.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANCOMMANDBUFFERS_HPP_
#define SRC_WRAPPER_VULKANCOMMANDBUFFERS_HPP_

#include "VulkanCommandBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanError.hpp"
#include "VulkanFrameBuffers.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanVertexBuffer.hpp"

#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::move, std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanCommandBuffers {
	public:
		VulkanCommandBuffers(
				VulkanDevice& device,
				VulkanCommandPool& commandPool,
				VulkanRenderPass& renderPass,
				VulkanFrameBuffers& frameBuffers,
				VkExtent2D& targetExtent,
				VulkanPipeline& pipeline,
				VulkanVertexBuffer& vertexBuffer,
				unsigned long numberOfVertices
		);
		virtual ~VulkanCommandBuffers();

		// creation and destruction
		void create();
		void destroy();

		// getters
		VulkanCommandBuffer& get(unsigned int index);
		const VulkanCommandBuffer& get(unsigned int index) const;
		unsigned int getNumberOfBuffers() const;

		// not copyable, only moveable
		VulkanCommandBuffers(const VulkanCommandBuffers&) = delete;
		VulkanCommandBuffers(VulkanCommandBuffers&& other) noexcept;
		VulkanCommandBuffers& operator=(const VulkanCommandBuffers&) = delete;
		VulkanCommandBuffers& operator=(VulkanCommandBuffers&& other) noexcept;

	private:
		VulkanDevice& parent;
		VulkanCommandPool& pool;
		VulkanRenderPass& pass;
		VulkanFrameBuffers& fBuffers;
		VkExtent2D& extent;
		VulkanPipeline& pipe;
		VulkanVertexBuffer& vBuffer;

		std::vector<VulkanCommandBuffer> buffers;
		std::vector<VkCommandBuffer> data;
		unsigned long nVertices;
		bool _created;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: outsourced to create()
	inline VulkanCommandBuffers::VulkanCommandBuffers(
			VulkanDevice& device,
			VulkanCommandPool& commandPool,
			VulkanRenderPass& renderPass,
			VulkanFrameBuffers& frameBuffers,
			VkExtent2D& targetExtent,
			VulkanPipeline& pipeline,
			VulkanVertexBuffer& vertexBuffer,
			unsigned long numberOfVertices
	) :		parent(device),
			pool(commandPool),
			pass(renderPass),
			fBuffers(frameBuffers),
			extent(targetExtent),
			pipe(pipeline),
			vBuffer(vertexBuffer),
			nVertices(numberOfVertices),
			_created(false) {
		this->create();
	}

	// destructor: outsourced to destroy()
	inline VulkanCommandBuffers::~VulkanCommandBuffers() {
		this->destroy();
	}

	// create the command buffers
	inline void VulkanCommandBuffers::create() {
		// allocate command buffers
		const unsigned int num = this->fBuffers.getNumberOfBuffers();

		this->buffers.reserve(num);
		this->data.resize(num);

		VkCommandBufferAllocateInfo vulkanAllocInfo = {};

		vulkanAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		vulkanAllocInfo.commandPool = this->pool.get();
		vulkanAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		vulkanAllocInfo.commandBufferCount = num;

		VkResult vulkanResult = vkAllocateCommandBuffers(
				this->parent.get(),
				&vulkanAllocInfo,
				this->data.data()
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create command buffers: " + Wrapper::VulkanError(vulkanResult).str());

		// fill command buffers
		unsigned short n = 0;

		for(auto& commandBufferData : this->data) {
			this->buffers.emplace_back(
					commandBufferData,
					this->pass,
					this->fBuffers.get(n),
					this->extent,
					this->pipe,
					this->vBuffer,
					this->nVertices
			);

			++n;
		}

		this->_created = true;
	}

	// destroy the command buffers
	inline void VulkanCommandBuffers::destroy() {
		if(this->_created) {
			this->buffers.clear();

			vkFreeCommandBuffers(
					this->parent.get(),
					this->pool.get(),
					static_cast<unsigned int>(this->data.size()),
					this->data.data()
			);

			this->_created = false;

			this->data.clear();
		}
	}

	// get reference to the instance of an index-specified command buffer
	inline VulkanCommandBuffer& VulkanCommandBuffers::get(unsigned int index) {
		return this->buffers.at(index);
	}

	// get const reference to the instance of an index-specified command buffer
	inline const VulkanCommandBuffer& VulkanCommandBuffers::get(unsigned int index) const {
		return this->buffers.at(index);
	}

	// get number of command buffers
	inline unsigned int VulkanCommandBuffers::getNumberOfBuffers() const {
		return this->buffers.size();
	}

	// move constructor
	inline VulkanCommandBuffers::VulkanCommandBuffers(VulkanCommandBuffers&& other) noexcept
			:	parent(other.parent),
				pool(other.pool),
				pass(other.pass),
				fBuffers(other.fBuffers),
				extent(other.extent),
				pipe(other.pipe),
				vBuffer(other.vBuffer),
				buffers(std::move(other.buffers)),
				data(std::move(other.data)),
				nVertices(other.nVertices),
				_created(other._created) {
		other._created = false;
	}

	// move assignment
	inline VulkanCommandBuffers& VulkanCommandBuffers::operator=(VulkanCommandBuffers&& other) noexcept {
		using std::swap;

		swap(this->parent, other.parent);
		swap(this->pool, other.pool);
		swap(this->pass, other.pass);
		swap(this->fBuffers, other.fBuffers);
		swap(this->extent, other.extent);
		swap(this->pipe, other.pipe);
		swap(this->buffers, other.buffers);
		swap(this->data, other.data);
		swap(this->nVertices, other.nVertices);
		swap(this->_created, other._created);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANCOMMANDBUFFERS_HPP_ */
