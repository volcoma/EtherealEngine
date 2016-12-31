#pragma once

#include "handle_set.hpp"
#include "assert.hpp"
#include <thread>

namespace core
{

	/**
	* @brief      A thread-safe handle based object pool, provides O(1) amortized operations.
	*
	* @tparam     T     The type of object.
	* @tparam     N     The max size of available object.
	*/
	template<typename T, size_t N> struct HandleObjectSet
	{
		using index_t = Handle::index_t;
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		using array_t = std::array<uint8_t, sizeof(aligned_storage_t)*N>;
		using mutex_t = std::mutex;
		using handle_set_t = HandleSet<N>;

		virtual ~HandleObjectSet();

		/**
		* @brief      Create a constructed object, and a associated unique handle.
		*
		* @param[in]  args  Variadic arguments to construct object.
		*
		* @tparam     Args  The type traits of variadic arguments.
		*
		* @return     Returns associated unique handle.
		*/
		template<typename ... Args> Handle create(Args&&... args);

		/**
		* @brief      Fetch object assigned with handle.
		*
		* @param[in]  Handle  The unique handle of object.
		*
		* @return     Returns nullptr_t if no object assigned to this handle.
		*/
		T* fetch(Handle handle);

		/**
		* @brief      Determines if the handle and its interanl object is alive
		*
		* @param[in]  Handle  The unique handle of object.
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(Handle handle) const;

		/**
		* @brief      Recycle the handle, and its internal object.
		*
		* @param[in]  Handle  The unique handle of object.
		*/
		bool free(Handle handle);

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
		using const_iterator_t = HashSetIterator<HandleSet<N>>;

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
		T* fetch_without_check(Handle handle);

		array_t _buffer;
		handle_set_t _handles;
	};

	/**
	* @brief      Dynamic version of HandleObjectSet, the capacity will increase automaticly.
	*
	* @tparam     T     The type of object.
	*/
	template<typename T, size_t N> struct DynamicHandleObjectSet
	{
		using index_t = Handle::index_t;
		using aligned_storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;
		using mutex_t = std::mutex;

		virtual ~DynamicHandleObjectSet();

		/**
		* @brief      Create a constructed object, and a associated unique handle.
		*
		* @param[in]  args  Variadic arguments to construct object.
		*
		* @tparam     Args  The type traits of variadic arguments.
		*
		* @return     Returns associated unique handle.
		*/
		template<typename ... Args> Handle create(Args&&... args);

		/**
		* @brief      Fetch object assigned with handle.
		*
		* @param[in]  Handle  The unique handle of object.
		*
		* @return     Returns nullptr_t if no object assigned to this handle.
		*/
		T* fetch(Handle handle);

		/**
		* @brief      Determines if the handle and its interanl object is alive
		*
		* @param[in]  Handle  The unique handle of object.
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(Handle handle) const;

		/**
		* @brief      Recycle the handle, and its internal object.
		*
		* @param[in]  Handle  The unique handle of object.
		*/
		bool free(Handle handle);

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
		using const_iterator_t = HashSetIterator<DynamicHandleSet>;

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
		T* fetch_without_check(Handle handle);

		std::mutex _malloc_mutex;
		std::vector<uint8_t*> _chunks;
		DynamicHandleSet _handles;
	};

	template<typename T, size_t N>
	HandleObjectSet<T, N>::~HandleObjectSet()
	{
		clear();
	}

	template<typename T, size_t N>
	template<typename ... Args> Handle HandleObjectSet<T, N>::create(Args&&... args)
	{
		if (auto handle = _handles.create())
		{
			if (auto object = fetch(handle))
			{
				::new (object) T(std::forward<Args>(args)...);
				return handle;
			}
		}

		return Handle();
	}

	template<typename T, size_t N>
	inline T* HandleObjectSet<T, N>::fetch(Handle handle)
	{
		return is_alive(handle) ? fetch_without_check(handle) : nullptr;
	}

	template<typename T, size_t N>
	inline T* HandleObjectSet<T, N>::fetch_without_check(Handle handle)
	{
		return (T*)_buffer.data() + sizeof(aligned_storage_t)*handle.get_index();
	}

	template<typename T, size_t N>
	inline bool HandleObjectSet<T, N>::is_alive(Handle handle) const
	{
		return _handles.is_alive(handle);
	}

	template<typename T, size_t N>
	inline bool HandleObjectSet<T, N>::free(Handle handle)
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
	inline void HandleObjectSet<T, N>::clear()
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
	inline typename HandleObjectSet<T, N>::index_t HandleObjectSet<T, N>::size() const
	{
		return _handles.size();
	}

	template<typename T, size_t N>
	inline typename HandleObjectSet<T, N>::const_iterator_t HandleObjectSet<T, N>::begin() const
	{
		return _handles.begin();
	}

	template<typename T, size_t N>
	inline typename HandleObjectSet<T, N>::const_iterator_t HandleObjectSet<T, N>::end() const
	{
		return _handles.end();
	}

	template<typename T, size_t N>
	DynamicHandleObjectSet<T, N>::~DynamicHandleObjectSet()
	{
		clear();
		for (auto chunk : _chunks)
			delete[] chunk;
		_chunks.clear();
	}

	template<typename T, size_t N>
	template<typename ... Args> Handle DynamicHandleObjectSet<T, N>::create(Args&&... args)
	{
		if (auto handle = _handles.create())
		{
			{
				std::unique_lock<std::mutex> lock(_malloc_mutex);
				while (handle.get_index() >= N*_chunks.size())
				{
					auto chunk = new (std::nothrow) uint8_t[sizeof(aligned_storage_t)*N];
					Ensures(chunk != nullptr);
					_chunks.push_back(chunk);
				}
			}

			if (auto object = fetch(handle))
			{
				::new (object) T(std::forward<Args>(args)...);
				return handle;
			}
		}

		return Handle();
	}

	template<typename T, size_t N>
	inline T* DynamicHandleObjectSet<T, N>::fetch(Handle handle)
	{
		return is_alive(handle) ? fetch_without_check(handle) : nullptr;
	}

	template<typename T, size_t N>
	inline T* DynamicHandleObjectSet<T, N>::fetch_without_check(Handle handle)
	{
		auto index = handle.get_index() / N;
		auto offset = (handle.get_index() % N) * sizeof(aligned_storage_t);
		return (T*)(_chunks[index] + offset);
	}

	template<typename T, size_t N>
	inline bool DynamicHandleObjectSet<T, N>::is_alive(Handle handle) const
	{
		return _handles.is_alive(handle);
	}

	template<typename T, size_t N>
	inline bool DynamicHandleObjectSet<T, N>::free(Handle handle)
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
	inline void DynamicHandleObjectSet<T, N>::clear()
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
	inline typename size_t DynamicHandleObjectSet<T, N>::size() const
	{
		return _handles.size();
	}

	template<typename T, size_t N>
	inline typename DynamicHandleObjectSet<T, N>::const_iterator_t DynamicHandleObjectSet<T, N>::begin() const
	{
		return _handles.begin();
	}

	template<typename T, size_t N>
	inline typename DynamicHandleObjectSet<T, N>::const_iterator_t DynamicHandleObjectSet<T, N>::end() const
	{
		return _handles.end();
	}

}