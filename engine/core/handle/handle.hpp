#pragma once

#include <functional>

namespace core
{

//
// Its a common practice to use a unique id for resource management. This has
// several advantages:
// 1. it would allow for the resource to be destroyed without leaving dangling
// pointers;
// 2. it helps for saving state externally;
// 3. data can be shuffled around in memory as needed.
//

/**
* @brief      A versioned unique identifier.
*/
struct handle
{
	using index_t = uint16_t;
	const static index_t invalid = uint16_t(-1);

	handle() = default;
	handle(const handle&) = default;
	handle(index_t index, index_t version);

	handle& operator=(const handle&) = default;

	index_t get_index() const;
	index_t get_version() const;

	/**
	* @brief      Determines if this handle is valid, both _index and _version not
	* equals invalid.
	*
	* @return     True if valid, False otherwise.
	*/
	bool is_valid() const;

	/**
	* @brief      Boolean conversion operator for fast check the validation of
	* handle.
	*
	* @return     True if valid, False otherwise.
	*/
	operator bool() const;

	/**
	* @brief      Comparisons of equality with other handle
	*
	* @param[in]  rhs   The right hand side handle
	*
	* @return     Returns true if them have same _index and _version value
	*/
	bool operator==(const handle& rhs) const;
	bool operator!=(const handle& rhs) const;

	/**
	* @brief      Comparisons with other handle, its used for sorting containers.
	*
	* @param[in]  rhs   The right hand side handle
	*
	* @return     Returns true we are less than other in strict partially order.
	*/
	bool operator<(const handle& rhs) const;

	/**
	* @brief      Invalidate this handle, reset _index and _version to invalid
	*/
	void invalidate();

protected:
	index_t _index = invalid;
	index_t _version = invalid;
};

///

inline handle::handle(index_t index, index_t version) : _index(index), _version(version)
{
}

inline handle::index_t handle::get_index() const
{
	return _index;
}

inline handle::index_t handle::get_version() const
{
	return _version;
}

inline bool handle::is_valid() const
{
	return _index != invalid && _version != invalid;
}

inline handle::operator bool() const
{
	return is_valid();
}

inline bool handle::operator==(const handle& rhs) const
{
	return _index == rhs._index && _version == rhs._version;
}

inline bool handle::operator!=(const handle& rhs) const
{
	return !(*this == rhs);
}

inline bool handle::operator<(const handle& rhs) const
{
	return _index == rhs._index ? _version < rhs._version : _index < rhs._index;
}

inline void handle::invalidate()
{
	_index = invalid;
	_version = invalid;
}
}

namespace std
{
template <>
struct hash<core::handle>
{
	std::size_t operator()(const core::handle& handle) const
	{
		return (((size_t)handle.get_version()) << 16) & ((size_t)handle.get_index());
	}
};

template <>
struct hash<std::pair<core::handle, core::handle>>
{
	std::size_t operator()(const std::pair<core::handle, core::handle>& value) const
	{
		return std::hash<core::handle>()(value.first) ^ std::hash<core::handle>()(value.second);
	}
};
}
