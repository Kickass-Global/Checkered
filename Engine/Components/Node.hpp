//
// Created by Jackson Cougar Wiebe on 3/17/2020.
//

#ifndef ENGINE_NODE_HPP
#define ENGINE_NODE_HPP

#include <algorithm>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <variant>

namespace Component {

struct NodeEntry {
  int momentary : 1;
  int ttl : 1;
  int active : 1;
  std::shared_ptr<class ComponentBase> component;
  NodeEntry(bool momentary, int ttl,
            std::shared_ptr<class ComponentBase> component);
};

struct Node {
  std::unordered_map<std::type_index, std::vector<NodeEntry>> components;

  /**
   * Constructs an object T with the supplied arguments in the store.
   * @tparam T the type of the object
   * @tparam Args the argument types
   * @param args the arguments to construct the object using
   * @return returns a shared_ptr to the constructed object.
   */
  template <typename T, typename... Args>
  std::shared_ptr<T> create(int ttl, Args &... args) {

    static_assert(std::is_base_of<ComponentInterface, T>::value);

    // check to see if the store has an available allocation we can reuse
    int uses = 0;
    auto it = std::find_if( // O(n) #todo
        components[typeid(T)].begin(), components[typeid(T)].end(),
        [&uses](NodeEntry &entry) { return !entry.component; });

    if (it != components[typeid(T)].end()) {
      it->component.reset(new T(args...)); // copy-construct a new component
                                           // using existing allocation...
      return std::static_pointer_cast<T>(it->component);
    }

    auto component = std::make_shared<T>(args...);
    components[typeid(T)].emplace_back(ttl > 0, ttl, component);
    return std::static_pointer_cast<T>(components[typeid(T)].back().component);
  }

  template <typename T> void deactivate(T *component) {
    auto it = std::find_if( // O(n) #todo
        components[typeid(T)].begin(), components[typeid(T)].end(),
        [&component](NodeEntry &entry) {
          return entry.component.get() == component;
        });
    if (it != components[typeid(T)].end()) {
      it->active = false;
    }
  }

  template <typename T> void activate(T *component) {
    auto it = std::find_if( // O(n) #todo
        components[typeid(T)].begin(), components[typeid(T)].end(),
        [&component](NodeEntry &entry) {
          return entry.component.get() == component;
        });
    if (it != components[typeid(T)].end()) {
      it->active = true;
    }
  }

  template <typename T> std::vector<T *> getComponentsOfType() {

    static_assert(std::is_base_of<ComponentInterface, T>::value);

    auto it = components.find(typeid(T));
    if (it == components.end())
      return {};

    std::vector<T *> result;
    result.reserve(it->second.size());

    for (auto ref : it->second) {
      if (!ref.active)
        continue;
      auto ptr = std::static_pointer_cast<T>(ref.component).get();
      result.push_back(ptr);
    }

    return result;
  }

  void addComponents() {}

  template <typename T> void addComponents(std::shared_ptr<T> component) {
    addComponent(component);
  }

  template <typename T, typename... Ts>
  void addComponents(T component, std::shared_ptr<Ts>... components) {

    addComponent(component);
    addComponents(components...);
  }

  template <typename T>
  std::shared_ptr<T> addComponent(std::shared_ptr<T> component) {
    static_assert(std::is_base_of<ComponentInterface, T>::value);

    components[typeid(T)].emplace_back(false, 0, std::move(component));
    return std::dynamic_pointer_cast<T>(components[typeid(T)].back().component);
  }

  template <typename T, int ttl, typename... Args>
  std::shared_ptr<T> emplaceComponent(Args... args) {
    static_assert(std::is_base_of<ComponentInterface, T>::value);

    auto component = create<T>(ttl, args...);
    return component;
  }

  template <typename T, typename... Args>
  std::shared_ptr<T> emplaceComponent(Args &&... args) {
    static_assert(std::is_base_of<ComponentInterface, T>::value);

    auto component = create<T>(0, args...);
    return component;
  }

  template <typename T>
  std::shared_ptr<T> addExternalComponent(std::shared_ptr<T> component) {

    static_assert(std::is_base_of<ComponentInterface, T>::value);

    components[typeid(T)].emplace_back(component);
    return std::dynamic_pointer_cast<T>(components[typeid(T)].back().component);
  }

  template <typename T>
  std::shared_ptr<T> addMomentaryComponent(std::shared_ptr<T> component) {

    static_assert(std::is_base_of<ComponentInterface, T>::value);

    components[typeid(T)].emplace_back(true, 1, std::move(component));
    return std::dynamic_pointer_cast<T>(components[typeid(T)].back().component);
  }

  template <typename T> void eraseComponent(T *component) {
    auto it = std::find_if(
        components[typeid(T)].begin(), components[typeid(T)].end(),
        [component](NodeEntry &element) {
          return static_cast<T *>(element.component.get()) == component;
        });
    if (it != components[typeid(T)].end()) {
      components[typeid(T)].erase(it);
    }
  }

  template <typename T> void eraseComponentsOfType() {
    static_assert(std::is_base_of<ComponentInterface, T>::value);
    components.erase(typeid(T));
  }

  void prune() {
    for (auto &[key, values] : components) {
      auto begin = std::remove_if(values.begin(), values.end(), [](auto entry) {
        return entry.momentary && --entry.ttl < 1;
      });
      values.erase(begin, values.end());
    }
  }

  void eraseAllComponents() {
    for (auto &[key, values] : components) {
      values.erase(values.begin(), values.end());
    }
    components.clear();
  }
};
} // namespace Component

#endif // ENGINE_NODE_HPP
