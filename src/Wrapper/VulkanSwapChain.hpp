/*
 * VulkanSwapChain.hpp
 *
 * Wraps the swap chain used by the Vulkan API - including its images and image views.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANSWAPCHAIN_HPP_
#define SRC_WRAPPER_VULKANSWAPCHAIN_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSurface.hpp"

#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Main/Window.hpp"

#include <vulkan/vulkan_core.h>

#include <algorithm>	// std::clamp
#include <utility>		// std::move, std::swap
#include <vector>		// std::vector

namespace spacelite::Wrapper {

	/*
	 * DECLARATION
	 */
	class VulkanSwapChain {
	public:
		VulkanSwapChain(
				VulkanDevice& device,
				Main::Window& targetWindow,
				VulkanSurface& targetSurface,
				VulkanPhysicalDevice& physicalDevice,
				unsigned int maxFramesInFlight
		);
		virtual ~VulkanSwapChain();

		// creation and destruction
		void create();
		void destroy() noexcept;

		// getters
		VkSwapchainKHR& get();
		const VkSwapchainKHR& get() const;
		VkFormat& getImageFormat();
		const VkFormat& getImageFormat() const;
		VkExtent2D& getExtent();
		const VkExtent2D& getExtent() const;
		unsigned int getNumberOfImageViews() const;
		const VulkanImageView& getImageView(unsigned int index) const;
		unsigned int getInFlightMax() const;

		// not copyable, only moveable
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain(VulkanSwapChain&& other) noexcept;
		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(VulkanSwapChain&& other) noexcept;

	private:
		// references and instance
		VulkanDevice& parent;
		VkSwapchainKHR instance;
		Main::Window& window;
		VulkanSurface& surface;
		VulkanPhysicalDevice& physical;

		// images, image views, format and extent of the swap chain
		std::vector<VkImage> images;
		std::vector<VulkanImageView> imageViews;
		VkFormat imageFormat;
		VkExtent2D extent;

		// settings
		unsigned int inFlightMaxTarget;	// requested maximum number of frames in flight
		unsigned int inFlightMax;		// actual maximum number of frames in flight

		// static private helper functions
		static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		static VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		static VkExtent2D chooseExtent(const VkExtent2D& windowSize, const VkSurfaceCapabilitiesKHR& capabilities);

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */

	// constructor: outsourced to create()
	inline VulkanSwapChain::VulkanSwapChain(
			VulkanDevice& device,
			Main::Window& targetWindow,
			VulkanSurface& targetSurface,
			VulkanPhysicalDevice& physicalDevice,
			unsigned int maxFramesInFlight
	) : 	parent(device),
			instance(VK_NULL_HANDLE),
			window(targetWindow),
			surface(targetSurface),
			physical(physicalDevice),
			imageFormat(VK_FORMAT_UNDEFINED),
			inFlightMaxTarget(maxFramesInFlight),
			inFlightMax(inFlightMaxTarget) {
		this->create();
	}

	// destructor: outsourced to destroy()
	inline VulkanSwapChain::~VulkanSwapChain() {
		this->destroy();
	}

	// get reference to the instance of the swap chain
	inline VkSwapchainKHR& VulkanSwapChain::get() {
		return this->instance;
	}

	// get const reference to the instance of the swap chain
	inline const VkSwapchainKHR& VulkanSwapChain::get() const {
		return this->instance;
	}

	// get reference to the image format of the swap chain
	inline VkFormat& VulkanSwapChain::getImageFormat() {
		return this->imageFormat;
	}

	// get const reference to the image format of the swap chain
	inline const VkFormat& VulkanSwapChain::getImageFormat() const {
		return this->imageFormat;
	}

	// get reference to the extent of the swap chain
	inline VkExtent2D& VulkanSwapChain::getExtent() {
		return this->extent;
	}

	// get const reference to the extent of the swap chain
	inline const VkExtent2D& VulkanSwapChain::getExtent() const {
		return this->extent;
	}

	// get number of image views in the swap chain
	inline unsigned int VulkanSwapChain::getNumberOfImageViews() const {
		return this->imageViews.size();
	}

	// get maximum number of frames to be rendered in flight inside the swap chain
	inline unsigned int VulkanSwapChain::getInFlightMax() const {
		return this->inFlightMax;
	}

	// get index-specified image view from the swap chain
	inline const VulkanImageView& VulkanSwapChain::getImageView(unsigned int index) const {
		return this->imageViews.at(index);
	}

	// create the swap chain
	inline void VulkanSwapChain::create() {
		Struct::VulkanSwapChainSupport vulkanSwapChainSupport = this->physical.getSwapChainSupport();

		// update window (frame buffer) size
		this->window.updateSize();

		// choose format, mode and extent
		VkSurfaceFormatKHR vulkanSurfaceFormat = VulkanSwapChain::chooseSurfaceFormat(vulkanSwapChainSupport.formats);
		VkPresentModeKHR vulkanPresentMode = VulkanSwapChain::choosePresentMode(vulkanSwapChainSupport.presentModes);
		VkExtent2D vulkanExtent = VulkanSwapChain::chooseExtent(
				{ this->window.getWidth(), this->window.getHeight() },
				vulkanSwapChainSupport.capabilities
		);

		// set (valid) number of images in the swap chain
		unsigned int vulkanImageCount = vulkanSwapChainSupport.capabilities.minImageCount + 1;

		if(vulkanImageCount < this->inFlightMaxTarget + 1)
			vulkanImageCount = this->inFlightMaxTarget + 1;

		if(
				vulkanSwapChainSupport.capabilities.maxImageCount
				&& vulkanImageCount > vulkanSwapChainSupport.capabilities.maxImageCount
		)
			vulkanImageCount = vulkanSwapChainSupport.capabilities.maxImageCount;

		if(this->inFlightMaxTarget + 1 < vulkanImageCount)
			this->inFlightMax = vulkanImageCount - 1;

		VkSwapchainCreateInfoKHR vulkanCreateInfo = {};

		vulkanCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		vulkanCreateInfo.surface = this->surface.get();
		vulkanCreateInfo.minImageCount = vulkanImageCount;
		vulkanCreateInfo.imageFormat = vulkanSurfaceFormat.format;
		vulkanCreateInfo.imageColorSpace = vulkanSurfaceFormat.colorSpace;
		vulkanCreateInfo.imageExtent = vulkanExtent;
		vulkanCreateInfo.imageArrayLayers = 1;
		vulkanCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const Struct::VulkanQueueFamilies& vulkanQueueFamily = this->physical.findQueueFamilies();

		unsigned int vulkanQueueFamilyIndices[] = {
				vulkanQueueFamily.graphicsFamily.value(),
				vulkanQueueFamily.presentFamily.value()
		};

		if(vulkanQueueFamily.graphicsFamily != vulkanQueueFamily.presentFamily) {
			vulkanCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			vulkanCreateInfo.queueFamilyIndexCount = 2;
			vulkanCreateInfo.pQueueFamilyIndices = vulkanQueueFamilyIndices;
		}
		else {
			vulkanCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			vulkanCreateInfo.queueFamilyIndexCount = 0;
			vulkanCreateInfo.pQueueFamilyIndices = nullptr;
		}

		vulkanCreateInfo.preTransform = vulkanSwapChainSupport.capabilities.currentTransform;
		vulkanCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		vulkanCreateInfo.presentMode = vulkanPresentMode;
		vulkanCreateInfo.clipped = VK_TRUE;
		vulkanCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult vulkanResult = vkCreateSwapchainKHR(
				this->parent.get(),
				&vulkanCreateInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create swap chain: " + Wrapper::VulkanError(vulkanResult).str());

		// save properties of swap chain
		vkGetSwapchainImagesKHR(this->parent.get(), this->instance, &vulkanImageCount, nullptr);

		this->images.resize(vulkanImageCount);

		vkGetSwapchainImagesKHR(this->parent.get(), this->instance, &vulkanImageCount, this->images.data());

		this->imageFormat = vulkanSurfaceFormat.format;
		this->extent = vulkanExtent;

		// create image views
		for(const auto& image : this->images)
			this->imageViews.emplace_back(this->parent, image, this->imageFormat);
	}

	// destroy the swap chain (may not throw an exception as it is used by the destructor, too)
	inline void VulkanSwapChain::destroy() noexcept {
		this->imageViews.clear();

		if(this->instance) {
			vkDestroySwapchainKHR(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);

			this->instance = VK_NULL_HANDLE;
		}
	}

	// static helper function to choose the swap surface format
	inline VkSurfaceFormatKHR VulkanSwapChain::chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		// search for the optimal format
		for(const auto& availableFormat : availableFormats)
			if(
					availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM
					&& availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
			)
				return availableFormat;

		// if the optimal format is not supported, choose the first format instead
		return availableFormats[0];
	}

	// static helper function to choose the swap presentation mode
	inline VkPresentModeKHR VulkanSwapChain::choosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		// select triple buffering if available
		for(const auto& availablePresentMode : availablePresentModes)
			if(availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;

		// if triple buffering is not supported, select double buffering instead
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	// static helper function to choose the swap extent
	inline VkExtent2D VulkanSwapChain::chooseExtent(const VkExtent2D& windowSize, const VkSurfaceCapabilitiesKHR& capabilities) {
		if(capabilities.currentExtent.width != std::numeric_limits<unsigned int>::max())
			return capabilities.currentExtent;
		else {
			VkExtent2D actualExtent(windowSize);

			actualExtent.width = std::clamp(
					windowSize.width,
					capabilities.minImageExtent.width,
					capabilities.maxImageExtent.width
			);
			actualExtent.height = std::clamp(
					windowSize.height,
					capabilities.minImageExtent.height,
					capabilities.maxImageExtent.height
			);

			return actualExtent;
		 }
	}

	// move constructor
	inline VulkanSwapChain::VulkanSwapChain(VulkanSwapChain&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				window(other.window),
				surface(other.surface),
				physical(other.physical),
				images(other.images),
				imageViews(std::move(other.imageViews)),
				imageFormat(other.imageFormat),
				extent(other.extent),
				inFlightMaxTarget(other.inFlightMaxTarget),
				inFlightMax(other.inFlightMax) {
		other.instance = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanSwapChain& VulkanSwapChain::operator=(VulkanSwapChain&& other) noexcept {
		this->instance = other.instance;
		this->inFlightMaxTarget = other.inFlightMaxTarget;
		this->inFlightMax = other.inFlightMax;

		other.instance = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->window, other.window);
		swap(this->surface, other.surface);
		swap(this->physical, other.physical);
		swap(this->images, other.images);
		swap(this->imageViews, other.imageViews);
		swap(this->imageFormat, other.imageFormat);
		swap(this->extent, other.extent);

		return *this;
	}

} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANSWAPCHAIN_HPP_ */
