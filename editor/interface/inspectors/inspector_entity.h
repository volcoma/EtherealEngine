#pragma once
#include "inspector.h"
#include "runtime/ecs/ecs.h"
#include "core/common/pathname_tag_tree.h"

class inspector_entity : public inspector
{
private:
	struct component;
public:
	inspector_entity();
	inspector_entity(const inspector_entity& other);
	virtual ~inspector_entity();

	REFLECTABLE(inspector_entity, inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
private:
	std::unique_ptr<component> _component;
};

INSPECTOR_REFLECT(inspector_entity, runtime::entity)