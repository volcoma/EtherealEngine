#include "handle_set.hpp"
#include "assert.hpp"

namespace core
{

	Handle DynamicHandleSet::create()
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_freeslots.size() > 0)
		{
			index_t index = _freeslots.back();
			_freeslots.pop_back();

			Expects(_versions[index] < Handle::invalid - 1);
// 				"too much versions,"
// 				"please considering change the representation of Handle::index_t.");
			return Handle(index, ++_versions[index]);
		}

		_versions.push_back(1);
		Expects(_versions.size() < Handle::invalid);
// 			"too much handles,"
// 			"please considering change the representation of Handle::index_t.");
		return Handle(static_cast<index_t>(_versions.size() - 1), 1);
	}

	bool DynamicHandleSet::is_alive(Handle handle) const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		const index_t index = handle.get_index();
		const index_t version = handle.get_version();
		return index < _versions.size() && (_versions[index] & 0x1) == 1 && _versions[index] == version;
	}

	bool DynamicHandleSet::free(Handle handle)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		const auto index = handle.get_index();
		const auto version = handle.get_version();

		if (index >= _versions.size() || (_versions[index] & 0x1) != 1 || _versions[index] != version)
			return false;

		_versions[handle.get_index()]++;
		_freeslots.push_back(handle.get_index());
		return true;
	}

	void DynamicHandleSet::clear()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_versions.clear();
		_freeslots.clear();
	}

	DynamicHandleSet::const_iterator_t DynamicHandleSet::begin() const
	{
		std::unique_lock<std::mutex> lock(_mutex);

		if (_versions.size() == 0)
			return end();

		Handle handle = Handle(0, _versions[0]);
		return const_iterator_t(
			*this,
			(_versions[0] & 0x1) == 1 ? handle : find_next_available(handle));
	}

	DynamicHandleSet::const_iterator_t DynamicHandleSet::end() const
	{
		return const_iterator_t(*this, Handle());
	}

	Handle DynamicHandleSet::find_next_available(Handle handle) const
	{
		if (!handle.is_valid())
			return Handle();

		for (index_t i = (handle.get_index() + 1); i < _versions.size(); i++)
		{
			if ((_versions[i] & 0x1) == 1)
				return Handle(i, _versions[i]);
		}

		return Handle();
	}

}