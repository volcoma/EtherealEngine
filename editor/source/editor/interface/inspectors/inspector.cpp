#include "inspector.h"

void Tooltip(const rttr::property& prop)
{
	if (gui::IsItemHovered())
	{
		gui::SetMouseCursor(ImGuiMouseCursor_Help);
		auto tooltip = prop.get_metadata("Tooltip");
		if (tooltip)
		{
			gui::SetTooltip(tooltip.to_string().c_str());
		}
	}
}

void Tooltip(const std::string& tooltip)
{
	if (gui::IsItemHovered())
	{
		gui::SetMouseCursor(ImGuiMouseCursor_Help);
		gui::SetTooltip(tooltip.c_str());	
	}
}

PropertyLayout::PropertyLayout(const rttr::property& prop, bool columns /*= true*/)
{
	
	if (columns)
		gui::Columns(2, nullptr, false);

	gui::TextUnformatted(prop.get_name().data());
	Tooltip(prop);
	if (columns)
		gui::SetColumnOffset(1, 140.0f);
	gui::NextColumn();

	gui::PushID(prop.get_name().data());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

PropertyLayout::PropertyLayout(const std::string& name, bool columns /*= true*/)
{
	if (columns)
		gui::Columns(2, nullptr, false);

	gui::TextUnformatted(name.c_str());
	if (columns)
		gui::SetColumnOffset(1, 140.0f);
	gui::NextColumn();

	gui::PushID(name.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

PropertyLayout::PropertyLayout(const std::string& name, const std::string& tooltip, bool columns /*= true*/)
{
	if (columns)
		gui::Columns(2, nullptr, false);

	gui::TextUnformatted(name.c_str());
	Tooltip(tooltip);
	if (columns)
		gui::SetColumnOffset(1, 140.0f);
	gui::NextColumn();

	gui::PushID(name.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvailWidth());
}

PropertyLayout::~PropertyLayout()
{
	gui::PopItemWidth();
	gui::PopID();
	gui::Columns(1);
}
