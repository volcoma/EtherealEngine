#pragma once

namespace audio
{

namespace priv
{
////////////////////////////////////////////////////////////
/// Let's define a macro to quickly check every OpenAL API call
////////////////////////////////////////////////////////////
// The do-while loop is needed so that al_check can be used as a single statement in if/else branches
#define al_check(expr)                                                                                       \
	do                                                                                                       \
	{                                                                                                        \
		expr;                                                                                                \
		audio::priv::al_check_error(__FILE__, __LINE__, #expr);                                              \
	} while(false)

////////////////////////////////////////////////////////////
/// Check the last OpenAL error
///
/// \param file Source file where the call is located
/// \param line Line number of the source file where the call is located
/// \param expression The evaluated expression as a string
///
////////////////////////////////////////////////////////////
void al_check_error(const char* file, unsigned int line, const char* expression);

} // namespace priv
}
