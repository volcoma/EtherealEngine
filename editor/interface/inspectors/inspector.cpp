#include "inspector.h"

REFLECT(inspector)
{
	rttr::registration::class_<inspector>("inspector");
}

void Tooltip(const rttr::property& prop)
{
	if(gui::IsItemHovered())
	{
		//gui::SetMouseCursor(ImGuiMouseCursor_Help);
		auto tooltip = prop.get_metadata("tooltip");
		if(tooltip)
		{
			gui::BeginTooltip();
			gui::TextUnformatted(tooltip.to_string().c_str());
			gui::EndTooltip();
		}
	}
}

void Tooltip(const std::string& tooltip)
{
	if(gui::IsItemHovered())
	{
		//gui::SetMouseCursor(ImGuiMouseCursor_Help);
		gui::BeginTooltip();
		gui::TextUnformatted(tooltip.c_str());
		gui::EndTooltip();
	}
}
property_layout::property_layout(const rttr::property& prop, bool columns /*= true*/)
{
	std::string pretty_name = prop.get_name().to_string();
	auto meta_pretty_name = prop.get_metadata("pretty_name");
	if(meta_pretty_name)
		pretty_name = meta_pretty_name.get_value<std::string>();

	if (columns)
	{
		gui::Columns(1);
		gui::Columns(2, nullptr, false);
	}

	gui::AlignTextToFramePadding();
	gui::TextUnformatted(pretty_name.c_str());

	Tooltip(prop);

	gui::NextColumn();

	gui::PushID(pretty_name.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

property_layout::property_layout(const std::string& name, bool columns /*= true*/)
{
	if (columns)
	{
		gui::Columns(1);
		gui::Columns(2, nullptr, false);
	}

	gui::AlignTextToFramePadding();
	gui::TextUnformatted(name.c_str());

	gui::NextColumn();

	gui::PushID(name.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

property_layout::property_layout(const std::string& name, const std::string& tooltip, bool columns /*= true*/)
{
	if (columns)
	{
		gui::Columns(1);
		gui::Columns(2, nullptr, false);
	}

	gui::AlignTextToFramePadding();
	gui::TextUnformatted(name.c_str());

	Tooltip(tooltip);

	gui::NextColumn();

	gui::PushID(name.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

property_layout::~property_layout()
{
	gui::PopItemWidth();
	gui::PopID();
	gui::Columns(1);
}
