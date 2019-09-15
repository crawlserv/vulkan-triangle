/*
 * VulkanAllocator.hpp
 *
 *  Created on: Sep 14, 2019
 *      Author: ans
 */

#ifndef SRC_HELPER_VULKANALLOCATOR_HPP_
#define SRC_HELPER_VULKANALLOCATOR_HPP_

#include <vulkan/vulkan_core.h>

#include <cstdlib> // aligned_alloc, free

namespace spacelite::Helper {
	/*
	 * DECLARATION
	 */
	class VulkanAllocator {
	public:
		/* STATIC CONSTANTS */
		static const VkAllocationCallbacks allocatorStruct;
		static const VkAllocationCallbacks * ptr;
		/* END STATIC CONSTANTS */

		/* SINGLETON */
		static VulkanAllocator& getInstance() {
			static VulkanAllocator instance;

			return instance;
		}

	private:
		VulkanAllocator();

	public:
		VulkanAllocator(const VulkanAllocator&) = delete;
		void operator=(const VulkanAllocator&) = delete;
		/* END SINGLETON */

		static void * allocate(void * pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
		static void * reallocate(void * pUserData, void * pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope);
		static void free(void * pUserData, void * pMemory);
		static unsigned long long getAllocated();
		static unsigned long long getReAllocated();

	private:
		static unsigned long long allocated;
		static unsigned long long reAllocated;
	};

	/*
	 * IMPLEMENTATION
	 */
	inline const VkAllocationCallbacks VulkanAllocator::allocatorStruct {
		nullptr,
		VulkanAllocator::allocate,
		VulkanAllocator::reallocate,
		VulkanAllocator::free,
		nullptr,
		nullptr
	};

	inline const VkAllocationCallbacks * VulkanAllocator::ptr = &VulkanAllocator::allocatorStruct;
	inline unsigned long long VulkanAllocator::allocated = 0;
	inline unsigned long long VulkanAllocator::reAllocated = 0;

	inline VulkanAllocator::VulkanAllocator() {}

	inline void * VulkanAllocator::allocate(
			void * pUserData __attribute__ ((unused)),
			size_t size,
			size_t alignment,
			VkSystemAllocationScope allocationScope __attribute__ ((unused))
	) {
		VulkanAllocator::allocated += size;

		return aligned_alloc(alignment, size);
	}

	inline void * VulkanAllocator::reallocate(
			void * pUserData __attribute__ ((unused)),
			void * pOriginal,
			size_t size,
			size_t alignment __attribute__ ((unused)),
			VkSystemAllocationScope allocationScope __attribute__ ((unused))
	) {
		VulkanAllocator::reAllocated += size;

		return realloc(pOriginal, size);
	}

	inline void VulkanAllocator::free(void * pUserData __attribute__ ((unused)), void * pMemory) {
		::free(pMemory);
	}

	inline unsigned long long VulkanAllocator::getAllocated() {
		return VulkanAllocator::allocated;
	}

	inline unsigned long long VulkanAllocator::getReAllocated() {
		return VulkanAllocator::reAllocated;
	}

} /* spacelite::Helper::VulkanAllocator */

#endif
