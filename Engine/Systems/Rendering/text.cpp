//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#include "text.hpp"

#include <utility>

Component::Text::Text(std::string text, float fontSize, const glm::vec3 &color) : text(std::move(text)),
                                                                                  font_size(fontSize), color(color) {}
