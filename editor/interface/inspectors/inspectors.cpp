#include "inspectors.h"
#include <unordered_map>
#include <vector>

struct InspectorRegistry
{
	InspectorRegistry()
	{
		auto inspector_types = rttr::type::get<inspector>().get_derived_classes();
		for (auto& inspector_type : inspector_types)
		{
			auto inspected_type_var = inspector_type.get_metadata(INSPECTED_TYPE);
			if (inspected_type_var)
			{
				auto inspected_type = inspected_type_var.get_value<rttr::type>();
				auto inspector_var = inspector_type.create();
				if (inspector_var)
				{
					type_map[inspected_type] = inspector_var.get_value<std::shared_ptr<inspector>>();
				}
			}
		}
	}
	std::unordered_map<rttr::type, std::shared_ptr<inspector>> type_map;
};

std::shared_ptr<inspector> get_inspector(rttr::type type)
{
	static InspectorRegistry registry;
	return registry.type_map[type];
}

bool inspect_var(rttr::variant& var, bool read_only, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	rttr::instance object = var;
	auto type = object.get_derived_type();
	auto name = type.get_name();
	auto properties = type.get_properties();

	bool changed = false;

	auto inspector = get_inspector(type);
	if (inspector)
	{
		changed |= inspector->inspect(var, read_only, get_metadata);
	}
	else if (properties.empty())
	{
		if (type.is_enumeration())
		{
            auto enumeration = type.get_enumeration();
            changed |= inspect_enum(var, enumeration, read_only);
		}
	}
	else
	{
		for (auto& prop : properties)
		{
			auto prop_var = prop.get_value(object);
			auto prop_name = prop.get_name();
			bool is_readonly = prop.is_readonly();
			bool is_array = prop.is_array();
			bool is_enum = prop.is_enumeration();
			rttr::instance prop_object = prop_var;
			bool has_inspector = !!get_inspector(prop_object.get_derived_type());
			bool details = !has_inspector && !is_enum;
			property_layout layout(prop);
			bool open = true;
			if (details)
			{
				gui::AlignFirstTextHeightToWidgets();
				open = gui::TreeNode("details");
			}

			if (open)
			{
				auto get_meta = [&prop](const rttr::variant& name) -> rttr::variant
				{
					return prop.get_metadata(name);
				};
				if (is_array)
				{
					changed |= inspect_array(prop_var, is_readonly);
				}
				else if (is_enum)
				{
                    auto enumeration = prop.get_enumeration();
                    changed |= inspect_enum(prop_var, enumeration, is_readonly);
				}
				else
				{
					changed |= inspect_var(prop_var, is_readonly, get_meta);
				}

				if(details)
					gui::TreePop();
			}
			
			if (changed && !is_readonly)
			{
				prop.set_value(object, prop_var);
			}
		}

	}

	return changed;
}

bool inspect_array(rttr::variant& var, bool read_only)
{
	auto array_view = var.create_array_view();
	auto size = array_view.get_size();
	bool changed = false;
	auto int_size = static_cast<int>(size);

	if (array_view.is_dynamic())
	{
		property_layout layout("Size");
		if (gui::InputInt("", &int_size))
		{
			if (int_size < 0)
				int_size = 0;
			size = static_cast<std::size_t>(int_size);
			changed |= array_view.set_size(size);
		}
	}
	for (std::size_t i = 0; i < size; ++i)
	{
		auto value = array_view.get_value(i);
		std::string element = "Element ";
		element += std::to_string(i);

		property_layout layout(element.data());

		changed |= inspect_var(value);

		if (changed)
			array_view.set_value(i, value);

	}

	return changed;
}


bool inspect_enum(rttr::variant& var, rttr::enumeration& data, bool read_only)
{
	auto strings = data.get_names();
	std::vector<const char*> cstrings{};
	cstrings.reserve(strings.size());

	for (const auto& string : strings)
		cstrings.push_back(string.data());

	if (read_only)
	{
		int listbox_item_current = var.to_int();
		gui::AlignFirstTextHeightToWidgets();
		gui::TextUnformatted(cstrings[listbox_item_current]);
	}
	else
	{
		int listbox_item_current = var.to_int();
		int listbox_item_size = static_cast<int>(cstrings.size());
		if (gui::Combo("", &listbox_item_current, cstrings.data(), listbox_item_size, listbox_item_size))
		{
			rttr::variant arg(listbox_item_current);
			arg.convert(var.get_type());
			var = arg;
			return true;
		}
	}
	
	return false;
}
