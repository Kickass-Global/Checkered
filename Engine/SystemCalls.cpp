//
// Created by root on 9/1/20.
//

#include "SystemCalls.h"
#include <sstream>
#include <algorithm>

std::ostream &Engine::operator<<(std::ostream &out, Engine::Name name) {
    const unsigned int width = 20;

    std::ostringstream buff;
    buff << "[";
    std::copy_n(name.value.begin(),
            std::min(name.value.size(), static_cast<size_t>(width - 3)),
            std::ostreambuf_iterator(buff));
    buff << "] ";

    return out << std::setw(width) << buff.str();
}

void Engine::assertLog(bool test, std::string msg) {
    assertLog<module>(test, msg);
}
