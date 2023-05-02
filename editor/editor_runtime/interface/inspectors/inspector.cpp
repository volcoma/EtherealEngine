#include "inspector.h"

void Tooltip(const rttr::property& prop)
{
	auto& g = *gui::GetCurrentContext();
	if(!g.DragDropActive && gui::IsItemHovered())
	{
		gui::SetMouseCursor(ImGuiMouseCursor_Help);
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
	auto& g = *gui::GetCurrentContext();
	if(!g.DragDropActive && gui::IsItemHovered())
	{
		gui::SetMouseCursor(ImGuiMouseCursor_Help);
		gui::BeginTooltip();
		gui::TextUnformatted(tooltip.c_str());
		gui::EndTooltip();
	}
}
property_layout::property_layout(const rttr::property& prop, bool columns /*= true*/)
{
    columns_ = columns;
	std::string pretty_name = prop.get_name().to_string();
	auto meta_pretty_name = prop.get_metadata("pretty_name");
	if(meta_pretty_name)
	{
		pretty_name = meta_pretty_name.get_value<std::string>();
	}

    name_ = pretty_name;

    push_layout();
}

property_layout::property_layout(const std::string& name, bool columns /*= true*/)
{
    columns_ = columns;

    push_layout();
}

property_layout::property_layout(const std::string& name, const std::string& tooltip, bool columns /*= true*/)
{
    columns_ = columns;
    name_ = name;

    push_layout();
}

property_layout::~property_layout()
{
    pop_layout();
}

void property_layout::push_layout()
{
    if(columns_)
	{
//        if(gui::TableGetColumnCount() > 1)
//		{
//            gui::PopID();
//            gui::PopItemWidth();
//			gui::EndTable();
//		}
		gui::BeginTable(("properties##" + name_).c_str(), 2);
        gui::TableNextRow();
        gui::TableNextColumn();
	}

	gui::AlignTextToFramePadding();
	gui::TextUnformatted(name_.c_str());

    if(columns_)
    {
        gui::TableNextColumn();
    }

    gui::PushID(name_.c_str());
	gui::PushItemWidth(gui::GetContentRegionAvail().x);
}

void property_layout::pop_layout()
{
    gui::PopID();
	gui::PopItemWidth();
    if(columns_)
    {
        if(gui::TableGetColumnCount() > 1)
        {
            gui::EndTable();
        }
    }
}

void inspector::before_inspect(const rttr::property& prop)
{
    layout_ = std::make_unique<property_layout>(prop);
}

void inspector::after_inspect(const rttr::property& prop)
{
    layout_.reset();
}
