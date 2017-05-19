#include "inspector_entity.h"
#include "inspectors.h"


struct inspector_entity::component
{
	struct instance
	{
		path_name_tag_tree _tree;
		std::vector<runtime::chandle<runtime::component>> _list;
		instance();
		void setup(const std::vector<runtime::chandle<runtime::component>>& list);
		void inspect(bool& changed);
	};

	struct type
	{
		path_name_tag_tree _tree;
		path_name_tag_tree::iterator _itor;
		std::vector<rttr::type> _list;

		type();

		void inspect(ImGuiTextFilter& filter, runtime::entity data);
	};

	instance _instance;
	type _type;
};

inspector_entity::component::instance::instance() :
	_tree('/', -1)
{ }

void inspector_entity::component::instance::setup(const std::vector<runtime::chandle<runtime::component>>& list)
{
	_list = list;
	_tree.reset();
	{
		size_t i = 0;
		for (auto& ptr : _list)
		{
			auto component = ptr.lock().get();

			auto info = rttr::type::get(*component);

			auto meta_category = info.get_metadata("Category");
			auto meta_id = info.get_metadata("Id");

			std::string name = info.get_name();

			if (meta_id)
			{
				name = meta_id.to_string();
			}

			if (meta_category)
			{
				std::string category = meta_category.to_string();
				name = category + "/" + name;
			}
			else
			{
				name = "Default/" + name;
			}
			_tree.set(name, i++);
		}
	}
}
void inspector_entity::component::instance::inspect(bool& changed)
{
	bool opened = true;

	auto it = _tree.create_iterator();

	while (it.steps())
	{
		while (it.steping())
		{
			const char* name = it.name().data();

			if (it.is_leaf() ? it.tag() < _list.size() : it.tag() > 0)
			{
				bool remove = false;

				if (opened)
				{
					gui::SetNextTreeNodeOpen(true, ImGuiSetCond_FirstUseEver);
					if (gui::CollapsingHeader(name, &opened))
					{
						gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 8.0f);
						gui::TreePush(name);

						if (it.step_in())
						{
							continue;
						}
						else
						{
							rttr::variant var = _list[it.tag()].lock().get();
							changed |= inspect_var(var);

							gui::TreePop();
							gui::PopStyleVar();
						}
					}

					if (!opened && it.is_leaf())
					{
						opened = remove = true;
					}
				}
				else
				{
					if (it.step_in())
					{
						continue;
					}
					else
					{
						remove = true;
					}
				}

				if (remove)
				{
					auto& component_ptr = _list[it.tag()];
					auto component = component_ptr.lock().get();

					component->get_entity().remove(component_ptr.lock());

				}
			}

			it.step();
		}

		if (it.step_out())
		{
			opened = it.step();
			gui::TreePop();
			gui::PopStyleVar();
		}
	}
}


inspector_entity::component::type::type() :
	_tree('/', -1)
{
	auto types = rttr::type::get<runtime::component>().get_derived_classes();
	for (auto& info : types)
	{
		auto meta_category = info.get_metadata("Category");
		auto meta_id = info.get_metadata("Id");
		
		std::string name = info.get_name();

		if (meta_id)
		{
			name = meta_id.to_string();
		}

		if (meta_category)
		{
			std::string category = meta_category.to_string();
			name = category + "/" + name;
		}
		else
		{
			name = "Default/" + name;
		}
		_tree.set(name, _list.size());
		_list.push_back(info);
			
	}
	_tree.setup_iterator(_itor);
}

void inspector_entity::component::type::inspect(ImGuiTextFilter& filter, runtime::entity data)
{
	gui::Separator();
	
	if (gui::ButtonEx("<", ImVec2(0, 0), _itor.steps() > 1 ? 0 : ImGuiButtonFlags_Disabled))
	{
		_itor.step_out();
	}
	gui::Separator();


	if (_itor.step_stack_pop())
	{
		_itor.step_in();
	}
	else
	{
		_itor.step_reset();
	}

	while (_itor.steping())
	{
		std::string name = _itor.name();

		auto component_type = _list[_itor.tag()];

		if (filter.PassFilter(name.c_str()))
		{
			if (!_itor.is_leaf())
			{
				name += "/";
			}
			if (gui::Selectable(name.c_str()))
			{
				if (_itor.is_leaf())
				{
					auto cstructor = component_type.get_constructor();

					auto c = cstructor.invoke();
					auto c_ptr = c.get_value<std::shared_ptr<runtime::component>>();
					if (c_ptr)
						data.assign(c_ptr);

					gui::CloseCurrentPopup();
				}
				else
				{
					_itor.step_stack_push();
				}
			}
		}

		_itor.step();
	}
}

inspector_entity::inspector_entity() 
	: _component(std::make_unique<component>())
{ }

inspector_entity::inspector_entity(const inspector_entity& other)
	: _component(std::make_unique<component>())
{ }

inspector_entity::~inspector_entity()
{
}

bool inspector_entity::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<runtime::entity>();
	if (!data)
		return false;
	bool changed = false;
	{
		property_layout propName("Name");
		rttr::variant varName = data.to_string();
		
		changed |= inspect_var(varName);

		if(changed)
		{
			data.set_name(varName.to_string());
		}
	}
	ImGui::Separator();
	_component->_instance.setup(data.all_components());

	_component->_instance.inspect(changed);

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

		_component->_type.inspect(filter, data);

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