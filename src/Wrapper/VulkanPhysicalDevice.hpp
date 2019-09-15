/*
 * VulkanPhysicalDevice.hpp
 *
 * Wraps a physical device used by the Vulkan API - and its selection process.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANPHYSICALDEVICE_HPP_
#define SRC_WRAPPER_VULKANPHYSICALDEVICE_HPP_

#include "VulkanError.hpp"
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"

#include "../Main/Exception.hpp"
#include "../Struct/VulkanQueueFamilies.hpp"
#include "../Struct/VulkanSwapChainSupport.hpp"

#include <vulkan/vulkan_core.h>

#include <set>		// std::set
#include <utility>	// std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {
	/*
	 * DECLARATION
	 */
	class VulkanPhysicalDevice {
	public:
		VulkanPhysicalDevice(
				VulkanInstance& instance,
				VulkanSurface& surface,
				const std::vector<const char *>& deviceExtensions
		);
		virtual ~VulkanPhysicalDevice();

		// getters
		VkPhysicalDevice& get();
		const VkPhysicalDevice& get() const;
		Struct::VulkanSwapChainSupport getSwapChainSupport() const;

		// public helper functions
		const Struct::VulkanQueueFamilies& findQueueFamilies() const;
		unsigned int findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) const;

		// not copyable, only moveable
		VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
		VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept;
		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice&) = delete;
		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&& other) noexcept;

	private:
		VulkanInstance& parent;
		VkPhysicalDevice instance;
		VulkanSurface& surface;
		Struct::VulkanQueueFamilies queueFamily;

		// private helper functions
		unsigned int rateDevice(const VkPhysicalDevice& device, const std::vector<const char *>& deviceExtensions) const;
		Struct::VulkanSwapChainSupport getSwapChainSupport(const VkPhysicalDevice& device) const;
		Struct::VulkanQueueFamilies findQueueFamilies(const VkPhysicalDevice& device) const;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: select physical device
	inline VulkanPhysicalDevice::VulkanPhysicalDevice(
			VulkanInstance& instance,
			VulkanSurface& surface,
			const std::vector<const char *>& deviceExtensions
	) : parent(instance), instance(VK_NULL_HANDLE), surface(surface) {
		unsigned int vulkanDeviceCount = 0;

		vkEnumeratePhysicalDevices(this->parent.get(), &vulkanDeviceCount, nullptr);

		if(!vulkanDeviceCount)
			throw Exception("No supported GPUs found");

		std::vector<VkPhysicalDevice> vulkanDevices(vulkanDeviceCount);

		vkEnumeratePhysicalDevices(this->parent.get(), &vulkanDeviceCount, vulkanDevices.data());

		unsigned int highScore = 0;

		for(const VkPhysicalDevice& device : vulkanDevices) {
			// choose current device depending on score
			unsigned int score = this->rateDevice(device, deviceExtensions);

			if(score > highScore) {
				this->instance = device;

				highScore = score;
			}
		}

		if(!highScore)
			throw Exception("Could not find a suitable GPU");

		this->queueFamily = this->findQueueFamilies(this->instance);
	}

	// destructor: de-select physical device
	inline VulkanPhysicalDevice::~VulkanPhysicalDevice() {}

	// get reference to the instance of the physical device
	inline VkPhysicalDevice& VulkanPhysicalDevice::get() {
		return this->instance;
	}

	// get const reference to the instance of the physical device
	inline const VkPhysicalDevice& VulkanPhysicalDevice::get() const {
		return this->instance;
	}

	// get supported swap chain capabilities, formats and presentation modes
	inline Struct::VulkanSwapChainSupport VulkanPhysicalDevice::getSwapChainSupport() const {
		return this->getSwapChainSupport(this->instance);
	}

	// find memory type
	inline unsigned int VulkanPhysicalDevice::findMemoryType(unsigned int typeFilter, VkMemoryPropertyFlags properties) const {
		VkPhysicalDeviceMemoryProperties vulkanMemProperties;

		vkGetPhysicalDeviceMemoryProperties(this->instance, &vulkanMemProperties);

		for(unsigned int i = 0; i < vulkanMemProperties.memoryTypeCount; ++i)
			if(
					(typeFilter & (1 << i))
					&& (vulkanMemProperties.memoryTypes[i].propertyFlags & properties) == properties
			)
				return i;

		throw Exception("Graphics::vulkanFindMemoryType(): Failed to find suitable memory");
	}

	// find queue families
	inline const Struct::VulkanQueueFamilies& VulkanPhysicalDevice::findQueueFamilies() const {
		return this->queueFamily;
	}

	// rate the specified physical device
	inline unsigned int VulkanPhysicalDevice::rateDevice(
			const VkPhysicalDevice& device,
			const std::vector<const char *>& deviceExtensions
	) const {
		// get device properties and features
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;

		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		// rate device
		unsigned int score = 0;

		// discrete GPUs have a significant performance advantage
		if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		// maximum possible size of textures affects graphics quality
		score += deviceProperties.limits.maxImageDimension2D;

	/*
		// application can't function without geometry shaders
		if(!deviceFeatures.geometryShader)
			return 0;
	*/

		// check for suitable queue families
		Struct::VulkanQueueFamilies vulkanQueueFamilyIndices = this->findQueueFamilies(device);

		if(!vulkanQueueFamilyIndices.isComplete())
			return 0;

		// check for extensions
		unsigned int vulkanExtensionCount = 0;

		vkEnumerateDeviceExtensionProperties(device, nullptr, &vulkanExtensionCount, nullptr);

		std::vector<VkExtensionProperties> vulkanAvailableExtensions(vulkanExtensionCount);

		vkEnumerateDeviceExtensionProperties(
				device,
				nullptr,
				&vulkanExtensionCount,
				vulkanAvailableExtensions.data()
		);

		std::set<std::string> vulkanRequiredExtensions(
				deviceExtensions.begin(),
				deviceExtensions.end()
		);

		for(const auto& extension : vulkanAvailableExtensions)
			vulkanRequiredExtensions.erase(extension.extensionName);

		if(!vulkanRequiredExtensions.empty())
			return 0;

		// check whether swap chain is adequate
		Struct::VulkanSwapChainSupport vulkanSwapChainSupport = this->getSwapChainSupport(device);

		if(vulkanSwapChainSupport.formats.empty() || vulkanSwapChainSupport.presentModes.empty())
			return 0;

		// device could be used: return score
		return score;
	}

	// find queue families for the specified physical device
	inline Struct::VulkanQueueFamilies VulkanPhysicalDevice::findQueueFamilies(const VkPhysicalDevice& device) const {
		Struct::VulkanQueueFamilies vulkanQueueFamilyIndices;

		unsigned int vulkanQueueFamilyCount = 0;

		vkGetPhysicalDeviceQueueFamilyProperties(device, &vulkanQueueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> vulkanQueueFamilies(vulkanQueueFamilyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(device, &vulkanQueueFamilyCount, vulkanQueueFamilies.data());

		int i = 0;

		for(const auto& queueFamily : vulkanQueueFamilies) {
			VkBool32 presentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, this->surface.get(), &presentSupport);

			if(queueFamily.queueCount > 0) {
				if(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					vulkanQueueFamilyIndices.graphicsFamily = i;

				if(presentSupport)
					vulkanQueueFamilyIndices.presentFamily = i;
			}

			if(vulkanQueueFamilyIndices.isComplete())
				break;

		    ++i;
		}

		return vulkanQueueFamilyIndices;
	}

	// get swap chain support details
	inline Struct::VulkanSwapChainSupport VulkanPhysicalDevice::getSwapChainSupport(const VkPhysicalDevice& device) const {
		Struct::VulkanSwapChainSupport vulkanSwapChainSupport;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, this->surface.get(), &vulkanSwapChainSupport.capabilities);

		unsigned int vulkanFormatCount = 0;

		vkGetPhysicalDeviceSurfaceFormatsKHR(device, this->surface.get(), &vulkanFormatCount, nullptr);

		if(vulkanFormatCount) {
			vulkanSwapChainSupport.formats.resize(vulkanFormatCount);

			vkGetPhysicalDeviceSurfaceFormatsKHR(
					device,
					this->surface.get(),
					&vulkanFormatCount,
					vulkanSwapChainSupport.formats.data()
			);
		}

		unsigned int vulkanPresentModeCount = 0;

		vkGetPhysicalDeviceSurfacePresentModesKHR(device, this->surface.get(), &vulkanPresentModeCount, nullptr);

		if(vulkanPresentModeCount) {
			vulkanSwapChainSupport.presentModes.resize(vulkanPresentModeCount);

			vkGetPhysicalDeviceSurfacePresentModesKHR(
					device,
					this->surface.get(),
					&vulkanPresentModeCount,
					vulkanSwapChainSupport.presentModes.data()
			);
		}

		return vulkanSwapChainSupport;
	}

	// move constructor
	inline VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				surface(other.surface),
				queueFamily(other.queueFamily) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanPhysicalDevice& VulkanPhysicalDevice::operator=(VulkanPhysicalDevice&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->surface, other.surface);
		swap(this->queueFamily, other.queueFamily);

		return *this;
	}
}

#endif /* SRC_WRAPPER_VULKANPHYSICALDEVICE_HPP_ */
