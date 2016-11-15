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

	//-----------------------------------------------------------------------------
	//  Name : saveEntity ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void saveEntity(const std::string& name, const Entity& data);
	
	//-----------------------------------------------------------------------------
	//  Name : tryLoadEntity ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool tryLoadEntity(const std::string& name, Entity& outData);
	
	//-----------------------------------------------------------------------------
	//  Name : saveData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void saveData(const std::string& fullPath, const std::vector<Entity>& data);
	
	//-----------------------------------------------------------------------------
	//  Name : loadData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool loadData(const std::string& fullPath, std::vector<Entity>& outData);
	
	//-----------------------------------------------------------------------------
	//  Name : serializeData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void serializeData(std::ostream& stream, const std::vector<Entity>& data);
	
	//-----------------------------------------------------------------------------
	//  Name : deserializeData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool deserializeData(std::istream& stream, std::vector<ecs::Entity>& outData);

	/// Event manager
	EventManager events;
	/// Entity manager
	EntityManager entities;
	/// System manager
	SystemManager systems;

	//TODO move this to a more appropriate place
	/// Keep count of serialized entities
	std::map<uint32_t, Entity> serializationMap;
};
