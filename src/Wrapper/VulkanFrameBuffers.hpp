/*
 * VulkanFrameBuffers.hpp
 *
 * Wraps the frame buffers (one for each image view) used by the Vulkan API.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANFRAMEBUFFERS_HPP_
#define SRC_WRAPPER_VULKANFRAMEBUFFERS_HPP_

#include "VulkanDevice.hpp"
#include "VulkanFrameBuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanSwapChain.hpp"

#include <utility>	// std::move, std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanFrameBuffers {
	public:
		VulkanFrameBuffers(VulkanDevice& device, VulkanSwapChain& swapChain, VulkanRenderPass& renderPass);
		virtual ~VulkanFrameBuffers();

		// creation and destruction
		void create();
		void destroy();

		// getters
		VulkanFrameBuffer& get(unsigned int index);
		const VulkanFrameBuffer& get(unsigned int index) const;
		unsigned int getNumberOfBuffers() const;

		// not copyable, only moveable
		VulkanFrameBuffers(const VulkanFrameBuffers&) = delete;
		VulkanFrameBuffers(VulkanFrameBuffers&& other) noexcept;
		VulkanFrameBuffers& operator=(const VulkanFrameBuffers&) = delete;
		VulkanFrameBuffers& operator=(VulkanFrameBuffers&& other) noexcept;

	private:
		VulkanDevice& parent;
		std::vector<VulkanFrameBuffer> buffers;
		VulkanSwapChain& chain;
		VulkanRenderPass& pass;
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: outsourced to create()
	inline VulkanFrameBuffers::VulkanFrameBuffers(
			VulkanDevice& device,
			VulkanSwapChain& swapChain,
			VulkanRenderPass& renderPass
	) : parent(device), chain(swapChain), pass(renderPass) {
		this->create();
	}

	// destructor: outsourced to destroy()
	inline VulkanFrameBuffers::~VulkanFrameBuffers() {}

	// create the frame buffers
	inline void VulkanFrameBuffers::create() {
		this->buffers.reserve(this->chain.getNumberOfImageViews());

		for(unsigned int n = 0; n < this->chain.getNumberOfImageViews(); ++n)
			this->buffers.emplace_back(this->parent, this->chain.getImageView(n), this->pass, this->chain.getExtent());
	}

	// destroy the frame buffers
	inline void VulkanFrameBuffers::destroy() {
		this->buffers.clear();
	}

	// get reference to the instance of an index-specified frame buffer
	inline VulkanFrameBuffer& VulkanFrameBuffers::get(unsigned int index) {
		return this->buffers.at(index);
	}

	// get const reference to the instance of an index-specified frame buffer
	inline const VulkanFrameBuffer& VulkanFrameBuffers::get(unsigned int index) const {
		return this->buffers.at(index);
	}

	// get number of frame buffers
	inline unsigned int VulkanFrameBuffers::getNumberOfBuffers() const {
		return this->buffers.size();
	}

	// move constructor
	inline VulkanFrameBuffers::VulkanFrameBuffers(VulkanFrameBuffers&& other) noexcept
			:	parent(other.parent),
				buffers(std::move(other.buffers)),
				chain(other.chain),
				pass(other.pass) {}

	// move assignment
	inline VulkanFrameBuffers& VulkanFrameBuffers::operator=(VulkanFrameBuffers&& other) noexcept {
		using std::swap;

		swap(this->parent, other.parent);
		swap(this->buffers, other.buffers);
		swap(this->chain, other.chain);
		swap(this->pass, other.pass);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANFRAMEBUFFERS_HPP_ */
