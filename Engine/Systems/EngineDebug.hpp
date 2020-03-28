//
// Created by Jackson Cougar Wiebe on 3/14/2020.
//

#ifndef ENGINE_ENGINEDEBUG_HPP
#define ENGINE_ENGINEDEBUG_HPP

#include <string>
#include <iostream>
#include <glm/fwd.hpp>
#include <sstream>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

namespace Engine {

    namespace {
        const char module[] = "Engine";
    }

    struct Name {
        std::string value;

        explicit Name(const char *module_name) : value(module_name) {}
    };

    std::ostream &operator<<(std::ostream &out, const glm::vec3 &vector);

    std::ostream &operator<<(std::ostream &out, const glm::quat &quat);

    std::ostream &operator<<(std::ostream &out, Name name);

    /**
     * Asserts that given condition is successful and logs the given message.
     * @tparam module name to tag log messages with.
     * @param test the condition to test.
     * @param msg the message to log. (Should be in the form <msg> <SUCCEEDED>)
     */
    template<char const *m = module, class ... Ts>
    void assertLog(bool test, Ts...args) {

        if (!test) {

            std::ostringstream buffer;

            buffer << Name(m);
            (buffer << ... << args);
            buffer << " [FAILURE]\n";

            std::cerr << buffer.str() << std::flush;
            OutputDebugString(buffer.str().c_str());

            throw;
        }

    }

    enum Importance {
        low, medium, high
    };

    constexpr Importance loggingLevel = high;


    template<char const *m, Importance importance, class ... Ts>
    void log(Ts...args) {

        if (importance >= loggingLevel) {

            std::ostringstream buffer;

            buffer << Name(m);
            (buffer << ... << args);
            buffer << "\n";

            std::cout << buffer.str() << std::flush;
            OutputDebugString(buffer.str().c_str());
        }
    }

    template<char const *m, class ... Ts>
    void log(Ts...args) {

        log<module, medium>(args...);
    }

    template<Importance importance = medium, class ... Ts>
    void log(Ts...args) {

        log<module, importance>(args...);
    }

    template<Importance importance = medium, class ... Ts>
    void log(const Ts &&...args) {

        log<module, importance>(args...);
    }

}

using namespace Engine;

#endif //ENGINE_ENGINEDEBUG_HPP
