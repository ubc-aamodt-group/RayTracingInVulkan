#include "StorageBuffer.hpp"
#include "Vulkan/Buffer.hpp"
#include <cstring>

namespace Assets {

StorageBuffer::StorageBuffer(const Vulkan::Device& device)
{
	const auto bufferSize = sizeof(StorageBufferObject);

	buffer_.reset(new Vulkan::Buffer(device, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
	memory_.reset(new Vulkan::DeviceMemory(buffer_->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)));
}

StorageBuffer::StorageBuffer(StorageBuffer&& other) noexcept :
	buffer_(other.buffer_.release()),
	memory_(other.memory_.release())
{
}

StorageBuffer::~StorageBuffer()
{
	buffer_.reset();
	memory_.reset(); // release memory after bound buffer has been destroyed
}

void StorageBuffer::SetValue(const StorageBufferObject& ubo)
{
	const auto data = memory_->Map(0, sizeof(StorageBufferObject));
	printf("RTV: Setting uniform buffer value at %p using memcpy\n", data);
	std::memcpy(data, &ubo, sizeof(ubo));
	memory_->Unmap();
}

}
