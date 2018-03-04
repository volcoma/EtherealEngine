#pragma once
#include "core/math/math_includes.h"
#include <chrono>
#include <string>
#include <vector>
namespace runtime
{

struct node_animation
{
	using seconds_t = std::chrono::duration<float>;
	template <typename T>
	struct key
	{
		seconds_t time = seconds_t(0);
		T value = {};
	};

	/// The name of the node affected by this animation. The node
	/// must exist and it must be unique.
	std::string node_name;

	/// The position keys of this animation channel. Positions are
	/// specified as 3D vector.
	///
	/// If there are position keys, there will also be at least one
	/// scaling and one rotation key.
	std::vector<key<math::vec3>> position_keys;

	/// The rotation keys of this animation channel. Rotations are
	/// given as quaternions.
	///
	/// If there are rotation keys, there will also be at least one
	/// scaling and one position key.
	std::vector<key<math::quat>> rotation_keys;

	/// The scaling keys of this animation channel. Scalings are
	/// specified as 3D vector.
	///
	/// If there are scaling keys, there will also be at least one
	/// position and one rotation key.
	std::vector<key<math::vec3>> scaling_keys;
};

struct animation
{
    using seconds_t = node_animation::seconds_t;
	/// The name of the animation. If the modeling package this data was
	/// exported from does support only a single animation channel, this
	/// name is usually empty.
	std::string name;

	/// Duration of the animation in seconds
	seconds_t duration = seconds_t(0);

	/// The node animation channels. Each channel affects a single node.
	std::vector<node_animation> channels;
};

// elapsed_time = math::modf(elapsed_time + dt , anim.duration);

} // namespace runtime
