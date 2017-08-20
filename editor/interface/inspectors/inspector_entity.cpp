#include "inspector_entity.h"
#include "inspectors.h"

bool inspector_entity::inspect(rttr::variant& var, bool readOnly,
							   std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<runtime::entity>();
	if(!data)
		return false;
	bool changed = false;
	{
		property_layout prop_name("Name");
		rttr::variant var_name = data.to_string();

		changed |= inspect_var(var_name);

		if(changed)
		{
			data.set_name(var_name.to_string());
		}
	}
	ImGui::Separator();

	auto components = data.all_components();
	for(auto& component_ptr : components)
	{
		bool opened = true;
		auto component = component_ptr.lock().get();
		auto component_type = rttr::type::get(*component);
        
        std::string name = component_type.get_name().data();
        auto meta_id = component_type.get_metadata("pretty_name");
        if(meta_id)
        {
            name = meta_id.to_string();
        }
              
		gui::SetNextTreeNodeOpen(true, ImGuiCond_FirstUseEver);
		if(gui::CollapsingHeader(name.c_str(), &opened))
		{
			gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.0f);
			gui::TreePush(name.c_str());

			rttr::variant component_var = component;
			changed |= inspect_var(component_var);

			gui::TreePop();
			gui::PopStyleVar();
		}

		if(!opened)
		{
			component->get_entity().remove(component_ptr.lock());
		}
	}

	gui::Separator();
	if(gui::Button("+COMPONENT"))
	{
		gui::OpenPopup("COMPONENT_MENU");
		gui::SetNextWindowPos(gui::GetCursorScreenPos());
	}

	if(gui::BeginPopup("COMPONENT_MENU"))
	{
		static ImGuiTextFilter filter;
		filter.Draw("Filter", 180);
		gui::Separator();
		gui::BeginChild("COMPONENT_MENU_CONTEXT", ImVec2(gui::GetContentRegionAvailWidth(), 200.0f));

		auto component_types = rttr::type::get<runtime::component>().get_derived_classes();

		for(auto& component_type : component_types)
		{
			// If any constructors registered
			auto cstructor = component_type.get_constructor();
			if(cstructor)
			{
                std::string name = component_type.get_name().data();
                auto meta_id = component_type.get_metadata("pretty_name");
                if(meta_id)
                {
                    name = meta_id.to_string();
                }
				if(!filter.PassFilter(name.c_str()))
					continue;

				if(gui::Selectable(name.c_str()))
				{
					auto c = cstructor.invoke();
					auto c_ptr = c.get_value<std::shared_ptr<runtime::component>>();

					if(c_ptr)
						data.assign(c_ptr);

					gui::CloseCurrentPopup();
				}
			}
		}

		gui::EndChild();
		gui::EndPopup();
	}

	if(changed)
	{
		var = data;
		return true;
	}

	return false;
}
