#include "memory_pool.hpp"
#include <cstdlib>
#include <cstring>

namespace core
{

memory_pool::memory_pool(std::size_t block_size, std::size_t chunk_size)
{
	_first_free_block = invalid;
	_available = 0;
	_block_size = block_size < sizeof(std::size_t) ? sizeof(std::size_t) : block_size;
	_chunk_entries_size = chunk_size;
}

memory_pool::~memory_pool()
{
	free_all();
}

void* memory_pool::malloc()
{
	if(_first_free_block == invalid)
	{
		// trying to grow this pool if we are out of free block
		_first_free_block = grow();
		if(_first_free_block == invalid)
		{
			// LOGW("failed to allocate memory[%d byte(s)] from system to grow pool",
			//	_block_size * _chunk_entries_size);
			return nullptr;
		}
	}

	auto block = _chunks[_first_free_block / _chunk_entries_size] +
				 (_first_free_block % _chunk_entries_size) * _block_size;

	_first_free_block = *(std::size_t*)block;
	_available--;

	return static_cast<void*>(block);
}

void memory_pool::free(void* block)
{
	// find block index of the element
	std::size_t index = invalid;
	std::size_t offset = _chunk_entries_size * _block_size;

	for(std::size_t i = 0; i < _chunks.size(); ++i)
	{
		if((std::size_t)block >= (std::size_t)_chunks[i] &&
		   (std::size_t)block < (std::size_t)_chunks[i] + offset)
		{
			index = i * _chunk_entries_size + ((std::size_t)block - (std::size_t)_chunks[i]) / _block_size;
			break;
		}
	}

	if(index == invalid)
	{
		// LOGW("try to free block which does NOT belongs to this memory pool.");
		return;
	}

	std::memset(block, 0xCC, _block_size);

	// recycle this memory block, add it to the first of free list
	*(std::size_t*)block = _first_free_block;
	_first_free_block = index;
	_available++;
}

void memory_pool::free_all()
{
	// returns allocated chunk to system
	for(auto chunk : _chunks)
		::free(chunk);

	_chunks.clear();
	_available = 0;
	_first_free_block = invalid;
}

std::size_t memory_pool::grow()
{
	auto chunk = static_cast<uint8_t*>(::malloc(_chunk_entries_size * _block_size));
	memset(chunk, 0xCC, _chunk_entries_size * _block_size);

	if(chunk == nullptr)
		return invalid;

	auto iterator = chunk;
	auto offset = _chunk_entries_size * _chunks.size();
	for(std::size_t i = 1; i < _chunk_entries_size; ++i, iterator += _block_size)
		*(std::size_t*)iterator = offset + i;
	*(std::size_t*)iterator = invalid;

	_available += _chunk_entries_size;
	_chunks.push_back(chunk);
	return offset;
}
}
