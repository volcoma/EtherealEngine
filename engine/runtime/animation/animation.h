#pragma once
#include "core/math/math_includes.h"
#include <string>
#include <vector>

namespace runtime
{

enum class anim_behaviour
{
	/// The value from the default node transformation is taken.
	DEFAULT = 0x0,

	/// The nearest key value is used without interpolation.
	CONSTANT = 0x1,

	/// The value of the nearest two keys is linearly
	/// extrapolated for the current time value.
	LINEAR = 0x2,

	/// The animation is repeated.
	///
	/// If the animation key go from n to m and the current
	/// time is t, use the value at (t-n) % (|m-n|).
	REPEAT = 0x3,
};

struct node_animation
{
	template <typename T>
	struct key
	{
		double time = 0.0;
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
	/// given as quaternions,  which are 4D vectors.
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

	/// Defines how the animation behaves before the first
	/// key is encountered.
	///
	/// The default value is anim_behaviour::DEFAULT (the original
	/// transformation matrix of the affected node is used).
	anim_behaviour pre_state = anim_behaviour::DEFAULT;

	/// Defines how the animation behaves after the last
	/// key was processed.
	///
	/// The default value is anim_behaviour::DEFAULT (the original
	/// transformation matrix of the affected node is taken).
	anim_behaviour post_state = anim_behaviour::DEFAULT;
};

struct animation
{
	/// The name of the animation. If the modeling package this data was
	/// exported from does support only a single animation channel, this
	/// name is usually empty (length is zero).
	std::string name;

	/// Duration of the animation in ticks.
	double duration = 0.0;

	/// Ticks per second. 0 if not specified in the imported file
	double ticks_per_second = 0.0;

	/// The node animation channels. Each channel affects a single node.
	std::vector<node_animation> channels;
};
}
