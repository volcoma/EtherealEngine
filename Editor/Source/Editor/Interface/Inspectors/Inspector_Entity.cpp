#include "Inspector_Entity.h"
#include "Inspectors.h"

bool Inspector_Entity::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<ecs::Entity>();
	if (!data)
		return false;
	bool changed = false;
	{
		PropertyLayout propName("Name");
		rttr::variant varName = data.to_string();
		
		changed |= inspectVar(varName);

		if(changed)
		{
			data.setName(varName.to_string());
		}
	}

	auto components = data.all_components();
	for (auto& component_ptr : components)
	{
		bool opened = true;
		auto component = component_ptr.lock().get();

		auto info = rttr::type::get(*component);

		gui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);

		bool expanded = gui::CollapsingHeader(info.get_name().data(), &opened);
		if (expanded)
		{
		
			gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.0f);
			gui::TreePush(info.get_name().data());

			rttr::variant componentVar = component;
			changed |= inspectVar(componentVar);
			
			gui::TreePop();
			gui::PopStyleVar();

		}


		if (!opened)
		{
			component->getEntity().remove(component_ptr.lock());
		}
	}

	gui::Separator();
	if (gui::Button("+Component"))
	{
		gui::OpenPopup("ComponentMenu");
		
	}

	if (gui::BeginPopup("ComponentMenu"))
	{
		static ImGuiTextFilter filter;
		filter.Draw("Filter", 180);
		gui::Separator();
		gui::BeginChild("ComponentMenuContent", ImVec2(gui::GetContentRegionAvailWidth(), 200.0f));
		auto component_types = rttr::type::get<ecs::Component>().get_derived_classes();
		for (auto& component_type : component_types)
		{

			// If any constructors registered
			auto cstructor = component_type.get_constructor();
			auto meta = cstructor.get_metadata("CanExecuteInEditor");
			if (cstructor && meta)
			{
				bool invoke_in_editor = meta.to_bool();
				if (invoke_in_editor)
				{
					if (!filter.PassFilter(component_type.get_name().data()))
						continue;

					if (gui::Selectable(component_type.get_name().data()))
					{
						auto c = cstructor.invoke();
						auto c_ptr = c.get_value<std::shared_ptr<Component>>();
						if (c_ptr)
							data.assign(c_ptr);

						gui::CloseCurrentPopup();
					}

				}
			}
		}
		gui::EndChild();
		gui::EndPopup();
	}

	if (changed)
	{
		var = data;
		return true;
	}

	return false;
}