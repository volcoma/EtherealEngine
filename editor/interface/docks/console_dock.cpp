#include "console_dock.h"
#include "../../console/console_log.h"

void console_dock::render(const ImVec2& area)
{
	if (!_console_log)
		return;

	gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 180);
	gui::PopStyleVar();
	// TODO: display items starting from the bottom
	gui::SameLine();
	if (gui::SmallButton("Clear"))
	{
		_console_log->clearLog();
	}
	gui::Separator();

	// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use gui::TextUnformatted(log.begin(), log.end());
	// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
	// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
	gui::BeginChild("ScrollingRegion", ImVec2(0, -gui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (gui::BeginPopupContextWindow())
	{
		if (gui::Selectable("Clear"))
			_console_log->clearLog();
		gui::EndPopup();
	}
	gui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	auto items = _console_log->get_items();

	for (auto& pair_msg : items)
	{
		const char* item_cstr = pair_msg.first.c_str();
		if (!filter.PassFilter(item_cstr))
			continue;
		const auto& colorization = _console_log->get_level_colorization(pair_msg.second);
		ImVec4 col = { colorization[0], colorization[1], colorization[2], colorization[3] };
		gui::PushStyleColor(ImGuiCol_Text, col);
		gui::TextWrapped(item_cstr);
		gui::PopStyleColor();
	}
	if (_console_log->get_pending_entries() > 0)
		gui::SetScrollHere();

	_console_log->set_pending_entries(0);

	gui::PopStyleVar();
	gui::EndChild();
	gui::Separator();

	// Command-line
	std::string inputBuff;
	inputBuff.resize(64, 0);
	inputBuff.shrink_to_fit();
	gui::PushItemWidth(gui::GetContentRegionAvailWidth() * 0.5f);
	if (gui::InputText(
		"Enter Command",
		&inputBuff[0],
		inputBuff.size(),
		ImGuiInputTextFlags_EnterReturnsTrue)
		)
	{
		// copy from c_str to remove trailing zeros
		std::string command = inputBuff.c_str();
		std::string errorMsg = _console_log->process_input(command);
		if (errorMsg != "")
		{
			APPLOG_NOTICE(errorMsg.c_str());
		}
		// Demonstrate keeping auto focus on the input box
		if (gui::IsItemHovered() || (gui::IsRootWindowOrAnyChildFocused() && !gui::IsAnyItemActive() && !gui::IsMouseClicked(0)))
			gui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}

	gui::PopItemWidth();

}

