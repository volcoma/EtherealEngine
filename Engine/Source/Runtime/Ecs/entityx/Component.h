#pragma once
#include "Entity.h"
#include <cstdint>

#include "Core/reflection/reflection.h"
#include "Core/serialization/serialization.h"
namespace entityx
{

#define COMPONENT(type)	\
public:						\
virtual ComponentId getId_v() const	\
{							\
	return type::getId();	\
}							\
static ComponentId getId()	\
{							\
	static ComponentId id = nextId();\
	return id;				\
}							\
ComponentHandle<type> makeHandle()	\
{									\
	return getEntity().component<type>();\
}									\
virtual std::shared_ptr<Component> clone() const								\
{																				\
	return std::static_pointer_cast<Component>(std::make_shared<type>(*this));	\
}

class Component
{
	REFLECTABLE(Component)
	SERIALIZABLE(Component)
	friend class EntityManager;
public:
	//-----------------------------------------------------------------------------
	//  Name : Component ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Component() = default;
	
	//-----------------------------------------------------------------------------
	//  Name : Component ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Component(const Component& component) = default;
	
	//-----------------------------------------------------------------------------
	//  Name : ~Component (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~Component() = default;
	
	//-----------------------------------------------------------------------------
	//  Name : clone (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual std::shared_ptr<Component> clone() const = 0;
	
	//-----------------------------------------------------------------------------
	//  Name : getId_v (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ComponentId getId_v() const = 0;

	//-----------------------------------------------------------------------------
	//  Name : touch (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------	
	virtual void touch(const std::string& context) { mDirty = true; }

	//-----------------------------------------------------------------------------
	//  Name : isDirty (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool isDirty() const { return mDirty; }

	//-----------------------------------------------------------------------------
	//  Name : onEntitySet (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onEntitySet() {}

	//-----------------------------------------------------------------------------
	//  Name : getEntity ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Entity getEntity() { return mEntity; }

protected:
	//-----------------------------------------------------------------------------
	//  Name : nextId ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static ComponentId nextId()
	{
		static ComponentId idCounter = 0;
		return idCounter++;
	}

	/// Owning Entity
	Entity mEntity;
	/// Was the component touched.
	bool mDirty = false;
};
}
