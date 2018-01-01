#pragma once

namespace audio
{

namespace priv
{
////////////////////////////////////////////////////////////
/// Let's define a macro to quickly check every OpenAL API call
////////////////////////////////////////////////////////////
//#if $is($debug)
// If in debug mode, perform a test on every call
// The do-while loop is needed so that alCheck can be used as a single statement in if/else branches
#define alCheck(expr)                                                                                        \
	do                                                                                                       \
	{                                                                                                        \
		expr;                                                                                                \
		audio::priv::alCheckError(__FILE__, __LINE__, #expr);                                                \
	} while(false)

//#else

// Else, we don't add any overhead
//    #define alCheck(expr) (expr)

//#endif

////////////////////////////////////////////////////////////
/// Check the last OpenAL error
///
/// \param file Source file where the call is located
/// \param line Line number of the source file where the call is located
/// \param expression The evaluated expression as a string
///
////////////////////////////////////////////////////////////
void alCheckError(const char* file, unsigned int line, const char* expression);

} // namespace priv
}
