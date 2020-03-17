//
// Created by Jackson Cougar Wiebe on 3/16/2020.
//

#ifndef ENGINE_TYPES_HPP
#define ENGINE_TYPES_HPP

namespace Engine {
	struct BoxModel {
		float top;
		float right;
		float bottom;
		float left;
	};

	struct RelativeAnchor {
		float u;
		float v;
	};

	struct Offset {
		float x;
		float y;
	};

	struct Rectangle {
		float x;
		float y;
		float width;
		float height;

		Rectangle operator-(const BoxModel& box) {
			auto& [top, right, bottom, left] = box;



			return { width - right - left, height - top - bottom };
		}
	};

	struct Plot {
		BoxModel margin = { 10,10,10,10 };
		Rectangle rectangle = { 50, 50, 100, 100 };

		Plot plot_at(Rectangle destination, RelativeAnchor anchor)
		{
			auto& [dx, dy, dw, dh] = destination;
			auto& [sx, sy, sw, sh] = rectangle;
			auto& [u, v] = anchor;

			auto ax = (0.5f * u + 0.5f) * dw;
			auto ay = (0.5f * v + 0.5f) * dh;

			return { margin, {dx + ax - sx, dy + ay - sy, sw, sh } };
		}
	};



}


#endif //ENGINE_TYPES_HPP
