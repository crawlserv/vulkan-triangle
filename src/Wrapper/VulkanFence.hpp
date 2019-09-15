/*
 * VulkanFence.hpp
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef WRAPPER_VULKANFENCE_HPP_
#define WRAPPER_VULKANFENCE_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanFence{
	public:
		VulkanFence(VulkanDevice& device);
		virtual ~VulkanFence();

		// getters
		VkFence& get();
		const VkFence& get() const;

		// waiter
		void waitFor() const;

		// not copyable, only moveable
		VulkanFence(const VulkanFence&) = delete;
		VulkanFence(VulkanFence&& other) noexcept;
		VulkanFence& operator=(const VulkanFence&) = delete;
		VulkanFence& operator=(VulkanFence&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkFence instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the semaphore
	inline VulkanFence::VulkanFence(VulkanDevice& device) : parent(device), instance(VK_NULL_HANDLE) {
		VkFenceCreateInfo vulkanFenceInfo = {};

		vulkanFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		vulkanFenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		VkResult vulkanResult = vkCreateFence(
				this->parent.get(),
				&vulkanFenceInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create fence: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy the semaphore
	inline VulkanFence::~VulkanFence() {
		if(this->instance)
			vkDestroyFence(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the semaphore
	inline VkFence& VulkanFence::get() {
		return this->instance;
	}

	// get const reference to the instance of the semaphore
	inline const VkFence& VulkanFence::get() const {
		return this->instance;
	}

	// wait for fence
	inline void VulkanFence::waitFor() const {
		vkWaitForFences(this->parent.get(), 1, &(this->instance), VK_TRUE, UINT64_MAX);
	}

	// move constructor
	inline VulkanFence::VulkanFence(VulkanFence&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanFence& VulkanFence::operator=(VulkanFence&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* WRAPPER_VULKANFENCE_HPP_ */
