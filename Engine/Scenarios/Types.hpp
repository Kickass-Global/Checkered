//
// Created by Jackson Cougar Wiebe on 3/16/2020.
//

#ifndef ENGINE_TYPES_HPP
#define ENGINE_TYPES_HPP

#include <vector>

namespace Engine {

struct BoxModelLayer {
  float top;
  float right;
  float bottom;
  float left;
};

struct PixelOffset {
  int x;
  int y;
};

struct RelativeAnchor {
  float u;
  float v;
  int px_u;
  int px_v;

  RelativeAnchor() = default;
  RelativeAnchor(float u, float v) : u(u), v(v), px_u(0), px_v(0) {}
  RelativeAnchor(float u, float v, int x, int y)
      : u(u), v(v), px_u(x), px_v(y) {}

  RelativeAnchor operator+(PixelOffset offset) {
    return {u, v, offset.x, offset.y};
  }

  std::tuple<float, float> operator()(float width, float height) {
    return {(0.5f + u * 0.5f) * width + static_cast<float>(px_u),
            (0.5f + v * 0.5f) * height + static_cast<float>(px_v)};
  }
};

struct Offset {
  float x;
  float y;
  Offset(float x, float y);
};

struct Rectangle {
  float x;
  float y;
  float width;
  float height;
  Rectangle(float x, float y, float width, float height);
  Rectangle() = default;

  std::vector<Rectangle> subdivide(unsigned int n) {
    auto sub_item_height = height / n;
    auto sub_item_y = y;
    std::vector<Rectangle> subdivisions;
    subdivisions.reserve(n);

    for (unsigned int i = 0; i < n; ++i) {
      subdivisions.emplace_back(x, sub_item_y, width, sub_item_height);
      sub_item_y += sub_item_height;
    }

    return subdivisions;
  }
};

struct BoxModel {
  BoxModelLayer margin = {10, 10, 10, 10};
  Rectangle box = {50, 50, 100, 100};

  BoxModel() = default;

  explicit BoxModel(float x, float y, float width, float height)
      : box(x, y, width, height) {}

  /**
   * Returns this box placed in the destination rectangle so that both anchors
   * overlap.
   */
  Rectangle plot(Rectangle dst, RelativeAnchor dst_anchor,
                 RelativeAnchor src_anchor) {
    auto &&[dx, dy, dw, dh] = dst;
    auto &&[sx, sy, sw, sh] = box;
    auto &&[dax, day] = dst_anchor(dw, dh);
    auto &&[sax, say] = src_anchor(sw, sh);

    return {dx + dax - sax, dy + day - say, sw, sh};
  };
};
} // namespace Engine

#endif // ENGINE_TYPES_HPP
