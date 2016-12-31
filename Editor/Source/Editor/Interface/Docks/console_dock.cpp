#include "docks.h"
#include "../../console/console_log.h"

namespace Docks
{
	void renderConsole(ImVec2 area, ConsoleLog& console)
	{
		gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		static ImGuiTextFilter filter;
		filter.Draw("Filter", 180);
		gui::PopStyleVar();
		// TODO: display items starting from the bottom
		gui::SameLine();
		if (gui::SmallButton("Clear"))
		{
			console.clearLog();
		}
		gui::Separator();

		// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use gui::TextUnformatted(log.begin(), log.end());
		// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
		// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
		gui::BeginChild("ScrollingRegion", ImVec2(0, -gui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
		if (gui::BeginPopupContextWindow())
		{
			if (gui::Selectable("Clear"))
				console.clearLog();
			gui::EndPopup();
		}
		gui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

		ConsoleLog::ItemContainer itemsCopy = console.getItems();

		static std::map<logging::level::level_enum, ImVec4> colorMappings
		{
			{ logging::level::trace,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::debug,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::info,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::notice,{ 1.0f, 5.0f, 0.0f, 1.0f } },
			{ logging::level::warn,{ 1.0f, 0.494f, 0.0f, 1.0f } },
			{ logging::level::err,{ 1.0f, 0.0f, 0.0f, 1.0f } },
			{ logging::level::critical,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::alert,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::emerg,{ 1.0f, 1.0f, 1.0f, 1.0f } },
			{ logging::level::off,{ 1.0f, 1.0f, 1.0f, 1.0f } },

		};

		for (auto& pairMsg : itemsCopy)
		{
			const char* itemCstr = pairMsg.first.c_str();
			if (!filter.PassFilter(itemCstr))
				continue;
			ImVec4 col = colorMappings[pairMsg.second];
			gui::PushStyleColor(ImGuiCol_Text, col);
			gui::TextWrapped(itemCstr);
			gui::PopStyleColor();
		}
		if (console.getPendingEntries() > 0)
			gui::SetScrollHere();

		console.setPendingEntries(0);

		gui::PopStyleVar();
		gui::EndChild();
		gui::Separator();

		// Command-line
		std::string inputBuff;
		inputBuff.resize(64, 0);
		inputBuff.shrink_to_fit();
		if (gui::InputText(
			"Enter Command", 
			&inputBuff[0],
			inputBuff.size(),
			ImGuiInputTextFlags_EnterReturnsTrue)
			)
		{
			// copy from c_str to remove trailing zeros
			std::string command = inputBuff.c_str();
			std::string errorMsg = console.processInput(command);
			if (errorMsg != "")
			{
				auto logger = logging::get("Log");
				logger->notice() << errorMsg;
			}
			// Demonstrate keeping auto focus on the input box
			if (gui::IsItemHovered() || (gui::IsRootWindowOrAnyChildFocused() && !gui::IsAnyItemActive() && !gui::IsMouseClicked(0)))
				gui::SetKeyboardFocusHere(-1); // Auto focus previous widget
		}
	}

};