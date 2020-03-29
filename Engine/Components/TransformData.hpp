//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_TRANSFORMDATA_HPP
#define ENGINE_TRANSFORMDATA_HPP

#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

/*
 * Contains the position, rotation, and scale data
 */
struct TransformData {
    glm::vec3 translation;
    glm::quat orientation;
    float scale;
};


#endif //ENGINE_TRANSFORMDATA_HPP
