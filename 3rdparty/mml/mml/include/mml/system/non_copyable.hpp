#ifndef MML_NONCOPYABLE_HPP
#define MML_NONCOPYABLE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/system/export.hpp>

namespace mml
{
////////////////////////////////////////////////////////////
/// \brief Utility class that makes any derived
///        class non-copyable
///
////////////////////////////////////////////////////////////
class MML_SYSTEM_API non_copyable
{
protected:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// Because this class has a copy constructor, the compiler
    /// will not automatically generate the default constructor.
    /// That's why we must define it explicitly.
    ///
    ////////////////////////////////////////////////////////////
    non_copyable() {}

private:

    ////////////////////////////////////////////////////////////
    /// \brief Disabled copy constructor
    ///
    /// By making the copy constructor private, the compiler will
    /// trigger an error if anyone outside tries to use it.
    /// To prevent non_copyable or friend classes from using it,
    /// we also give no definition, so that the linker will
    /// produce an error if the first protection was inefficient.
    ///
    ////////////////////////////////////////////////////////////
    non_copyable(const non_copyable&);

    ////////////////////////////////////////////////////////////
    /// \brief Disabled assignment operator
    ///
    /// By making the assignment operator private, the compiler will
    /// trigger an error if anyone outside tries to use it.
    /// To prevent non_copyable or friend classes from using it,
    /// we also give no definition, so that the linker will
    /// produce an error if the first protection was inefficient.
    ///
    ////////////////////////////////////////////////////////////
    non_copyable& operator =(const non_copyable&);
};

} // namespace mml


#endif // MML_NONCOPYABLE_HPP