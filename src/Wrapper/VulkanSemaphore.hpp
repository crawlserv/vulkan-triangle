/*
 * VulkanSemaphore.hpp
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef WRAPPER_VULKANSEMAPHORE_HPP_
#define WRAPPER_VULKANSEMAPHORE_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanSemaphore {
	public:
		VulkanSemaphore(VulkanDevice& device);
		virtual ~VulkanSemaphore();

		// getters
		VkSemaphore& get();
		const VkSemaphore& get() const;

		// not copyable, only moveable
		VulkanSemaphore(const VulkanSemaphore&) = delete;
		VulkanSemaphore(VulkanSemaphore&& other) noexcept;
		VulkanSemaphore& operator=(const VulkanSemaphore&) = delete;
		VulkanSemaphore& operator=(VulkanSemaphore&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkSemaphore instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the semaphore
	inline VulkanSemaphore::VulkanSemaphore(VulkanDevice& device) : parent(device), instance(VK_NULL_HANDLE) {
		VkSemaphoreCreateInfo vulkanSemaphoreInfo = {};

		vulkanSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkResult vulkanResult = vkCreateSemaphore(
				this->parent.get(),
				&vulkanSemaphoreInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create semaphore: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy the semaphore
	inline VulkanSemaphore::~VulkanSemaphore() {
		if(this->instance)
			vkDestroySemaphore(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the semaphore
	inline VkSemaphore& VulkanSemaphore::get() {
		return this->instance;
	}

	// get const reference to the instance of the semaphore
	inline const VkSemaphore& VulkanSemaphore::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanSemaphore::VulkanSemaphore(VulkanSemaphore&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanSemaphore& VulkanSemaphore::operator=(VulkanSemaphore&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* WRAPPER_VULKANSEMAPHORE_HPP_ */
