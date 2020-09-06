#pragma once

#include "Device.h"

#include <map>
#include <string>

struct MemoryChunk
{
	uint32_t offset;
	bool in_use;
};

struct VulkanMemoryProperties
{
	uint32_t memory_index;
	uint32_t size;
	bool is_image;
};

struct VulkanMemory
{
	VkDeviceMemory memory;
	VulkanMemoryProperties properties;
	uint32_t offset;
};

struct MemoryBlock
{
	VkDevice device;
	VkDeviceMemory memory;
	std::vector<MemoryChunk> memory_chunks;
	uint32_t chunk_size;
	uint32_t in_use;
	uint32_t num_chunks;
};

struct VulkanMemoryManagerParameters
{
	VulkanDevice device;
};

struct VulkanMemoryManager
{
	VkDevice device;
	VkPhysicalDevice physical_device;
	std::map<std::string, std::vector<MemoryBlock>> memory_blocks;
};

struct MemoryBlockParameters
{
	VkDevice device;
	VulkanMemoryProperties properties;
	uint32_t num_chunks;
};

// Initializes values in VulkanMemoryManager
void create_memory_manager(VulkanMemoryManager &manager, VulkanMemoryManagerParameters &parameters);

// Frees memory in VulkanMemoryManager
void cleanup_memory_manager(VulkanMemoryManager &manager);

// Creates a string id for the set of properties
std::string properties_to_string(VulkanMemoryProperties properties);

// Checks if memory block is full
bool memory_block_is_full(MemoryBlock &block);

// Checks if memory block is empty
bool memory_block_is_empty(MemoryBlock &block);

// Allocates block of memory
void alloc_memory_block(MemoryBlock &block, MemoryBlockParameters &parameters);

// Retrieves the requested chunk of memory from the block
VulkanMemory retrieve_chunk_from_block(MemoryBlock &block);

// Frees a chunk in a memory block
void free_chunk_in_block(MemoryBlock &block, VulkanMemory &memory);

// Frees block of memory
void free_memory_block(MemoryBlock &block);

// Retrieves memory
VulkanMemory get_memory(VulkanMemoryManager &manager, VulkanMemoryProperties &properties);

// Frees memory
void free_memory(VulkanMemoryManager &manager, VulkanMemory &memory);