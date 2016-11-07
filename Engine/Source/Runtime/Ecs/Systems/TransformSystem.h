#pragma once

#include "../entityx/System.h"
#include <vector>
using namespace entityx;

class TransformComponent;
class TransformSystem : public System<TransformSystem>
{
public:
	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameBegin(EntityManager &entities, EventManager &events, TimeDelta dt);

	//-----------------------------------------------------------------------------
	//  Name : getRoots ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<ComponentHandle<TransformComponent>>& getRoots() const { return mRoots; }
private:
	/// Scene roots
	std::vector<ComponentHandle<TransformComponent>> mRoots;
};
