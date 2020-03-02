// Engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <glad/glad.h>

#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Camera/CameraSystem.h"
#include "Systems/Pipeline/MeshLoader.h"
#include "Systems/Physics/PhysicsSystem.h"
#include "Systems/hornSystem/hornSystem.h"

#include "Components/SceneComponent.h"
#include "Systems/Debug/LiveReloadSystem.h"

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"



#include "Components/ComponentId.h"