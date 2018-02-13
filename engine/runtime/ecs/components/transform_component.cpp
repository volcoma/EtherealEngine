#include "transform_component.h"
#include "core/logging/logging.h"
#include <algorithm>

void transform_component::on_entity_set()
{
	for(auto& child : children_)
	{
		if(child.valid())
		{
			auto child_transform = child.get_component<transform_component>().lock();
			if(child_transform)
			{
				child_transform->parent_ = get_entity();
			}
		}
	}
}

transform_component::~transform_component()
{
	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			parent_transform->remove_child(get_entity());
		}
	}
	for(auto& child : children_)
	{
		if(child.valid())
		{
			child.destroy();
		}
	}
}

transform_component& transform_component::move(const math::vec3& amount)
{
	math::vec3 new_pos = get_position();
	new_pos += get_x_axis() * amount.x;
	new_pos += get_y_axis() * amount.y;
	new_pos += get_z_axis() * amount.z;

	// Pass through to set_position so that any derived classes need not
	// override the 'move' method in order to catch this position change.
	set_position(new_pos);
	return *this;
}

transform_component& transform_component::move_local(const math::vec3& amount)
{
	math::vec3 new_pos = get_local_position();
	new_pos += get_local_x_axis() * amount.x;
	new_pos += get_local_y_axis() * amount.y;
	new_pos += get_local_z_axis() * amount.z;

	// Pass through to set_position so that any derived classes need not
	// override the 'move' method in order to catch this position change.
	set_local_position(new_pos);
	return *this;
}

transform_component& transform_component::set_local_position(const math::vec3& position)
{
	// Set new cell relative position
	local_transform_.set_position(position);
	set_local_transform(local_transform_);
	return *this;
}

transform_component& transform_component::set_position(const math::vec3& position)
{
	// Rotate a copy of the current math::transform.
	math::transform m = get_transform();
	m.set_position(position);

	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
			m = inv_parent_transform * m;
		}
	}

	set_local_transform(m);
	return *this;
}

math::vec3 transform_component::get_local_scale()
{
	return local_transform_.get_scale();
}

const math::vec3& transform_component::get_local_position()
{
	return local_transform_.get_position();
}

math::quat transform_component::get_local_rotation()
{
	return local_transform_.get_rotation();
}

math::vec3 transform_component::get_local_x_axis()
{
	return local_transform_.x_unit_axis();
}

math::vec3 transform_component::get_local_y_axis()
{
	return local_transform_.y_unit_axis();
}

math::vec3 transform_component::get_local_z_axis()
{
	return local_transform_.z_unit_axis();
}

const math::vec3& transform_component::get_position()
{
	return get_transform().get_position();
}

math::quat transform_component::get_rotation()
{
	return get_transform().get_rotation();
}

math::vec3 transform_component::get_x_axis()
{
	return get_transform().x_unit_axis();
}

math::vec3 transform_component::get_y_axis()
{
	return get_transform().y_unit_axis();
}

math::vec3 transform_component::get_z_axis()
{
	return get_transform().z_unit_axis();
}

math::vec3 transform_component::get_scale()
{
	return get_transform().get_scale();
}

const math::transform& transform_component::get_transform()
{
	// the transform should be resolved
	resolve();
	return world_transform_;
}

const math::transform& transform_component::get_local_transform() const
{
	// Return reference to our internal matrix
	return local_transform_;
}

transform_component& transform_component::look_at(float x, float y, float z)
{
	look_at(get_position(), math::vec3(x, y, z));
	return *this;
}

transform_component& transform_component::look_at(const math::vec3& point)
{
	look_at(get_position(), point);
	return *this;
}

transform_component& transform_component::look_at(const math::vec3& eye, const math::vec3& at)
{
	math::transform m;
	m.look_at(eye, at);

	// Update the component position / orientation through the common base method.
	math::vec3 translation;
	math::quat orientation;

	if(m.decompose(orientation, translation))
	{
		set_rotation(orientation);
		set_position(translation);
	}
	return *this;
}

transform_component& transform_component::look_at(const math::vec3& eye, const math::vec3& at,
												  const math::vec3& up)
{
	math::transform m;
	m.look_at(eye, at, up);

	// Update the component position / orientation through the common base method.
	math::vec3 translation;
	math::quat orientation;

	if(m.decompose(orientation, translation))
	{
		set_rotation(orientation);
		set_position(translation);
	}
	return *this;
}

transform_component& transform_component::rotate_local(float x, float y, float z)
{
	// Do nothing if rotation is disallowed.
	if(!can_rotate())
		return *this;

	local_transform_.rotate_local(math::radians(x), math::radians(y), math::radians(z));
	set_local_transform(local_transform_);
	return *this;
}

transform_component& transform_component::rotate_axis(float degrees, const math::vec3& axis)
{
	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if(!can_rotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::vec3 vPos = get_position();
		math::transform t;
		t.rotate_axis(math::radians(degrees), axis);
		t.transform_coord(vPos, vPos);
		set_position(vPos);

	} // End if !canRotate()
	else
	{
		// Rotate a copy of the current math::transform.
		math::transform m = get_transform();
		m.rotate_axis(math::radians(degrees), axis);

		if(parent_.valid())
		{
			auto parent_transform = parent_.get_component<transform_component>().lock();
			if(parent_transform)
			{
				math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
				m = inv_parent_transform * m;
			}
		}

		set_local_transform(m);

	} // End if canRotate()
	return *this;
}

transform_component& transform_component::rotate(float x, float y, float z)
{
	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if(!can_rotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::transform t;
		math::vec3 position = get_position();
		t.rotate(math::radians(x), math::radians(y), math::radians(z));
		t.transform_coord(position, position);
		set_position(position);

	} // End if !canRotate()
	else
	{
		// Scale a copy of the cell math::transform
		// Set orientation of new math::transform
		math::transform m = get_transform();
		m.rotate(math::radians(x), math::radians(y), math::radians(z));

		if(parent_.valid())
		{
			auto parent_transform = parent_.get_component<transform_component>().lock();
			if(parent_transform)
			{
				math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
				m = inv_parent_transform * m;
			}
		}

		set_local_transform(m);

	} // End if canRotate()
	return *this;
}

transform_component& transform_component::rotate(float x, float y, float z, const math::vec3& center)
{
	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if(!can_rotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::transform t;
		math::vec3 position = get_position() - center;
		t.rotate(math::radians(x), math::radians(y), math::radians(z));
		t.transform_coord(position, position);
		set_position(position + center);

	} // End if !canRotate()
	else
	{

		// Scale a copy of the cell math::transform
		// Set orientation of new math::transform
		math::transform m = get_transform();
		m.rotate(math::radians(x), math::radians(y), math::radians(z));

		if(parent_.valid())
		{
			auto parent_transform = parent_.get_component<transform_component>().lock();
			if(parent_transform)
			{
				math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
				m = inv_parent_transform * m;
			}
		}

		set_local_transform(m);

	} // End if canRotate()
	return *this;
}

transform_component& transform_component::set_scale(const math::vec3& s)
{
	// If scaling is disallowed, only process position change. Otherwise
	// perform full scale.
	if(!can_scale())
		return *this;

	// Scale a copy of the cell math::transform
	// Set orientation of new math::transform
	math::transform m = get_transform();
	m.set_scale(s);

	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
			m = inv_parent_transform * m;
		}
	}

	set_local_transform(m);
	return *this;
}

transform_component& transform_component::set_local_scale(const math::vec3& scale)
{
	// Do nothing if scaling is disallowed.
	if(!can_scale())
		return *this;
	local_transform_.set_scale(scale);
	set_local_transform(local_transform_);
	return *this;
}

transform_component& transform_component::set_rotation(const math::quat& rotation)
{
	// Do nothing if rotation is disallowed.
	if(!can_rotate())
		return *this;

	// Set orientation of new math::transform
	math::transform m = get_transform();
	m.set_rotation(rotation);

	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
			m = inv_parent_transform * m;
		}
	}

	set_local_transform(m);
	return *this;
}

transform_component& transform_component::set_local_rotation(const math::quat& rotation)
{
	// Do nothing if rotation is disallowed.
	if(!can_rotate())
		return *this;

	// Set orientation of new math::transform
	local_transform_.set_rotation(rotation);
	set_local_transform(local_transform_);

	return *this;
}

transform_component& transform_component::reset_rotation()
{
	// Do nothing if rotation is disallowed.
	if(!can_rotate())
		return *this;
	set_rotation(math::quat{});
	return *this;
}

transform_component& transform_component::reset_scale()
{
	// Do nothing if scaling is disallowed.
	if(!can_scale())
		return *this;

	set_scale(math::vec3{1.0f, 1.0f, 1.0f});
	return *this;
}

transform_component& transform_component::reset_local_rotation()
{
	// Do nothing if rotation is disallowed.
	if(!can_rotate())
		return *this;

	set_local_rotation(math::quat{});

	return *this;
}

transform_component& transform_component::reset_local_scale()
{
	// Do nothing if scaling is disallowed.
	if(!can_scale())
		return *this;

	set_local_scale(math::vec3{1.0f, 1.0f, 1.0f});

	return *this;
}

transform_component& transform_component::reset_pivot()
{
	// Do nothing if pivot adjustment is disallowed.
	if(!can_adjust_pivot())
		return *this;

	return *this;
}

bool transform_component::can_scale() const
{
	// Default is to allow scaling.
	return true;
}

bool transform_component::can_rotate() const
{
	// Default is to allow rotation.
	return true;
}

bool transform_component::can_adjust_pivot() const
{
	// Default is to allow pivot adjustment.
	return true;
}

transform_component& transform_component::set_parent(runtime::entity parent)
{
	set_parent(parent, true, false);

	return *this;
}

bool check_parent(const runtime::entity& e, const runtime::entity& parent)
{
	if(e == parent)
		return false;

	if(parent.valid() && (parent.has_component<transform_component>() == false))
		return false;

	if(e.valid())
	{
		auto e_transform = e.get_component<transform_component>().lock();
		if(e_transform)
		{
			for(const auto& child : e_transform->get_children())
			{
				if(false == check_parent(child, parent))
					return false;
			}
		}
	}

	return true;
}

transform_component& transform_component::set_parent(runtime::entity parent, bool world_position_stays,
													 bool local_position_stays)
{
	// Skip if this is a no-op.
	if(check_parent(get_entity(), parent) == false)
		return *this;

	// Before we do anything, make sure that all pending math::transform
	// operations are resolved (including those applied to our parent).
	math::transform cached_world_transform;
	if(world_position_stays)
	{
		resolve(true);
		cached_world_transform = get_transform();
	}
	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			parent_transform->remove_child(get_entity());
		}
	}

	parent_ = parent;

	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			parent_transform->attach_child(get_entity());
		}
	}

	if(world_position_stays)
	{
		resolve(true);
		set_transform(cached_world_transform);
	}
	else
	{
		if(!local_position_stays)
			set_local_transform(math::transform::identity);
	}

	set_dirty(is_dirty());

	// Success!
	return *this;
}

const runtime::entity& transform_component::get_parent() const
{
	return parent_;
}

void transform_component::attach_child(const runtime::entity& child)
{
	children_.push_back(child);

	set_dirty(is_dirty());
}

void transform_component::remove_child(const runtime::entity& child)
{
	children_.erase(std::remove_if(std::begin(children_), std::end(children_),
								   [&child](const auto& other) { return child == other; }),
					std::end(children_));
}

void transform_component::cleanup_dead_children()
{
	children_.erase(std::remove_if(std::begin(children_), std::end(children_),
								   [](const auto& other) { return other.valid() == false; }),
					std::end(children_));
}

transform_component& transform_component::set_transform(const math::transform& tr)
{
	if(world_transform_.compare(tr, 0.0001f) == 0)
		return *this;

	math::vec3 position, scaling;
	math::quat orientation;
	tr.decompose(scaling, orientation, position);

	math::transform m = get_transform();
	m.set_scale(scaling);
	m.set_rotation(orientation);
	m.set_position(position);

	if(parent_.valid())
	{
		auto parent_transform = parent_.get_component<transform_component>().lock();
		if(parent_transform)
		{
			math::transform inv_parent_transform = math::inverse(parent_transform->get_transform());
			m = inv_parent_transform * m;
		}
	}

	set_local_transform(m);
	return *this;
}

transform_component& transform_component::set_local_transform(const math::transform& trans)
{
	if(local_transform_.compare(trans, 0.0001f) == 0)
		return *this;

	set_dirty(true);

	local_transform_ = trans;
	return *this;
}

void transform_component::resolve(bool force)
{
	if(force || is_dirty())
	{
		if(parent_.valid())
		{
			auto parent_transform = parent_.get_component<transform_component>().lock();
			if(parent_transform)
			{
				world_transform_ = parent_transform->get_transform() * local_transform_;
			}
			else
			{
				world_transform_ = local_transform_;
			}
		}
		else
		{
			world_transform_ = local_transform_;
		}

		set_dirty(false);
	}
}

bool transform_component::is_dirty() const
{
	return dirty_;
}

void transform_component::set_dirty(bool dirty)
{
	dirty_ = dirty;

	if(dirty_ == true)
	{
		touch();

		for(const auto& child : children_)
		{
			if(child.valid())
			{
				auto child_transform = child.get_component<transform_component>().lock();
				if(child_transform)
				{
					child_transform->set_dirty(dirty);
				}
			}
		}
	}
}

const std::vector<runtime::entity>& transform_component::get_children() const
{
	return children_;
}
