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
	virtual ~Component() {}
	Component() {}
	Component(const Component& component) { }
	virtual std::shared_ptr<Component> clone() const = 0;
	virtual ComponentId getId_v() const = 0;
	
	virtual void touch(const std::string& context) { mDirty = true; }
	virtual bool isDirty() const { return mDirty; }
	virtual void onEntitySet() {}
	Entity getEntity() { return mEntity; }
protected:
	static ComponentId nextId()
	{
		static ComponentId idCounter = 0;
		return idCounter++;
	}

	Entity mEntity;
	bool mDirty = false;
};



}
