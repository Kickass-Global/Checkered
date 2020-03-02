//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#include "EngineStore.h"

EngineStore::ComponentInstance::ComponentInstance(unsigned int isDirty, ComponentInterface *data) : is_dirty(isDirty),
                                                                                                    data(data) {}
