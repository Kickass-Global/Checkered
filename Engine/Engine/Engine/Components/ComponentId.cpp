//
// Created by root on 11/1/20.
//

#include "ComponentId.h"

std::ostream &Component::operator<<(std::ostream &out, const Component::ClassId &id) {
    return out << (int)id;
}
