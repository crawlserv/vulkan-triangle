/*
 * VulkanCommandPool.hpp
 *
 * Wraps a command pool used by the Vulkan API.
 *
 *  Created on: Sep 14, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANCOMMANDPOOL_HPP_
#define SRC_WRAPPER_VULKANCOMMANDPOOL_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Struct/VulkanQueueFamilies.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanCommandPool {
	public:
		VulkanCommandPool(VulkanDevice& device, const Struct::VulkanQueueFamilies& queryFamilies);
		virtual ~VulkanCommandPool();

		// getters
		VkCommandPool& get();
		const VkCommandPool& get() const;

		// resetter
		void reset();

		// not copyable, only moveable
		VulkanCommandPool(const VulkanCommandPool&) = delete;
		VulkanCommandPool(VulkanCommandPool&& other) noexcept;
		VulkanCommandPool& operator=(const VulkanCommandPool&) = delete;
		VulkanCommandPool& operator=(VulkanCommandPool&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkCommandPool instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create command pool
	inline VulkanCommandPool::VulkanCommandPool(
			VulkanDevice& device,
			const Struct::VulkanQueueFamilies& queryFamilies
	) : parent(device), instance(VK_NULL_HANDLE) {
		const Struct::VulkanQueueFamilies& vulkanQueueFamily = queryFamilies;

		VkCommandPoolCreateInfo vulkanPoolInfo = {};

		vulkanPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		vulkanPoolInfo.queueFamilyIndex = vulkanQueueFamily.graphicsFamily.value();
		vulkanPoolInfo.flags = 0;

		VkResult vulkanResult = vkCreateCommandPool(
				this->parent.get(),
				&vulkanPoolInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create command pool: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy command pool
	inline VulkanCommandPool::~VulkanCommandPool() {
		if(this->instance)
			vkDestroyCommandPool(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the command pool
	inline VkCommandPool& VulkanCommandPool::get() {
		return this->instance;
	}

	// get const reference to the instance of the command pool
	inline const VkCommandPool& VulkanCommandPool::get() const {
		return this->instance;
	}

	// reset the command pool (to be used between frames)
	inline void VulkanCommandPool::reset() {
		vkResetCommandPool(this->parent.get(), this->instance, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	}

	// move constructor
	inline VulkanCommandPool::VulkanCommandPool(VulkanCommandPool&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanCommandPool& VulkanCommandPool::operator=(VulkanCommandPool&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::Wrapper */



#endif /* SRC_WRAPPER_VULKANCOMMANDPOOL_HPP_ */
