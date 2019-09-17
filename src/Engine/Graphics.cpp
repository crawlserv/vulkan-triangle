/*
 * Graphics.cpp
 *
 * Graphics engine.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#include "Graphics.hpp"

namespace spacelite::Engine {

/* STATIC CONSTANTS */
const Struct::EngineInfo Graphics::engineInfo(
		ENGINE_NAME,
		ENGINE_VERSION_MAJOR,
		ENGINE_VERSION_MINOR,
		ENGINE_VERSION_PATCH,
		ENGINE_VULKAN_VERSION
);
const Struct::VulkanRequirements Graphics::vulkanRequirements(
		{ "VK_LAYER_KHRONOS_validation" },
		{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }
);
const unsigned int Graphics::maxFramesInFlight = 2;
const std::vector<Struct::Vertex> Graphics::vertices = { // temporary (for testing)
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
};
const Struct::ShaderFiles Graphics::shaderFiles = {
		"shd/bin/default.vert.spv",
		"shd/bin/default.frag.spv"
};
/* END STATIC CONSTANTS */


// constructor: create objects and show header with application and engine
Graphics::Graphics(const Struct::AppInfo& appInfo, Main::Window& window)
		:	counter(0),
			speed(0.),
			targetWindow(window),
			vulkanInstance(
					appInfo,
					Graphics::engineInfo
#ifndef NDEBUG
					,
					Graphics::vulkanRequirements,
					vulkanDebugInfo
#endif
			),
#ifndef NDEBUG
			vulkanDebug(vulkanInstance, vulkanDebugInfo),
#endif
			vulkanSurface(vulkanInstance, targetWindow),
			vulkanPhysicalDevice(vulkanInstance, vulkanSurface, Graphics::vulkanRequirements.deviceExtensions),
			vulkanDevice(vulkanPhysicalDevice, Graphics::vulkanRequirements),
			vulkanSwapChain(vulkanDevice, targetWindow, vulkanSurface, vulkanPhysicalDevice, Graphics::maxFramesInFlight),
			vulkanRenderPass(vulkanDevice, vulkanSwapChain.getImageFormat()),
			vulkanPipeline(vulkanDevice, vulkanRenderPass, vulkanSwapChain.getExtent(), Graphics::shaderFiles),
			vulkanFrameBuffers(vulkanDevice, vulkanSwapChain, vulkanRenderPass),
			vulkanCommandPool(vulkanDevice, vulkanPhysicalDevice.findQueueFamilies()),
			vulkanVertexBuffer(
					vulkanDevice,
					vulkanPhysicalDevice,
					true,
					sizeof(Graphics::vertices[0]) * Graphics::vertices.size(),
					Graphics::vertices.data()
			),
			vulkanCommandBuffers(
					vulkanDevice,
					vulkanCommandPool,
					vulkanRenderPass,
					vulkanFrameBuffers,
					vulkanSwapChain.getExtent(),
					vulkanPipeline,
					vulkanVertexBuffer,
					Graphics::vertices.size()
			),
			vulkanDeviceWaitIdle(vulkanDevice),
			vulkanCurrentFrame(0) {
	// print application info to stdout
	std::cout	<< appInfo.name
				<< " v"
				<< appInfo.verMajor
				<< '.'
				<< appInfo.verMinor
				<< '.'
				<< appInfo.verPatch
				<< " on "
				<< Graphics::engineInfo.name.c_str()
				<< " v"
				<< Graphics::engineInfo.verMajor
				<< '.'
				<< Graphics::engineInfo.verMinor
				<< '.'
				<< Graphics::engineInfo.verPatch
				<< "\n using "
				<< Main::Window::getInfo()
				<< "\n using Vulkan API v"
				<< VK_VERSION_MAJOR(Graphics::engineInfo.vulkanVersion)
				<< "."
				<< VK_VERSION_MINOR(Graphics::engineInfo.vulkanVersion)
				<< "\n";

	// create Vulkan API-specific synchronization objects (to be wrapped)
	for(unsigned short n = 0; n < this->vulkanSwapChain.getInFlightMax(); ++n) {
		this->vulkanImageAvailableSemaphores.emplace_back(this->vulkanDevice);
		this->vulkanRenderFinishedSemaphores.emplace_back(this->vulkanDevice);
		this->vulkanInFlightFences.emplace_back(this->vulkanDevice);
	}

	std::cout	<< "(started in " << (double) this->timer.since() / 1000 << "ms)"
				<< std::endl;

	this->timer.reset();
}

// destructor: show memory debugging stats (if necessary)
Graphics::~Graphics() {
	std::cout	<< "(average speed was "
				<< this->speed
				<< "tps)"
				<< std::endl;

#ifndef NDEBUG
	std::cout	<< "(allocated "
				<< Helper::VulkanAllocator::getAllocated()
				<< ", reallocated "
				<< Helper::VulkanAllocator::getReAllocated()
				<< " bytes)"
				<< std::endl;
#endif
}

// tick: draw a frame
void Graphics::tick() {
	const double elapsedTime = (double) this->timer.since() / 1000;

	timer.reset();

	this->speed = (this->counter * this->speed + 1000 / elapsedTime) / (this->counter + 1);

	++(this->counter);

	this->vulkanDrawFrame();
}

// draw a frame using the Vulkan API
void Graphics::vulkanDrawFrame() {
	(this->vulkanInFlightFences)[this->vulkanCurrentFrame].waitFor();

	// acquire image from swap chain
	unsigned int vulkanImageIndex = 0;

	VkResult vulkanResult = vkAcquireNextImageKHR(
			this->vulkanDevice.get(),
			this->vulkanSwapChain.get(),
			std::numeric_limits<unsigned int>::max(),
			this->vulkanImageAvailableSemaphores[this->vulkanCurrentFrame].get(),
			VK_NULL_HANDLE,
			&vulkanImageIndex
	);

	if(vulkanResult == VK_ERROR_OUT_OF_DATE_KHR) {
		this->vulkanRecreateSwapChain();

		return;
	}
	else if(vulkanResult != VK_SUCCESS && vulkanResult != VK_SUBOPTIMAL_KHR)
		throw Exception("Could not acquire swap chain image: " + Wrapper::VulkanError(vulkanResult).str());

	// submit command buffer
	VkSubmitInfo vulkanSubmitInfo = {};

	vulkanSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore vulkanWaitSemaphores[] = { this->vulkanImageAvailableSemaphores[this->vulkanCurrentFrame].get() };
	VkPipelineStageFlags vulkanWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	vulkanSubmitInfo.waitSemaphoreCount = 1;
	vulkanSubmitInfo.pWaitSemaphores = vulkanWaitSemaphores;
	vulkanSubmitInfo.pWaitDstStageMask = vulkanWaitStages;
	vulkanSubmitInfo.commandBufferCount = 1;
	vulkanSubmitInfo.pCommandBuffers = &(this->vulkanCommandBuffers.get(vulkanImageIndex).get());

	VkSemaphore vulkanSignalSemaphores[] = { this->vulkanRenderFinishedSemaphores[this->vulkanCurrentFrame].get() };

	vulkanSubmitInfo.signalSemaphoreCount = 1;
	vulkanSubmitInfo.pSignalSemaphores = vulkanSignalSemaphores;

	vkResetFences(this->vulkanDevice.get(), 1, &((this->vulkanInFlightFences)[this->vulkanCurrentFrame].get()));

	vulkanResult = vkQueueSubmit(
			this->vulkanDevice.getGraphicsQueue(),
			1,
			&vulkanSubmitInfo,
			this->vulkanInFlightFences[this->vulkanCurrentFrame].get()
	);

	if(vulkanResult != VK_SUCCESS)
		throw Exception("Could not submit command buffer: " + Wrapper::VulkanError(vulkanResult).str());

	// submit result back to swap chain
	VkPresentInfoKHR vulkanPresentInfo = {};

	vulkanPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	vulkanPresentInfo.waitSemaphoreCount = 1;
	vulkanPresentInfo.pWaitSemaphores = vulkanSignalSemaphores;

	VkSwapchainKHR vulkanSwapChains[] = { this->vulkanSwapChain.get() };

	vulkanPresentInfo.swapchainCount = 1;
	vulkanPresentInfo.pSwapchains = vulkanSwapChains;
	vulkanPresentInfo.pImageIndices = &vulkanImageIndex;
	vulkanPresentInfo.pResults = nullptr;

	vulkanResult = vkQueuePresentKHR(this->vulkanDevice.getPresentQueue(), &vulkanPresentInfo);

	if(
			vulkanResult == VK_ERROR_OUT_OF_DATE_KHR
			|| vulkanResult == VK_SUBOPTIMAL_KHR
			|| this->targetWindow.isResized()
	) {
		this->targetWindow.resetResize();

		this->vulkanRecreateSwapChain();

		return;
	}
	else if(vulkanResult != VK_SUCCESS)
    	throw Exception("Could not present swap chain image: " + Wrapper::VulkanError(vulkanResult).str());

	this->vulkanCurrentFrame = (this->vulkanCurrentFrame + 1) % this->vulkanSwapChain.getInFlightMax();
}

// re-create the swap chain, e.g. after changes to the window
void Graphics::vulkanRecreateSwapChain() {
	// wait for the device to become idle
	this->vulkanDeviceWaitIdle.wait();

	// destroy old swap chain
	this->vulkanCommandBuffers.destroy();
	this->vulkanFrameBuffers.destroy();
	this->vulkanPipeline.destroy();
	this->vulkanRenderPass.destroy();
	this->vulkanSwapChain.destroy();

	// reset command pool memory
	this->vulkanCommandPool.reset(); // saving some memory (TODO: reset command pool more often?!)

	// create new swap chain
	this->vulkanSwapChain.create();
	this->vulkanRenderPass.create();
	this->vulkanPipeline.create();
	this->vulkanFrameBuffers.create();
	this->vulkanCommandBuffers.create();
}

} /* namespace spacelite::Engine */
