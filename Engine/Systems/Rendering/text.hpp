//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_TEXT_HPP
#define ENGINE_TEXT_HPP

#include <ComponentBase.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <Types.hpp>

namespace Component {

    enum class eAlign {
        right = -2, left = 0, center = -1
    };
    enum class eVerticalAlign {
        top = -2, middle = -1, bottom = 0
    };

    template<typename T>
    T operator*(eAlign a, T b) { return static_cast<int>(a) * b; }

    template<typename T>
    T operator/(eAlign a, T b) { return static_cast<int>(a) / b; }

    template<typename T>
    T operator/(eVerticalAlign a, T b) { return static_cast<int>(a) / b; }

    struct Text : public ComponentBase {
        std::string text = "";
        float font_size = 12;
        glm::vec3 color = {0, 0, 0};
        Engine::Rectangle plot;
        Engine::RelativeAnchor dst = {0, 0};
        Engine::RelativeAnchor src = {0, 0};
        eAlign align = eAlign::center;
        eVerticalAlign vertical_align = eVerticalAlign::top;

        explicit Text(std::string text);
        template<typename... Args>
        void set_text(Args... args) {

            std::ostringstream buffer;
            (buffer << ... << args);

            text = buffer.str();
        }

        template<typename... Args>
        Text(Args... args) {
            
            std::ostringstream buffer;
            (buffer << ... << args);

            text = buffer.str();
        }
    };
}


#endif //ENGINE_TEXT_HPP
