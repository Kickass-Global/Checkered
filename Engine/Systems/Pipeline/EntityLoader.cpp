//
// Created by root on 17/1/20.
//

#include "EntityLoader.h"
#include "../../Components/GameObject.h"

template<>
void Pipeline::load_meta_data<Component::Mesh>(const json &config, std::shared_ptr<Component::Mesh> data) {
    for (auto[key, value] : config["entity"]["meta"].items()) {
        auto offset = value["offset"].get<int>();
        auto is_component_reference = value.contains("component-id");
        if (is_component_reference) {
            auto classId = value["component-id"]["class-id"];
            auto dataName = value["component-id"]["data"];

            auto componentId = Library::getAsset(dataName, parse(classId));
            data->shader = componentId;
        }
    }
}

template<>
void Pipeline::load_meta_data<Component::GameObject>(const json &config, std::shared_ptr<Component::GameObject> data) {
    for (auto[key, value] : config["entity"]["meta"].items()) {
        auto offset = value["offset"].get<int>();
        auto is_component_reference = value.contains("component-id");
        if (is_component_reference) {
            auto classId = value["component-id"]["class-id"];
            auto dataName = value["component-id"]["data"];
        }
    }
}

void Pipeline::load_components(const json &config, Component::ComponentId entity) {

    for (auto[key, value] : config["entity"]["components"].items()) {
        auto classId = parse(value["entity"]["class-id"]);
        auto data = value["entity"]["data"];
        auto componentId = Library::getAsset(data, classId);
        entity.attachExistingComponent(componentId);
        switch (classId) {
            case Component::ClassId::Mesh:
                load_meta_data<Component::Mesh>(value, componentId.data<Component::Mesh>());
                break;
        }
    }
}
