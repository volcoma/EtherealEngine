#include "inspectors.h"
#include <unordered_map>
#include <vector>

struct inspector_registry
{
	inspector_registry()
	{
		auto inspector_types = rttr::type::get<inspector>().get_derived_classes();
		for(auto& inspector_type : inspector_types)
		{
			auto inspected_type_var = inspector_type.get_metadata("inspected_type");
			if(inspected_type_var)
			{
				auto inspected_type = inspected_type_var.get_value<rttr::type>();
				auto inspector_var = inspector_type.create();
				if(inspector_var)
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
	static inspector_registry registry;
	return registry.type_map[type];
}

bool inspect_var(rttr::variant& var, bool skip_custom, bool read_only,
				 const inspector::meta_getter& get_metadata)
{
	rttr::instance object = var;
	auto type = object.get_derived_type();
	auto properties = type.get_properties();

	bool changed = false;

	auto inspector = get_inspector(type);
	if(!skip_custom && inspector)
	{
		changed |= inspector->inspect(var, read_only, get_metadata);
	}
	else if(properties.empty())
	{
		if(type.is_enumeration())
		{
			auto enumeration = type.get_enumeration();
			changed |= inspect_enum(var, enumeration, read_only);
		}
	}
	else
	{
		for(auto& prop : properties)
		{
			bool prop_changed = false;
			auto prop_var = prop.get_value(object);
			bool is_readonly = prop.is_readonly();
			bool is_array = prop_var.is_sequential_container();
			bool is_associative_container = prop_var.is_associative_container();
			bool is_enum = prop.is_enumeration();
			rttr::instance prop_object = prop_var;
			bool has_inspector = !!get_inspector(prop_object.get_derived_type());
			bool details = !has_inspector && !is_enum;
			property_layout layout(prop);
			bool open = true;
			if(details)
			{
				gui::AlignTextToFramePadding();
				open = gui::TreeNode("details");
			}

			if(open)
			{
				auto get_meta = [&prop](const rttr::variant& name) -> rttr::variant {
					return prop.get_metadata(name);
				};
				if(is_array)
				{
					prop_changed |= inspect_array(prop_var, is_readonly, get_meta);
				}
				else if(is_associative_container)
				{
					prop_changed |= inspect_associative_container(prop_var, is_readonly);
				}
				else if(is_enum)
				{
					auto enumeration = prop.get_enumeration();
					prop_changed |= inspect_enum(prop_var, enumeration, is_readonly);
				}
				else
				{
					prop_changed |= inspect_var(prop_var, false, is_readonly, get_meta);
				}

				if(details && open)
					gui::TreePop();
			}

			if(prop_changed && !is_readonly)
			{
				prop.set_value(object, prop_var);
			}

			changed |= prop_changed;
		}
	}

	return changed;
}

bool inspect_array(rttr::variant& var, bool read_only, const inspector::meta_getter& get_metadata)
{
	auto view = var.create_sequential_view();
	auto size = view.get_size();
	bool changed = false;
	auto int_size = static_cast<int>(size);

	if(view.is_dynamic())
	{
		property_layout layout("Size");
        
        if(!read_only)
        {
            if(gui::InputInt("", &int_size))
            {
                if(int_size < 0)
                    int_size = 0;
                size = static_cast<std::size_t>(int_size);
                changed |= view.set_size(size);
            }
        }
        else
        {
            gui::AlignTextToFramePadding();
            gui::TextUnformatted(std::to_string(int_size).c_str());
        }
		
	}

	for(std::size_t i = 0; i < size; ++i)
	{
		auto value = view.get_value(i).extract_wrapped_value();
		std::string element = "Element ";
		element += std::to_string(i);

		property_layout layout(element.data());

		changed |= inspect_var(value, false, read_only, get_metadata);

		if(changed)
			view.set_value(i, value);
	}

	return changed;
}

bool inspect_associative_container(rttr::variant& var, bool read_only)
{
	auto associative_view = var.create_associative_view();
	// auto size = associative_view.get_size();
	bool changed = false;

	return changed;
}

bool inspect_enum(rttr::variant& var, rttr::enumeration& data, bool read_only)
{
	auto strings = data.get_names();
	std::vector<const char*> cstrings{};
	cstrings.reserve(strings.size());

	for(const auto& string : strings)
		cstrings.push_back(string.data());

	if(read_only)
	{
		int listbox_item_current = var.to_int();
		gui::AlignTextToFramePadding();
		gui::TextUnformatted(cstrings[std::size_t(listbox_item_current)]);
	}
	else
	{
		int listbox_item_current = var.to_int();
		int listbox_item_size = static_cast<int>(cstrings.size());
		if(gui::Combo("", &listbox_item_current, cstrings.data(), listbox_item_size, listbox_item_size))
		{
			rttr::variant arg(listbox_item_current);
			arg.convert(var.get_type());
			var = arg;
			return true;
		}
	}

	return false;
}

rttr::variant get_meta_empty(const rttr::variant& other)
{
	return rttr::variant();
}
