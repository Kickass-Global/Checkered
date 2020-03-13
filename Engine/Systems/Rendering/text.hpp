//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_TEXT_HPP
#define ENGINE_TEXT_HPP

#include <ComponentBase.h>
#include <glm/vec3.hpp>

namespace Component {
    struct Text : public ComponentBase {
        std::string text = "";
        float font_size = 12;
        glm::vec3 color = {0, 0, 0};

        Text(std::string text, float fontSize, const glm::vec3 &color);

    };
}


#endif //ENGINE_TEXT_HPP
