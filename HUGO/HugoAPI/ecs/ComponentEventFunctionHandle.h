#pragma once

#include <functional>
#include <type_traits>

#include "Component.h"

template <typename T>
using ComponentEventFunction = std::function<void(T&)>;

template<typename T>
class ComponentEvent;

class AbstractComponentEventFunctionHandle {
    public:
    virtual void release() = 0;
};

/// @brief Handle to release functions associated with an event that is related with objects of component T
/// @tparam T Component
template <typename T>
class ComponentEventFunctionHandle: public AbstractComponentEventFunctionHandle {

    static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");

    ComponentEventFunction<T> fn;

    ComponentEventFunctionHandle(ComponentEventFunction<T> newFn): fn(newFn) {}

    friend class ComponentEvent<T>;
public:
    void release() override {
        fn = nullptr;
    }
};
