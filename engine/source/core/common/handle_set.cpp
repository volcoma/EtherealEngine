#include "handle_set.hpp"
#include "assert.hpp"

namespace core
{

	handle dynamic_handle_set::create()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_freeslots.size() > 0)
		{
			index_t index = _freeslots.back();
			_freeslots.pop_back();

			Expects(_versions[index] < handle::invalid - 1 &&
 				"too much versions,"
 				"please considering change the representation of Handle::index_t.");
			return handle(index, ++_versions[index]);
		}

		_versions.push_back(1);
		Expects(_versions.size() < handle::invalid &&
 			"too much handles,"
 			"please considering change the representation of Handle::index_t.");
		return handle(static_cast<index_t>(_versions.size() - 1), 1);
	}

	bool dynamic_handle_set::is_alive(handle hndl) const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		const index_t index = hndl.get_index();
		const index_t version = hndl.get_version();
		return index < _versions.size() && (_versions[index] & 0x1) == 1 && _versions[index] == version;
	}

	bool dynamic_handle_set::free(handle hndl)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		const auto index = hndl.get_index();
		const auto version = hndl.get_version();

		if (index >= _versions.size() || (_versions[index] & 0x1) != 1 || _versions[index] != version)
			return false;

		_versions[hndl.get_index()]++;
		_freeslots.push_back(hndl.get_index());
		return true;
	}

	void dynamic_handle_set::clear()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_versions.clear();
		_freeslots.clear();
	}

	dynamic_handle_set::const_iterator_t dynamic_handle_set::begin() const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_versions.size() == 0)
			return end();

		handle hndl = handle(0, _versions[0]);
		return const_iterator_t(
			*this,
			(_versions[0] & 0x1) == 1 ? hndl : find_next_available(hndl));
	}

	dynamic_handle_set::const_iterator_t dynamic_handle_set::end() const
	{
		return const_iterator_t(*this, handle());
	}

	handle dynamic_handle_set::find_next_available(handle hndl) const
	{
		if (!hndl.is_valid())
			return handle();

		for (index_t i = (hndl.get_index() + 1); i < _versions.size(); i++)
		{
			if ((_versions[i] & 0x1) == 1)
				return handle(i, _versions[i]);
		}

		return handle();
	}

}