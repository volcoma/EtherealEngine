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
#if __cplusplus >= 201703L
	static auto get_index(const Key& k)
	{
		const auto & [ idx, gen ] = k;
		return idx;
	}
	static auto get_generation(const Key& k)
	{
		const auto & [ idx, gen ] = k;
		return gen;
	}
	template <class Integral>
	static void set_index(Key& k, Integral value)
	{
		auto & [ idx, gen ] = k;
		idx = value;
	}
	static void increment_generation(Key& k)
	{
		auto & [ idx, gen ] = k;
		++gen;
	}
#else
	static auto get_index(const Key& k)
	{
		using std::get;
		return get<0>(k);
	}
	static auto get_generation(const Key& k)
	{
		using std::get;
		return get<1>(k);
	}
	template <class Integral>
	static void set_index(Key& k, Integral value)
	{
		using std::get;
		get<0>(k) = key_size_type(value);
	}
	static void increment_generation(Key& k)
	{
		using std::get;
		++get<1>(k);
	}
#endif

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

	slot_map() = default;
	slot_map(const slot_map&) = default;
	slot_map(slot_map&&) = default;
	slot_map& operator=(const slot_map&) = default;
	slot_map& operator=(slot_map&&) = default;
	~slot_map() = default;

	// The at() functions have both generation counter checking
	// and bounds checking, and throw if either check fails.
	// O(1) time and space complexity.
	//
	reference at(const key_type& key)
	{
		auto value_iter = this->find(key);
		if(value_iter == this->end())
		{
			SLOT_MAP_THROW_EXCEPTION(std::out_of_range, "at");
		}
		return *value_iter;
	}
	const_reference at(const key_type& key) const
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
	reference operator[](const key_type& key)
	{
		return *find_unchecked(key);
	}
	const_reference operator[](const key_type& key) const
	{
		return *find_unchecked(key);
	}

	// The find() functions have generation counter checking.
	// If the check fails, the result of end() is returned.
	// O(1) time and space complexity.
	//
	iterator find(const key_type& key)
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
	const_iterator find(const key_type& key) const
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
	iterator find_unchecked(const key_type& key)
	{
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}
	const_iterator find_unchecked(const key_type& key) const
	{
		auto slot_iter = std::next(slots_.begin(), get_index(key));
		auto value_iter = std::next(values_.begin(), get_index(*slot_iter));
		return value_iter;
	}

	// All begin() and end() variations have O(1) time and space complexity.
	//
	iterator begin()
	{
		return values_.begin();
	}
	iterator end()
	{
		return values_.end();
	}
	const_iterator begin() const
	{
		return values_.begin();
	}
	const_iterator end() const
	{
		return values_.end();
	}
	const_iterator cbegin() const
	{
		return values_.begin();
	}
	const_iterator cend() const
	{
		return values_.end();
	}
	reverse_iterator rbegin()
	{
		return values_.rbegin();
	}
	reverse_iterator rend()
	{
		return values_.rend();
	}
	const_reverse_iterator rbegin() const
	{
		return values_.rbegin();
	}
	const_reverse_iterator rend() const
	{
		return values_.rend();
	}
	const_reverse_iterator crbegin() const
	{
		return values_.rbegin();
	}
	const_reverse_iterator crend() const
	{
		return values_.rend();
	}

	// Functions for checking the size and capacity of the adapted container
	// have the same complexity as the adapted container.
	// reserve(n) has the complexity of the adapted container, and uses
	// additional time which is linear on the increase in size.
	// This is caused by adding the new slots to the free list.
	//
	bool empty() const
	{
		return values_.size() == 0;
	}
	size_type size() const
	{
		return values_.size();
	}
	// size_type max_size() const; TODO, NO SEMANTICS
	size_type capacity() const
	{
		return values_.capacity();
	}
	void reserve(size_type n)
	{
		values_.reserve(n);
		reserve_slots(n);
	}

	// Functions for accessing and modifying the capacity of the slots container.
	// These are beneficial as allocating more slots than values will cause the
	// generation counter increases to be more evenly distributed across the slots.
	// TODO [ajo]: The above comment is false, at least for this implementation.
	//
	void reserve_slots(size_type n)
	{
		slots_.reserve(n);
		reverse_map_.reserve(n);
	}
	size_type capacity_slots() const
	{
		return slots_.capacity();
	}

	// These operations have O(1) time and space complexity.
	// When size() == capacity() an allocation is required
	// which has O(n) time and space complexity.
	//
	key_type insert(const mapped_type& value)
	{
		return this->emplace(value);
	}
	key_type insert(mapped_type&& value)
	{
		return this->emplace(std::move(value));
	}

	template <typename... Args>
	key_type emplace(Args&&... args)
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
	iterator erase(iterator pos)
	{
		return this->erase(const_iterator(pos));
	}
	iterator erase(iterator first, iterator last)
	{
		return this->erase(const_iterator(first), const_iterator(last));
	}
	iterator erase(const_iterator pos)
	{
		auto slot_iter = this->slot_iter_from_value_iter(pos);
		return erase_slot_iter(slot_iter);
	}
	iterator erase(const_iterator first, const_iterator last)
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
	size_type erase(const key_type& key)
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
	void clear()
	{
		// This resets the generation counters, which "undefined-behavior-izes" at() and find() for the old
		// keys.
		slots_.clear();
		values_.clear();
		reverse_map_.clear();
		next_available_slot_index_ = key_size_type{};
	}

	// swap is not mentioned in P0661r1 but it should be.
	void swap(slot_map& rhs)
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
	Container<mapped_type>& c() & noexcept
	{
		return values_;
	}
	const Container<mapped_type>& c() const & noexcept
	{
		return values_;
	}
	Container<mapped_type>&& c() && noexcept
	{
		return std::move(values_);
	}
	const Container<mapped_type>&& c() const && noexcept
	{
		return std::move(values_);
	}

private:
	slot_iterator slot_iter_from_value_iter(const_iterator value_iter)
	{
		auto value_index = std::distance(const_iterator(values_.begin()), value_iter);
		auto slot_index = *std::next(reverse_map_.begin(), value_index);
		return std::next(slots_.begin(), slot_index);
	}
	iterator erase_slot_iter(slot_iterator slot_iter)
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
            *reverse_map_iter = static_cast<key_size_type>(std::distance(slots_.begin(), slot_back_iter));
		}
		values_.pop_back();
		reverse_map_.pop_back();
		// Expire this key.
		this->set_index(*slot_iter, next_available_slot_index_);
		this->increment_generation(*slot_iter);
		next_available_slot_index_ = static_cast<key_size_type>(slot_index);
		return std::next(values_.begin(), value_index);
	}

	Container<key_type> slots_;			   // high_water_mark() entries
	Container<key_size_type> reverse_map_; // exactly size() entries
	Container<mapped_type> values_;		   // exactly size() entries
	key_size_type next_available_slot_index_{};
};

template <class T, class Key, template <class...> class Container>
void swap(slot_map<T, Key, Container>& lhs, slot_map<T, Key, Container>& rhs)
{
	lhs.swap(rhs);
}

} // namespace nonstd
