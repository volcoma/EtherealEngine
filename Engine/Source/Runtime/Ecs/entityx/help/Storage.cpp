/*
 * Copyright (C) 2012-2014 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#include "Storage.h"

namespace entityx
{

	ComponentStorage::ComponentStorage(std::size_t size)
	{
		expand(size);
	}

	ComponentStorage::~ComponentStorage()
	{

	}


	void ComponentStorage::expand(std::size_t n)
	{
		data.resize(n);
	}


	void ComponentStorage::reserve(std::size_t n)
	{
		data.reserve(n);
	}


	std::shared_ptr<Component> ComponentStorage::get(std::size_t n)
	{
		assert(n < size());
		return data[n];
	}


	const std::shared_ptr<Component> ComponentStorage::get(std::size_t n) const
	{
		assert(n < size());
		return data[n];
	}


	void ComponentStorage::destroy(std::size_t n)
	{
		assert(n < size());
		auto& element = data[n];
		element.reset();
	}


	std::weak_ptr<Component> ComponentStorage::set(unsigned int index, std::shared_ptr<Component> component)
	{
		data[index] = component;
		return component;
	}

}  // namespace entityx
