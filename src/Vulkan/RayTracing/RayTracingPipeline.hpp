#pragma once

#include "Vulkan/Vulkan.hpp"
#include <memory>
#include <vector>


// #define USE_PROCEDURALS

namespace Assets
{
	class Scene;
	class UniformBuffer;
}

namespace Vulkan
{
	class DescriptorSetManager;
	class ImageView;
	class PipelineLayout;
	class SwapChain;
}

namespace Vulkan::RayTracing
{
	class DeviceProcedures;
	class TopLevelAccelerationStructure;

	class RayTracingPipeline final
	{
	public:

		VULKAN_NON_COPIABLE(RayTracingPipeline)

		RayTracingPipeline(
			const DeviceProcedures& deviceProcedures,
			const SwapChain& swapChain,
			const TopLevelAccelerationStructure& accelerationStructure,
			const ImageView& accumulationImageView,
			const ImageView& outputImageView,
			const std::vector<Assets::UniformBuffer>& uniformBuffers,
			const Assets::Scene& scene,
			const uint32_t shaderType);
		~RayTracingPipeline();

		uint32_t RayGenShaderIndex() const { return rayGenIndex_; }
		uint32_t MissShaderIndex() const { return missIndex_; }
		uint32_t TriangleHitGroupIndex() const { return triangleHitGroupIndex_; }
		#ifdef USE_PROCEDURALS
		uint32_t ProceduralHitGroupIndex() const { return proceduralHitGroupIndex_; }
		uint32_t ProceduralCubeHitGroupIndex() const { return proceduralCubeHitGroupIndex_; }
		uint32_t ProceduralCylinderHitGroupIndex() const { return proceduralCylinderHitGroupIndex_; }
		// uint32_t ProceduralMandelbulbHitGroupIndex() const { return proceduralMandelbulbHitGroupIndex_; }
		#endif

		VkDescriptorSet DescriptorSet(uint32_t index) const;
		const class PipelineLayout& PipelineLayout() const { return *pipelineLayout_; }

	private:

		const SwapChain& swapChain_;

		VULKAN_HANDLE(VkPipeline, pipeline_)

		std::unique_ptr<DescriptorSetManager> descriptorSetManager_;
		std::unique_ptr<class PipelineLayout> pipelineLayout_;

		uint32_t rayGenIndex_;
		uint32_t missIndex_;
		uint32_t triangleHitGroupIndex_;
		#ifdef USE_PROCEDURALS
		uint32_t proceduralHitGroupIndex_;
		uint32_t proceduralCubeHitGroupIndex_;
		uint32_t proceduralCylinderHitGroupIndex_;
		// uint32_t proceduralMandelbulbHitGroupIndex_;
		#endif
	};

}
