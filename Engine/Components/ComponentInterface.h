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

namespace Component {
    typedef int ComponentId;
    static ComponentId next_id = 800000000;

    class ComponentInterface {
    public:
        [[nodiscard]] virtual struct Node &getChildren() = 0;

        [[nodiscard]] virtual ComponentId getId() const = 0;

    };

    struct NodeEntry {
        int momentary : 1;
        int ttl : 1;
        std::shared_ptr<class ComponentBase> component;

        NodeEntry(int momentary, int ttl, std::shared_ptr<class ComponentBase> component);
    };

    struct Node {
        std::unordered_map<std::type_index, std::vector<NodeEntry>> components;

        template<typename T>
        std::vector<T *> getComponentsOfType() {

            static_assert(std::is_base_of<ComponentInterface, T>::value);

            auto it = components.find(typeid(T));
            if (it == components.end()) return {};

            std::vector<T *> result;
            result.reserve(it->second.size());

            for (auto ref : it->second) {
                auto ptr = std::static_pointer_cast<T>(ref.component).get();
                result.push_back(ptr);
            }

            return result;
        }

        template<typename T>
        std::shared_ptr<T> addComponent(std::shared_ptr<T> component) {

            static_assert(std::is_base_of<ComponentInterface, T>::value);

            components[typeid(T)].emplace_back(0, 0, std::move(component));
            return std::dynamic_pointer_cast<T>(components[typeid(T)].back().component);
        }

        template<typename T>
        std::shared_ptr<T> addMomentaryComponent(std::shared_ptr<T> component) {

            static_assert(std::is_base_of<ComponentInterface, T>::value);

            components[typeid(T)].emplace_back(1, 1, std::move(component));
            return std::dynamic_pointer_cast<T>(components[typeid(T)].back().component);
        }

        template<typename T>
        void eraseComponentsOfType() {

            static_assert(std::is_base_of<ComponentInterface, T>::value);
            components.erase(typeid(T));
        }

        template<typename T>
        void eraseComponent(ComponentId id) {

            static_assert(std::is_base_of<ComponentInterface, T>::value);

            auto it = std::find_if(
                components[typeid(T)].begin(), components[typeid(T)].end(),
                [id](auto elem) { return elem.component->getId() == id; }
            );

            if (it != components[typeid(T)].end()) components[typeid(T)].erase(it);
        }

        void prune() {

            for (auto&[key, values] : components) {
                auto begin = std::remove_if(
                    values.begin(), values.end(), [](auto entry) {
                        return entry.momentary && --entry.ttl < 1;
                    }
                );
                values.erase(begin, values.end());
            }
        }

        void eraseAllComponents() {

            for (auto&[key, values] : components) {
                values.clear();
            }
        }

    };
}

#endif //ENGINE_COMPONENTINTERFACE_H
