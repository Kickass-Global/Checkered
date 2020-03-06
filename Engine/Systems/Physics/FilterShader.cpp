

#include "FilterShader.h"

PxFilterFlags Physics::FilterShader::setupFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags,
    const void* constantBlock, PxU32 constantBlockSize) {

    //let triggers through
    std::cout << "trigger callback";
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1)) {

        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;

    }
    //generate contacts for all that were not filtered
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    if ((filterData0.word0 & filterData1.word1) && (filterData0.word1 & filterData1.word0)) {
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    }

    return PxFilterFlag::eDEFAULT;


}

 void Physics::FilterShader::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask) {

    PxFilterData filterData;
    filterData.word0 = filterGroup;
    filterData.word1 = filterMask;

    

    const PxU32 numShapes = actor->getNbShapes();
    PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
    actor->getShapes(shapes, numShapes);
    for (PxU32 i = 0; i < numShapes; i++) {
        PxShape* shape = shapes[i];
        shape->setSimulationFilterData(filterData);
    }
    free(shapes);


}