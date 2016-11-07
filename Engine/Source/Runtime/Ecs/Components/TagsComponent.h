/*
 * Copyright (C) 2012 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#pragma once

#include <unordered_set>
#include <string>
#include "../entityx/Component.h"

 /**
  * Allow entities to be tagged with strings.
  *
  * entity.assign<TagsComponent>("tag1", "tag2");
  *
  * ComponentPtr<TagsComponent> tags;
  * for (Entity entity : entity_manager.entities_with_components(tags))
  */
using namespace entityx;

class TagsComponent : public Component
{
	COMPONENT(TagsComponent)
		REFLECTABLE(TagsComponent, Component)
public:
	/**
	 * Construct a new TagsComponent with the given tags.
	 *
	 * eg. TagsComponent tags("a", "b", "c");
	 */
	template <typename ... Args>
	TagsComponent(const std::string &tag, const Args & ... tags)
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