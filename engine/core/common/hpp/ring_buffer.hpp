#pragma once

#include "ring_view.hpp"
#include <array>
#include <set>
#include <vector>

namespace hpp
{

template<typename T>
struct write_proxy
{
    write_proxy(T& t)
        : buffer(t)
    {
        buffer.set_recording_changes(true);
    }
    ~write_proxy()
    {
        buffer.set_recording_changes(false);
    }

    T& buffer;
};

template <typename container_t>
class ring_buffer : public ring_span<typename container_t::value_type>
{
public:
    using container_type = container_t;
    using value_type = typename container_t::value_type;
    using size_type = typename container_t::size_type;
    using view_type = ring_span<value_type>;

    explicit ring_buffer() noexcept(std::is_nothrow_default_constructible<container_type>::value)
    {
        reset(0, 0);
    }
    template <class... Args>
    ring_buffer(size_type first_idx, size_type size,
                Args&&... args) noexcept(noexcept(container_type(std::forward<Args>(args)...)))
        : container_(std::forward<Args>(args)...)
    {
        reset(first_idx, size);
    }

    ring_buffer(const ring_buffer& rhs) noexcept(std::is_nothrow_copy_constructible<container_type>::value)
        : ring_buffer(&*std::begin(rhs) - &*std::begin(rhs.container_), rhs.size(), rhs.container_)
    {
    }

    ring_buffer(ring_buffer&& rhs) noexcept(std::is_nothrow_move_constructible<container_type>::value)
    {
        *this = std::move(rhs);
    }

    explicit ring_buffer(const container_type& rhs) noexcept(
        std::is_nothrow_copy_constructible<container_type>::value)
        : ring_buffer(0, rhs.size(), rhs)
    {
    }

    explicit ring_buffer(container_type&& rhs) noexcept(
        std::is_nothrow_move_constructible<container_type>::value)
        : ring_buffer(0, rhs.size(), std::move(rhs))
    {
    }

    ring_buffer&
    operator=(const ring_buffer& rhs) noexcept(std::is_nothrow_copy_assignable<container_type>::value)
    {
        const auto first_idx = (&*std::cbegin(rhs) - &*std::cbegin(rhs.container_));
        container_ = rhs.container_;
        static_cast<view_type&>(*this) = view_type(std::begin(container_), std::end(container_),
                                                   std::begin(container_) + first_idx, rhs.size());
        return *this;
    }

    ring_buffer& operator=(ring_buffer&& rhs) noexcept(std::is_nothrow_copy_assignable<container_type>::value)
    {
        const auto first_idx = (&*std::cbegin(rhs) - &*std::cbegin(rhs.container_));
        container_ = std::move(rhs.container_);
        static_cast<view_type&>(*this) = view_type(std::begin(container_), std::end(container_),
                                                   std::begin(container_) + first_idx, rhs.size());
        return *this;
    }

    write_proxy<ring_buffer> write() noexcept
    {
        return {*this};
    }

    void clear() noexcept
    {
        reset(0, 0);
    }

    const container_type& container() const noexcept
    {
        return container_;
    }

private:
    void reset(size_type first_idx, size_type size)
    {
        if(!container_.empty())
        {
            static_cast<view_type&>(*this) = view_type(std::begin(container_), std::end(container_),
                                                       std::begin(container_) + first_idx, size);
        }

        this->clear_changes();
    }

    container_type container_{};
};

template <typename T, size_t N>
using stack_ringbuffer = ring_buffer<std::array<T, N>>;

template <typename T>
using heap_ringbuffer = ring_buffer<std::vector<T>>;

} // namespace hpp
