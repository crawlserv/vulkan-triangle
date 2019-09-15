/*
 * VulkanDevice.hpp
 *
 * Wraps a logical device with a presentation and a graphics queue.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANDEVICE_HPP_
#define SRC_WRAPPER_VULKANDEVICE_HPP_

#include "VulkanError.hpp"
#include "VulkanPhysicalDevice.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Struct/VulkanRequirements.hpp"

#include <vulkan/vulkan_core.h>

#include <set>		// std::set
#include <utility>	// std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanDevice {
	public:
		VulkanDevice(
				VulkanPhysicalDevice& physicalDevice,
				const Struct::VulkanRequirements& requirements
		);
		virtual ~VulkanDevice();

		// getters
		VkDevice& get();
		const VkDevice& get() const;
		VkQueue getPresentQueue();
		VkQueue getGraphicsQueue();

		// operator
		operator bool() const;

		// not copyable, only moveable
		VulkanDevice(const VulkanDevice&) = delete;
		VulkanDevice(VulkanDevice&& other) noexcept;
		VulkanDevice& operator=(const VulkanDevice&) = delete;
		VulkanDevice& operator=(VulkanDevice&& other) noexcept;

	private:
		VulkanPhysicalDevice& parent;
		VkDevice instance;
		VkQueue presentQueue;
		VkQueue graphicsQueue;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the logical device
	inline VulkanDevice::VulkanDevice(
			VulkanPhysicalDevice& physicalDevice,
			const Struct::VulkanRequirements& requirements
	) : parent(physicalDevice), instance(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE), graphicsQueue(VK_NULL_HANDLE) {
		const Struct::VulkanQueueFamilies& vulkanQueueFamilyIndices = this->parent.findQueueFamilies();

		std::vector<VkDeviceQueueCreateInfo> vulkanQueueCreateInfos;
		std::set<unsigned int> vulkanUniqueQueueFamilies = {
				vulkanQueueFamilyIndices.graphicsFamily.value(),
				vulkanQueueFamilyIndices.presentFamily.value()
		};

		float queuePriority = 1.0f;

		for(unsigned int queueFamily : vulkanUniqueQueueFamilies) {
			VkDeviceQueueCreateInfo vulkanQueueCreateInfo = {};

			vulkanQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			vulkanQueueCreateInfo.queueFamilyIndex = queueFamily;
			vulkanQueueCreateInfo.queueCount = 1;
			vulkanQueueCreateInfo.pQueuePriorities = &queuePriority;

			vulkanQueueCreateInfos.push_back(vulkanQueueCreateInfo);
		}

		VkPhysicalDeviceFeatures vulkanDeviceFeatures = {};

		VkDeviceCreateInfo vulkanCreateInfo = {};

		vulkanCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		vulkanCreateInfo.queueCreateInfoCount = static_cast<unsigned int>(vulkanQueueCreateInfos.size());
		vulkanCreateInfo.pQueueCreateInfos = vulkanQueueCreateInfos.data();
		vulkanCreateInfo.pEnabledFeatures = &vulkanDeviceFeatures;
		vulkanCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requirements.deviceExtensions.size());
		vulkanCreateInfo.ppEnabledExtensionNames = requirements.deviceExtensions.data();

	#ifdef NDEBUG
		vulkanCreateInfo.enabledLayerCount = 0;
	#else
		vulkanCreateInfo.enabledLayerCount = static_cast<uint32_t>(requirements.validationLayers.size());
		vulkanCreateInfo.ppEnabledLayerNames = requirements.validationLayers.data();
	#endif

		VkResult vulkanResult = vkCreateDevice(
				this->parent.get(),
				&vulkanCreateInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create logical graphics device: " + Wrapper::VulkanError(vulkanResult).str());

		// get rendering queue from device
		vkGetDeviceQueue(this->instance, vulkanQueueFamilyIndices.presentFamily.value(), 0, &(this->presentQueue));
		vkGetDeviceQueue(this->instance, vulkanQueueFamilyIndices.graphicsFamily.value(), 0, &(this->graphicsQueue));
	}

	// destructor: destroy the logical device
	inline VulkanDevice::~VulkanDevice() {
		if(this->instance)
			vkDestroyDevice(this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the logical device
	inline VkDevice& VulkanDevice::get() {
		return this->instance;
	}

	// get const reference to the instance of the logical device
	inline const VkDevice& VulkanDevice::get() const {
		return this->instance;
	}

	// get the presentation queue
	inline VkQueue VulkanDevice::getPresentQueue() {
		return this->presentQueue;
	}

	// get the graphics queue
	inline VkQueue VulkanDevice::getGraphicsQueue() {
		return this->graphicsQueue;
	}

	// bool operator: return whether the instance to the logical device is valid
	inline VulkanDevice::operator bool() const {
		return this->instance != VK_NULL_HANDLE;
	}

	// move constructor
	inline VulkanDevice::VulkanDevice(VulkanDevice&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				presentQueue(other.presentQueue),
				graphicsQueue(other.graphicsQueue) {
		other.instance = VK_NULL_HANDLE;
		other.presentQueue = VK_NULL_HANDLE;
		other.graphicsQueue = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanDevice& VulkanDevice::operator=(VulkanDevice&& other) noexcept {
		this->instance = other.instance;
		this->presentQueue = other.presentQueue;
		this->graphicsQueue = other.graphicsQueue;

		other.instance = VK_NULL_HANDLE;
		other.presentQueue = VK_NULL_HANDLE;
		other.graphicsQueue = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}

} /* spacelite::VulkanDevice */

#endif /* SRC_WRAPPER_VULKANDEVICE_HPP_ */
