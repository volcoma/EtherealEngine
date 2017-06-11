#pragma once

#include "handle_set.hpp"
#include "../common/assert.hpp"
#include <thread>

namespace core
{

	/**
	* @brief      A thread-safe handle based object pool, provides O(1) amortized operations.
	*
	* @tparam     T     The type of object.
	* @tparam     N     The max size of available object.
	*/
	template<typename T, size_t N> 
	struct handle_object_set
	{
		using index_t = handle::index_t;
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		using array_t = std::array<uint8_t, sizeof(aligned_storage_t)*N>;
		using mutex_t = std::mutex;
	
		virtual ~handle_object_set();

		/**
		* @brief      Create a constructed object, and a associated unique handle.
		*
		* @param[in]  args  Variadic arguments to construct object.
		*
		* @tparam     Args  The type traits of variadic arguments.
		*
		* @return     Returns associated unique handle.
		*/
		template<typename ... Args> handle create(Args&&... args);

		/**
		* @brief      Fetch object assigned with handle.
		*
		* @param[in]  handle  The unique handle of object.
		*
		* @return     Returns nullptr_t if no object assigned to this handle.
		*/
		T* fetch(handle handle);

		/**
		* @brief      Determines if the handle and its interanl object is alive
		*
		* @param[in]  handle  The unique handle of object.
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(handle handle) const;

		/**
		* @brief      Recycle the handle, and its internal object.
		*
		* @param[in]  handle  The unique handle of object.
		*/
		bool free(handle handle);

		/**
		* @brief      Reset this object pool to initial state, and destroy all the objects.
		*             Not thread-safe.
		*/
		void clear();

		/**
		* @brief      Size of alive handles.
		*
		* @return     Returns size of alive handles.
		*/
		index_t size() const;

	public:
		using const_iterator_t = hash_set_iterator<handle_set<N>>;

		/**
		* @brief      Create an constant iterator referring to the first alive handle.
		*
		* @return     Returns created iterator.
		*/
		const_iterator_t begin() const;

		/**
		* @brief      Create an constant iterator referring to the past-the-end handle.
		*
		* @return     Returns created iterator.
		*/
		const_iterator_t end() const;

	protected:
		T* fetch_without_check(handle handle);

		array_t _buffer;
		handle_set<N> _handles;
	};

	/**
	* @brief      Dynamic version of handle_object_set, the capacity will increase automaticly.
	*
	* @tparam     T     The type of object.
	*/
	template<typename T, size_t N> 
	struct dynamic_handle_object_set_t
	{
		using index_t = handle::index_t;
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		using mutex_t = std::mutex;

		virtual ~dynamic_handle_object_set_t();

		/**
		* @brief      Create a constructed object, and a associated unique handle.
		*
		* @param[in]  args  Variadic arguments to construct object.
		*
		* @tparam     Args  The type traits of variadic arguments.
		*
		* @return     Returns associated unique handle.
		*/
		template<typename ... Args> handle create(Args&&... args);

		/**
		* @brief      Fetch object assigned with handle.
		*
		* @param[in]  handle  The unique handle of object.
		*
		* @return     Returns nullptr_t if no object assigned to this handle.
		*/
		T* fetch(handle handle);

		/**
		* @brief      Determines if the handle and its interanl object is alive
		*
		* @param[in]  handle  The unique handle of object.
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(handle handle) const;

		/**
		* @brief      Recycle the handle, and its internal object.
		*
		* @param[in]  handle  The unique handle of object.
		*/
		bool free(handle handle);

		/**
		* @brief      Reset this object pool to initial state, and destroy all the objects,
		*             Not thread-safe.
		*/
		void clear();

		/**
		* @brief      Size of alive handles.
		*
		* @return     Returns size of alive handles.
		*/
		size_t size() const;

	public:
		using const_iterator_t = hash_set_iterator<dynamic_handle_set>;

		/**
		* @brief      Create an constant iterator referring to the first alive handle.
		*
		* @return     Returns created iterator.
		*/
		const_iterator_t begin() const;

		/**
		* @brief      Create an constant iterator referring to the past-the-end handle.
		*
		* @return     Returns created iterator.
		*/
		const_iterator_t end() const;

	protected:
		T* fetch_without_check(handle handle);

		std::mutex _malloc_mutex;
		std::vector<uint8_t*> _chunks;
		dynamic_handle_set _handles;
	};

	template<typename T, size_t N>
	handle_object_set<T, N>::~handle_object_set()
	{
		clear();
	}

	template<typename T, size_t N>
	template<typename ... Args> handle handle_object_set<T, N>::create(Args&&... args)
	{
		if (auto handle = _handles.create())
		{
			if (auto object = fetch(handle))
			{
				::new (object) T(std::forward<Args>(args)...);
				return handle;
			}
		}

		return handle();
	}

	template<typename T, size_t N>
	inline T* handle_object_set<T, N>::fetch(handle handle)
	{
		return is_alive(handle) ? fetch_without_check(handle) : nullptr;
	}

	template<typename T, size_t N>
	inline T* handle_object_set<T, N>::fetch_without_check(handle handle)
	{
		return (T*)_buffer.data() + sizeof(aligned_storage_t)*handle.get_index();
	}

	template<typename T, size_t N>
	inline bool handle_object_set<T, N>::is_alive(handle handle) const
	{
		return _handles.is_alive(handle);
	}

	template<typename T, size_t N>
	inline bool handle_object_set<T, N>::free(handle handle)
	{
		if (_handles.free(handle))
		{
			auto object = fetch_without_check(handle);
			object->~T();
			return true;
		}

		return false;
	}

	template<typename T, size_t N>
	inline void handle_object_set<T, N>::clear()
	{
		for (auto handle : _handles)
		{
			if (_handles.free(handle))
			{
				auto object = fetch_without_check(handle);
				object->~T();
			}
		}
	}

	template<typename T, size_t N>
	inline typename handle_object_set<T, N>::index_t handle_object_set<T, N>::size() const
	{
		return _handles.size();
	}

	template<typename T, size_t N>
	inline typename handle_object_set<T, N>::const_iterator_t handle_object_set<T, N>::begin() const
	{
		return _handles.begin();
	}

	template<typename T, size_t N>
	inline typename handle_object_set<T, N>::const_iterator_t handle_object_set<T, N>::end() const
	{
		return _handles.end();
	}

	template<typename T, size_t N>
	dynamic_handle_object_set_t<T, N>::~dynamic_handle_object_set_t()
	{
		clear();
		for (auto chunk : _chunks)
			delete[] chunk;
		_chunks.clear();
	}

	template<typename T, size_t N>
	template<typename ... Args> handle dynamic_handle_object_set_t<T, N>::create(Args&&... args)
	{
		if (auto handle = _handles.create())
		{
			{
				std::unique_lock<std::mutex> lock(_malloc_mutex);
				while (handle.get_index() >= N*_chunks.size())
				{
					auto chunk = new (std::nothrow) uint8_t[sizeof(aligned_storage_t)*N];
					ensures(chunk != nullptr);
					_chunks.push_back(chunk);
				}
			}

			if (auto object = fetch(handle))
			{
				::new (object) T(std::forward<Args>(args)...);
				return handle;
			}
		}

		return handle();
	}

	template<typename T, size_t N>
	inline T* dynamic_handle_object_set_t<T, N>::fetch(handle handle)
	{
		return is_alive(handle) ? fetch_without_check(handle) : nullptr;
	}

	template<typename T, size_t N>
	inline T* dynamic_handle_object_set_t<T, N>::fetch_without_check(handle handle)
	{
		auto index = handle.get_index() / N;
		auto offset = (handle.get_index() % N) * sizeof(aligned_storage_t);
		return (T*)(_chunks[index] + offset);
	}

	template<typename T, size_t N>
	inline bool dynamic_handle_object_set_t<T, N>::is_alive(handle handle) const
	{
		return _handles.is_alive(handle);
	}

	template<typename T, size_t N>
	inline bool dynamic_handle_object_set_t<T, N>::free(handle handle)
	{
		if (_handles.free(handle))
		{
			auto object = fetch_without_check(handle);
			object->~T();
			return true;
		}
		return false;
	}

	template<typename T, size_t N>
	inline void dynamic_handle_object_set_t<T, N>::clear()
	{
		for (auto handle : _handles)
		{
			if (_handles.free(handle))
			{
				auto object = fetch_without_check(handle);
				object->~T();
			}
		}
	}

	template<typename T, size_t N>
    inline typename std::size_t dynamic_handle_object_set_t<T, N>::size() const
	{
		return _handles.size();
	}

	template<typename T, size_t N>
	inline typename dynamic_handle_object_set_t<T, N>::const_iterator_t dynamic_handle_object_set_t<T, N>::begin() const
	{
		return _handles.begin();
	}

	template<typename T, size_t N>
	inline typename dynamic_handle_object_set_t<T, N>::const_iterator_t dynamic_handle_object_set_t<T, N>::end() const
	{
		return _handles.end();
	}

}
