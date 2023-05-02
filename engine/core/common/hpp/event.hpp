#pragma once
#include "delegate.hpp"
#include "optional.hpp"
#include "sentinel.hpp"
#include "source_location.hpp"
#include <algorithm>
#include <map>
#include <utility>

namespace hpp
{
template<typename T>
class event;

template<typename... Args>
class event<void(Args...)>
{
public:
    using slot_type = delegate<void(Args...)>;
    using slot_key = uint64_t;
    using slot_sentinel = hpp::optional<hpp::sentinel>;

    struct slot_t
    {
        slot_key key{};
        slot_type slot{};
        slot_sentinel sentinel = hpp::nullopt;
        hpp::source_location location = hpp::source_location::current();
        bool removed{};
    };

    using slot_priority = int64_t;
    using slot_container = std::multimap<slot_priority, slot_t, std::greater<slot_priority>>;

    template<class C>
    slot_key connect(C* const object_ptr,
                     void (C::*const method_ptr)(Args...),
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, slot_priority(0), location, object_ptr, method_ptr);
    }

    template<class C>
    slot_key connect(C* const object_ptr,
                     void (C::*const method_ptr)(Args...) const,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, slot_priority(0), location, object_ptr, method_ptr);
    }

    template<typename T,
             typename = typename std::enable_if<!std::is_same<event, typename std::decay<T>::type>::value>::type>
    slot_key connect(T&& f, const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, slot_priority(0), location, std::forward<T>(f));
    }

    template<class C>
    slot_key connect(slot_priority priority,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...),
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, priority, location, object_ptr, method_ptr);
    }

    template<class C>
    slot_key connect(slot_priority priority,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...) const,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, priority, location, object_ptr, method_ptr);
    }

    template<typename T,
             typename = typename std::enable_if<!std::is_same<event, typename std::decay<T>::type>::value>::type>
    slot_key connect(slot_priority priority,
                     T&& f,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(hpp::nullopt, priority, location, std::forward<T>(f));
    }

    template<class C>
    slot_key connect(const slot_sentinel& sentinel,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...),
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, slot_priority(0), location, object_ptr, method_ptr);
    }

    template<class C>
    slot_key connect(const slot_sentinel& sentinel,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...) const,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, slot_priority(0), location, object_ptr, method_ptr);
    }

    template<typename T,
             typename = typename std::enable_if<!std::is_same<event, typename std::decay<T>::type>::value>::type>
    slot_key connect(const slot_sentinel& sentinel,
                     T&& f,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, slot_priority(0), location, std::forward<T>(f));
    }

    template<class C>
    slot_key connect(const slot_sentinel& sentinel,
                     slot_priority priority,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...),
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, priority, location, object_ptr, method_ptr);
    }

    template<class C>
    slot_key connect(const slot_sentinel& sentinel,
                     slot_priority priority,
                     C* const object_ptr,
                     void (C::*const method_ptr)(Args...) const,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, priority, location, object_ptr, method_ptr);
    }

    template<typename T,
             typename = typename std::enable_if<!std::is_same<event, typename std::decay<T>::type>::value>::type>
    slot_key connect(const slot_sentinel& sentinel,
                     slot_priority priority,
                     T&& f,
                     const hpp::source_location& location = hpp::source_location::current())
    {
        return get_impl().connect_impl(sentinel, priority, location, std::forward<T>(f));
    }

    template<class C>
    void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...))
    {
        if(!has_impl())
        {
            return;
        }
        slot_type slot(object_ptr, method_ptr);
        get_impl().disconnect_impl(slot);
    }

    template<class C>
    void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...) const)
    {
        if(!has_impl())
        {
            return;
        }
        slot_type slot(object_ptr, method_ptr);
        get_impl().disconnect_impl(slot);
    }

    template<typename T,
             typename = typename std::enable_if<!std::is_same<event, typename std::decay<T>::type>::value &&
                                                !std::is_same<slot_key, typename std::decay<T>::type>::value>::type>
    void disconnect(T&& f)
    {
        if(!has_impl())
        {
            return;
        }
        slot_type slot(std::forward<T>(f));
        get_impl().disconnect_impl(slot);
    }

    void disconnect_current() noexcept
    {
        if(!has_impl())
        {
            return;
        }
        disconnect(get_impl().current_id_);
    }

    void disconnect(const slot_key& key) noexcept
    {
        if(!has_impl())
        {
            return;
        }
        get_impl().disconnect_impl(key);
    }

    /// Emits the events you wish to send to the call-backs
    /// \param args The arguments to emit to the slots connected to the signal
    void emit(Args... args) const
    {
        if(!has_impl())
        {
            return;
        }
        get_impl().emit_impl(args...);
    }

    /// Emits events you wish to send to call-backs
    /// \param args The arguments to emit to the slots connected to the signal
    /// \note
    /// This is equvialent to emit.
    void operator()(Args... args) const
    {
        emit(args...);
    }

    // comparision operators for sorting and comparing

    bool operator==(const event& s) const
    {
        if(empty() && s.empty())
        {
            return true;
        }

        if(has_impl() && s.has_impl())
        {
            return get_impl().slots_ == s.get_impl().slots_;
        }

        return false;
    }

    bool operator!=(const event& s) const
    {
        return !operator==(s);
    }

    bool empty() const
    {
        if(!has_impl())
        {
            return true;
        }
        return get_impl().slots_.empty();
    }

    const slot_container& get_slots() const
    {
        if(!has_impl())
        {
            static const slot_container empty;
            return empty;
        }
        return get_impl().slots_;
    }

    event() = default;
    event(const event& rhs)
    {
        if(rhs.has_impl())
        {
            get_impl() = rhs.get_impl();
        }
    }

    event& operator=(const event& rhs)
    {
        if(this == &rhs)
        {
            return *this;
        }
        if(rhs.has_impl())
        {
            get_impl() = rhs.get_impl();
        }
        else
        {
            impl_.reset();
        }
        return *this;
    }

    event(event&& rhs) noexcept
    {
        impl_ = std::move(rhs.impl_);
    }

    event& operator=(event&& rhs) noexcept
    {
        if(this == &rhs)
        {
            return *this;
        }
        impl_ = std::move(rhs.impl_);
        return *this;
    }

private:
    struct impl
    {
        static bool check_for_remove(slot_t& slot)
        {
            if(slot.removed)
            {
                return true;
            }

            if(slot.sentinel != hpp::nullopt && slot.sentinel.value().expired())
            {
                slot.removed = true;
                return true;
            }

            return false;
        }

        template<typename... A>
        slot_key connect_impl(const slot_sentinel& sentinel,
                              slot_priority priority,
                              const hpp::source_location& location,
                              A&&... args)
        {
            auto id = free_id_++;

            slot_t slot{slot_key(id), slot_type(std::forward<A>(args)...), sentinel, location, false};

            slots_.emplace(priority, std::move(slot));
            return id;
        }

        void disconnect_impl(const slot_key& key)
        {
            for(auto it = std::begin(slots_); it != std::end(slots_); ++it)
            {
                auto& element_slot = it->second;
                if(element_slot.key != key)
                {
                    continue;
                }

                if(depth_ == 0)
                {
                    slots_.erase(it);
                }
                else
                {
                    element_slot.removed = true;
                }
                return;
            }
        }

        void disconnect_impl(slot_type& slot)
        {
            for(auto it = std::begin(slots_); it != std::end(slots_); ++it)
            {
                auto& element_slot = it->second;
                if(element_slot.slot == slot)
                {
                    if(depth_ == 0)
                    {
                        slots_.erase(it);
                    }
                    else
                    {
                        element_slot.removed = true;
                    }
                    return;
                }
            }
        }

        void emit_impl(Args... args) const
        {
            bool collect_garbage{};
            depth_++;
            for(auto& slot : slots_)
            {
                current_id_ = slot.second.key;

                if(check_for_remove(slot.second))
                {
                    collect_garbage = true;
                    continue;
                }

                slot.second.slot(args...);

                if(check_for_remove(slot.second))
                {
                    collect_garbage = true;
                    continue;
                }
            }
            depth_--;

            if(collect_garbage)
            {
                auto it = std::begin(slots_);
                while(it != std::end(slots_))
                {
                    if(it->second.removed)
                    {
                        it = slots_.erase(it);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }

        mutable uint32_t depth_{};
        mutable slot_key current_id_{};
        slot_key free_id_ = 1;
        /// The slots connected to the signal
        mutable slot_container slots_;
    };

    bool has_impl() const noexcept
    {
        return !!impl_;
    }

    impl& get_impl() const
    {
        return *impl_;
    }

    impl& get_impl()
    {
        if(!impl_)
        {
            impl_ = std::make_unique<impl>();
        }

        return *impl_;
    }

    std::unique_ptr<impl> impl_;
};

template <typename T>
class counted_event;

template <typename... Args>
class counted_event<void(Args...)> : public hpp::event<void(Args...)>
{
public:
    void reset_counts() noexcept
    {
        call_count_ = 0;
        prev_call_count_ = 0;
    }

    auto are_counts_equal() const noexcept -> bool
    {
        return call_count_ == prev_call_count_;
    }

    void increment_count() noexcept
    {
        call_count_++;
    }

    auto get_call_count() const noexcept -> uint64_t
    {
        return call_count_;
    }

    void equalize_counts()
    {
        prev_call_count_ = call_count_;
    }

private:

    uint64_t call_count_{};
    uint64_t prev_call_count_{};
};

} // end of namespace hpp
