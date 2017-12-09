#include "inspector_coretypes.h"
#include "core/string_utils/string_utils.h"

bool inspector_bool::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<bool>();

	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(data ? "true" : "false");
	}
	else
	{
		if(gui::Checkbox("", &data))
		{
			var = data;
			return true;
		}
	}

	return false;
}

bool inspector_float::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.to_float();
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		float min = 0.0f;
		float max = 0.0f;
		float step = 0.05f;
		std::string format = "%.3f";

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_float();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_float();

		auto format_var = get_metadata("format");
		if(format_var)
			format = format_var.to_string();

		auto step_var = get_metadata("step");
		if(step_var)
			step = step_var.to_float();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderFloat("", &data, min, max, format.c_str()))
			{
				var = data;
				return true;
			}
		}
		else
		{
			if(gui::DragFloat("", &data, step, min, max))
			{
				var = data;
				return true;
			}
		}
	}

	return false;
}

bool inspector_double::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<float>(var.to_double());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		float min = 0.0f;
		float max = 0.0f;
		float step = 0.05f;
		std::string format = "%.3f";

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_float();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_float();

		auto format_var = get_metadata("format");
		if(format_var)
			format = format_var.to_string();

		auto step_var = get_metadata("step");
		if(step_var)
			step = step_var.to_float();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderFloat("", &data, min, max, format.c_str()))
			{
				var = data;
				return true;
			}
		}
		else
		{
			if(gui::DragFloat("", &data, step, min, max))
			{
				var = data;
				return true;
			}
		}
	}
	return false;
}

bool inspector_int8::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_int8());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int8_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::int8_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_int16::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_int16());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int16_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::int16_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_int32::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_int32());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int32_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::int32_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_int64::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_int64());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int64_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::int64_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_uint8::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_uint8());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint8_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::uint8_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_uint16::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_uint16());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint16_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::uint16_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_uint32::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_uint32());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint32_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::uint32_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_uint64::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = static_cast<int>(var.to_uint64());
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto min_var = get_metadata("min");
		if(min_var)
			min = min_var.to_int();

		auto max_var = get_metadata("max");
		if(max_var)
			max = max_var.to_int();

		bool is_range = max_var.is_valid();

		if(is_range)
		{
			if(gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint64_t>(data);
				return true;
			}
		}
		else
		{
			if(gui::DragInt("", &data))
			{
				var = static_cast<std::uint64_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool inspector_string::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<std::string>();
	if(read_only)
	{
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(data.c_str());
	}
	else
	{
		static std::array<char, 64> input_buff;
		input_buff.fill(0);
		std::memcpy(input_buff.data(), data.c_str(), data.size() < 64 ? data.size() : 64);
		if(gui::InputText("", input_buff.data(), input_buff.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			// copy from c_str to remove trailing zeros
			var = std::string(input_buff.data());
			return true;
		}
	}

	return false;
}
