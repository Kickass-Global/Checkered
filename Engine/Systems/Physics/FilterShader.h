

#include <PxPhysicsAPI.h>
#include <PxFiltering.h>
#include <PxRigidDynamic.h>
#include <stdio.h>
#include <iostream>

using namespace physx;

namespace Physics {

	class FilterShader : public PxSimulationFilterCallback{




	public:
		

		static PxFilterFlags setupFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
			PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags,
			const void* constantBlock, PxU32 constantBlockSize);

		static void setupQueryFiltering(PxRigidActor * actor, PxU32 filterGroup, PxU32 filterMask);

		static void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);
	};

}