/*
 * VulkanInstance.hpp
 *
 * Wraps an instance of the Vulkan API.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANINSTANCE_HPP_
#define SRC_WRAPPER_VULKANINSTANCE_HPP_

#ifndef NDEBUG
#include "VulkanDebugInfo.hpp"
#endif

#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Struct/AppInfo.hpp"
#include "../Struct/EngineInfo.hpp"
#include "../Struct/VulkanRequirements.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#error "Please compile with GLFW_INCLUDE_VULKAN to include the Vulkan API when using GLFW."
#endif

#include <GLFW/glfw3.h>	// includes the Vulkan API, too (-DGLFW_INCLUDE_VULKAN required!)

#include <cstring>	// std::strcmp

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanInstance {
	public:
		VulkanInstance(
				const Struct::AppInfo& appInfo,
				const Struct::EngineInfo& engineInfo
#ifndef NDEBUG
				,
				const Struct::VulkanRequirements& requirements,
				const Wrapper::VulkanDebugInfo& debugInfo
#endif
		);
		virtual ~VulkanInstance();

		VkInstance& get();
		const VkInstance& get() const;

		// not copyable, only moveable
		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance(VulkanInstance&& other) noexcept;
		VulkanInstance& operator=(const VulkanInstance&) = delete;
		VulkanInstance& operator=(VulkanInstance&& other) noexcept;

	private:
		VkInstance instance;

		VkApplicationInfo getApplicationInfo(const Struct::AppInfo& appInfo, const Struct::EngineInfo& engineInfo);
		void checkValidationLayers(const Struct::VulkanRequirements& vulkanRequirements);
		std::vector<const char*> getRequiredExtensions();

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	inline VulkanInstance::VulkanInstance(
			const Struct::AppInfo& appInfo,
			const Struct::EngineInfo& engineInfo
#ifndef NDEBUG
			,
			const Struct::VulkanRequirements& requirements,
			const Wrapper::VulkanDebugInfo& debugInfo
#endif
	) : instance(VK_NULL_HANDLE) {
		// get application info
		VkApplicationInfo vulkanApplicationInfo = this->getApplicationInfo(appInfo, engineInfo);

		// check support for validation layers (if debugging is enabled)
		#ifndef NDEBUG
			this->checkValidationLayers(requirements);
		#endif

		// get required instance extensions
		auto extensions = this->getRequiredExtensions();

		// set creation info
		VkInstanceCreateInfo vulkanCreateInfo = {};

		vulkanCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		vulkanCreateInfo.pApplicationInfo = &vulkanApplicationInfo;
		vulkanCreateInfo.enabledExtensionCount = static_cast<unsigned int>(extensions.size());
		vulkanCreateInfo.ppEnabledExtensionNames = extensions.data();

		#ifdef NDEBUG
			vulkanCreateInfo.enabledLayerCount = 0;
			vulkanCreateInfo.pNext = nullptr;
		#else
			vulkanCreateInfo.enabledLayerCount = static_cast<unsigned int>(requirements.validationLayers.size());
			vulkanCreateInfo.ppEnabledLayerNames = requirements.validationLayers.data();

			vulkanCreateInfo.pNext = debugInfo.getPtr();
		#endif

		VkResult vulkanResult = vkCreateInstance(
				&vulkanCreateInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not initialize Vulkan API: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy instance
	inline VulkanInstance::~VulkanInstance() {
		if(this->instance)
			vkDestroyInstance(this->instance, Helper::VulkanAllocator::ptr);
	}

	// get instance
	inline VkInstance& VulkanInstance::get() {
		return this->instance;
	}

	// get const instance
	inline const VkInstance& VulkanInstance::get() const {
		return this->instance;
	}

	// set application info
	inline VkApplicationInfo VulkanInstance::getApplicationInfo(const Struct::AppInfo& appInfo, const Struct::EngineInfo& engineInfo) {
		VkApplicationInfo vulkanAppInfo = {};

		vulkanAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		vulkanAppInfo.pApplicationName = appInfo.name.c_str();
		vulkanAppInfo.applicationVersion = VK_MAKE_VERSION(appInfo.verMajor, appInfo.verMinor, appInfo.verPatch);
		vulkanAppInfo.pEngineName = engineInfo.name.c_str();
		vulkanAppInfo.engineVersion = VK_MAKE_VERSION(engineInfo.verMajor, engineInfo.verMinor, engineInfo.verPatch);
		vulkanAppInfo.apiVersion = engineInfo.vulkanVersion;

		return vulkanAppInfo;
	}

	// check support for validation layers
	inline void VulkanInstance::checkValidationLayers(const Struct::VulkanRequirements& vulkanRequirements) {
		unsigned int vulkanValidationLayerCount = 0;

		vkEnumerateInstanceLayerProperties(&vulkanValidationLayerCount, nullptr);

		std::vector<VkLayerProperties> vulkanAvailableLayers(vulkanValidationLayerCount);

		vkEnumerateInstanceLayerProperties(&vulkanValidationLayerCount, vulkanAvailableLayers.data());

		for(const auto& layerName : vulkanRequirements.validationLayers) {
			bool layerFound = false;

			for(const auto& layerProperties : vulkanAvailableLayers) {
				if(std::strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;

					break;
				}
			}

			if(!layerFound)
				throw Exception(
						std::string("VulkanInstance::checkValidationLayers(): ")
						+ layerName
						+ " missing"
				);
		}
	}

	// get required instance extensions (needed by GLFW)
	inline std::vector<const char*> VulkanInstance::getRequiredExtensions() {
		unsigned int glfwExtensionCount = 0;
		const char ** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		#ifndef NDEBUG
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		#endif

		return extensions;
	}

	// move constructor
	inline VulkanInstance::VulkanInstance(VulkanInstance&& other) noexcept : instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanInstance& VulkanInstance::operator=(VulkanInstance&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANINSTANCE_HPP_ */
