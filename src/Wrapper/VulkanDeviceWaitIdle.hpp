/*
 * VulkanDeviceWaitIdle.hpp
 *
 * Simply waits for a device used by the Vulkan API on its destruction.
 *
 * Can be added to a class after all other Vulkan API-related objects to safeguard their destruction.
 *
 *  Created on: Sep 15, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANDEVICEWAITIDLE_HPP_
#define SRC_WRAPPER_VULKANDEVICEWAITIDLE_HPP_

#include <vulkan/vulkan_core.h>

#include "VulkanDevice.hpp"

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanDeviceWaitIdle {
	public:
		// construction and destruction
		VulkanDeviceWaitIdle(const VulkanDevice& device);
		~VulkanDeviceWaitIdle();

		// wait for the device
		void wait() const;

		// not copyable, not moveable
		VulkanDeviceWaitIdle(const VulkanDeviceWaitIdle&) = delete;
		void operator=(const VulkanDeviceWaitIdle&) = delete;
		VulkanDeviceWaitIdle(VulkanDeviceWaitIdle&&) = delete;
		VulkanDeviceWaitIdle& operator=(VulkanDeviceWaitIdle&&) = delete;

	private:
		// reference to the device
		const VulkanDevice& ref;
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: save const reference to the device
	inline VulkanDeviceWaitIdle::VulkanDeviceWaitIdle(const VulkanDevice& device) : ref(device) {}

	// destructor: wait for the device to become idle
	inline VulkanDeviceWaitIdle::~VulkanDeviceWaitIdle() {
		this->wait();
	}

	// wait for the device to become idle
	inline void VulkanDeviceWaitIdle::wait() const {
		if(this->ref)
			vkDeviceWaitIdle(this->ref.get());
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANDEVICEWAITIDLE_HPP_ */
