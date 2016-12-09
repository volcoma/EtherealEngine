#pragma once

#include "entityx/quick.h"
#include <vector>
#include <fstream>

namespace ecs
{
	namespace utils
	{
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
		bool deserializeData(std::istream& stream, std::vector<Entity>& outData);
	}
}
