#pragma once

namespace imguizmo
{
	// call BeginFrame right after ImGui_XXXX_NewFrame();
	void set_view_rect(float x, float y, float width, float height);

	// return true if mouse cursor is over any gizmo control (axis, plan or screen component)
	bool is_over();

	// return true if mouse IsOver or if the gizmo is in moving state
	bool is_using();

	// enable/disable the gizmo. Stay in the state until next call to Enable.
	// gizmo is rendered with gray half transparent color when disabled
	void enable(bool enable);

	// Render a cube with face color corresponding to face normal. Usefull for debug/tests
	void draw_cube(const float *view, const float *projection, float *matrix);

	// call it when you want a gizmo
	// Needs view and projection matrices. 
	// matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional
	// translation is applied in world space
	enum OPERATION
	{
		TRANSLATE,
		ROTATE,
		SCALE
	};

	enum MODE
	{
		LOCAL,
		WORLD
	};

	void manipulate(const float *view, const float *projection, OPERATION operation, MODE mode, float *matrix, float *deltaMatrix = 0, float *snap = 0);
};
