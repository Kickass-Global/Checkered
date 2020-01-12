//
// Created by root on 9/1/20.
//

#ifndef ENGINE_SYSTEMCALLS_H
#define ENGINE_SYSTEMCALLS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <utility>

#undef assert

namespace Engine {

    struct Name {
        std::string value;
        explicit Name(std::string&& name) : value(name) {}
    };

    std::ostream& operator<<(std::ostream& out, Name name);

    /**
     * Asserts that given condition is successful and logs given message.
     * @tparam module name to tag log messages with.
     * @param test the condition to test.
     * @param msg the message to log. (Should be in the form <msg> <SUCCEEDED>)
     */
    template<char const *module>
    void assert(bool test, std::string msg) {

        auto result = test ? "SUCCEEDED" : "FAILED";

        if(!test) {
            std::cerr << Name(module) << msg << " " << result << std::endl;
            exit(-1);
        }
    }


    enum Importance {
        low,
        medium,
        high
    };

    inline static Importance loggingLevel = medium;

    template<char const *module, typename ... Ts>
    void log(Ts...args)
    {
        if (loggingLevel >= medium)
        {
            std::cout << Name(module);
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }
    
    template<char const* module, Importance importance, typename ... Ts>
    void log(Ts...args)
    {
        if (importance >= loggingLevel) {
            std::cout << Name(module);
            (std::cout << ... << args);
            std::cout << std::endl;
        }
    }
}
#endif //ENGINE_SYSTEMCALLS_H