#pragma once
#include <memory>

namespace audio
{
namespace priv
{
class listener_impl;
}

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : listener (Class)
/// <summary>
/// Receiver of a sound. It can control 3d spatial and listener properties like
/// 3d position, 3d orientation, volume etc.
/// </summary>
//-----------------------------------------------------------------------------
class listener
{
public:
	listener();
	~listener();

	//-----------------------------------------------------------------------------
	//  Name : set_volume ()
	/// <summary>
	/// A value of 1.0 means unattenuated. Each division by 2 equals an attenuation
	/// of about -6dB. Each multiplicaton by 2 equals an amplification of about +6dB.
	/// A value of 0.0 is meaningless with respect to a logarithmic scale; it is
	/// silent.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_volume(float volume);

	//-----------------------------------------------------------------------------
	//  Name : set_position ()
	/// <summary>
	/// The listener location in three dimensional space.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_position(const float* position3);

	//-----------------------------------------------------------------------------
	//  Name : set_velocity ()
	/// <summary>
	/// Specifies the current velocity in local space.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_velocity(const float* velocity3);

	//-----------------------------------------------------------------------------
	//  Name : set_orientation ()
	/// <summary>
	/// Effectively two three dimensional vectors. The first vector is the front (or
	/// "at") and the second is the top (or "up").
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_orientation(const float* direction3, const float* up3);

private:
	/// pimpl idiom
	std::unique_ptr<priv::listener_impl> _impl;
};
}
