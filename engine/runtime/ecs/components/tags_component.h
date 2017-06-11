#pragma once

#include <unordered_set>
#include <string>
#include "../ecs.h"

 /**
  * Allow entities to be tagged with strings.
  *
  * entity.assign<tags_component>("tag1", "tag2");
  *
  * ComponentPtr<tags_component> tags;
  * for (entity entity : entity_manager.entities_with_components(tags))
  */

class tags_component : public runtime::component_impl<tags_component>
{
	SERIALIZABLE(tags_component)
	REFLECTABLE(tags_component, component)
public:
	/**
	 * Construct a new tags_component with the given tags.
	 *
	 * eg. tags_component tags("a", "b", "c");
	 */
	template <typename ... Args>
	tags_component(const std::string &tag, const Args & ... tags)
	{
		set_tags(tag, tags ...);
	}

	std::unordered_set<std::string> tags;

private:
	template <typename ... Args>
	void set_tags(const std::string &tag1, const std::string &tag2, const Args & ... tags)
	{
		this->tags.insert(tag1);
		set_tags(tag2, tags ...);
	}

	void set_tags(const std::string &tag)
	{
		tags.insert(tag);
	}
};
