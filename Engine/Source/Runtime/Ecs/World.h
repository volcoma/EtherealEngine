#pragma once

#include "entityx/quick.h"
#include <vector>
#include <fstream>

using namespace entityx;

struct World
{
	//-----------------------------------------------------------------------------
	//  Name : World ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	World();

	//-----------------------------------------------------------------------------
	//  Name : reset ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void reset();

	/// Event manager
	EventManager events;
	/// Entity manager
	EntityManager entities;
	/// System manager
	SystemManager systems;
};

struct Prefab
{
	Entity instantiate();
	std::istream data;
};