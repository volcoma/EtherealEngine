#pragma once

#include "Component.h"

namespace entityx 
{
struct Component;
// Used to be able to assign unique ids to each component type.
struct ComponentRegistry
{
    using Id = std::uint16_t;

	static Id getId(const Component& component);
	template<typename T>
	static Id getId()
	{
 		return T::getId();
	}
protected:

};

}

