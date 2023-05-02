// hpp-small-vector v2.02
//
// std::vector-like class with a static buffer for initial capacity
// Example:
//
// hpp::small_vector<int, 4, 5> myvec; // a small_vector of size 0, initial capacity 4, and revert size 4 (below 5)
// myvec.resize(2); // vector is {0,0} in static buffer
// myvec[1] = 11; // vector is {0,11} in static buffer
// myvec.push_back(7); // vector is {0,11,7}  in static buffer
// myvec.insert(myvec.begin() + 1, 3); // vector is {0,3,11,7} in static buffer
// myvec.push_back(5); // vector is {0,3,11,7,5} in dynamically allocated memory buffer
// myvec.erase(myvec.begin());  // vector is {3,11,7,5} back in static buffer
// myvec.resize(5); // vector is {3,11,7,5,0} back in dynamically allocated memory
//
//
// Reference:
//
// hpp::small_vector is fully compatible with std::vector with
// the following exceptions:
// * when reducing the size with erase or resize the new size may fall below
//   RevertToStaticBelow (if it is not 0). In such a case the vector will
//   revert to using its static buffer, invalidating all iterators (contrary
//   to the standard)
// * a method is added `revert_to_static()` which reverts to the static buffer
//   if possible and does nothing if the size doesn't allow it
//
// Other notes:
//
// * the default value for RevertToStaticBelow is zero. This means that once a dynamic
//   buffer is allocated the data will never be put into the static one, even if the
//   size allows it. Even if clear() is called. The only way to do so is to call
//   shrink_to_fit() or revert_to_static()
// * shrink_to_fit will free and reallocate if size != capacity and the data
//   doesn't fit into the static buffer. It also will revert to the static buffer
//   whenever possible regardless of the RevertToStaticBelow value
//
//
//                  Configuration
//
// The library has two configuration options. They can be set as #define-s
// before including the header file, but it is recommended to change the code
// of the library itself with the values you want, especially if you include
// the library in many compilation units (as opposed to, say, a precompiled
// header or a central header).
//
//                  Config out of range error handling
//
// An out of range error is a runtime error which is triggered when a method is
// called with an iterator that doesn't belong to the vector's current range.
// For example: vec.erase(vec.end() + 1);
//
// This is set by defining HPP_SMALL_VECTOR_ERROR_HANDLING to one of the
// following values:
// * HPP_SMALL_VECTOR_ERROR_HANDLING_NONE - no error handling. Crashes WILL
//      ensue if the error is triggered.
// * HPP_SMALL_VECTOR_ERROR_HANDLING_THROW - std::out_of_range is thrown.
// * HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT - asserions are triggered.
// * HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT_AND_THROW - combines assert and
//      throw to catch errors more easily in debug mode
//
// To set this setting by editing the file change the line:
// ```
// #   define HPP_SMALL_VECTOR_ERROR_HANDLING HPP_SMALL_VECTOR_ERROR_HANDLING_THROW
// ```
// to the default setting of your choice
//
//                  Config bounds checks:
//
// By default bounds checks are made in debug mode (via an asser) when accessing
// elements (with `at` or `[]`). Iterators are not checked (yet...)
//
// To disable them, you can define HPP_SMALL_VECTOR_NO_DEBUG_BOUNDS_CHECK
// before including the header.
//
//
#pragma once

#include <type_traits>
#include <cstddef>
#include <memory>

#define HPP_SMALL_VECTOR_ERROR_HANDLING_NONE  0
#define HPP_SMALL_VECTOR_ERROR_HANDLING_THROW 1
#define HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT 2
#define HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT_AND_THROW 3

#if !defined(HPP_SMALL_VECTOR_ERROR_HANDLING)
#   define HPP_SMALL_VECTOR_ERROR_HANDLING HPP_SMALL_VECTOR_ERROR_HANDLING_THROW
#endif


#if HPP_SMALL_VECTOR_ERROR_HANDLING == HPP_SMALL_VECTOR_ERROR_HANDLING_NONE
#   define I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(cond)
#elif HPP_SMALL_VECTOR_ERROR_HANDLING == HPP_SMALL_VECTOR_ERROR_HANDLING_THROW
#   include <stdexcept>
#   define I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(cond) if (cond) throw std::out_of_range("hpp::small_vector out of range")
#elif HPP_SMALL_VECTOR_ERROR_HANDLING == HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT
#   include <cassert>
#   define I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(cond) assert(!(cond) && "hpp::small_vector out of range")
#elif HPP_SMALL_VECTOR_ERROR_HANDLING == HPP_SMALL_VECTOR_ERROR_HANDLING_ASSERT_AND_THROW
#   include <stdexcept>
#   include <cassert>
#   define I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(cond) \
    do { if (cond) { assert(false && "hpp::small_vector out of range"); throw std::out_of_range("hpp::small_vector out of range"); } } while(false)
#else
#error "Unknown HPP_SMALL_VECTOR_ERRROR_HANDLING"
#endif


#if defined(HPP_SMALL_VECTOR_NO_DEBUG_BOUNDS_CHECK)
#   define I_HPP_SMALL_VECTOR_BOUNDS_CHECK(i)
#else
#   include <cassert>
#   define I_HPP_SMALL_VECTOR_BOUNDS_CHECK(i) assert((i) < this->size())
#endif

namespace hpp
{

template<typename T, size_t StaticCapacity = 16, size_t RevertToStaticBelow = 0, class Alloc = std::allocator<T>>
struct small_vector : private Alloc
{
    static_assert(RevertToStaticBelow <= StaticCapacity + 1, "hpp::small_vector: the RevertToStaticBelow shouldn't exceed the static capacity by more than one");

    using atraits = std::allocator_traits<Alloc>;
public:
    using allocator_type = Alloc;
    using value_type = typename atraits::value_type;
    using size_type = typename atraits::size_type;
    using difference_type = typename atraits::difference_type;
    using reference = T&;
    using const_reference = const T&;
    using pointer = typename atraits::pointer;
    using const_pointer = typename atraits::const_pointer;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr size_t static_capacity = StaticCapacity;
    static constexpr intptr_t revert_to_static_below = RevertToStaticBelow;

    small_vector()
        : small_vector(Alloc())
    {}

    small_vector(const Alloc& alloc)
        : Alloc(alloc)
        , capacity_(StaticCapacity)
    {
        begin_ = end_ = static_begin_ptr();
    }

    explicit small_vector(size_t count, const Alloc& alloc = Alloc())
        : small_vector(alloc)
    {
        resize(count);
    }

    explicit small_vector(size_t count, const T& value, const Alloc& alloc = Alloc())
        : small_vector(alloc)
    {
        assign_fill(count, value);
    }

    template <class InputIterator, typename = decltype(*std::declval<InputIterator>())>
    small_vector(InputIterator first, InputIterator last, const Alloc& alloc = Alloc())
        : small_vector(alloc)
    {
        assign_copy(first, last);
    }

    small_vector(std::initializer_list<T> l, const Alloc& alloc = Alloc())
        : small_vector(alloc)
    {
        assign_move(l.begin(), l.end());
    }

    small_vector(const small_vector& v)
        : small_vector(v, atraits::select_on_container_copy_construction(v.get_allocator()))
    {}

    small_vector(const small_vector& v, const Alloc& alloc)
        : small_vector(alloc)
    {
        assign_copy(v.begin(), v.end());
    }

    small_vector(small_vector&& v) noexcept
        : Alloc(std::move(v.get_alloc()))
        , capacity_(v.capacity_)
    {
        take_impl(v);
    }

    ~small_vector()
    {
        destroy_all();

        if (!is_static())
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }
    }

    small_vector& operator=(const small_vector& v)
    {
        if (this == &v)
        {
            // prevent self usurp
            return *this;
        }

        destroy_all();
        assign_copy(v.begin(), v.end());

        return *this;
    }

    small_vector& operator=(small_vector&& v) noexcept
    {
        if (this == &v)
        {
            // prevent self usurp
            return *this;
        }

        destroy_all();
        if (!is_static())
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }

        get_alloc() = std::move(v.get_alloc());
        capacity_ = v.capacity_;

        take_impl(v);

        return *this;
    }

    void assign(size_type count, const T& value)
    {
        destroy_all();
        assign_fill(count, value);
    }

    template <class InputIterator, typename = decltype(*std::declval<InputIterator>())>
    void assign(InputIterator first, InputIterator last)
    {
        destroy_all();
        assign_copy(first, last);
    }

    void assign(std::initializer_list<T> ilist)
    {
        destroy_all();
        assign_move(ilist.begin(), ilist.end());
    }

    allocator_type get_allocator() const
    {
        return get_alloc();
    }

    const_reference at(size_type i) const
    {
        I_HPP_SMALL_VECTOR_BOUNDS_CHECK(i);
        return *(begin_ + i);
    }

    reference at(size_type i)
    {
        I_HPP_SMALL_VECTOR_BOUNDS_CHECK(i);
        return *(begin_ + i);
    }

    const_reference operator[](size_type i) const
    {
        return at(i);
    }

    reference operator[](size_type i)
    {
        return at(i);
    }

    const_reference front() const
    {
        return at(0);
    }

    reference front()
    {
        return at(0);
    }

    const_reference back() const
    {
        return *(end_ - 1);
    }

    reference back()
    {
        return *(end_ - 1);
    }

    const_pointer data() const noexcept
    {
        return begin_;
    }

    pointer data() noexcept
    {
        return begin_;
    }

    // iterators
    iterator begin() noexcept
    {
        return begin_;
    }

    const_iterator begin() const noexcept
    {
        return begin_;
    }

    const_iterator cbegin() const noexcept
    {
        return begin_;
    }

    iterator end() noexcept
    {
        return end_;
    }

    const_iterator end() const noexcept
    {
        return end_;
    }

    const_iterator cend() const noexcept
    {
        return end_;
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    // capacity
    bool empty() const noexcept
    {
        return begin_ == end_;
    }

    size_t size() const noexcept
    {
        return end_ - begin_;
    }

    size_t max_size() const noexcept
    {
        return atraits::max_size();
    }

    void reserve(size_type new_cap)
    {
        if (new_cap <= capacity_) return;

        const auto cdr = choose_data(new_cap);

        assert(cdr.ptr != begin_); // should've been handled by new_cap <= capacity_
        assert(cdr.ptr != static_begin_ptr()); // we should never reserve into static memory

        auto s = size();

        // now we need to transfer the existing elements into the new buffer
        for (size_type i = 0; i < s; ++i)
        {
            atraits::construct(get_alloc(), cdr.ptr + i, std::move(*(begin_ + i)));
        }

        // free old elements
        for (size_type i = 0; i < s; ++i)
        {
            atraits::destroy(get_alloc(), begin_ + i);
        }

        if (!is_static())
        {
            // we've moved from dyn to dyn memory, so deallocate the old one
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }

        begin_ = cdr.ptr;
        end_ = begin_ + s;
        capacity_ = cdr.cap;
    }

    size_t capacity() const noexcept
    {
        return capacity_;
    }

    void shrink_to_fit()
    {
        const auto s = size();

        if (s == capacity_) return; // we're at max
        if (is_static()) return; // can't shrink static buf

        auto old_begin = begin_;
        auto old_end = end_;
        auto old_cap = capacity_;

        if (s < StaticCapacity)
        {
            // revert to static capacity
            begin_ = end_ = static_begin_ptr();
            capacity_ = StaticCapacity;
        }
        else
        {
            // alloc new smaller buffer
            begin_ = end_ = atraits::allocate(get_alloc(), s);
            capacity_ = s;
        }

        for (auto p = old_begin; p != old_end; ++p)
        {
            atraits::construct(get_alloc(), end_, std::move(*p));
            ++end_;
            atraits::destroy(get_alloc(), p);
        }

        atraits::deallocate(get_alloc(), old_begin, old_cap);
    }

    // only revert if possible
    // otherwise don't shrink
    // return true if reverted
    bool revert_to_static()
    {
        const auto s = size();
        if (is_static()) return true; //we're already there
        if (s > StaticCapacity) return false; // nothing we can do

        shrink_to_fit();
        return true;
    }

    // modifiers
    void clear() noexcept
    {
        destroy_all();

        if (RevertToStaticBelow > 0 && !is_static())
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
            begin_ = end_ = static_begin_ptr();
            capacity_ = StaticCapacity;
        }
        else
        {
            end_ = begin_;
        }
    }

    iterator insert(const_iterator position, const value_type& val)
    {
        auto pos = grow_at(position, 1);
        atraits::construct(get_alloc(), pos, val);
        return pos;
    }

    iterator insert(const_iterator position, value_type&& val)
    {
        auto pos = grow_at(position, 1);
        atraits::construct(get_alloc(), pos, std::move(val));
        return pos;
    }

    iterator insert(const_iterator position, size_type count, const value_type& val)
    {
        auto pos = grow_at(position, count);
        for (size_type i = 0; i < count; ++i)
        {
            atraits::construct(get_alloc(), pos + i, val);
        }
        return pos;
    }

    template <typename InputIterator, typename = decltype(*std::declval<InputIterator>())>
    iterator insert(const_iterator position, InputIterator first, InputIterator last)
    {
        auto pos = grow_at(position, last - first);
        size_type i = 0;
        auto np = pos;
        for (auto p = first; p != last; ++p, ++np)
        {
            atraits::construct(get_alloc(), np, *p);
        }
        return pos;
    }

    iterator insert(const_iterator position, std::initializer_list<T> ilist)
    {
        auto pos = grow_at(position, ilist.size());
        size_type i = 0;
        for (auto& elem : ilist)
        {
            atraits::construct(get_alloc(), pos + i, std::move(elem));
            ++i;
        }
        return pos;
    }

    template<typename... Args>
    iterator emplace(const_iterator position, Args&&... args)
    {
        auto pos = grow_at(position, 1);
        atraits::construct(get_alloc(), pos, std::forward<Args>(args)...);
        return pos;
    }

    iterator erase(const_iterator position)
    {
        return shrink_at(position, 1);
    }

    iterator erase(const_iterator first, const_iterator last)
    {
        I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(first > last);
        return shrink_at(first, last - first);
    }

    void push_back(const_reference val)
    {
        auto pos = grow_at(end_, 1);
        atraits::construct(get_alloc(), pos, val);
    }

    void push_back(T&& val)
    {
        auto pos = grow_at(end_, 1);
        atraits::construct(get_alloc(), pos, std::move(val));
    }

    template<typename... Args>
    reference emplace_back(Args&&... args)
    {
        auto pos = grow_at(end_, 1);
        atraits::construct(get_alloc(), pos, std::forward<Args>(args)...);
        return *pos;
    }

    void pop_back()
    {
        shrink_at(end_ - 1, 1);
    }

    void resize(size_type n, const value_type& v)
    {
        reserve(n);

        auto new_end = begin_ + n;

        while (end_ > new_end)
        {
            atraits::destroy(get_alloc(), --end_);
        }

        while (new_end > end_)
        {
            atraits::construct(get_alloc(), end_++, v);
        }
    }

    void resize(size_type n)
    {
        reserve(n);

        auto new_end = begin_ + n;

        while (end_ > new_end)
        {
            atraits::destroy(get_alloc(), --end_);
        }

        while (new_end > end_)
        {
            atraits::construct(get_alloc(), end_++);
        }
    }

    bool is_static() const
    {
        return begin_ == static_begin_ptr();
    }

private:
    const T* static_begin_ptr() const
    {
        return reinterpret_cast<const_pointer>(static_data_ + 0);
    }

    T* static_begin_ptr()
    {
        return reinterpret_cast<pointer>(static_data_ + 0);
    }

    void destroy_all()
    {
        for (auto p = begin_; p != end_; ++p)
        {
            atraits::destroy(get_alloc(), p);
        }
    }

    void take_impl(small_vector& v)
    {
        if (v.is_static())
        {
            begin_ = end_ = static_begin_ptr();
            for (auto p = v.begin_; p != v.end_; ++p)
            {
                atraits::construct(get_alloc(), end_, std::move(*p));
                ++end_;
            }

            v.destroy_all();
        }
        else
        {
            begin_ = v.begin_;
            end_ = v.end_;
        }

        v.begin_ = v.end_ = v.static_begin_ptr();
        v.capacity_ = StaticCapacity;
    }

    // increase the size by splicing the elements in such a way that
    // a hole of uninitialized elements is left at position, with size num
    // returns the (potentially new) address of the hole
    T* grow_at(const T* cp, size_t num)
    {
        auto position = const_cast<T*>(cp);

        I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(position < begin_ || position > end_);

        const auto s = size();
        const auto cdr = choose_data(s + num);

        if (cdr.ptr == begin_)
        {
            // no special transfers needed

            end_ = begin_ + s + num;

            for (auto p = end_ - num - 1; p >= position; --p)
            {
                atraits::construct(get_alloc(), p + num, std::move(*p));
                atraits::destroy(get_alloc(), p);
            }

            return position;
        }
        else
        {
            // we need to transfer the elements into the new buffer

            position = cdr.ptr + (position - begin_);

            auto p = begin_;
            auto np = cdr.ptr;

            for (; np != position; ++p, ++np)
            {
                atraits::construct(get_alloc(), np, std::move(*p));
            }

            np += num; // hole
            for (; p != end_; ++p, ++np)
            {
                atraits::construct(get_alloc(), np, std::move(*p));
            }

            // destroy old
            for (p = begin_; p != end_; ++p)
            {
                atraits::destroy(get_alloc(), p);
            }

            if (!is_static())
            {
                // we've moved from dyn memory, so deallocate the old one
                atraits::deallocate(get_alloc(), begin_, capacity_);
            }

            begin_ = cdr.ptr;
            end_ = begin_ + s + num;
            capacity_ = cdr.cap;

            return position;
        }
    }

    T* shrink_at(const T* cp, size_t num)
    {
        auto position = const_cast<T*>(cp);

        I_HPP_SMALL_VECTOR_OUT_OF_RANGE_IF(position < begin_ || position > end_ || position + num > end_);

        const auto s = size();
        if (s - num == 0)
        {
            clear();
            return end_;
        }

        const auto cdr = choose_data(s - num);

        if (cdr.ptr == begin_)
        {
            // no special transfers needed

            for (auto p = position, np = position + num; np != end_; ++p, ++np)
            {
                atraits::destroy(get_alloc(), p);
                atraits::construct(get_alloc(), p, std::move(*np));
            }

            for (auto p = end_ - num; p != end_; ++p)
            {
                atraits::destroy(get_alloc(), p);
            }

            end_ -= num;
        }
        else
        {
            // we need to transfer the elements into the new buffer

            assert(cdr.ptr == static_begin_ptr()); // since we're shrinking that's the only way to have a new buffer

            auto p = begin_, np = cdr.ptr;
            for (; p != position; ++p, ++np)
            {
                atraits::construct(get_alloc(), np, std::move(*p));
                atraits::destroy(get_alloc(), p);
            }

            for (; p != position + num; ++p)
            {
                atraits::destroy(get_alloc(), p);
            }

            for (; np != cdr.ptr + s - num; ++p, ++np)
            {
                atraits::construct(get_alloc(), np, std::move(*p));
                atraits::destroy(get_alloc(), p);
            }

            // we've moved from dyn memory, so deallocate the old one
            atraits::deallocate(get_alloc(), begin_, capacity_);

            position = cdr.ptr + (position - begin_);
            begin_ = cdr.ptr;
            end_ = np;
            capacity_ = StaticCapacity;
        }

        return position;
    }

    void assign_fill(size_type count, const T& value)
    {
        const auto cdr = choose_data(count);

        end_ = cdr.ptr;
        for (size_t i=0; i<count; ++i)
        {
            atraits::construct(get_alloc(), end_, value);
            ++end_;
        }

        if (!is_static() && begin_ != cdr.ptr)
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }

        begin_ = cdr.ptr;
        capacity_ = cdr.cap;
    }

    template <class InputIterator>
    void assign_copy(InputIterator first, InputIterator last)
    {
        const auto cdr = choose_data(last - first);

        end_ = cdr.ptr;
        for (auto p = first; p != last; ++p)
        {
            atraits::construct(get_alloc(), end_, *p);
            ++end_;
        }

        if (!is_static() && begin_ != cdr.ptr)
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }

        begin_ = cdr.ptr;
        capacity_ = cdr.cap;
    }

    template <class InputIterator>
    void assign_move(InputIterator first, InputIterator last)
    {
        const auto cdr = choose_data(last - first);

        end_ = cdr.ptr;
        for (auto p = first; p != last; ++p)
        {
            atraits::construct(get_alloc(), end_, std::move(*p));
            ++end_;
        }

        if (!is_static() && begin_ != cdr.ptr)
        {
            atraits::deallocate(get_alloc(), begin_, capacity_);
        }

        begin_ = cdr.ptr;
        capacity_ = cdr.cap;
    }

    struct choose_data_result {
        T* ptr;
        size_t cap;
    };
    choose_data_result choose_data(size_t desired_capacity)
    {
        choose_data_result ret = {begin_, capacity_};

        if (!is_static())
        {
            // we're at the dyn buffer, so see if it needs resize or revert to static

            if (desired_capacity > capacity_)
            {
                while (ret.cap < desired_capacity)
                {
                    // grow by roughly 1.5
                    ret.cap *= 3;
                    ++ret.cap;
                    ret.cap /= 2;
                }

                ret.ptr = atraits::allocate(get_alloc(), ret.cap);
            }
            else if (desired_capacity < RevertToStaticBelow)
            {
                // we're reverting to the static buffer
                ret.ptr = static_begin_ptr();
                ret.cap = StaticCapacity;
            }

            // else, do nothing
            // the capacity is enough and we don't revert to static
        }
        else if (desired_capacity > StaticCapacity)
        {
            // we must move to dyn memory
            // first move to dyn memory, use desired cap

            ret.cap = desired_capacity;
            ret.ptr = atraits::allocate(get_alloc(), ret.cap);
        }
        // else, do nothing
        // the capacity is and we're in the static buffer

        return ret;
    }

    allocator_type& get_alloc() { return *this; }
    const allocator_type& get_alloc() const { return *this; }

    pointer begin_; // begin either points to static_data_ or to new memory
    pointer end_;
    size_t capacity_;
    typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type static_data_[StaticCapacity];
};

template<typename T,
    size_t StaticCapacityA, size_t RevertToStaticBelowA, class AllocA,
    size_t StaticCapacityB, size_t RevertToStaticBelowB, class AllocB
>
bool operator==(const small_vector<T, StaticCapacityA, RevertToStaticBelowA, AllocA>& a,
    const small_vector<T, StaticCapacityB, RevertToStaticBelowB, AllocB>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    for (size_t i = 0; i < a.size(); ++i)
    {
        if (!(a[i] == b[i]))
            return false;
    }

    return true;
}

template<typename T,
    size_t StaticCapacityA, size_t RevertToStaticBelowA, class AllocA,
    size_t StaticCapacityB, size_t RevertToStaticBelowB, class AllocB
>
bool operator!=(const small_vector<T, StaticCapacityA, RevertToStaticBelowA, AllocA>& a,
    const small_vector<T, StaticCapacityB, RevertToStaticBelowB, AllocB>& b)

{
    return !operator==(a, b);
}

}
