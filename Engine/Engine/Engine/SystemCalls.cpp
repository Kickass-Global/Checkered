//
// Created by root on 9/1/20.
//

#include "SystemCalls.h"
#include <sstream>
#include <algorithm>

std::ostream &system_calls::operator<<(std::ostream &out, system_calls::Name name) {
    const unsigned int width = 13;

    std::ostringstream buff;
    buff << "[";
    std::copy_n(name.value.begin(),
            std::min(name.value.size(), static_cast<size_t>(width - 3)),
            std::ostreambuf_iterator(buff));
    buff << "] ";

    return out << std::setw(width) << buff.str();
}
