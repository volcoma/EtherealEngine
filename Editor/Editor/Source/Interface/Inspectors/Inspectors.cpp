#include "Inspectors.h"
#include <unordered_map>
#include <vector>

struct InspectorRegistry
{
	InspectorRegistry()
	{
		auto inspectorTypes = rttr::type::get<Inspector>().get_derived_classes();
		for (auto& inspectorType : inspectorTypes)
		{
			auto inspectedTypeVar = inspectorType.get_metadata(INSPECTED_TYPE);
			if (inspectedTypeVar)
			{
				auto inspectedType = inspectedTypeVar.get_value<rttr::type>();
				auto inspectorVar = inspectorType.create();
				if (inspectorVar)
				{
					auto inspector = inspectorVar.get_value<std::shared_ptr<Inspector>>();
					typeMap[inspectedType] = inspector;
				}
			}
		}
	}
	std::unordered_map<rttr::type, std::shared_ptr<Inspector>> typeMap;
};

std::shared_ptr<Inspector> getInspector(rttr::type type)
{
	static InspectorRegistry registry;
	auto inspector = registry.typeMap[type];
	return inspector;
}

bool inspectVar(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	rttr::instance object = var;
	auto type = object.get_derived_type();
	auto name = type.get_name();
	auto properties = type.get_properties();

	bool changed = false;
	if (properties.empty())
	{
		auto inspector = getInspector(type);
		if (inspector)
		{
			changed |= inspector->inspect(var, readOnly, get_metadata);
		}
		else
		{
			if (type.is_enumeration())
			{
				changed |= inspectEnum(var, type.get_enumeration(), readOnly);
			}
		}

	}
	else
	{
		for (auto& prop : properties)
		{
			auto propVar = prop.get_value(object);
			auto propName = prop.get_name();
			bool rdOnly = prop.is_readonly();
			bool isArray = prop.is_array();
			bool isEnum = prop.is_enumeration();
			rttr::instance propObject = propVar;
			bool hasInspector = !!getInspector(propObject.get_derived_type());
			bool details = !hasInspector && !isEnum;
			PropertyLayout layout(prop);
			bool open = true;
			if (details)
			{
				open = gui::TreeNode("details");
			}

			if (open)
			{
				auto getMeta = [&prop](const rttr::variant& name) -> rttr::variant
				{
					return prop.get_metadata(name);
				};
				if (isArray)
				{
					changed |= inspectArray(propVar, rdOnly);
				}
				else if (isEnum)
				{
					changed |= inspectEnum(propVar, prop.get_enumeration(), rdOnly);
				}
				else
				{
					changed |= inspectVar(propVar, rdOnly, getMeta);
				}

				if(details)
					gui::TreePop();
			}
			

			if (changed && !rdOnly)
			{
				prop.set_value(object, propVar);
			}
		}

	}

	return changed;
}

bool inspectArray(rttr::variant& var, bool readOnly)
{
	auto array_view = var.create_array_view();
	auto size = array_view.get_size();
	bool changed = false;
	auto intSize = static_cast<int>(size);

	if (array_view.is_dynamic())
	{
		PropertyLayout layout("Size");
		if (gui::InputInt("", &intSize))
		{
			if (intSize < 0)
				intSize = 0;
			size = static_cast<std::size_t>(intSize);
			changed |= array_view.set_size(size);
		}
	}
	for (std::size_t i = 0; i < size; ++i)
	{
		auto value = array_view.get_value(i);
		std::string element = "Element ";
		element += std::to_string(i);

		PropertyLayout layout(element.data());

		changed |= inspectVar(value);

		if (changed)
			array_view.set_value(i, value);

	}


	return changed;
}


bool inspectEnum(rttr::variant& var, rttr::enumeration& data, bool readOnly)
{
	auto strings = data.get_names();
	std::vector<const char*> cstrings{};

	for (const auto& string : strings)
		cstrings.push_back(string.data());

	if (readOnly)
	{
		int listbox_item_current = var.to_int();
		gui::TextUnformatted(cstrings[listbox_item_current]);
	}
	else
	{
		int listbox_item_current = var.to_int();

		if (gui::Combo("", &listbox_item_current, cstrings.data(), static_cast<int>(cstrings.size()), static_cast<int>(cstrings.size())))
		{
			rttr::variant arg(listbox_item_current);
			arg.convert(var.get_type());
			var = arg;
			return true;
		}
	}
	
	return false;
}