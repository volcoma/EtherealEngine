#include "ComponentRegistry.h"



Id entityx::ComponentRegistry::getId(const Component& component)
{
	return component.getId_v();
}
