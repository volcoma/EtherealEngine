#pragma once
#include "core/subsystem.h"
#include "interface/docks/imguidock.h"
#include "interface/gizmos/imguizmo.h"
#include "runtime/ecs/ecs.h"
#include "runtime/assets/asset_handle.h"
#include <chrono>

struct Texture;
namespace editor
{
	struct EditState : core::Subsystem
	{
		struct DragData
		{
			rttr::variant object;
			std::string description;
		};
		struct SelectionData
		{
			rttr::variant object;
		};
		struct SnapData
		{
			///
			math::vec3 translation_snap = { 1.0f, 1.0f, 1.0f };
			///
			float rotation_degree_snap = 15.0f;
			///
			float scale_snap = 0.1f;
		};
		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize();

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose();

		//-----------------------------------------------------------------------------
		//  Name : select ()
		/// <summary>
		/// Selects an object. Can be anything.
		/// </summary>
		//-----------------------------------------------------------------------------
		void select(rttr::variant object);

		//-----------------------------------------------------------------------------
		//  Name : unselect ()
		/// <summary>
		/// Clears the selection data.
		/// </summary>
		//-----------------------------------------------------------------------------
		void unselect();

		//-----------------------------------------------------------------------------
		//  Name : drag ()
		/// <summary>
		/// Starts dragging an object. Can be anything.
		/// </summary>
		//-----------------------------------------------------------------------------
		void drag(rttr::variant object, const std::string& description = "");

		//-----------------------------------------------------------------------------
		//  Name : drop ()
		/// <summary>
		/// Clears the drag data.
		/// </summary>
		//-----------------------------------------------------------------------------
		void drop();

		/// editor camera
		runtime::Entity camera;
		/// current scene
		std::string scene;
		/// enable editor grid
		bool show_grid = true;
		/// enable wireframe selection
		bool wireframe_selection = true;
		/// current manipulation gizmo operation.
		imguizmo::OPERATION operation = imguizmo::TRANSLATE;
		/// current manipulation gizmo space.
		imguizmo::MODE mode = imguizmo::LOCAL;
		/// drag data containing dragged object
		DragData drag_data;
		/// selection data containing selected object
		SelectionData selection_data;
		/// snap data containging various snap options
		SnapData snap_data;
		/// editor icons lookup map
		std::unordered_map<std::string, AssetHandle<Texture>> icons;
	};

}