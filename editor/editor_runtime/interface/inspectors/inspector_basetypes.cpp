#include "inspector_basetypes.h"

bool inspector_irect32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<irect32_t>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	std::string format = "%.0f";
	std::int32_t min = 0;
	std::int32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_int32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_int32();

	changed |= gui::DragIntNEx(namesLT, &data.left, 2, 0.05f, min, max, format.c_str());
	changed |= gui::DragIntNEx(namesRB, &data.right, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_urect32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<urect32_t>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	std::string format = "%.0f";
	std::uint32_t min = 0;
	std::uint32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_uint32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_uint32();

	changed |= gui::DragUIntNEx(namesLT, &data.left, 2, 0.05f, min, max, format.c_str());
	changed |= gui::DragUIntNEx(namesRB, &data.right, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_frect_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<frect_t>();
	bool changed = false;
	const char* namesLT[] = {"L", "T"};
	const char* namesRB[] = {"R", "B"};

	std::string format = "%.3f";
	float min = 0.0f;
	float max = 0.0f;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_float();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_float();

	changed |= gui::DragFloatNEx(namesLT, &data.left, 2, 0.05f, min, max, format.c_str());
	changed |= gui::DragFloatNEx(namesRB, &data.right, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_ipoint32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<ipoint32_t>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	std::string format = "%.0f";
	std::int32_t min = 0;
	std::int32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_int32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_int32();

	changed |= gui::DragIntNEx(names, &data.x, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_upoint32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<upoint32_t>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	std::string format = "%.0f";
	std::uint32_t min = 0;
	std::uint32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_uint32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_uint32();

	changed |= gui::DragUIntNEx(names, &data.x, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_fpoint_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<fpoint_t>();
	bool changed = false;
	const char* names[] = {"X", "Y"};

	std::string format = "%.3f";
	float min = 0.0f;
	float max = 0.0f;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_float();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_float();

	changed |= gui::DragFloatNEx(names, &data.x, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_isize32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<isize32_t>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	std::string format = "%.0f";
	std::int32_t min = 0;
	std::int32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_int32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_int32();

	changed |= gui::DragIntNEx(names, &data.width, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_usize32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<usize32_t>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	std::string format = "%.0f";
	std::uint32_t min = 0;
	std::uint32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_uint32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_uint32();

	changed |= gui::DragUIntNEx(names, &data.width, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_fsize_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<fsize_t>();
	bool changed = false;
	const char* names[] = {"W", "H"};

	std::string format = "%.3f";
	float min = 0.0f;
	float max = 0.0f;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_float();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_float();
	changed |= gui::DragFloatNEx(names, &data.width, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_irange32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<irange32_t>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	std::string format = "%.0f";
	std::int32_t min = 0;
	std::int32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_int32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_int32();

	changed |= gui::DragIntNEx(names, &data.min, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_urange32_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<urange32_t>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	std::string format = "%.0f";
	std::uint32_t min = 0;
	std::uint32_t max = 0;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_uint32();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_uint32();

	changed |= gui::DragUIntNEx(names, &data.min, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}

bool inspector_frange_t::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<frange_t>();
	bool changed = false;
	const char* names[] = {"Min", "Max"};

	std::string format = "%.3f";
	float min = 0.0f;
	float max = 0.0f;

	auto format_var = get_metadata("format");
	if(format_var)
		format = format_var.to_string();
	auto min_var = get_metadata("min");
	if(min_var)
		min = min_var.to_float();
	auto max_var = get_metadata("max");
	if(max_var)
		max = max_var.to_float();

	changed |= gui::DragFloatNEx(names, &data.min, 2, 0.05f, min, max, format.c_str());
	if(changed)
	{
		var = data;
		return true;
	}
	return false;
}
