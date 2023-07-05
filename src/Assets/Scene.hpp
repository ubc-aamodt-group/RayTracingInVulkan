#pragma once

#include "Vulkan/Vulkan.hpp"
#include <memory>
#include <vector>

namespace Vulkan
{
	class Buffer;
	class CommandPool;
	class DeviceMemory;
	class Image;
}

namespace Assets
{
	class Model;
	class Texture;
	class TextureImage;

	class Scene final
	{
	public:

		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator = (const Scene&) = delete;
		Scene& operator = (Scene&&) = delete;

		Scene(Vulkan::CommandPool& commandPool, std::vector<Model>&& models, std::vector<Texture>&& textures);
		~Scene();

		const std::vector<Model>& Models() const { return models_; }
		bool HasProcedurals() const { return static_cast<bool>(proceduralBuffer_); }
		bool HasProceduralCubes() const { return static_cast<bool>(proceduralCubeBuffer_); }
		bool HasProceduralCylinder() const { return static_cast<bool>(proceduralCylinderBuffer_); }
		// bool HasProceduralMandelbulb() const { return static_cast<bool>(proceduralMandelbulbBuffer_); }

		const Vulkan::Buffer& VertexBuffer() const { return *vertexBuffer_; }
		const Vulkan::Buffer& IndexBuffer() const { return *indexBuffer_; }
		const Vulkan::Buffer& MaterialBuffer() const { return *materialBuffer_; }
		const Vulkan::Buffer& OffsetsBuffer() const { return *offsetBuffer_; }
		const Vulkan::Buffer& AabbBuffer() const { return *aabbBuffer_; }
		const Vulkan::Buffer& AabbCubeBuffer() const { return *aabbCubeBuffer_; }
		const Vulkan::Buffer& AabbCylinderBuffer() const { return *aabbCylinderBuffer_; }
		// const Vulkan::Buffer& AabbMandelbulbBuffer() const { return *aabbMandelbulbBuffer_; }
		const Vulkan::Buffer& ProceduralBuffer() const { return *proceduralBuffer_; }
		const Vulkan::Buffer& ProceduralCubeBuffer() const { return *proceduralCubeBuffer_; }
		const Vulkan::Buffer& ProceduralCylinderBuffer() const { return *proceduralCylinderBuffer_; }
		// const Vulkan::Buffer& ProceduralMandelbulbBuffer() const { return *proceduralMandelbulbBuffer_; }
		const std::vector<VkImageView> TextureImageViews() const { return textureImageViewHandles_; }
		const std::vector<VkSampler> TextureSamplers() const { return textureSamplerHandles_; }

	private:

		const std::vector<Model> models_;
		const std::vector<Texture> textures_;

		std::unique_ptr<Vulkan::Buffer> vertexBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> vertexBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> indexBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> indexBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> materialBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> materialBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> offsetBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> offsetBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> aabbBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> aabbBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> aabbCubeBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> aabbCubeBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> aabbCylinderBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> aabbCylinderBufferMemory_;

		// std::unique_ptr<Vulkan::Buffer> aabbMandelbulbBuffer_;
		// std::unique_ptr<Vulkan::DeviceMemory> aabbMandelbulbBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> proceduralBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> proceduralBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> proceduralCubeBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> proceduralCubeBufferMemory_;

		std::unique_ptr<Vulkan::Buffer> proceduralCylinderBuffer_;
		std::unique_ptr<Vulkan::DeviceMemory> proceduralCylinderBufferMemory_;

		// std::unique_ptr<Vulkan::Buffer> proceduralMandelbulbBuffer_;
		// std::unique_ptr<Vulkan::DeviceMemory> proceduralMandelbulbBufferMemory_;

		std::vector<std::unique_ptr<TextureImage>> textureImages_;
		std::vector<VkImageView> textureImageViewHandles_;
		std::vector<VkSampler> textureSamplerHandles_;
	};

}
