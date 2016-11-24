#include "Inspector_CoreTypes.h"
#include "Core/common/string_utils.h"

bool Inspector_Bool::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<bool>();

	if (readOnly)
	{
		gui::TextUnformatted(data ? "true" : "false");
	}
	else
	{
		if (gui::Checkbox("", &data))
		{
			var = data;
			return true;
		}
	}
	
	return false;
}


bool Inspector_Float::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.to_float();
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		float min = 0.0f;
		float max = 0.0f;
		float step = 0.05f;
		std::string format = "%.3f";

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_float();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_float();

		auto formatVar = get_metadata("Format");
		if (formatVar)
			format = formatVar.to_string();

		auto stepVar = get_metadata("Step");
		if (stepVar)
			step = stepVar.to_float();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderFloat("", &data, min, max, format.c_str()))
			{
				var = data;
				return true;
			}
		}
		else
		{
			if (gui::DragFloat("", &data, step))
			{
				var = data;
				return true;
			}
		}
	}

	
	return false;
}


bool Inspector_Double::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<float>(var.to_double());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		float min = 0.0f;
		float max = 0.0f;
		float step = 0.05f;
		std::string format = "%.3f";

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_float();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_float();

		auto formatVar = get_metadata("Format");
		if (formatVar)
			format = formatVar.to_string();

		auto stepVar = get_metadata("Step");
		if (stepVar)
			step = stepVar.to_float();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderFloat("", &data, min, max, format.c_str()))
			{
				var = data;
				return true;
			}
		}
		else
		{
			if (gui::DragFloat("", &data, step))
			{
				var = data;
				return true;
			}
		}
	}
	return false;
}

bool Inspector_Int8::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_int8());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int8_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::int8_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_Int16::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_int16());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int16_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::int16_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_Int32::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_int32());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();
		
		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int32_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::int32_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_Int64::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_int64());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::int64_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::int64_t>(data);
				return true;
			}
		}
	}

	return false;
}


bool Inspector_UInt8::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_uint8());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint8_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::uint8_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_UInt16::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_uint16());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint16_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::uint16_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_UInt32::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_uint32());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint32_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::uint32_t>(data);
				return true;
			}
		}
	}

	return false;
}

bool Inspector_UInt64::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = static_cast<int>(var.to_uint64());
	if (readOnly)
	{
		gui::TextUnformatted(std::to_string(data).c_str());
	}
	else
	{
		int min = 0;
		int max = 0;

		auto minVar = get_metadata("Min");
		if (minVar)
			min = minVar.to_int();

		auto maxVar = get_metadata("Max");
		if (maxVar)
			max = maxVar.to_int();

		bool isRange = maxVar.is_valid();
		
		if (isRange)
		{
			if (gui::SliderInt("", &data, min, max))
			{
				var = static_cast<std::uint64_t>(data);
				return true;
			}
		}
		else
		{
			if (gui::DragInt("", &data))
			{
				var = static_cast<std::uint64_t>(data);
				return true;
			}
		}
	}

	return false;
}


bool Inspector_String::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<std::string>();
	if (readOnly)
	{
		gui::TextUnformatted(data.c_str());
	}
	else
	{
		std::string inputBuff;
		inputBuff.resize(64, 0);
		std::copy(data.begin(), data.end(), inputBuff.begin());
		if (gui::InputText("", &inputBuff[0], inputBuff.size(), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			// copy from c_str to remove trailing zeros
			var = std::string(inputBuff.c_str());
			return true;
		}
	}

	return false;
}