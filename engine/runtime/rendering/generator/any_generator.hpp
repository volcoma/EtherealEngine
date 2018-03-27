#ifndef GENERATOR_ANYGENERATOR_HPP
#define GENERATOR_ANYGENERATOR_HPP

#include <memory>

namespace generator
{

/// A type erasing container that can store any generator that generates type T.
/// @tparam T Type returned by the generate() -function.
template <typename T>
class any_generator
{
public:
	template <typename generator>
	any_generator(generator gen)
		: base_{new derived<generator>{std::move(gen)}}
	{
	}

	any_generator(const any_generator& that)
		: base_{that.base_->clone()}
	{
	}

	any_generator(any_generator&&) = default;

	any_generator& operator=(const any_generator& that)
	{
		base_ = that.base_->clone();
		return *this;
	}

	any_generator& operator=(any_generator&&) = default;

	T generate() const
	{
		return base_->generate();
	}

	bool done() const noexcept
	{
		return base_->done();
	}

	void next()
	{
		base_->next();
	}

private:
	class base
	{
	public:
		virtual ~base() = default;
		virtual std::unique_ptr<base> clone() const = 0;
		virtual T generate() const = 0;
		virtual bool done() const noexcept = 0;
		virtual void next() = 0;
	};

	template <typename generator>
	class derived : public base
	{
	public:
		derived(generator gen)
			: generator_{std::move(gen)}
		{
		}

		std::unique_ptr<base> clone() const override
		{
			return std::unique_ptr<base>(new derived{generator_});
		}

		T generate() const override
		{
			return generator_.generate();
		}

		bool done() const noexcept override
		{
			return generator_.done();
		}

		void next() override
		{
			generator_.next();
		}

	private:
		generator generator_;
	};

	std::unique_ptr<base> base_;
};
}

#endif
