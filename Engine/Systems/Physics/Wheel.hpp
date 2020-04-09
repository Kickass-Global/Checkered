//
// Created by Jackson Cougar Wiebe on 4/6/2020.
//

#ifndef ENGINE_WHEEL_HPP
#define ENGINE_WHEEL_HPP

#include <ComponentBase.h>
#include <WorldTransform.h>

namespace Component {
struct Wheel : public ComponentBase {
  std::shared_ptr<class PaintedMesh> mesh;
  Wheel();

  void setTransform(const physx::PxTransform &T);
};
} // namespace Component
#endif // ENGINE_WHEEL_HPP
