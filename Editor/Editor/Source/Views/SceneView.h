#pragma once

#include "IEditorView.h"
#include "Runtime/Assets/AssetHandle.h"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------
struct Texture;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : SceneView (Class)
/// <summary>
/// 
/// </summary>
//-----------------------------------------------------------------------------
class SceneView : public IEditorView
{
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	SceneView();
	~SceneView();
    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
   
    // Updates
	virtual void render(AppWindow& window);
private:
	AssetHandle<Texture> mTranslationIcon;
	AssetHandle<Texture> mRotationIcon;
	AssetHandle<Texture> mScaleIcon;
	AssetHandle<Texture> mLocalSystemIcon;
	AssetHandle<Texture> mGlobalSystemIcon;
	AssetHandle<Texture> mPlayIcon;
	AssetHandle<Texture> mPauseIcon;
	AssetHandle<Texture> mStopIcon;
	AssetHandle<Texture> mStepIcon;
};