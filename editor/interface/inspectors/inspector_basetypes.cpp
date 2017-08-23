#include "inspector_basetypes.h"

bool inspector_irect::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<irect>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	changed |= gui::DragIntNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragIntNEx(namesRB, &data.right, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_urect::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<urect>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	changed |= gui::DragUIntNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragUIntNEx(namesRB, &data.right, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_frect::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<frect>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	changed |= gui::DragFloatNEx(namesLT, &data.left, 2, 0.05f);
	changed |= gui::DragFloatNEx(namesRB, &data.right, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_ipoint::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<ipoint>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	changed |= gui::DragIntNEx(names, &data.x, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_upoint::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<upoint>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	changed |= gui::DragUIntNEx(names, &data.x, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_fpoint::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fpoint>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	changed |= gui::DragFloatNEx(names, &data.x, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_isize::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<isize>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	changed |= gui::DragIntNEx(names, &data.width, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_usize::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<usize>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	changed |= gui::DragUIntNEx(names, &data.width, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_fsize::inspect(rttr::variant& var, bool read_only,
							  std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<fsize>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	changed |= gui::DragFloatNEx(names, &data.width, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_irange::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<irange>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	changed |= gui::DragIntNEx(names, &data.Min, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_urange::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<urange>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	changed |= gui::DragUIntNEx(names, &data.Min, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_frange::inspect(rttr::variant& var, bool read_only,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<frange>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	changed |= gui::DragFloatNEx(names, &data.Min, 2, 0.05f);
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}
