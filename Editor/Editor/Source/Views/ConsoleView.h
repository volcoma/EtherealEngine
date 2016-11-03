#pragma once

#include "IEditorView.h"
#include "Runtime/runtime.h"

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : ConsoleView (Class)
/// <summary>
/// 
/// </summary>
//-----------------------------------------------------------------------------
class ConsoleView : public IEditorView, public logging::sinks::base_sink<std::mutex>//, public IConsole
{
public:
    //-------------------------------------------------------------------------
    // Constructors & Destructors
    //-------------------------------------------------------------------------
	ConsoleView();
	~ConsoleView();
    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
	static std::shared_ptr<ConsoleView> instance();

	void _sink_it(const logging::details::log_msg& msg) override;

	void flush() override;
    // Updates
    virtual void				render			(AppWindow& window) override;
	//-------------------------------------------------------------------------
	// Public Virtual Methods (LogOutput)
	//-------------------------------------------------------------------------
	void						clearLog		( );

private:
	using ItemContainer = std::vector<std::pair<std::string, logging::level::level_enum>>;
	char						InputBuf[256];
	ItemContainer				Items;
	bool						ScrollToBottom = true;
};