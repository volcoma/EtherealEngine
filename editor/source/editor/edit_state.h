#pragma once
#include "core/subsystem/subsystem.h"
#include "interface/docks/imguidock.h"
#include "interface/gizmos/imguizmo.h"
#include "runtime/ecs/ecs.h"
#include "runtime/assets/asset_handle.h"
#include <chrono>

struct texture;
namespace editor
{
	struct editor_state : core::subsystem
	{
		struct drag_type
		{
			rttr::variant object;
			std::string description;
		};

		struct selection
		{
			rttr::variant object;
		};

		struct snap
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
		//  Name : save_editor_camera ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void save_editor_camera();

		//-----------------------------------------------------------------------------
		//  Name : load_editor_camera ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void load_editor_camera();

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

		void frame_end(std::chrono::duration<float> dt);
		/// editor camera
		runtime::entity camera;
		/// current scene
		std::string scene;
		/// enable editor grid
		bool show_grid = true;
		/// enable wireframe selection
		bool wireframe_selection = true;
		/// current manipulation gizmo operation.
		imguizmo::operation operation = imguizmo::translate;
		/// current manipulation gizmo space.
		imguizmo::mode mode = imguizmo::local;
		/// drag data containing dragged object
		drag_type drag_data;
		/// selection data containing selected object
		selection selection_data;
		/// snap data containging various snap options
		snap snap_data;
		/// editor icons lookup map
		std::unordered_map<std::string, asset_handle<texture>> icons;
	};

}