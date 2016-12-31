#pragma once

#include "handle.hpp"

#include <array>
#include <vector>
#include <mutex>

namespace core
{

	template<typename T>
	struct HashSetIterator : public std::iterator<std::forward_iterator_tag, Handle>
	{
		HashSetIterator(const T& handles, Handle position);

		HashSetIterator operator ++ (int dummy);
		HashSetIterator& operator ++ ();

		bool operator == (const HashSetIterator& rhs) const;
		bool operator != (const HashSetIterator& rhs) const;

		Handle operator* () const;

	protected:
		const T& _handles;
		Handle _position;
	};

	/**
	* @brief      A thread-safe handle creation and recycle manager.
	*
	* @tparam     S     The max size of available handles, should be less than Handle::invalid.
	*/
	template<size_t N> struct HandleSet
	{
		static_assert(N < Handle::invalid,
			"The max size of handle set should be less than Handle::invalid.");

		using mutex_t = std::mutex;
		using index_t = Handle::index_t;
		using array_t = std::array<index_t, N>;

	public:
		HandleSet();

		/**
		* @brief      Create a unique handle and mark it as alive internally.
		*
		* @return     Returns alive handle if create successfully, invalid otherwise.
		*/
		Handle create();

		/**
		* @brief      Determines if alive.
		*
		* @param[in]  handle  The handle
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(Handle handle) const;


		/**
		* @brief      Recycle this handle index, and mark it's version as dead.
		*
		* @param[in]  handle  The handle
		*
		* @return     True if freed, False otherwise
		*/
		bool free(Handle handle);

		/**
		* @brief      Reset this handle pool to initial state.
		*/
		void clear();

		/**
		* @brief      Size of alive handles.
		*
		* @return     Returns size of alive handles.
		*/
		size_t size() const;


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
		friend struct HashSetIterator<HandleSet<N>>;
		Handle find_next_available(Handle handle) const;

		mutable mutex_t _mutex;
		size_t _available;
		array_t _versions;
		array_t _freeslots;
	};

	/**
	* @brief      Dynamic version of HandleSet, the capacity will increase automaticly.
	*
	*/
	struct DynamicHandleSet
	{
		using mutex_t = std::mutex;
		using index_t = Handle::index_t;

	public:
		/**
		* @brief      Create a unique handle and mark it as alive internally.
		*
		* @return     Returns alive handle if create successfully, invalid otherwise.
		*/
		Handle create();

		/**
		* @brief      Determines if alive.
		*
		* @param[in]  handle  The handle
		*
		* @return     True if alive, False otherwise.
		*/
		bool is_alive(Handle handle) const;


		/**
		* @brief      Recycle this handle index, and mark it's version as dead.
		*
		* @param[in]  handle  The handle
		*
		* @return     True if freed, False otherwise
		*/
		bool free(Handle handle);

		/**
		* @brief      Reset this handle pool to initial state.
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
		friend struct HashSetIterator<DynamicHandleSet>;
		Handle find_next_available(Handle handle) const;

		mutable mutex_t _mutex;
		std::vector<index_t> _versions;
		std::vector<index_t> _freeslots;
	};

	//
	template<size_t N> inline HandleSet<N>::HandleSet()
	{
		clear();
	}

	template<size_t N> inline Handle HandleSet<N>::create()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_available > 0)
		{
			index_t index = _freeslots[--_available];
			ASSERT(_versions[index] < Handle::invalid - 1,
				"too much versions,"
				"please considering change the representation of Handle::index_t.");
			return Handle(index, ++_versions[index]);
		}

		return Handle();
	}

	template<size_t N> inline bool HandleSet<N>::is_alive(Handle handle) const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		auto index = handle.get_index();
		auto version = handle.get_version();

		return index < N && (_versions[index] & 0x1) == 1 && _versions[index] == version;
	}

	template<size_t N> inline bool HandleSet<N>::free(Handle handle)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		const auto index = handle.get_index();
		const auto version = handle.get_version();

		if (index >= N || (_versions[index] & 0x1) != 1 || _versions[index] != version)
			return false;

		_versions[handle.get_index()]++;
		_freeslots[_available++] = handle.get_index();
		return true;
	}

	template<size_t N> inline void HandleSet<N>::clear()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		memset(_versions.data(), 0, sizeof(index_t)*N);

		for (index_t i = 0; i < N; i++)
			_freeslots[i] = N - i - 1;

		_available = N;
	}

	template<size_t N> inline typename size_t HandleSet<N>::size() const
	{
		return N - _available;
	}

	template<size_t N> inline typename HandleSet<N>::const_iterator_t HandleSet<N>::begin() const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_versions.size() == 0)
			return end();

		Handle handle = Handle(0, _versions[0]);
		return const_iterator_t(
			*this,
			(_versions[0] & 0x1) == 1 ? handle : find_next_available(handle));
	}

	template<size_t N> inline typename HandleSet<N>::const_iterator_t HandleSet<N>::end() const
	{
		return const_iterator_t(*this, Handle());
	}

	template<size_t N> inline Handle HandleSet<N>::find_next_available(Handle handle) const
	{
		for (index_t i = (handle.get_index() + 1); i < static_cast<index_t>(N - _available); ++i)
		{
			if ((_versions[i] & 0x1) == 1)
				return Handle(i, _versions[i]);
		}
		return Handle();
	}

	inline size_t DynamicHandleSet::size() const
	{
		return _versions.size() - _freeslots.size();
	}

	template<typename T> inline HashSetIterator<T>::HashSetIterator(const T& handles, Handle position)
		: _handles(handles), _position(position)
	{}

	template<typename T> inline HashSetIterator<T> HashSetIterator<T>::operator ++ (int dummy)
	{
		auto tmp = *this;
		++(*this);
		return tmp;
	}

	template<typename T> inline HashSetIterator<T>& HashSetIterator<T>::operator ++ ()
	{
		std::unique_lock<std::mutex> lock(_handles._mutex);
		_position = _handles.find_next_available(_position);
		return *this;
	}

	template<typename T> inline bool HashSetIterator<T>::operator == (const HashSetIterator& rhs) const
	{
		return &_handles == &rhs._handles && _position == rhs._position;
	}

	template<typename T> inline bool HashSetIterator<T>::operator != (const HashSetIterator& rhs) const
	{
		return !(*this == rhs);
	}

	template<typename T> inline Handle HashSetIterator<T>::operator* () const
	{
		return _position;
	}

}