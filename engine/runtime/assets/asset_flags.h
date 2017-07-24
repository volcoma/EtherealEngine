#pragma once

namespace runtime
{

enum class load_mode
{
	sync,
	async
};

enum class load_flags
{
	standard,
	reload,
	do_not_unload
};
}
