#ifndef GENERATOR_ANYPATH_HPP
#define GENERATOR_ANYPATH_HPP

#include <memory>

#include "any_generator.hpp"
#include "edge.hpp"
#include "path_vertex.hpp"

namespace generator
{

/// A type erasing container that can store any path.
class any_path
{
public:
	template <typename path_t>
	any_path(path_t path)
		: base_{new derived<path_t>{std::move(path)}}
	{
	}

	any_path(const any_path& that);

	any_path(any_path&&) = default;

	any_path& operator=(const any_path& that);

	any_path& operator=(any_path&&) = default;

	any_generator<edge_t> edges() const noexcept;

	any_generator<path_vertex_t> vertices() const noexcept;

private:
	class base
	{
	public:
		virtual ~base();
		virtual std::unique_ptr<base> clone() const = 0;
		virtual any_generator<edge_t> edges() const = 0;
		virtual any_generator<path_vertex_t> vertices() const = 0;
	};

	template <typename path_t>
	class derived : public base
	{
	public:
		derived(path_t path)
			: path_(std::move(path))
		{
		}

		std::unique_ptr<base> clone() const override
		{
			return std::unique_ptr<base>{new derived{path_}};
		}

		any_generator<edge_t> edges() const override
		{
			return path_.edges();
		}

		any_generator<path_vertex_t> vertices() const override
		{
			return path_.vertices();
		}

		path_t path_;
	};

	std::unique_ptr<base> base_;
};
}

#endif
