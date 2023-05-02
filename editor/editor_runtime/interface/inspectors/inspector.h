#pragma once

#include <core/reflection/reflection.h>
#include <core/reflection/registration.h>

#include <editor_core/gui/gui.h>

struct property_layout
{
	property_layout(const rttr::property& prop, bool columns = true);

	property_layout(const std::string& name, bool columns = true);

	property_layout(const std::string& name, const std::string& tooltip, bool columns = true);

	~property_layout();

    void push_layout();
    void pop_layout();

	std::string name_;
    bool columns_{};
};


struct inspector
{
	REFLECTABLEV(inspector)

	using meta_getter = std::function<rttr::variant(const rttr::variant&)>;

	virtual ~inspector() = default;

    virtual void before_inspect(const rttr::property& prop);
    virtual void after_inspect(const rttr::property& prop);
	virtual bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata) = 0;


    std::unique_ptr<property_layout> layout_{};
};

REFLECT_INLINE(inspector)
{
	rttr::registration::class_<inspector>("inspector");
}
#define INSPECTOR_REFLECT(inspector_type, inspected_type)                                                    \
	REFLECT_INLINE(inspector_type)                                                                           \
	{                                                                                                        \
		rttr::registration::class_<inspector_type>(#inspector_type)(                                         \
			rttr::metadata("inspected_type", rttr::type::get<inspected_type>()))                             \
			.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                         \
	}

#define DECLARE_INSPECTOR(inspector_type, inspected_type)                                                    \
	struct inspector_type : public inspector                                                                 \
	{                                                                                                        \
		REFLECTABLEV(inspector_type, inspector)                                                              \
		bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);                   \
	};                                                                                                       \
	INSPECTOR_REFLECT(inspector_type, inspected_type)
