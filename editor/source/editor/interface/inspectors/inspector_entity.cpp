#include "inspector_entity.h"
#include "inspectors.h"

struct Inspector_Entity::component
{
	struct instance;
	struct type;

	instance* _instance;
	type* _type;

	component();
	~component();
};

struct Inspector_Entity::component::instance
{
	PathNameTagTree _tree;
	std::vector<runtime::CHandle<runtime::Component>> _list;

	instance();

	void setup(const std::vector<runtime::CHandle<runtime::Component>>& list);

	void inspect(bool& changed);
};

struct Inspector_Entity::component::type 
{
	PathNameTagTree _tree;
	PathNameTagTree::iterator _itor;
	std::vector<rttr::type*> _list;

	type();

	void inspect(ImGuiTextFilter& filter, runtime::Entity data);
};

Inspector_Entity::component::component() :
	_instance(new instance()),
	_type(new type())
{ }
Inspector_Entity::component::~component()
{
	delete _instance;
	delete _type;
}

Inspector_Entity::component::instance::instance() :
	_tree('/', -1)
{ }

void Inspector_Entity::component::instance::setup(const std::vector<runtime::CHandle<runtime::Component>>& list)
{
	_list = list;
	_tree.reset();
	{
		size_t i = 0;
		for (auto& ptr : _list)
		{
			auto component = ptr.lock().get();

			auto info = rttr::type::get(*component);

			_tree.set(info.get_name(), i++);
		}
	}
}
void Inspector_Entity::component::instance::inspect(bool& changed)
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
							rttr::variant componentVar = _list[it.tag()].lock().get();
							changed |= inspect_var(componentVar);

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
			it.step();

			gui::TreePop();
			gui::PopStyleVar();
		}
	}
}


Inspector_Entity::component::type::type() :
	_tree('/', -1)
{
	auto types = rttr::type::get<runtime::Component>().get_derived_classes();
	for (auto& it : types)
	{
		// If any constructors registered
		auto cstructor = it.get_constructor();
		auto meta = cstructor.get_metadata("CanExecuteInEditor");
		if (cstructor && meta)
		{
			bool invoke_in_editor = meta.to_bool();
			if (invoke_in_editor)
			{
				_tree.set(it.get_name().data(), _list.size());
				_list.push_back(&it);
			}
		}
	}
	_tree.setup_iterator(_itor);
}

void Inspector_Entity::component::type::inspect(ImGuiTextFilter& filter, runtime::Entity data)
{
	gui::Separator();
	std::string path = "<";
	if (gui::ButtonEx(path.data(), ImVec2(0, 0), _itor.steps() > 1 ? 0 : ImGuiButtonFlags_Disabled))
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
		const char* name = _itor.name().data();

		auto component_type = _list[_itor.tag()];

		if (filter.PassFilter(name))
		{
			if (gui::Selectable(name))
			{
				if (_itor.is_leaf())
				{
					auto cstructor = component_type->get_constructor();

					auto c = cstructor.invoke();
					auto c_ptr = c.get_value<std::shared_ptr<runtime::Component>>();
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

Inspector_Entity::Inspector_Entity() :
	_component(new component())
{ }

Inspector_Entity::~Inspector_Entity()
{
	delete _component;
}

bool Inspector_Entity::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<runtime::Entity>();
	if (!data)
		return false;
	bool changed = false;
	{
		PropertyLayout propName("Name");
		rttr::variant varName = data.to_string();
		
		changed |= inspect_var(varName);

		if(changed)
		{
			data.set_name(varName.to_string());
		}
	}
	
	_component->_instance->setup(data.all_components());

	_component->_instance->inspect(changed);

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
		
		_component->_type->inspect(filter, data);

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