/*
 * VulkanPipeline.hpp
 *
 * Wraps a graphics pipeline used by the Vulkan API.
 *
 *  Created on: Sep 13, 2019
 *      Author: ans
 */

#ifndef SRC_WRAPPER_VULKANPIPELINE_HPP_
#define SRC_WRAPPER_VULKANPIPELINE_HPP_

#include "VulkanDevice.hpp"
#include "VulkanError.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanShaderModule.hpp"

#include "../Helper/File.hpp"
#include "../Helper/VulkanAllocator.hpp"
#include "../Main/Exception.hpp"
#include "../Struct/ShaderFiles.hpp"

#include <vulkan/vulkan_core.h>

#include <utility>	// std::move, std::swap
#include <vector>	// std::vector

namespace spacelite::Wrapper {

	/* DECLARATION
	 *
	 */
	class VulkanPipeline {
	public:
		VulkanPipeline(
				VulkanDevice& device,
				VulkanRenderPass& renderPass,
				VkExtent2D& targetExtent,
				const Struct::ShaderFiles& shaderFiles
		);
		virtual ~VulkanPipeline();

		// creation and destruction
		void create();
		void destroy() noexcept;

		// getters
		VkPipeline& get();
		const VkPipeline& get() const;

		// not copyable, only moveable
		VulkanPipeline(const VulkanPipeline&) = delete;
		VulkanPipeline(VulkanPipeline&& other) noexcept;
		VulkanPipeline& operator=(const VulkanPipeline&) = delete;
		VulkanPipeline& operator=(VulkanPipeline&& other) noexcept;

	private:
		VulkanDevice& parent;
		VkPipeline instance;
		VkPipelineLayout layout;
		VulkanRenderPass& pass;
		VkExtent2D& extent;

		std::vector<char> vertexShader;
		std::vector<char> fragmentShader;

		MAIN_EXCEPTION_CLASS();
	};

	/*
	 * IMPLEMENTATION
	 */
	// constructor: outsourced to create()
	inline VulkanPipeline::VulkanPipeline(
			VulkanDevice& device,
			VulkanRenderPass& renderPass,
			VkExtent2D& targetExtent,
			const Struct::ShaderFiles& shaderFiles
	)
			: parent(device),
			  instance(VK_NULL_HANDLE),
			  layout(VK_NULL_HANDLE),
			  pass(renderPass),
			  extent(targetExtent),
			  vertexShader(Helper::File::readBinary(shaderFiles.vertexShader)),
			  fragmentShader(Helper::File::readBinary(shaderFiles.fragmentShader)) {
		this->create();
	}

	// destructor: outsourced to destroy()
	inline VulkanPipeline::~VulkanPipeline() {
		this->destroy();
	}

	// get reference to the instance of the graphics pipeline
	inline VkPipeline& VulkanPipeline::get() {
		return this->instance;
	}

	// get const reference to the instance of the graphics pipeline
	inline const VkPipeline& VulkanPipeline::get() const {
		return this->instance;
	}

	// create the graphics pipeline
	inline void VulkanPipeline::create() {
		// setup shaders
		VulkanShaderModule vulkanVertShaderModule(this->parent, this->vertexShader);
		VulkanShaderModule fragShaderModule(this->parent, this->fragmentShader);

		VkPipelineShaderStageCreateInfo vulkanVertShaderStageInfo = {};

		vulkanVertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vulkanVertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;	// shader stage
		vulkanVertShaderStageInfo.module = vulkanVertShaderModule.get();// shader code
		vulkanVertShaderStageInfo.pName = "main";						// shader entry point
		vulkanVertShaderStageInfo.pSpecializationInfo = nullptr;		// shader constants (none atm)

		VkPipelineShaderStageCreateInfo vulkanFragShaderStageInfo = {};

		vulkanFragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vulkanFragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;	// shader stage
		vulkanFragShaderStageInfo.module = fragShaderModule.get();		// shader code
		vulkanFragShaderStageInfo.pName = "main";						// shader entry point
		vulkanFragShaderStageInfo.pSpecializationInfo = nullptr;		// shader constants (none atm)

		VkPipelineShaderStageCreateInfo vulkanShaderStages[] = { vulkanVertShaderStageInfo, vulkanFragShaderStageInfo };

		// setup vertex input
		VkPipelineVertexInputStateCreateInfo vulkanVertexInputInfo = {};

		auto vulkanBindingDescription = Struct::Vertex::getBindingDescription();
		auto vulkanAttributeDescriptions = Struct::Vertex::getAttributeDescriptions();

		vulkanVertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vulkanVertexInputInfo.vertexBindingDescriptionCount = 1;
		vulkanVertexInputInfo.vertexAttributeDescriptionCount = static_cast<unsigned int>(vulkanAttributeDescriptions.size());;
		vulkanVertexInputInfo.pVertexBindingDescriptions = &vulkanBindingDescription;
		vulkanVertexInputInfo.pVertexAttributeDescriptions = vulkanAttributeDescriptions.data();

		// setup input assembly
		VkPipelineInputAssemblyStateCreateInfo vulkanInputAssembly = {};

		vulkanInputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vulkanInputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		vulkanInputAssembly.primitiveRestartEnable = VK_FALSE;

		// setup viewport and scissor
		VkViewport vulkanViewport = {};

		vulkanViewport.x = 0.0f;
		vulkanViewport.y = 0.0f;
		vulkanViewport.width = (float) this->extent.width;
		vulkanViewport.height = (float) this->extent.height;
		vulkanViewport.minDepth = 0.0f;
		vulkanViewport.maxDepth = 1.0f;

		VkRect2D vulkanScissor = {};

		vulkanScissor.offset = {0, 0};
		vulkanScissor.extent = this->extent;

		VkPipelineViewportStateCreateInfo vulkanViewportState = {};

		vulkanViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vulkanViewportState.viewportCount = 1;
		vulkanViewportState.pViewports = &vulkanViewport;
		vulkanViewportState.scissorCount = 1;
		vulkanViewportState.pScissors = &vulkanScissor;

		// setup vulkanRasterizer
		VkPipelineRasterizationStateCreateInfo vulkanRasterizer = {};

		vulkanRasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		vulkanRasterizer.depthClampEnable = VK_FALSE;
		vulkanRasterizer.rasterizerDiscardEnable = VK_FALSE;
		vulkanRasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		vulkanRasterizer.lineWidth = 1.0f;
		vulkanRasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		vulkanRasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		vulkanRasterizer.depthBiasEnable = VK_FALSE;
		vulkanRasterizer.depthBiasConstantFactor = 0.0f;
		vulkanRasterizer.depthBiasClamp = 0.0f;
		vulkanRasterizer.depthBiasSlopeFactor = 0.0f;

		// setup multisampling
		VkPipelineMultisampleStateCreateInfo vulkanMultisampling = {};

		vulkanMultisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		vulkanMultisampling.sampleShadingEnable = VK_FALSE;
		vulkanMultisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		vulkanMultisampling.minSampleShading = 1.0f;
		vulkanMultisampling.pSampleMask = nullptr;
		vulkanMultisampling.alphaToCoverageEnable = VK_FALSE;
		vulkanMultisampling.alphaToOneEnable = VK_FALSE;

		// setup alpha and color blending
		VkPipelineColorBlendAttachmentState vulkanColorBlendAttachment = {};

		vulkanColorBlendAttachment.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT
				| VK_COLOR_COMPONENT_G_BIT
				| VK_COLOR_COMPONENT_B_BIT
				| VK_COLOR_COMPONENT_A_BIT;
/* to enable alpha blending:
		vulkanColorBlendAttachment.blendEnable = VK_TRUE;
		vulkanColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		vulkanColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
*/
		vulkanColorBlendAttachment.blendEnable = VK_FALSE;
		vulkanColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		vulkanColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		vulkanColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		vulkanColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		vulkanColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		vulkanColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo vulkanColorBlending = {};

		vulkanColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		vulkanColorBlending.logicOpEnable = VK_FALSE;
		vulkanColorBlending.logicOp = VK_LOGIC_OP_COPY;
		vulkanColorBlending.attachmentCount = 1;
		vulkanColorBlending.pAttachments = &vulkanColorBlendAttachment;
		vulkanColorBlending.blendConstants[0] = 0.0f;
		vulkanColorBlending.blendConstants[1] = 0.0f;
		vulkanColorBlending.blendConstants[2] = 0.0f;
		vulkanColorBlending.blendConstants[3] = 0.0f;

		// setup pipeline layout
		VkPipelineLayoutCreateInfo vulkanPipelineLayoutInfo = {};

		vulkanPipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		vulkanPipelineLayoutInfo.setLayoutCount = 0;
		vulkanPipelineLayoutInfo.pSetLayouts = nullptr;
		vulkanPipelineLayoutInfo.pushConstantRangeCount = 0;
		vulkanPipelineLayoutInfo.pPushConstantRanges = nullptr;

		VkResult vulkanResult = vkCreatePipelineLayout(
				this->parent.get(),
				&vulkanPipelineLayoutInfo,
				Helper::VulkanAllocator::ptr,
				&(this->layout)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create graphics pipeline layout: " + Wrapper::VulkanError(vulkanResult).str());

		// setup graphics pipeline
		VkGraphicsPipelineCreateInfo vulkanPipelineInfo = {};

		vulkanPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		vulkanPipelineInfo.stageCount = 2;
		vulkanPipelineInfo.pStages = vulkanShaderStages;
		vulkanPipelineInfo.pVertexInputState = &vulkanVertexInputInfo;
		vulkanPipelineInfo.pInputAssemblyState = &vulkanInputAssembly;
		vulkanPipelineInfo.pViewportState = &vulkanViewportState;
		vulkanPipelineInfo.pRasterizationState = &vulkanRasterizer;
		vulkanPipelineInfo.pMultisampleState = &vulkanMultisampling;
		vulkanPipelineInfo.pDepthStencilState = nullptr;
		vulkanPipelineInfo.pColorBlendState = &vulkanColorBlending;
		vulkanPipelineInfo.pDynamicState = nullptr;
		vulkanPipelineInfo.layout = this->layout;
		vulkanPipelineInfo.renderPass = this->pass.get();
		vulkanPipelineInfo.subpass = 0;
		vulkanPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		vulkanPipelineInfo.basePipelineIndex = -1;

		vulkanResult = vkCreateGraphicsPipelines(
				this->parent.get(),
				VK_NULL_HANDLE,
				1,
				&vulkanPipelineInfo,
				Helper::VulkanAllocator::ptr,
				&(this->instance)
		);

		if(vulkanResult != VK_SUCCESS)
			throw Exception("Could not create graphics pipeline: " + Wrapper::VulkanError(vulkanResult).str());
	}

	// destroy the graphics pipeline (may not throw an exception as it is used by the destructor, too)
	inline void VulkanPipeline::destroy() noexcept {
		if(this->instance) {
			vkDestroyPipeline(this->parent.get(), this->instance, Helper::VulkanAllocator::ptr);

			this->instance = VK_NULL_HANDLE;
		}

		if(this->layout) {
			vkDestroyPipelineLayout(this->parent.get(), this->layout, Helper::VulkanAllocator::ptr);

			this->layout = VK_NULL_HANDLE;
		}
	}

	// move constructor
	inline VulkanPipeline::VulkanPipeline(VulkanPipeline&& other) noexcept
			:	parent(other.parent),
				instance(other.instance),
				layout(other.layout),
				pass(other.pass),
				extent(other.extent),
				vertexShader(std::move(other.vertexShader)),
				fragmentShader(std::move(other.fragmentShader)) {
		other.instance = VK_NULL_HANDLE;
		other.layout = VK_NULL_HANDLE;
	}

	// move assignment
	inline VulkanPipeline& VulkanPipeline::operator=(VulkanPipeline&& other) noexcept {
		this->instance = other.instance;
		this->layout = other.layout;

		other.instance = VK_NULL_HANDLE;
		other.layout = VK_NULL_HANDLE;

		using std::swap;

		swap(this->parent, other.parent);
		swap(this->pass, other.pass);
		swap(this->extent, other.extent);
		swap(this->vertexShader, other.vertexShader);
		swap(this->fragmentShader, other.fragmentShader);

		return *this;
	}
} /* spacelite::Wrapper */

#endif /* SRC_WRAPPER_VULKANPIPELINE_HPP_ */
