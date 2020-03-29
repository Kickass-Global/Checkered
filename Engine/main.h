// Engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include <functional>
#include <iostream>
#include <vector>

#include "Systems/Camera/CameraSystem.h"
#include "Systems/Input/InputSystem.h"
#include "Systems/Physics/PhysicsSystem.h"
#include "Systems/Pipeline/MeshLoader.h"
#include "Systems/Pipeline/ShaderLoader.h"
#include "Systems/Rendering/RenderingBatch.h"
#include "Systems/Rendering/RenderingSystem.h"
#include "Systems/hornSystem/hornSystem.h"

#include "Components/SceneComponent.h"
#include "Systems/Debug/LiveReloadSystem.h"

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"

#include "Components/ComponentId.h"