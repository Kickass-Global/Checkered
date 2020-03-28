//
// Created by Jackson Cougar Wiebe on 3/14/2020.
//

#ifndef ENGINE_COMPONENTINTERFACE_H
#define ENGINE_COMPONENTINTERFACE_H

#include <any>
#include <unordered_map>
#include <typeindex>
#include <memory>
#include <algorithm>
#include <variant>
#include <EngineDebug.hpp>

namespace Component {
	typedef int ComponentId;
	static ComponentId next_id = 800000000;

	class ComponentInterface {
	public:
		[[nodiscard]] virtual struct Node& getChildren() = 0;
		[[nodiscard]] virtual ComponentId getId() const = 0;
	};
}

#endif //ENGINE_COMPONENTINTERFACE_H
