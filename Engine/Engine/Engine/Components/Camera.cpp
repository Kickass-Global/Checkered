//
// Created by root on 11/1/20.
//

#include "Camera.h"

Component::Camera::Camera() : position(),
                              rotation(1, 0, 0, 0),
                              viewport{0, 0, 640, 480} {}
