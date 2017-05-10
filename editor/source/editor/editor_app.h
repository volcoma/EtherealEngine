#pragma once

#include "runtime/system/app.h"

namespace editor
{
	struct editor_app : public runtime::app
	{
		//-----------------------------------------------------------------------------
		//  Name : start (virtual )
		/// <summary>
		///  setup after engine initialization and before running the main loop
		/// </summary>
		//-----------------------------------------------------------------------------
		virtual void start();
	};

}