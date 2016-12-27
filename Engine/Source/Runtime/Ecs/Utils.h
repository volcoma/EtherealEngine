#pragma once

#include "entityx/quick.h"
#include <vector>
#include <fstream>
#include "../System/FileSystem.h"


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
		void saveEntity(const fs::path& dir, const Entity& data);

		//-----------------------------------------------------------------------------
		//  Name : tryLoadEntity ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool tryLoadEntity(const fs::path& fullPath, Entity& outData);

		//-----------------------------------------------------------------------------
		//  Name : saveData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void saveData(const fs::path& fullPath, const std::vector<Entity>& data);

		//-----------------------------------------------------------------------------
		//  Name : loadData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool loadData(const fs::path& fullPath, std::vector<Entity>& outData);

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
