/*
 * VulkanImageView.hpp
 *
 * Wraps an image view used by the Vulkan API.
 *
 *  Created on: Sep 14, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANIMAGEVIEW_HPP_
#define SRC_WRAPPER_VULKANIMAGEVIEW_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"

#include "../Main/Exception.hpp"

#include <utility>	// std::swap

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanImageView {
	public:
		VulkanImageView(VulkanDevice& device, const VkImage& imagem, const VkFormat& format);
		virtual ~VulkanImageView();

		// getters
		VkImageView& get();
		const VkImageView& get() const;

		// not copyable, only moveable
		VulkanImageView(const VulkanImageView&) = delete;
		VulkanImageView(VulkanImageView&& other) noexcept;
		VulkanImageView& operator=(const VulkanImageView&) = delete;
		VulkanImageView& operator=(VulkanImageView&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkImageView instance;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: create the image view
	inline VulkanImageView::VulkanImageView(
			VulkanDevice& device,
			const VkImage& image,
			const VkFormat& format
	) : parent(device), instance(VK_NULL_HANDLE) {
		VkImageViewCreateInfo vulkanCreateInfo = {};

		vulkanCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		vulkanCreateInfo.image = image;
		vulkanCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		vulkanCreateInfo.format = format;
		vulkanCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		vulkanCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		vulkanCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		vulkanCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		vulkanCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vulkanCreateInfo.subresourceRange.baseMipLevel = 0;
		vulkanCreateInfo.subresourceRange.levelCount = 1;
		vulkanCreateInfo.subresourceRange.baseArrayLayer = 0;
		vulkanCreateInfo.subresourceRange.layerCount = 1;

		VkResult vulkanResult = vkCreateImageView(
				this->parent.get(),
				&vulkanCreateInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create image view: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destructor: destroy the image view
	inline VulkanImageView::~VulkanImageView() {
		vkDestroyImageView(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);
	}

	// get reference to the instance of the image view
	inline VkImageView& VulkanImageView::get() {
		return this->instance;
	}

	// get const reference to the instance of the image view
	inline const VkImageView& VulkanImageView::get() const {
		return this->instance;
	}

	// move constructor
	inline VulkanImageView::VulkanImageView(VulkanImageView&& other) noexcept
			:	parent(other.parent),
				instance(other.instance) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanImageView& VulkanImageView::operator=(VulkanImageView&& other) noexcept {
		this->instance = other.instance;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);

		return *this;
	}
} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANIMAGEVIEW_HPP_ */
