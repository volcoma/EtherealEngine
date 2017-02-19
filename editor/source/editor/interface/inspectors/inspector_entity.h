#pragma once
#include "inspector.h"
#include "runtime/ecs/ecs.h"
#include "core/common/pathname_tag_tree.h"

class Inspector_Entity : public Inspector
{
private:
	struct component;
public:
	Inspector_Entity();
	Inspector_Entity(const Inspector_Entity& other);
	virtual ~Inspector_Entity();

	REFLECTABLE(Inspector_Entity, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
private:
	std::unique_ptr<component> _component;
};

INSPECTOR_REFLECT(Inspector_Entity, runtime::Entity)