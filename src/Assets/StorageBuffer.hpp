#pragma once

#include "Utilities/Glm.hpp"
#include <memory>

namespace Vulkan
{
	class Buffer;
	class Device;
	class DeviceMemory;
}

namespace Assets
{
	class StorageBufferObject
	{
	public:

		glm::mat4 ModelView;
		glm::mat4 Projection;
		glm::mat4 ModelViewInverse;
		glm::mat4 ProjectionInverse;
		glm::vec3 LightPosition;
		float LightRadius;
		float Aperture;
		float FocusDistance;
		float HeatmapScale;
		uint32_t TotalNumberOfSamples;
		uint32_t NumberOfSamples;
		uint32_t NumberOfBounces;
		uint32_t NumberOfShadows;
		uint32_t RandomSeed;
		uint32_t Width;
		uint32_t Height;
		uint32_t HasSky; // bool
		uint32_t ShowHeatmap; // bool
	};

	class StorageBuffer
	{
	public:

		StorageBuffer(const StorageBuffer&) = delete;
		StorageBuffer& operator = (const StorageBuffer&) = delete;
		StorageBuffer& operator = (StorageBuffer&&) = delete;

		explicit StorageBuffer(const Vulkan::Device& device);
		StorageBuffer(StorageBuffer&& other) noexcept;
		~StorageBuffer();

		const Vulkan::Buffer& Buffer() const { return *buffer_; }

		void SetValue(const StorageBufferObject& ubo);

	private:

		std::unique_ptr<Vulkan::Buffer> buffer_;
		std::unique_ptr<Vulkan::DeviceMemory> memory_;
	};

}
