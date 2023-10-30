#include "Application.hpp"
#include "Buffer.hpp"
#include "CommandPool.hpp"
#include "CommandBuffers.hpp"
#include "DebugUtilsMessenger.hpp"
#include "DepthBuffer.hpp"
#include "Device.hpp"
#include "Fence.hpp"
#include "FrameBuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "SingleTimeCommands.hpp"
#include "Instance.hpp"
#include "PipelineLayout.hpp"
#include "RenderPass.hpp"
#include "Semaphore.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "Window.hpp"
#include "Assets/Model.hpp"
#include "Assets/Scene.hpp"
#include "Assets/UniformBuffer.hpp"
#include "Utilities/Exception.hpp"
#include <array>
#include <cstdlib>
#include <fstream>
#include <unistd.h>

namespace Vulkan {

Application::Application(const WindowConfig& windowConfig, const VkPresentModeKHR presentMode, const bool enableValidationLayers) :
	presentMode_(presentMode)
{
	const auto validationLayers = enableValidationLayers
		? std::vector<const char*>{"VK_LAYER_KHRONOS_validation"}
		: std::vector<const char*>();

	window_.reset(new class Window(windowConfig));
	instance_.reset(new Instance(*window_, validationLayers, VK_API_VERSION_1_2));
	debugUtilsMessenger_.reset(enableValidationLayers ? new DebugUtilsMessenger(*instance_, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr);
	surface_.reset(new Surface(*instance_));
}

Application::~Application()
{
	Application::DeleteSwapChain();

	commandPool_.reset();
	device_.reset();
	surface_.reset();
	debugUtilsMessenger_.reset();
	instance_.reset();
	window_.reset();
}

const std::vector<VkExtensionProperties>& Application::Extensions() const
{
	return instance_->Extensions();
}

const std::vector<VkLayerProperties>& Application::Layers() const
{
	return instance_->Layers();
}

const std::vector<VkPhysicalDevice>& Application::PhysicalDevices() const
{
	return instance_->PhysicalDevices();
}

void Application::SetPhysicalDevice(VkPhysicalDevice physicalDevice)
{
	if (device_)
	{
		Throw(std::logic_error("physical device has already been set"));
	}

	std::vector<const char*> requiredExtensions = 
	{
		// VK_KHR_swapchain
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	VkPhysicalDeviceFeatures deviceFeatures = {};
	
	SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, nullptr);
	OnDeviceSet();

	// Create swap chain and command buffers.
	CreateSwapChain();
}

void Application::Run()
{
	if (!device_)
	{
		Throw(std::logic_error("physical device has not been set"));
	}

	currentFrame_ = 0;
	// iterNum = 0;

	window_->DrawFrame = [this]() { DrawFrame(); };
	window_->OnKey = [this](const int key, const int scancode, const int action, const int mods) { OnKey(key, scancode, action, mods); };
	window_->OnCursorPosition = [this](const double xpos, const double ypos) { OnCursorPosition(xpos, ypos); };
	window_->OnMouseButton = [this](const int button, const int action, const int mods) { OnMouseButton(button, action, mods); };
	window_->OnScroll = [this](const double xoffset, const double yoffset) { OnScroll(xoffset, yoffset); };
	window_->Run();
	device_->WaitIdle();
}

void Application::SetPhysicalDevice(
	VkPhysicalDevice physicalDevice, 
	std::vector<const char*>& requiredExtensions, 
	VkPhysicalDeviceFeatures& deviceFeatures,
	void* nextDeviceFeatures)
{
	device_.reset(new class Device(physicalDevice, *surface_, requiredExtensions, deviceFeatures, nextDeviceFeatures));
	commandPool_.reset(new class CommandPool(*device_, device_->GraphicsFamilyIndex(), true));
}

void Application::OnDeviceSet()
{
}

void Application::CreateSwapChain()
{
	// Wait until the window is visible.
	while (window_->IsMinimized())
	{
		window_->WaitForEvents();
	}

	swapChain_.reset(new class SwapChain(*device_, presentMode_));
	depthBuffer_.reset(new class DepthBuffer(*commandPool_, swapChain_->Extent()));

	for (size_t i = 0; i != swapChain_->ImageViews().size(); ++i)
	{
		imageAvailableSemaphores_.emplace_back(*device_);
		renderFinishedSemaphores_.emplace_back(*device_);
		inFlightFences_.emplace_back(*device_, true);
		uniformBuffers_.emplace_back(*device_);
	}

	graphicsPipeline_.reset(new class GraphicsPipeline(*swapChain_, *depthBuffer_, uniformBuffers_, GetScene(), isWireFrame_));

	for (const auto& imageView : swapChain_->ImageViews())
	{
		swapChainFramebuffers_.emplace_back(*imageView, graphicsPipeline_->RenderPass());
	}

	commandBuffers_.reset(new CommandBuffers(*commandPool_, static_cast<uint32_t>(swapChainFramebuffers_.size())));
}

void Application::DeleteSwapChain()
{
	commandBuffers_.reset();
	swapChainFramebuffers_.clear();
	graphicsPipeline_.reset();
	uniformBuffers_.clear();
	inFlightFences_.clear();
	renderFinishedSemaphores_.clear();
	imageAvailableSemaphores_.clear();
	depthBuffer_.reset();
	swapChain_.reset();
}

void Application::DrawFrame()
{
	const auto noTimeout = std::numeric_limits<uint64_t>::max();

	auto& inFlightFence = inFlightFences_[currentFrame_];
	const auto imageAvailableSemaphore = imageAvailableSemaphores_[currentFrame_].Handle();
	const auto renderFinishedSemaphore = renderFinishedSemaphores_[currentFrame_].Handle();

	// inFlightFence.Wait(noTimeout);

	#ifdef OFFSCREEN_RENDERING
	uint32_t imageIndex = 0;
	auto result = VK_SUCCESS;
	#else
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(device_->Handle(), swapChain_->Handle(), noTimeout, imageAvailableSemaphore, nullptr, &imageIndex);
	#endif

	// #ifndef OFFSCREEN_RENDERING
	// TakeScreenshot("./heatmap.ppm", imageIndex);
	// #endif

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || isWireFrame_ != graphicsPipeline_->IsWireFrame())
	{
		RecreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		Throw(std::runtime_error(std::string("failed to acquire next image (") + ToString(result) + ")"));
	}

	const auto commandBuffer = commandBuffers_->Begin(imageIndex);
	Render(commandBuffer, imageIndex);
	commandBuffers_->End(imageIndex);

	UpdateUniformBuffer(imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkCommandBuffer commandBuffers[]{ commandBuffer };
	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };

	#ifdef OFFSCREEN_RENDERING
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = waitSemaphores;
	#else
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	#endif
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = commandBuffers;
	#ifdef OFFSCREEN_RENDERING
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	#else
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	#endif

	inFlightFence.Reset();

	VkResult r = vkQueueSubmit(device_->GraphicsQueue(), 1, &submitInfo, inFlightFence.Handle());
	printf("RES: %d\n", r);
	Check(r,
		"submit draw command buffer");

	inFlightFence.Wait(noTimeout);

	TakeScreenshot("./heatmap.ppm", imageIndex);

	#ifndef OFFSCREEN_RENDERING
	VkSwapchainKHR swapChains[] = { swapChain_->Handle() };
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional

	result = vkQueuePresentKHR(device_->PresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		RecreateSwapChain();
		return;
	}
	
	if (result != VK_SUCCESS)
	{
		Throw(std::runtime_error(std::string("failed to present next image (") + ToString(result) + ")"));
	}
	#endif

	if (iterNum >= 12)
	{
		exit(0);
	}

	iterNum++;

	currentFrame_ = (currentFrame_ + 1) % inFlightFences_.size();
}

void Application::Render(VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = graphicsPipeline_->RenderPass().Handle();
	renderPassInfo.framebuffer = swapChainFramebuffers_[imageIndex].Handle();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChain_->Extent();
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	{
		const auto& scene = GetScene();

		VkDescriptorSet descriptorSets[] = { graphicsPipeline_->DescriptorSet(imageIndex) };
		VkBuffer vertexBuffers[] = { scene.VertexBuffer().Handle() };
		const VkBuffer indexBuffer = scene.IndexBuffer().Handle();
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_->Handle());
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline_->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

		uint32_t vertexOffset = 0;
		uint32_t indexOffset = 0;

		for (const auto& model : scene.Models())
		{
			const auto vertexCount = static_cast<uint32_t>(model.NumberOfVertices());
			const auto indexCount = static_cast<uint32_t>(model.NumberOfIndices());

			vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, vertexOffset, 0);

			vertexOffset += vertexCount;
			indexOffset += indexCount;
		}
	}
	vkCmdEndRenderPass(commandBuffer);
}

void Application::TakeScreenshot(std::string filename, uint32_t imageIndex)
{
	// VkFormatProperties formatProps;
	//
	// vkGetPhysicalDeviceFormatProperties(device_->PhysicalDevice(), swapChain_->Format(), &formatProps);
	// if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
	// 	printf("No support for blitting from optimal tiled images\n");
	// }
	//
	// vkGetPhysicalDeviceFormatProperties(device_->PhysicalDevice(), VK_FORMAT_R8G8B8_UNORM, &formatProps);
	// if (!(formatProps.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT)) {
	// 	printf("No support for blitting to linearly tiled images\n");
	// }

	// printf("Image indx to capture: %d\n", imageIndex);

	const uint32_t width = swapChain_->Extent().width;
	const uint32_t height = swapChain_->Extent().height;

	Image dstImageAbs(
		Device(),
		swapChain_->Extent(),
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_TILING_LINEAR,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	DeviceMemory dstImageMem = dstImageAbs.AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkImageSubresource subResource { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0 };
	VkSubresourceLayout subResourceLayout;
	vkGetImageSubresourceLayout(Device().Handle(), dstImageAbs.Handle(), &subResource, &subResourceLayout);

	VkImage srcImg = swapChain_->Images()[imageIndex];

	// dstImageAbs.TransitionImageLayout(CommandPool(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	// swapChain_->OffscreenImage()->TransitionImageLayout(CommandPool(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

	SingleTimeCommands::Submit(CommandPool(), [&](VkCommandBuffer commandBuffer)
	{
		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = dstImageAbs.Handle();
		barrier.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		VkClearColorValue color = { .float32 = {1.0, 0.0, 1.0} };
		VkImageSubresourceRange imageSubresourceRange { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCmdClearColorImage(commandBuffer, dstImageAbs.Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &color, 1, &imageSubresourceRange);

		#ifndef OFFSCREEN_RENDERING
		VkImageMemoryBarrier barrier2 = {};
		barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier2.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//barrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier2.image = srcImg;
		barrier2.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barrier2.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		// barrier2.srcAccessMask = 0;
		barrier2.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier2);
		#endif

		VkImageCopy imageCopyRegion{};
		imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.srcSubresource.layerCount = 1;
		imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageCopyRegion.dstSubresource.layerCount = 1;
		imageCopyRegion.extent.width = width;
		imageCopyRegion.extent.height = height;
		imageCopyRegion.extent.depth = 1;

		// Issue the copy command
		vkCmdCopyImage(
			commandBuffer,
			srcImg, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			dstImageAbs.Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&imageCopyRegion);

		VkImageMemoryBarrier barrier3 = {};
		barrier3.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier3.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier3.newLayout = VK_IMAGE_LAYOUT_GENERAL;
		barrier3.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier3.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier3.image = dstImageAbs.Handle();
		barrier3.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barrier3.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier3.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier3);

		#ifndef OFFSCREEN_RENDERING
		VkImageMemoryBarrier barrier4 = {};
		barrier4.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier4.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier4.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		//barrier4.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier4.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier4.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier4.image = srcImg;
		barrier4.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		barrier4.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier4.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		// barrier4.dstAccessMask = 0;

		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier4);
		#endif
	});

	// const uint8_t *data = (const uint8_t*) dstImageMem.Map(0, VK_WHOLE_SIZE);
	const uint8_t *data;
	Check(vkMapMemory(Device().Handle(), dstImageMem.Handle(), 0, VK_WHOLE_SIZE, 0, (void**) &data), "map memory");
	data += subResourceLayout.offset;

	std::ofstream file(filename, std::ios::out | std::ios::binary);

	file << "P3\n" << width << "\n" << height << "\n" << 255 << "\n";

	for (uint32_t y = 0; y < height; y++)
	{
		uint8_t *row = (uint8_t*)data;
		for (uint32_t x = 0; x < width; x++)
		{
			// printf("%d %d %d\n", row[2], row[1], row[0]);
	   #ifdef OFFSCREEN_RENDERING
			file << (uint32_t) row[0] << ' ' << (uint32_t) row[1] << ' ' << (uint32_t) row[2] << '\n';
		#else
			file << (uint32_t) row[2] << ' ' << (uint32_t) row[1] << ' ' << (uint32_t) row[0] << '\n';
	   #endif
			row += 4;
		}
		data += subResourceLayout.rowPitch;
	}
	file.close();

	printf("Screenshot saved to disk!\n");

	vkUnmapMemory(Device().Handle(), dstImageMem.Handle());
}

void Application::UpdateUniformBuffer(const uint32_t imageIndex)
{
	uniformBuffers_[imageIndex].SetValue(GetUniformBufferObject(swapChain_->Extent()));
}

void Application::RecreateSwapChain()
{
	device_->WaitIdle();
	DeleteSwapChain();
	CreateSwapChain();
}

}
