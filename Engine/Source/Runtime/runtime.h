#pragma once

#include "Core/core.h"
#include "Graphics/graphics.h"

#include "System/Application.h"
#include "System/FileSystem.h"
#include "System/FileSystemWatcher.h"
#include "System/Timer.h"
#include "System/MessageBox.h"
#include "Rendering/RenderPass.h"
#include "Rendering/Material.h"
#include "Rendering/Program.h"
#include "Rendering/Shader.h"
#include "Rendering/Mesh.h"
#include "Rendering/Texture.h"
#include "Rendering/Model.h"
#include "Rendering/Debug/DebugDraw.h"
#include "Rendering/Light.h"

#include "Ecs/World.h"
#include "Ecs/Components/TransformComponent.h"
#include "Ecs/Components/CameraComponent.h"
#include "Ecs/Components/ModelComponent.h"
#include "Ecs/Components/LightComponent.h"

#include "Ecs/Systems/TransformSystem.h"
#include "Ecs/Systems/CameraSystem.h"
#include "Ecs/Systems/RenderingSystem.h"

#include "Meta/Math/Vector.hpp"
#include "Meta/Math/Transform.hpp"

#include "Meta/Ecs/Components/Component.hpp"
#include "Meta/Ecs/Components/TransformComponent.hpp"
#include "Meta/Ecs/Components/CameraComponent.hpp"
#include "Meta/Ecs/Components/ModelComponent.hpp"
#include "Meta/Ecs/Components/LightComponent.hpp"
#include "Meta/Ecs/Entity.hpp"

#include "Meta/Rendering/Material.hpp"
#include "Meta/Rendering/Model.hpp"
#include "Meta/Rendering/Program.hpp"
#include "Meta/Rendering/Texture.hpp"
#include "Meta/Rendering/Mesh.hpp"
#include "Meta/Rendering/Camera.hpp"

#include "Meta/Core/BaseTypes.hpp"
