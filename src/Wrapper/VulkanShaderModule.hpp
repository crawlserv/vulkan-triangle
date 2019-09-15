/*
 * VulkanShaderModule.hpp
 *
 * Wraps a shader module used by the Vulkan API.
 *
 *  Created on: Sep 14, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANSHADERMODULE_HPP_
#define SRC_WRAPPER_VULKANSHADERMODULE_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanShaderModule {
	public:
		VulkanShaderModule(VulkanDevice& device, const std::vector<char>& code);
		virtual ~VulkanShaderModule();

		// getters
		VkShaderModule& get();
		const VkShaderModule& get() const;

		// not copyable, only moveable
		VulkanShaderModule(const VulkanShaderModule&) = delete;
		VulkanShaderModule(VulkanShaderModule&& other) noexcept;
		VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
		VulkanShaderModule& operator=(VulkanShaderModule&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkShaderModule instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the shader module
	inline VulkanShaderModule::VulkanShaderModule(
			VulkanDevice& device,
			const std::vector<char>& code
	) : parent(device), instance(VK_NULL_HANDLE) {
		VkShaderModuleCreateInfo vulkanCreateInfo = {};

		vulkanCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		vulkanCreateInfo.codeSize = code.size();
		vulkanCreateInfo.pCode = reinterpret_cast<const unsigned int *>(code.data());

		VkResult vulkanResult = vkCreateShaderModule(
				this->parent.get(),
				&vulkanCreateInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create shader module: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy the shader module
	inline VulkanShaderModule::~VulkanShaderModule() {
		if(this->instance)
			vkDestroyShaderModule(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the shader module
	inline VkShaderModule& VulkanShaderModule::get() {
		return this->instance;
	}

	// get const reference to the instance of the shader module
	inline const VkShaderModule& VulkanShaderModule::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanShaderModule::VulkanShaderModule(VulkanShaderModule&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanShaderModule& VulkanShaderModule::operator=(VulkanShaderModule&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}
} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANSHADERMODULE_HPP_ */
