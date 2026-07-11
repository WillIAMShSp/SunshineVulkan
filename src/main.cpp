#include "Backend/RenderPass.h"
#include "Backend/RenderPassAttachment.h"
#include "Backend/DescriptorSetLayout.h"
#include "Backend/GraphicsPipeline.h"
#include "Backend/TextureSampler.h"
#include "Backend/Texture.h"
#include "Backend/VertexBuffer.h"
#include "Backend/IndexBuffer.h"
#include "Backend/DescriptorPool.h"
#include "Backend/UniformBuffer.h"
#include "Backend/DescriptorSet.h"
#include "Backend/Framebuffer.h"
#include "Backend/CommandBuffer.h"

#include <filesystem>

#ifndef RESOURCE_DIR
	#define RESOURCE_DIR ""
#endif


struct Vertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;


};
struct ModelViewProjectionBuffer
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 projection;

};


ModelViewProjectionBuffer buf;
void MVP()
{
	static auto starttime = std::chrono::high_resolution_clock::now();

	auto currenttime = std::chrono::high_resolution_clock::now();

	float time = std::chrono::duration<float, std::chrono::seconds::period>(currenttime - starttime).count();

	ModelViewProjectionBuffer mvp;

	mvp.model = glm::mat4(1.0);
	mvp.view = glm::lookAt(glm::vec3(0.0f, 1.f, -3.0f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.projection = glm::perspective(45.f, ((float)core.m_swapchainExtent.width) / ((float)core.m_swapchainExtent.height), 0.1f, 100.f);

	//mvp.projection[1][1] *= -1;
	buf = mvp;
}

const std::vector<Vertex> vertices = {
{{-0.5f, -0.5f, 3.f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
{{0.5f, -0.5f, 3.f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
{{0.5f, 0.5f, 3.f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
{{-0.5f, 0.5f, 3.f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};
const std::vector<uint8_t> indices =
{
	0, 1, 2, 2, 3, 0

};



float quadVertices[] =
{
	// x      y      z      u     v
   -1.0f, -1.0f, 0.0f,   0.0f, 1.0f,
   -1.0f,  1.0f, 0.0f,   0.0f, 0.0f,
	1.0f, -1.0f, 0.0f,   1.0f, 1.0f,
	1.0f,  1.0f, 0.0f,   1.0f, 0.0f
};



uint32_t quadIndices[] =
{
	0, 1, 2,
	2, 1, 3
};


const std::vector<Vertex> TESTvertices = {
{{-0.5f, -0.5f, 0.f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
{{0.5f, -0.5f, 0.f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
{{0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
{{-0.5f, 0.5f, 0.f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};









int main() {
	std::cout << std::filesystem::current_path() << std::endl;
	core.init();
	core.createMainRenderSetup(false);
	RenderPassAttachment attachment = PersistanceBackend::createRenderPassAttachment(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, core.m_swapchainImageFormat, 
		VK_SAMPLE_COUNT_1_BIT, 
		VK_ATTACHMENT_LOAD_OP_CLEAR, 
		VK_ATTACHMENT_STORE_OP_STORE,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	RenderPassAttachment depthAttachment = PersistanceBackend::createRenderPassAttachment(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, PersistanceUtils::findDepthFormat(), VK_SAMPLE_COUNT_1_BIT, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	AttachmentReferenceList refList;
	refList.add(attachment);
	AttachmentDescriptionList disList;
	disList.add(&attachment, 1);
	disList.add(&depthAttachment, 1);

	VkSubpassDescription description = PersistanceBackend::createSubpassDescription(&refList, &depthAttachment, nullptr, nullptr, 0);

	VkSubpassDependency dependency = PersistanceBackend::createSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0);
	
	VkRenderPass renderpass = PersistanceBackend::createRenderPass(&description, 1, &dependency, 1, disList);

	VkDescriptorSetLayoutBinding bindings[2] = {
		PersistanceBackend::createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT),
		PersistanceBackend::createDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)

	};
	VkDescriptorSetLayout layout = PersistanceBackend::createDescriptorSetLayout(bindings, 2);


	
	PipelineSettings settings;
	
	VertexInputStateLayout vertexbufferlayout;
	vertexbufferlayout.push<glm::vec3>();/*Configure vertex array layout*/
	vertexbufferlayout.push<glm::vec3>();//
	vertexbufferlayout.push<glm::vec2>();//
	settings.createVertexInputState(vertexbufferlayout);
	settings.defineInputAssemblyState();
	settings.createStaticViewPortAndScissors(0, 0, 0.f, 1.f, core.m_swapchainExtent, core.m_swapchainExtent, {0,0});
	settings.configureDepthStencilState(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS);
	settings.configureRasterizationStage();
	settings.configureMultisample();
	settings.configureColorBlend();
	//settings.UseDynamicViewport();

	PersistanceBackend::Shader shader;

	std::string basicVertexShaderSource = std::string(RESOURCE_DIR) + "/Shaders/basicvert.spv";
	std::string basicFragmentShaderSource = std::string(RESOURCE_DIR) + "/Shaders/basicfrag.spv";

	
	shader.createShaderStage(basicVertexShaderSource.c_str(), VK_SHADER_STAGE_VERTEX_BIT);
	shader.createShaderStage(basicFragmentShaderSource.c_str(), VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineLayout pipelayout = PersistanceBackend::createPipelineLayout(&layout, 1, nullptr, 0);
	VkPipeline graphicsPipeline = PersistanceBackend::createGraphicsPipeline(pipelayout, shader, settings, renderpass);

	std::string textureSource = std::string(RESOURCE_DIR) + "/Textures/Placeholder.png";	
	Texture texture = PersistanceBackend::createTexture(textureSource.c_str());
	VkSampler sampler = PersistanceBackend::createTextureSampler();

	Buffer vertexBuffer = PersistanceBackend::createVertexBuffer(vertices.data(), sizeof(vertices[0]), static_cast<uint32_t>(vertices.size()));
	Buffer indexBuffer = PersistanceBackend::createIndexBuffer(indices.data(), sizeof(indices[0]), static_cast<uint32_t>(indices.size()));

	Buffer testVBuffer = PersistanceBackend::createVertexBuffer(TESTvertices.data(), sizeof(TESTvertices[0]), static_cast<uint32_t>(TESTvertices.size()));


	
	DescriptorPoolSizeList desPoolList;
	
	VkDescriptorPoolSize uniformBufferSize = PersistanceBackend::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	VkDescriptorPoolSize samplerSize = PersistanceBackend::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

	desPoolList.add(&uniformBufferSize, 1);
	desPoolList.add(&samplerSize, 1);

	VkDescriptorPool descriptorPool = PersistanceBackend::createDescriptorPool(desPoolList);


	UniformBuffer uniformBuffer = PersistanceBackend::createUniformBuffer(sizeof(ModelViewProjectionBuffer));
	
	VkWriteDescriptorSet writeDescriptors[2];
	std::vector<VkDescriptorSet> descriptorSet = PersistanceBackend::allocateDescriptorSet(descriptorPool, PersistanceLib::MAXFRAMESINFLIGHT, layout);
	std::vector<VkDescriptorBufferInfo> uniformBufferinfo = PersistanceBackend::createDescriptorBufferInfo(uniformBuffer.buffers.data(), static_cast<uint32_t>(uniformBuffer.buffers.size()), 0, sizeof(ModelViewProjectionBuffer));
	writeDescriptors[0] = PersistanceBackend::createWriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, uniformBufferinfo.data(), nullptr, 0);
	std::vector<VkDescriptorImageInfo> imageInfo = PersistanceBackend::createDescriptorImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.imageview, sampler);
	writeDescriptors[1] = PersistanceBackend::createWriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, imageInfo.data(), 0);

	
	PersistanceBackend::updateDescriptorSets(descriptorSet, writeDescriptors, 2);

	Texture depthTexture;

	PersistanceUtils::createImage(core.m_swapchainExtent.width, core.m_swapchainExtent.height, PersistanceUtils::findDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthTexture.image, depthTexture.allocation, VK_SHARING_MODE_CONCURRENT, VK_IMAGE_LAYOUT_UNDEFINED);

	depthTexture.imageview = PersistanceUtils::createImageView(depthTexture.image, PersistanceUtils::findDepthFormat(), VK_IMAGE_ASPECT_DEPTH_BIT);


	Framebuffer framebuffer = PersistanceBackend::createFramebuffer(renderpass, screenwidth, screenheight, 1, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthTexture.imageview);

	VkCommandBuffer commandBuffer = PersistanceBackend::allocateCommandBuffer(core.m_graphicsCommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

#pragma region fullQuadTest

	VkDescriptorSetLayoutBinding fullQuadBinding = PersistanceBackend::createDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	VkDescriptorSetLayout fullQuadDescriptorSetLayout = PersistanceBackend::createDescriptorSetLayout(&fullQuadBinding, 1);


	PipelineSettings fullQuadPipeSettings;
	
	VertexInputStateLayout fullQuadVertexLayout;
	fullQuadVertexLayout.push<glm::vec3>();/*Configure vertex array layout*/
	fullQuadVertexLayout.push<glm::vec2>();
	fullQuadPipeSettings.createVertexInputState(fullQuadVertexLayout);
	fullQuadPipeSettings.defineInputAssemblyState();
	fullQuadPipeSettings.createStaticViewPortAndScissors(0, 0, 0.f, 1.f, core.m_swapchainExtent, core.m_swapchainExtent, { 0,0 });
	fullQuadPipeSettings.configureRasterizationStage();
	fullQuadPipeSettings.configureMultisample();
	fullQuadPipeSettings.configureColorBlend();

	PersistanceBackend::Shader fullQuadShader;

	std::string fullQuadVertexShaderSource = std::string(RESOURCE_DIR) + "/Shaders/fullQuad/fullquadvert.spv";
	std::string fullQuadFragmentShaderSource = std::string(RESOURCE_DIR) + "/Shaders/fullQuad/fullquadfrag.spv";
	
	fullQuadShader.createShaderStage(fullQuadVertexShaderSource.c_str(), VK_SHADER_STAGE_VERTEX_BIT);
	fullQuadShader.createShaderStage(fullQuadFragmentShaderSource.c_str(), VK_SHADER_STAGE_FRAGMENT_BIT);

	VkPipelineLayout fullQuadGraphicsPipelineLayout = PersistanceBackend::createPipelineLayout(&fullQuadDescriptorSetLayout, 1, nullptr, 0);
	VkPipeline fullQuadGraphicsPipeline = PersistanceBackend::createGraphicsPipeline(fullQuadGraphicsPipelineLayout, fullQuadShader, fullQuadPipeSettings, core.m_mainRenderPass);

	Buffer fullQuadVertexBuffer = PersistanceBackend::createVertexBuffer(quadVertices, sizeof(float), 20);
	Buffer fullQuadIndexBuffer = PersistanceBackend::createIndexBuffer(quadIndices, sizeof(uint32_t), 6);


	DescriptorPoolSizeList fullQuadDesPoolList;
	VkDescriptorPoolSize fullQuadSamplerSize = PersistanceBackend::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
	fullQuadDesPoolList.add(&fullQuadSamplerSize, 1);

	VkDescriptorPool fullQuadDescriptorPool = PersistanceBackend::createDescriptorPool(fullQuadDesPoolList);

	std::vector<VkDescriptorSet> fullQuadDescriptorSet = PersistanceBackend::allocateDescriptorSet(fullQuadDescriptorPool, PersistanceLib::MAXFRAMESINFLIGHT, fullQuadDescriptorSetLayout);
	std::vector<VkDescriptorImageInfo> fullQuadImageInfo = PersistanceBackend::createDescriptorImageInfoPerFrame(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, framebuffer.imageviews, sampler);

	VkWriteDescriptorSet fullQuadSamplerWriteDescriptorSet = PersistanceBackend::createWriteDescriptorSet(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0, nullptr, fullQuadImageInfo.data(), 0);

	PersistanceBackend::updateDescriptorSets(fullQuadDescriptorSet, &fullQuadSamplerWriteDescriptorSet, 1);

#pragma endregion


	while (core.isRunning()) 
	{
		core.pollEvents();


		core.startDrawing();
		PersistanceBackend::resetCommandBuffer(commandBuffer);
		PersistanceBackend::beginCommandBuffer(commandBuffer, 0);
		
		
		VkClearValue clearValues[2];
		clearValues[0].color = { 0.0f,0.0f,0.0f,0.0f };
		clearValues[1].depthStencil = { 1.f, 0 };
		
		PersistanceBackend::beginRenderPass(commandBuffer, renderpass, framebuffer, {0,0}, core.m_swapchainExtent, clearValues, 2);
		
		VkDeviceSize offsets = 0;

		core.bindGraphicsPipeline(commandBuffer, graphicsPipeline);

		MVP();

		PersistanceBackend::updateUniformBuffers(uniformBuffer, &buf, sizeof(ModelViewProjectionBuffer));

		core.drawIndexed(commandBuffer, &testVBuffer, 1, &offsets, indexBuffer, graphicsPipeline, pipelayout, &descriptorSet[core.m_currentFrame], 1);
		core.drawIndexed(commandBuffer, &vertexBuffer, 1, &offsets, indexBuffer, graphicsPipeline, pipelayout, &descriptorSet[core.m_currentFrame], 1);


		PersistanceBackend::endRenderPass(commandBuffer);




		core.beginMainRenderPass(commandBuffer);
		core.bindGraphicsPipeline(commandBuffer, fullQuadGraphicsPipeline);
		core.drawIndexed(commandBuffer, &fullQuadVertexBuffer, 1, &offsets, fullQuadIndexBuffer, fullQuadGraphicsPipeline, fullQuadGraphicsPipelineLayout, &fullQuadDescriptorSet[core.m_currentFrame], 1);
		PersistanceBackend::endRenderPass(commandBuffer);

		
		
		PersistanceBackend::endCommandBuffer(commandBuffer);
		core.endDrawingandPresent( &commandBuffer, 1);



	}
	core.waitForDeviceIdle();

	PersistanceBackend::cleanUpRenderPasses(&renderpass, 1);
	PersistanceBackend::cleanUpDescriptorPools(&fullQuadDescriptorPool,1);
	PersistanceBackend::cleanUpDescriptorPools(&descriptorPool, 1);
	PersistanceBackend::cleanUpFramebuffers(&framebuffer, 1);
	PersistanceBackend::cleanUpGraphicsPipeline(&graphicsPipeline, 1);
	PersistanceBackend::cleanUpGraphicsPipeline(&fullQuadGraphicsPipeline, 1);
	PersistanceBackend::cleanUpIndexBuffers(&indexBuffer, 1);
	PersistanceBackend::cleanUpIndexBuffers(&fullQuadIndexBuffer, 1);
	PersistanceBackend::cleanUpVertexBuffers(&vertexBuffer, 1);
	PersistanceBackend::cleanUpVertexBuffers(&fullQuadVertexBuffer, 1);
	PersistanceBackend::cleanUpVertexBuffers(&testVBuffer, 1);
	PersistanceBackend::cleanUpDescriptorSetLayouts(&layout, 1);
	PersistanceBackend::cleanUpDescriptorSetLayouts(&fullQuadDescriptorSetLayout, 1);
	PersistanceBackend::cleanUpTextures(&texture, 1);
	PersistanceBackend::cleanUpTextureSamplers(&sampler, 1);
	PersistanceBackend::cleanUpUniformBuffers(&uniformBuffer, 1);
	PersistanceBackend::cleanUpPipelineLayouts(&pipelayout, 1);
	PersistanceBackend::cleanUpPipelineLayouts(&fullQuadGraphicsPipelineLayout, 1);
	PersistanceBackend::cleanUpTextures(&depthTexture, 1);


	

	core.finalize();

}
