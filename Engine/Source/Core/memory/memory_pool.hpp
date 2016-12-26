#pragma once

#include <vector>
#include <type_traits>
#include <limits>

namespace core
{

	// an memory pool is a specialized allocator that allocates memory in large chunks
	// and deals them out in small slice, with O(1) amortized allocation and deallocation.
	// its useful for a variety of cases:
	// 1. reducing system call overhead when requesting multiple small memory blocks;
	// 2. improving cache efficiency by keeping memory contiguous;
	struct MemoryPool
	{
		MemoryPool(size_t block_size, size_t chunk_size);
		virtual ~MemoryPool();

		// accquire a unused block of memory
		void* malloc();
		// recycle the memory to pool
		void free(void*);
		// destruct all objects and frees all chunks allocated
		void free_all();

		// returns the number of constructed objects
		size_t size() const;
		// returns the capacity of current pool
		size_t capacity() const;

	protected:
		constexpr const static size_t invalid = std::numeric_limits<size_t>::max();

		size_t grow();

		std::vector<uint8_t*> _chunks;

		size_t _available;
		size_t _first_free_block;
		size_t _block_size;
		size_t _chunk_entries_size;
	};

	template<typename T, size_t Growth> struct MemoryPoolT : public MemoryPool
	{
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

		MemoryPoolT() : MemoryPool(sizeof(aligned_storage_t), Growth)
		{}
	};

	inline size_t MemoryPool::size() const
	{
		return capacity() - _available;
	}

	inline size_t MemoryPool::capacity() const
	{
		return _chunks.size() * _chunk_entries_size;
	}

}