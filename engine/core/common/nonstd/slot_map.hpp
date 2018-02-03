#pragma once

#include <type_traits>
#include <utility>
#include <vector>

#ifndef SLOT_MAP_THROW_EXCEPTION
#include <stdexcept>
#define SLOT_MAP_THROW_EXCEPTION(type, ...) throw type(__VA_ARGS__)
#endif

namespace nonstd
{

template <class T, class Key = std::pair<unsigned, unsigned>,
		  template <class...> class Container = std::vector>
class slot_map
{
	static constexpr auto get_index(const Key& k)
	{
		using std::get;
		return get<0>(k);
	}
	static constexpr auto get_generation(const Key& k)
	{
		using std::get;
		return get<1>(k);
	}
	template <class Integral>
	static constexpr void set_index(Key& k, Integral value)
	{
		using std::get;
		get<0>(k) = value;
	}
	static constexpr void increment_generation(Key& k)
	{
		using std::get;
		++get<1>(k);
	}

	using slot_iterator = typename Container<Key>::iterator;

public:
	using key_type = Key;
	using mapped_type = T;

	using key_size_type = decltype(slot_map::get_index(std::declval<Key>()));
	using key_generation_type = decltype(slot_map::get_generation(std::declval<Key>()));

	using container_type = Container<mapped_type>;
	using reference = typename container_type::reference;
	using const_reference = typename container_type::const_reference;
	using pointer = typename container_type::pointer;
	using const_pointer = typename container_type::const_pointer;
	using iterator = typename container_type::iterator;
	using const_iterator = typename container_type::const_iterator;
	using reverse_iterator = typename container_type::reverse_iterator;
	using const_reverse_iterator = typename container_type::const_reverse_iterator;

	using size_type = typename container_type::size_type;
	using value_type = typename container_type::value_type;

	static_assert(std::is_same<value_type, mapped_type>::value,
				  "Container<T>::value_type must be identical to T");

	constexpr slot_map() = default;
	constexpr slot_map(const slot_map&) = default;
	constexpr slot_map(slot_map&&) = default;
	constexpr slot_map& operator=(const slot_map&) = default;
	constexpr slot_map& operator=(slot_map&&) = default;
	~slot_map() = default;

	// The at() functions have both generation counter checking
	// and bounds checking, and throw if either check fails.
	// O(1) time and space complexity.
	//
	constexpr reference at(const key_type& key)
	{
		auto value_iter = this->find(key);
		if(value_iter == this->end())
		{
			SLOT_MAP_THROW_EXCEPTION(std::out_of_range, "at");
		}
		return *value_iter;
	}
	constexpr const_reference at(const key_type& key) const
	{
		auto value_iter = this->find(key);
		if(value_iter == this->end())
		{
			SLOT_MAP_THROW_EXCEPTION(std::out_of_range, "at");
		}
		return *value_iter;
	}

	// The bracket operator[] has a generation counter check.
	// If the check fails it is undefined behavior.
	// O(1) time and space complexity.
	//
	constexpr reference operator[](const key_type& key)
	{
		return *find_unchecked(key);
	}
	constexpr const_reference operator[](const key_type& key) const
	{
		return *find_unchecked(key);
	}

	// The find() functions have generation counter checking.
	// If the check fails, the result of end() is returned.
	// O(1) time and space complexity.
	//
	constexpr iterator find(const key_type& key)
	{
		auto slot_index = get_index(key);
		if(slot_index >= slots_.size())
		{
			return end();
		}
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		if(get_generation(*slot_iter) != get_generation(key))
		{
			return end();
		}
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}
	constexpr const_iterator find(const key_type& key) const
	{
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		if(get_generation(*slot_iter) != get_generation(key))
		{
			return end();
		}
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}

	// The find_unchecked() functions perform no checks of any kind.
	// O(1) time and space complexity.
	//
	constexpr iterator find_unchecked(const key_type& key)
	{
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}
	constexpr const_iterator find_unchecked(const key_type& key) const
	{
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}

	// All begin() and end() variations have O(1) time and space complexity.
	//
	constexpr iterator begin()
	{
		return values_.begin();
	}
	constexpr iterator end()
	{
		return values_.end();
	}
	constexpr const_iterator begin() const
	{
		return values_.begin();
	}
	constexpr const_iterator end() const
	{
		return values_.end();
	}
	constexpr const_iterator cbegin() const
	{
		return values_.begin();
	}
	constexpr const_iterator cend() const
	{
		return values_.end();
	}
	constexpr reverse_iterator rbegin()
	{
		return values_.rbegin();
	}
	constexpr reverse_iterator rend()
	{
		return values_.rend();
	}
	constexpr const_reverse_iterator rbegin() const
	{
		return values_.rbegin();
	}
	constexpr const_reverse_iterator rend() const
	{
		return values_.rend();
	}
	constexpr const_reverse_iterator crbegin() const
	{
		return values_.rbegin();
	}
	constexpr const_reverse_iterator crend() const
	{
		return values_.rend();
	}

	// Functions for checking the size and capacity of the adapted container
	// have the same complexity as the adapted container.
	// reserve(n) has the complexity of the adapted container, and uses
	// additional time which is linear on the increase in size.
	// This is caused by adding the new slots to the free list.
	//
	constexpr bool empty() const
	{
		return values_.size() == 0;
	}
	constexpr size_type size() const
	{
		return values_.size();
	}
	// constexpr size_type max_size() const; TODO, NO SEMANTICS
	constexpr size_type capacity() const
	{
		return values_.capacity();
	}
	constexpr void reserve(size_type n)
	{
		values_.reserve(n);
		reserve_slots(n);
	}

	// Functions for accessing and modifying the capacity of the slots container.
	// These are beneficial as allocating more slots than values will cause the
	// generation counter increases to be more evenly distributed across the slots.
	// TODO [ajo]: The above comment is false, at least for this implementation.
	//
	constexpr void reserve_slots(size_type n)
	{
		slots_.reserve(n);
		reverse_map_.reserve(n);
	}
	constexpr size_type capacity_slots() const
	{
		return slots_.capacity();
	}

	// These operations have O(1) time and space complexity.
	// When size() == capacity() an allocation is required
	// which has O(n) time and space complexity.
	//
	constexpr key_type insert(const mapped_type& value)
	{
		return this->emplace(value);
	}
	constexpr key_type insert(mapped_type&& value)
	{
		return this->emplace(std::move(value));
	}

	template <typename... Args>
	constexpr key_type emplace(Args&&... args)
	{
		auto value_pos = values_.size();
		values_.emplace_back(std::forward<Args>(args)...);
		reverse_map_.emplace_back(next_available_slot_index_);
		if(next_available_slot_index_ == slots_.size())
		{
			auto idx = next_available_slot_index_;
			++idx;
			slots_.emplace_back(key_type{idx, key_generation_type{}}); // make a new slot
		}
		auto slot_iter = std::next(slots_.begin(), next_available_slot_index_);
		next_available_slot_index_ = this->get_index(*slot_iter);
		this->set_index(*slot_iter, value_pos);
		this->increment_generation(*slot_iter);
		key_type result = *slot_iter;
		this->set_index(result, std::distance(slots_.begin(), slot_iter));
		return result;
	}

	// Each erase() version has an O(1) time complexity per value
	// and O(1) space complexity.
	//
	constexpr iterator erase(iterator pos)
	{
		return this->erase(const_iterator(pos));
	}
	constexpr iterator erase(iterator first, iterator last)
	{
		return this->erase(const_iterator(first), const_iterator(last));
	}
	constexpr iterator erase(const_iterator pos)
	{
		auto slot_iter = this->slot_iter_from_value_iter(pos);
		return erase_slot_iter(slot_iter);
	}
	constexpr iterator erase(const_iterator first, const_iterator last)
	{
		// Must use indexes, not iterators, because Container iterators might be invalidated by pop_back
		auto first_index = std::distance(this->cbegin(), first);
		auto last_index = std::distance(this->cbegin(), last);
		while(last_index != first_index)
		{
			--last_index;
			auto iter = std::next(this->cbegin(), last_index);
			this->erase(iter);
		}
		return std::next(this->begin(), first_index);
	}
	constexpr size_type erase(const key_type& key)
	{
		auto iter = this->find(key);
		if(iter == this->end())
		{
			return 0;
		}
		this->erase(iter);
		return 1;
	}

	// clear() has O(n) time complexity and O(1) space complexity.
	// It also has semantics differing from erase(begin(), end())
	// in that it also resets the generation counter of every slot
	// and rebuilds the free list.
	//
	constexpr void clear()
	{
		// This resets the generation counters, which "undefined-behavior-izes" at() and find() for the old
		// keys.
		slots_.clear();
		values_.clear();
		reverse_map_.clear();
		next_available_slot_index_ = key_size_type{};
	}

	// swap is not mentioned in P0661r1 but it should be.
	constexpr void swap(slot_map& rhs)
	{
		using std::swap;
		swap(slots_, rhs.slots_);
		swap(values_, rhs.values_);
		swap(reverse_map_, rhs.reverse_map_);
		swap(next_available_slot_index_, rhs.next_available_slot_index_);
	}

protected:
	// These accessors are not part of P0661R2 but are "modernized" versions
	// of the protected interface of std::priority_queue, std::stack, etc.
	constexpr Container<mapped_type>& c() & noexcept
	{
		return values_;
	}
	constexpr const Container<mapped_type>& c() const & noexcept
	{
		return values_;
	}
	constexpr Container<mapped_type>&& c() && noexcept
	{
		return std::move(values_);
	}
	constexpr const Container<mapped_type>&& c() const && noexcept
	{
		return std::move(values_);
	}

private:
	constexpr slot_iterator slot_iter_from_value_iter(const_iterator value_iter)
	{
		auto value_index = std::distance(const_iterator(values_.begin()), value_iter);
		auto slot_index = *std::next(reverse_map_.begin(), value_index);
		return std::next(slots_.begin(), slot_index);
	}
	constexpr iterator erase_slot_iter(slot_iterator slot_iter)
	{
		auto slot_index = std::distance(slots_.begin(), slot_iter);
		auto value_index = get_index(*slot_iter);
		auto value_iter = std::next(values_.begin(), value_index);
		auto value_back_iter = std::prev(values_.end());
		if(value_iter != value_back_iter)
		{
			auto slot_back_iter = slot_iter_from_value_iter(value_back_iter);
			*value_iter = std::move(*value_back_iter);
			this->set_index(*slot_back_iter, value_index);
			auto reverse_map_iter = std::next(reverse_map_.begin(), value_index);
			*reverse_map_iter = std::distance(slots_.begin(), slot_back_iter);
		}
		values_.pop_back();
		reverse_map_.pop_back();
		// Expire this key.
		this->set_index(*slot_iter, next_available_slot_index_);
		this->increment_generation(*slot_iter);
		next_available_slot_index_ = slot_index;
		return std::next(values_.begin(), value_index);
	}

	Container<key_type> slots_;			   // high_water_mark() entries
	Container<key_size_type> reverse_map_; // exactly size() entries
	Container<mapped_type> values_;		   // exactly size() entries
	key_size_type next_available_slot_index_{};
};

template <class T, class Key, template <class...> class Container>
constexpr void swap(slot_map<T, Key, Container>& lhs, slot_map<T, Key, Container>& rhs)
{
	lhs.swap(rhs);
}

} // namespace nonstd
