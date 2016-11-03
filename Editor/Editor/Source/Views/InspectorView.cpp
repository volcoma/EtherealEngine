#include "InspectorView.h"
#include "../PropertyEditors/PropertyEditors.h"
#include "../EditorApp.h"

void drawVar(rttr::variant var, bool collapsing = false, bool* opened = nullptr)
{
	rttr::instance object = var;
	auto info = object.get_derived_type();

	gui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);

	bool expanded = true;

	if (collapsing)
	{
		expanded = gui::CollapsingHeader(info.get_name().data(), opened);
	}

	if (expanded)
	{
		if (collapsing)
		{
			gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.0f);
			gui::TreePush(info.get_name().data());
		}
		
		edit_instance(var);

		if (collapsing)
		{
			gui::TreePop();
			gui::PopStyleVar();
		}
	}
}

void drawEntity(ecs::Entity& selected)
{

	std::string data = selected.to_string();
	char buffer[64];
	memset(buffer, 0, 64);
	memcpy(buffer, data.c_str(), data.size());

	{
		PropertyEdit propName("Name");
		if (gui::InputText("", buffer, math::countof(buffer), ImGuiInputTextFlags_EnterReturnsTrue))
		{
			selected.setName(std::string(buffer));
		}
	}

	auto components = selected.all_components();
	for (auto& component_ptr : components)
	{
		bool opened = true;
		auto component = component_ptr.lock().get();
		drawVar(component, true, &opened);
		if (!opened)
		{
			component->getEntity().remove(component_ptr.lock());
		}
	}

	gui::Separator();
	if (gui::BeginMenu("Add Component"))
	{
		auto component_types = rttr::type::get<Component>().get_derived_classes();
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
					if (gui::MenuItem(component_type.get_name().data()))
					{
						auto c = cstructor.invoke();
						auto c_ptr = c.get_value<std::shared_ptr<Component>>();
						if (c_ptr)
							selected.assign(c_ptr);
					}

				}
			}
		}
		gui::EndMenu();
	}
	
}


InspectorView::InspectorView()
{
	mName = "Inspector";
}


InspectorView::~InspectorView()
{
}

void InspectorView::render(AppWindow& window)
{
	ImGuiIO& io = gui::GetIO();
	ImVec2 sz = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
	gui::SetNextWindowPos(ImVec2(sz.x - sz.x / 2, sz.y - sz.y / 2));
	gui::SetNextWindowSize(sz, ImGuiSetCond_FirstUseEver);
	if (!gui::BeginDock(mName.c_str(), &mOpen, ImGuiWindowFlags_NoScrollbar))
	{
		gui::EndDock();
		return;
	}
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	{
		auto& selection = editState.entitySelection;
		auto& selected = selection.selected;
		if (selected)
		{
			drawEntity(selected);
		}

	}
	{
		auto& selection = editState.materialSelection;
		auto& selected = selection.selected;
		if (selected)
		{
			drawVar(selected);
		}
	}

	gui::EndDock();
}