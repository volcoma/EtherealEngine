#pragma once

#include <vector>
#include <type_traits>
#include <limits>
#include <cstdint>

namespace core
{

	// an memory pool is a specialized allocator that allocates memory in large chunks
	// and deals them out in small slice, with O(1) amortized allocation and deallocation.
	// its useful for a variety of cases:
	// 1. reducing system call overhead when requesting multiple small memory blocks;
	// 2. improving cache efficiency by keeping memory contiguous;
	struct memory_pool
	{
        memory_pool(std::size_t block_size, std::size_t chunk_size);
		virtual ~memory_pool();

		// accquire a unused block of memory
		void* malloc();
		// recycle the memory to pool
		void free(void*);
		// destruct all objects and frees all chunks allocated
		void free_all();

		// returns the number of constructed objects
        std::size_t size() const;
		// returns the capacity of current pool
        std::size_t capacity() const;

	protected:
        constexpr const static std::size_t invalid = std::numeric_limits<std::size_t>::max();

        std::size_t grow();

		std::vector<uint8_t*> _chunks;

        std::size_t _available;
        std::size_t _first_free_block;
        std::size_t _block_size;
        std::size_t _chunk_entries_size;
	};

    template<typename T, std::size_t Growth>
	struct memory_pool_t : public memory_pool
	{
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

		memory_pool_t() : memory_pool(sizeof(aligned_storage_t), Growth)
		{}
	};

    inline std::size_t memory_pool::size() const
	{
		return capacity() - _available;
	}

    inline std::size_t memory_pool::capacity() const
	{
		return _chunks.size() * _chunk_entries_size;
	}

}
