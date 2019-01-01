#ifndef MML_CLIPBOARDIMPLWIN32_HPP
#define MML_CLIPBOARDIMPLWIN32_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <string>

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Give access to the system clipboard
///
////////////////////////////////////////////////////////////
class clipboard_impl
{
public:
	////////////////////////////////////////////////////////////
	/// \brief Get the content of the clipboard as string data
	///
	/// This function returns the content of the clipboard
	/// as a string. If the clipboard does not contain string
	/// it returns an empty string object.
	///
	/// \return Current content of the clipboard
	///
	////////////////////////////////////////////////////////////
	static std::string get_string();

	////////////////////////////////////////////////////////////
	/// \brief Set the content of the clipboard as string data
	///
	/// This function sets the content of the clipboard as a
	/// string.
	///
	/// \param text string object containing the data to be sent
	/// to the clipboard
	///
	////////////////////////////////////////////////////////////
	static void set_string(const std::string& text);
};

} // namespace priv

} // namespace mml

#endif // MML_CLIPBOARDIMPLWIN32_HPP
