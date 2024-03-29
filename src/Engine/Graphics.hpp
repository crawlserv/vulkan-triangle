/*
 * Graphics.hpp
 *
 * Graphics engine.
 *
 *  Created on: Sep 12, 2019
 *      Author: ans
 */

#ifndef SRC_ENGINE_GRAPHICS_HPP_
#define SRC_ENGINE_GRAPHICS_HPP_

#include "Version.hpp"

#include "../Helper/File.hpp"
#include "../Helper/SingletonDestructor.hpp"
#include "../Helper/TimerHighRes.hpp"
#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Main/Window.hpp"
#include "../Struct/AppInfo.hpp"
#include "../Struct/EngineInfo.hpp"
#include "../Struct/ShaderFiles.hpp"
#include "../Struct/Vertex.hpp"
#include "../Struct/VulkanQueueFamilies.hpp"
#include "../Struct/VulkanRequirements.hpp"
#include "../Struct/VulkanSwapChainSupport.hpp"
#include "../Wrapper/VulkanCommandBuffers.hpp"
#include "../Wrapper/VulkanCommandPool.hpp"
#include "../Wrapper/VulkanDevice.hpp"
#include "../Wrapper/VulkanDeviceMemory.hpp"
#include "../Wrapper/VulkanDeviceWaitIdle.hpp"
#include "../Wrapper/VulkanError.hpp"
#include "../Wrapper/VulkanFence.hpp"
#include "../Wrapper/VulkanFrameBuffers.hpp"
#include "../Wrapper/VulkanInstance.hpp"
#include "../Wrapper/VulkanPhysicalDevice.hpp"
#include "../Wrapper/VulkanPipeline.hpp"
#include "../Wrapper/VulkanRenderPass.hpp"
#include "../Wrapper/VulkanSemaphore.hpp"
#include "../Wrapper/VulkanSurface.hpp"
#include "../Wrapper/VulkanSwapChain.hpp"
#include "../Wrapper/VulkanVertexBuffer.hpp"

#ifndef GLFW_INCLUDE_VULKAN
#error "Please compile with GLFW_INCLUDE_VULKAN to include the Vulkan API when using GLFW."
#endif

#ifndef NDEBUG
	#include "../Wrapper/VulkanDebug.hpp"
	#include "../Wrapper/VulkanDebugInfo.hpp"
#endif

#include <GLFW/glfw3.h>	// includes the Vulkan API, too (-DGLFW_INCLUDE_VULKAN required!)

#include <cassert>	// assert
#include <cstring>	// std::memcpy
#include <iostream>	// std::cout, std::endl, std::flush
#include <limits>	// std::numeric_limits
#include <set>		// std::set
#include <string>	// std::string, std::to_string
#include <vector>	// std::vector

namespace spacelite::Engine {
	class Graphics {

		/* STATIC CONSTANTS */
		static const Struct::EngineInfo engineInfo;
		static const Struct::VulkanRequirements vulkanRequirements;
		static const unsigned int maxFramesInFlight;
		static const std::vector<Struct::Vertex> vertices;
		static const Struct::ShaderFiles shaderFiles;
		/* END STATIC CONSTANTS */

		/* QUASI-SINGLETON W/ POINTER AND DESTRUCTOR */
	public:
		static Graphics& getInstance(
				const Struct::AppInfo& appInfo,
				Main::Window& window,
				Helper::SingletonDestructor<Graphics> * destructor = nullptr
		) {
			static class SingletonHolder {
			public:
				SingletonHolder(
						const Struct::AppInfo& appInfo,
						Main::Window& window,
						Helper::SingletonDestructor<Graphics> * destructor
				) : ptr(nullptr), destructionDelegated(false) {
					this->ptr = new Graphics(appInfo, window);

					assert(this->ptr);

					if(destructor) {
						destructor->setPointer(this->ptr);

						this->destructionDelegated = true;
					}
				}

				~SingletonHolder() {
					if(this->ptr && !(this->destructionDelegated))
						delete this->ptr;
				}

				Graphics& get() {
					assert(this->ptr);

					return *(this->ptr);
				}

			private:
				Graphics * ptr;

				bool destructionDelegated;
			} holder(appInfo, window, destructor);

			return holder.get();
		}

		virtual ~Graphics();

	private:
		Graphics(const Struct::AppInfo& appInfo, Main::Window& window);

	public:
		Graphics(const Graphics&) = delete;
		void operator=(const Graphics&) = delete;
		/* END SINGLETON */

		void tick();

		// not moveable
		Graphics(Graphics&&) = delete;
		Graphics& operator=(Graphics&&) = delete;

	private:
		// timer (needs to be constructed first), counter and speed
		Helper::TimerHighRes timer;
		unsigned long long counter;
		double speed; // speed in average ticks per second

		// reference to target window
		Main::Window& targetWindow;

		// Vulkan API-specific functions
		void vulkanDrawFrame();
		void vulkanRecreateSwapChain();

		// Vulkan API-related wrappers
#ifndef NDEBUG
		Wrapper::VulkanDebugInfo vulkanDebugInfo;
#endif
		Wrapper::VulkanInstance vulkanInstance;
#ifndef NDEBUG
		Wrapper::VulkanDebug vulkanDebug;
#endif
		Wrapper::VulkanSurface vulkanSurface;
		Wrapper::VulkanPhysicalDevice vulkanPhysicalDevice;
		Wrapper::VulkanDevice vulkanDevice;
		Wrapper::VulkanSwapChain vulkanSwapChain;
		Wrapper::VulkanRenderPass vulkanRenderPass;
		Wrapper::VulkanPipeline vulkanPipeline;
		Wrapper::VulkanFrameBuffers vulkanFrameBuffers;
		Wrapper::VulkanCommandPool vulkanCommandPool;
		Wrapper::VulkanVertexBuffer vulkanVertexBuffer;
		Wrapper::VulkanCommandBuffers vulkanCommandBuffers;
		std::vector<Wrapper::VulkanSemaphore> vulkanImageAvailableSemaphores;
		std::vector<Wrapper::VulkanSemaphore> vulkanRenderFinishedSemaphores;
		std::vector<Wrapper::VulkanFence> vulkanInFlightFences;

		// wait for the device before starting to destruct Vulkan API-related objects
		const Wrapper::VulkanDeviceWaitIdle vulkanDeviceWaitIdle;

		// rendering state
		unsigned short vulkanCurrentFrame;

		// exception sub-class
		MAIN_EXCEPTION_CLASS();
	};

} /* spacelite::Engine */

#endif /* SRC_ENGINE_GRAPHICS_HPP_ */
