//
// Created by root on 9/1/20.
//

#ifndef ENGINE_SYSTEMCALLS_H
#define ENGINE_SYSTEMCALLS_H

#include <string>
#include <iostream>

namespace system_calls {
    template<char const *module>
    void die(std::string msg) {
        std::cerr << "[" + module + "] " << msg << std::endl;
        exit(-1);
    }

    template<char const *module>
    void assert(bool test, std::string msg) {

        auto result = test ? "SUCCEEDED" : "FAILED";

        if(!test) {
            std::cerr << "[" << module << "] " << msg << " " << result << std::endl;
            exit(-1);
        }
        else {
            std::cout << "[" << module << "] " << msg << " " << result << std::endl;
        }

    }


}
#endif //ENGINE_SYSTEMCALLS_H
