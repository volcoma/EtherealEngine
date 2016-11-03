/*
 * Copyright (C) 2012-2014 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#pragma once

#include <cstddef>
#include <cassert>
#include <vector>
#include <memory>

namespace entityx {

	class Component;
	class ComponentStorage
	{
	public:
		ComponentStorage(std::size_t size = 100);

		~ComponentStorage();

		std::size_t size() const { return data.size(); }
		std::size_t capacity() const { return data.capacity(); }

		/// Ensure at least n elements will fit in the pool.
		void expand(std::size_t n);

		void reserve(std::size_t n);

		std::shared_ptr<Component> get(std::size_t n);

		const std::shared_ptr<Component> get(std::size_t n) const;

		template<typename T>
		std::shared_ptr<T> get(std::size_t n)
		{
			static_assert(std::is_base_of<Component, T>::value, "Invalid component type.");

			return std::static_pointer_cast<T>(get(n));
		}
		template<typename T>
		std::shared_ptr<T> get(std::size_t n) const
		{
			static_assert(std::is_base_of<Component, T>::value, "Invalid component type.");

			return std::static_pointer_cast<T>(get(n));
		}

		virtual void destroy(std::size_t n);


		template <typename T, typename ... Args>
		std::weak_ptr<T> set(unsigned int index, Args && ... args)
		{
			auto element = std::make_shared<T>(std::forward<Args>(args) ...);
			data[index] = std::move(element);
			return std::static_pointer_cast<T>(data[index]);
		}

		std::weak_ptr<Component> set(unsigned int index, std::shared_ptr<Component> component);


	private:
		std::vector<std::shared_ptr<Component>> data;
	};

}  // namespace entityx
