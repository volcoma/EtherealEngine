#include "inspector_basetypes.h"

bool Inspector_iRect::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<iRect>();
	bool changed = false;
	const char* namesLT[] = { "L", "T" };
	const char* namesRB[] = { "R", "B" };

	changed |= gui::DragIntNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragIntNEx(namesRB, &data.right, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_uRect::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<uRect>();
	bool changed = false;
	const char* namesLT[] = { "L", "T" };
	const char* namesRB[] = { "R", "B" };

	changed |= gui::DragUIntNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragUIntNEx(namesRB, &data.right, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_fRect::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fRect>();
	bool changed = false;
	const char* namesLT[] = { "L", "T" };
	const char* namesRB[] = { "R", "B" };

	changed |= gui::DragFloatNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragFloatNEx(namesRB, &data.right, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_iPoint::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<iPoint>();
	bool changed = false;
	const char* names[] = { "X", "Y" };

	changed |= gui::DragIntNEx(names, &data.x, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_uPoint::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<uPoint>();
	bool changed = false;
	const char* names[] = { "X", "Y" };

	changed |= gui::DragUIntNEx(names, &data.x, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_fPoint::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fPoint>();
	bool changed = false;
	const char* names[] = { "X", "Y" };

	changed |= gui::DragFloatNEx(names, &data.x, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_iSize::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<iSize>();
	bool changed = false;
	const char* names[] = { "W", "H" };

	changed |= gui::DragIntNEx(names, &data.width, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_uSize::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<uSize>();
	bool changed = false;
	const char* names[] = { "W", "H" };

	changed |= gui::DragUIntNEx(names, &data.width, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_fSize::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fSize>();
	bool changed = false;
	const char* names[] = { "W", "H" };

	changed |= gui::DragFloatNEx(names, &data.width, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_iRange::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<iRange>();
	bool changed = false;
	const char* names[] = { "Min", "Max" };

	changed |= gui::DragIntNEx(names, &data.Min, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_uRange::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<uRange>();
	bool changed = false;
	const char* names[] = { "Min", "Max" };

	changed |= gui::DragUIntNEx(names, &data.Min, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool Inspector_fRange::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fRange>();
	bool changed = false;
	const char* names[] = { "Min", "Max" };

	changed |= gui::DragFloatNEx(names, &data.Min, 2, 0.05f);
	if (changed)
	{
		var = data;
		return true;
	}
	return false;
}