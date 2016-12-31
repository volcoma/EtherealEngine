#pragma once

#include "core/ecs.h"
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
		void saveEntity(const fs::path& dir, const core::Entity& data);

		//-----------------------------------------------------------------------------
		//  Name : tryLoadEntity ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool tryLoadEntity(const fs::path& fullPath, core::Entity& outData);

		//-----------------------------------------------------------------------------
		//  Name : saveData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void saveData(const fs::path& fullPath, const std::vector<core::Entity>& data);

		//-----------------------------------------------------------------------------
		//  Name : loadData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool loadData(const fs::path& fullPath, std::vector<core::Entity>& outData);

		//-----------------------------------------------------------------------------
		//  Name : serializeData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void serializeData(std::ostream& stream, const std::vector<core::Entity>& data);

		//-----------------------------------------------------------------------------
		//  Name : deserializeData ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool deserializeData(std::istream& stream, std::vector<core::Entity>& outData);
	}
}
