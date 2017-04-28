#pragma once

#include "../common/type_traits.hpp"
#include "../common/assert.hpp"

#include <vector>
#include <unordered_map>
#include <chrono>

namespace core
{
	struct Subsystem
	{
		Subsystem() = default;
		Subsystem(const Subsystem&) = delete;
		Subsystem& operator = (const Subsystem&) = delete;
		virtual ~Subsystem() {}

		//-----------------------------------------------------------------------------
		//  Name : initialize (virtual )
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual bool initialize() { return true; }

		//-----------------------------------------------------------------------------
		//  Name : dispose (virtual )
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual void dispose() {}
	};

	struct SubsystemContext
	{
		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize();

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose();

		//-----------------------------------------------------------------------------
		//  Name : add_subsystem ()
		/// <summary>
		/// Spawn a new subsystem with type S and construct arguments
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename S, typename ... Args> 
		S* add_subsystem(Args&& ...);

		//-----------------------------------------------------------------------------
		//  Name : get_subsystem ()
		/// <summary>
		/// Retrieve the registered system instance, existence should be guaranteed
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename S> 
		S* get_subsystem();

		//-----------------------------------------------------------------------------
		//  Name : remove_subsystem ()
		/// <summary>
		/// Release and unregistered a subsystem from our context
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename S> 
		void remove_subsystem();

		//-----------------------------------------------------------------------------
		//  Name : has_subsystems ()
		/// <summary>
		/// Check if we have specified subsystems
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename S>
		bool has_subsystems() const;

		//-----------------------------------------------------------------------------
		//  Name : has_subsystems ()
		/// <summary>
		/// Check if we have specified subsystems
		/// </summary>
		//-----------------------------------------------------------------------------
		template<typename S1, typename S2, typename ... Args> 
		bool has_subsystems() const;

	protected:
		/// 
		std::vector<nonstd::type_info_polymorphic::index_t> _orders;
		///
		std::unordered_map<nonstd::type_info_polymorphic::index_t, Subsystem*> _subsystems;
	};

	//
	// IMPLEMENTATIONS of SUBSYSTEMS
	template<typename S, typename ... Args>
	S* SubsystemContext::add_subsystem(Args&& ... args)
	{
		auto index = nonstd::type_info_polymorphic::id<Subsystem, S>();
		Expects(!has_subsystems<S>());
			//"duplicated subsystem: %s.", typeid(S).name());

		auto sys = new (std::nothrow) S(std::forward<Args>(args)...);
		_orders.push_back(index);
		_subsystems.insert(std::make_pair(index, sys));

		Expects(sys->initialize());
			//"failed to initialize subsystem: %s.", typeid(S).name());

		return sys;
	}

	template<typename S>
	S* SubsystemContext::get_subsystem()
	{
		auto index = nonstd::type_info_polymorphic::id<Subsystem, S>();

		auto found = _subsystems.find(index);
		if (found != _subsystems.end())
			return static_cast<S*>(found->second);

		return nullptr;
	}

	template<typename S>
	void SubsystemContext::remove_subsystem()
	{
		auto index = nonstd::type_info_polymorphic::id<Subsystem, S>();

		auto found = _subsystems.find(index);
		if (found != _subsystems.end())
		{
			found->second->dispose();
			delete found->second;
			_orders.erase(index);
			_subsystems.erase(found);
		}
	}

	template<typename S>
	bool SubsystemContext::has_subsystems() const
	{
		auto index = nonstd::type_info_polymorphic::id<Subsystem, S>();
		return _subsystems.find(index) != _subsystems.end();
	}

	template<typename S1, typename S2, typename ... Args> 
	bool SubsystemContext::has_subsystems() const
	{
		return has_subsystems<S1>() && has_subsystems<S2, Args...>();
	}

	// retrieve the registered system instance, existence should be guaranteed
	template<typename S>
	S* get_subsystem();

	// spawn a new subsystem with type S and construct arguments
	template<typename S, typename ... Args> 
	S* add_subsystem(Args&& ... args);

	// release and unregistered a subsystem from our context
	template<typename S>
	void remove_subsystem();

	// check if we have specified subsystems
	template<typename ... Args> 
	bool has_subsystems();

	struct SubsystemContext;
	namespace details
	{
		enum class Status : uint8_t
		{
			IDLE,
			RUNNING,
			DISPOSED
		};

		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize();

		//-----------------------------------------------------------------------------
		//  Name : status ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		Status status();

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose();

		//-----------------------------------------------------------------------------
		//  Name : context ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		SubsystemContext& context();
	}

	template<typename S> 
	S* get_subsystem()
	{
		return details::status() != details::Status::RUNNING ?
			nullptr : details::context().get_subsystem<S>();
	}

	template<typename S, typename ... Args> 
	S* add_subsystem(Args&& ... args)
	{
		return details::status() != details::Status::RUNNING ?
			nullptr : details::context().add_subsystem<S>(std::forward<Args>(args)...);
	}

	template<typename S>
	void remove_subsystem()
	{
		if (details::status() == details::Status::RUNNING)
			details::context().remove_subsystem<S>();
	}

	template<typename ... Args> 
	bool has_subsystems()
	{
		return details::status() != details::Status::RUNNING ?
			false : details::context().has_subsystems<Args...>();
	}
}
