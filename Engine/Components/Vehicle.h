//
// Created by root on 17/1/20.



#ifndef ENGINE_VEHICLE_H
#define ENGINE_VEHICLE_H

#include <PxPhysicsAPI.h>
#include <vehicle/PxVehicleDrive4W.h>
#include <Events/Events.h>
#include <EventHandler.h>
#include <EventDelegate.h>
#include "..\Systems\Navigation\astar.h"
#include "Model.h"
#include "ComponentId.h"
#include "tags.h"
#include "damage.hpp"
#include <GLFW/glfw3.h>

#include "Engine.h" 

#include <glm/glm.hpp>
#include <Sound.h>

namespace Component {


	class Vehicle : public ComponentBase {
	public:
		bool is_outdated = true;
		std::shared_ptr<Model> model;
		ComponentId input{};
		std::shared_ptr<EventHandler<Engine::deltaTime>> onTickHandler;
		EventDelegate<Vehicle*> tickHandler = EventDelegate<Vehicle*>("handler");

		enum Type {
			Player,
			Taxi
		} type = Taxi;

		AStar path;
		glm::vec3 scale = { 1,1,1 };
		glm::quat rotation;
		glm::quat local_rotation = glm::rotate(0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 position;
		glm::vec3 local_position = { 0,0,0 };

		glm::mat4 world_transform() {
			return glm::translate(position) * glm::translate(local_position) * glm::mat4_cast(rotation) * glm::mat4_cast(local_rotation) * glm::scale(scale);
		}

		glm::mat4 physx_transform() {
			return glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
		}

		bool pxIsVehicleInAir;
		physx::PxVehicleDrive4WRawInputData pxVehicleInputData;
		physx::PxVehicleDrive4W* pxVehicle = nullptr;
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* pxFrictionPairs = nullptr;
		physx::PxReal pxSteerVsForwardSpeedData[16];
		physx::PxFixedSizeLookupTable<8> pxSteerVsForwardSpeedTable;
		physx::PxVehicleKeySmoothingData pxKeySmoothingData;
		physx::PxVehiclePadSmoothingData pxPadSmoothingData;

		void onTick(const Component::EventArgs<Engine::deltaTime>& args) {
			tickHandler(this);
		}

		Vehicle() :
			pxSteerVsForwardSpeedData{
					0.0f, 0.75f,
					5.0f, 0.75f,
					30.0f, 0.1f,
					120.0f, 0.1f,

		},
			pxKeySmoothingData{
					{
							6.0f,    //rise rate eANALOG_INPUT_ACCEL
							6.0f,    //rise rate eANALOG_INPUT_BRAKE
							6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
							2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
							2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
					},
					{
							10.0f,    //fall rate eANALOG_INPUT_ACCEL
							10.0f,    //fall rate eANALOG_INPUT_BRAKE
							10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
							5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
							5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
					}
		},
			pxPadSmoothingData{
					{
							6.0f,    //rise rate eANALOG_INPUT_ACCEL
							6.0f,    //rise rate eANALOG_INPUT_BRAKE
							6.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
							2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
							2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
					},
					{
							10.0f,    //fall rate eANALOG_INPUT_ACCEL
							10.0f,    //fall rate eANALOG_INPUT_BRAKE
							10.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
							5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
							5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
					}
		} {
			pxSteerVsForwardSpeedTable = physx::PxFixedSizeLookupTable<8>(pxSteerVsForwardSpeedData, 4);
			onTickHandler = Engine::EventSystem::createTickHandler(this, &Vehicle::onTick);
		}

	};


	struct ControlledVehicle : public ComponentBase {

		std::shared_ptr<Vehicle> vehicle;
		std::shared_ptr<EventHandler< GLFWgamepadstate, GLFWgamepadstate>> onGamePadStateChangedHandler;
		std::shared_ptr<EventHandler<int>> onKeyDownHandler;
		std::shared_ptr<EventHandler<int>> onKeyUpHandler;

		ControlledVehicle() : vehicle(Engine::createComponent<Vehicle>())
		{
			onGamePadStateChangedHandler = Engine::EventSystem::createHandler(this, &ControlledVehicle::onGamePadStateChanged);
			onKeyDownHandler = Engine::EventSystem::createHandler(this, &ControlledVehicle::onKeyDown);
			onKeyUpHandler = Engine::EventSystem::createHandler(this, &ControlledVehicle::onKeyUp);
			vehicle->type = Vehicle::Type::Player;
		}

		template <typename T> int sgn(T val) {
			return (T(0) < val) - (val < T(0));
		}
		
		float filter_axis_data(const float input)
		{
			auto normalize = [](auto value, auto min, auto max) {
				return (value - min) / (max - min);
			};
			if(input >= 0) return std::clamp(input, 0.2f, 1.0f);
			else return std::clamp(input, -1.0f, -0.2f);
		}


		void onKeyDown(const EventArgs<int>& args) {
			auto v = vehicle->pxVehicle->getRigidDynamicActor()->getLinearVelocity();
			auto key = std::get<0>(args.values);
	    	if (key == GLFW_KEY_W) {

				if (v.z < 0.0) {
					vehicle->pxVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);
				}
				vehicle->pxVehicleInputData.setAnalogAccel(1);
				Engine::createComponent<Component::Sound>("acceleration");
			    
			}
			
			
			if (key == GLFW_KEY_A) {
				vehicle->pxVehicleInputData.setAnalogSteer(1);
			}
			if (key == GLFW_KEY_D) {
				vehicle->pxVehicleInputData.setAnalogSteer(-1);
			}
			if (key == GLFW_KEY_S) {
				if (v.z > 0.1) {
					
					
					vehicle->pxVehicleInputData.setAnalogBrake(1);
					Engine::createComponent<Component::Sound>("breaking");
				}
				else {
					
					vehicle->pxVehicle->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eREVERSE);
					vehicle->pxVehicleInputData.setAnalogAccel(1);
					
					Engine::createComponent<Component::Sound>("acceleration");
				}
				
			}
		}

		void onKeyUp(const EventArgs<int>& args) {
			auto key = std::get<0>(args.values);
			if (key == GLFW_KEY_W) {
				vehicle->pxVehicleInputData.setAnalogAccel(0);
				Engine::createComponent<Component::Sound>("stopAcceleration");
				Engine::createComponent<Component::Sound>("stopBreaking");
			
			}
			if (key == GLFW_KEY_A) {
				vehicle->pxVehicleInputData.setAnalogSteer(0);
			}
			if (key == GLFW_KEY_D) {
				vehicle->pxVehicleInputData.setAnalogSteer(0);
			}
			if (key == GLFW_KEY_S) {
				vehicle->pxVehicleInputData.setAnalogBrake(0);
				Engine::createComponent<Component::Sound>("stopAcceleration");
				Engine::createComponent<Component::Sound>("stopBreaking");
			}
		}

		void onGamePadStateChanged(const EventArgs<GLFWgamepadstate, GLFWgamepadstate>& args)
		{
			auto previous = std::get<0>(args.values);
			auto current = std::get<1>(args.values);

			float control_deadzone = 0.3f;
			


			vehicle->pxVehicleInputData.setAnalogAccel(current.buttons[GLFW_GAMEPAD_BUTTON_A]);
			vehicle->pxVehicleInputData.setAnalogBrake(current.buttons[GLFW_GAMEPAD_BUTTON_X]);

			if (current.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -control_deadzone || current.axes[GLFW_GAMEPAD_AXIS_LEFT_X] >= control_deadzone) {
				vehicle->pxVehicleInputData.setAnalogSteer(-current.axes[GLFW_GAMEPAD_AXIS_LEFT_X]);
				

			}

			else {
				vehicle->pxVehicleInputData.setAnalogSteer(0);
			}
			




		}

	};

}


#endif //ENGINE_VEHICLE_H
