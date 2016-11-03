#pragma once

#include "IEditorView.h"
#include "Runtime/Ecs/World.h"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : HierarchyView (Class)
/// <summary>
/// 
/// </summary>
//-----------------------------------------------------------------------------
class HierarchyView : public IEditorView
{
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	HierarchyView();
	~HierarchyView();
    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
   
    // Updates
    virtual void						render						(AppWindow& window);

private:
	void drawEntity(ecs::Entity entity);
	void checkContextMenu(ecs::Entity entity);
	void checkDrag(ecs::Entity entity, bool isHovered);
};
