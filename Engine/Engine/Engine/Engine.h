// Engine.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>
#include <functional>

template<typename... Args>
class Event {
    std::vector<std::function<void(Args...)>> handlers;
public:
    void operator()(Args&... args) {
        for (auto&& handler : handlers) {
            if (handler) handler(std::forward<Args>(args)...);
        }
    }

    void operator+=(std::function<void(Args...)> handler) {
        handlers.push_back(handler);
    }
};

typedef float frametime;

constexpr char module[] = "Engine";

// TODO: Reference additional headers your program requires here.
