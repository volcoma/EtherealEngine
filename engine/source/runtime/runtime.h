#pragma once

#include "core/core.h"
#include "graphics/graphics.h"

#include "system/singleton.h"
#include "system/app.h"
#include "system/filesystem.h"
#include "system/filesystem_watcher.hpp"
#include "rendering/render_pass.h"
#include "rendering/material.h"
#include "rendering/program.h"
#include "rendering/shader.h"
#include "rendering/mesh.h"
#include "rendering/texture.h"
#include "rendering/model.h"
#include "rendering/debugdraw/debugdraw.h"
#include "rendering/light.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/model_component.h"
#include "ecs/components/light_component.h"

#include "ecs/systems/scene_graph.h"
#include "ecs/systems/camera_system.h"
#include "ecs/systems/deferred_rendering.h"

#include "meta/math/vector.hpp"
#include "meta/math/transform.hpp"

#include "meta/ecs/components/component.hpp"
#include "meta/ecs/components/transform_component.hpp"
#include "meta/ecs/components/camera_component.hpp"
#include "meta/ecs/components/model_component.hpp"
#include "meta/ecs/components/light_component.hpp"
#include "meta/ecs/entity.hpp"

#include "meta/rendering/material.hpp"
#include "meta/rendering/model.hpp"
#include "meta/rendering/program.hpp"
#include "meta/rendering/texture.hpp"
#include "meta/rendering/mesh.hpp"
#include "meta/rendering/camera.hpp"

#include "meta/core/basetypes.hpp"
