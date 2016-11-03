#include "ConsoleView.h"


ConsoleView::ConsoleView()
{
	memset(InputBuf, 0, sizeof(InputBuf));
	mName = "Console";
}


ConsoleView::~ConsoleView()
{
}

void ConsoleView::clearLog()
{
	Items.clear();
	ScrollToBottom = true;
}

std::shared_ptr<ConsoleView> ConsoleView::instance()
{
	static std::shared_ptr<ConsoleView> instance = std::make_shared<ConsoleView>();
	return instance;
}

void ConsoleView::_sink_it(const spdlog::details::log_msg& msg)
{
	Items.push_back({ msg.formatted.c_str(), msg.level });
	flush();
	ScrollToBottom = true;
}

void ConsoleView::flush()
{

}

void ConsoleView::render(AppWindow& window)
{

	ImGuiIO& io = gui::GetIO();
	ImVec2 sz = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
	gui::SetNextWindowPos(ImVec2(sz.x - sz.x / 2, sz.y - sz.y / 2));
	gui::SetNextWindowSize(sz, ImGuiSetCond_FirstUseEver);
	if (!gui::BeginDock(mName.c_str(), &mOpen))
	{
		gui::EndDock();
		return;
	}

	gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	static ImGuiTextFilter filter;
	filter.Draw("Filter", 180);
	gui::PopStyleVar();
	// TODO: display items starting from the bottom
	gui::SameLine();
	if (gui::SmallButton("Clear"))
	{
		clearLog();
	}
	gui::Separator();

	// Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use gui::TextUnformatted(log.begin(), log.end());
	// NB- if you have thousands of entries this approach may be too inefficient. You can seek and display only the lines that are visible - CalcListClipping() is a helper to compute this information.
	// If your items are of variable size you may want to implement code similar to what CalcListClipping() does. Or split your data into fixed height items to allow random-seeking into your list.
	gui::BeginChild("ScrollingRegion", ImVec2(0, -gui::GetItemsLineHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
	if (gui::BeginPopupContextWindow())
	{
		if (gui::Selectable("Clear"))
			clearLog();
		gui::EndPopup();
	}
	gui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	ItemContainer itemsCopy;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		itemsCopy = Items;
	}

	static std::map<logging::level::level_enum, ImVec4> colorMappings
	{
		{ logging::level::trace, {1.0f, 1.0f, 1.0f, 1.0f}},
		{ logging::level::debug,{ 1.0f, 1.0f, 1.0f, 1.0f } },
		{ logging::level::info,{ 1.0f, 1.0f, 1.0f, 1.0f } },
		{ logging::level::notice,{ 1.0f, 1.0f, 1.0f, 1.0f } },
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
	if (ScrollToBottom)
		gui::SetScrollHere();
	ScrollToBottom = false;

	gui::PopStyleVar();
	gui::EndChild();
	gui::Separator();
	auto lambda = [](ImGuiTextEditCallbackData* data) -> int
	{
		return 0;
	};
	// Command-line
	if (gui::InputText("Input", InputBuf, math::countof(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, lambda, (void*)this))
	{
		char* input_end = InputBuf + strlen(InputBuf);
		while (input_end > InputBuf && input_end[-1] == ' ') input_end--; *input_end = 0;
		std::string command(&InputBuf[0]);
		//if (command != "")
		//	executeCommand(t_command);
		memset(InputBuf, 0, sizeof(InputBuf));

		// Demonstrate keeping auto focus on the input box
		if (gui::IsItemHovered() || (gui::IsRootWindowOrAnyChildFocused() && !gui::IsAnyItemActive() && !gui::IsMouseClicked(0)))
			gui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}


	gui::EndDock();
}
