#pragma once

#include <vector>
#include <functional>
#include <type_traits>
#include <algorithm>

#include "EcsTypes.h"
#include "Component.h"
#include "System.h"
#include "ComponentEventFunctionHandle.h"

// To allow polymorphism
class IComponentEvent {
public:
    virtual ~IComponentEvent() = default;
};

/// @brief List of functions to call when an event ocurs over a component of type T
/// @tparam T Component
template <typename T>
class ComponentEvent : public IComponentEvent {

    static_assert(std::is_base_of_v<Component, T>, "T must inherit from Component");

    std::vector<std::shared_ptr<ComponentEventFunctionHandle<T>>> functionList = 
        std::vector<std::shared_ptr<ComponentEventFunctionHandle<T>>>();

public:

    /// @brief Adds the function to a list to be called when the event is dispatched
    /// @param fn The function of type `std::function<void(T&, EntityID)` to be called
    /// @return Handle to release (disconnect) the function from the event
    std::shared_ptr<ComponentEventFunctionHandle<T>> connect(ComponentEventFunction<T> fn) {

        std::shared_ptr<ComponentEventFunctionHandle<T>> handle = 
            std::shared_ptr<ComponentEventFunctionHandle<T>>(new ComponentEventFunctionHandle<T>(fn));
            // new has to be used because shared_ptr is not friends of ComponentEventFunctionHandle, 
            // but ComponentEventList is

        functionList.push_back(handle);

        return handle;
    }

    /// @brief Calls all the functions subscribed to the event , if any function was released via its handles it is
    ///         removed on the fly
    /// @param component the affected component
    /// @param entity the entity the component should be part of
    void dispatch(T& component) {

        for (size_t i = 0; i < functionList.size(); ++i) {

            if (functionList[i]->fn == nullptr) {

                std::swap(functionList[i], functionList.back());
                functionList.pop_back();
                --i;
            }
            else {

                (functionList[i]->fn)(component);
            }
        }
    }
};

/// @brief Temporal reference to connect events in a more explicit way
template<typename T>
class ComponentEventTemporalReference {

    std::shared_ptr<ComponentEvent<T>> event;

public:
    ComponentEventTemporalReference(std::shared_ptr<ComponentEvent<T>> event): event(event) {}

    template<typename Caller, typename Method>
    std::shared_ptr<ComponentEventFunctionHandle<T>> connect(Caller* instance, Method method) {

        static_assert(std::is_member_function_pointer_v<Method>,
            "method must be a member function pointer");

        return event->connect([instance, method] (T& component) {
                std::invoke(method, instance, component);
            }
        );
    }

    template<typename Func>
    std::shared_ptr<ComponentEventFunctionHandle<T>> connect(Func func) {

        static_assert(!std::is_member_function_pointer_v<Func>,
            "Use connect(instance, method) for member functions");

        return event->connect([func](T& component) {
            std::invoke(func, component);
        });
    }

    ComponentEventTemporalReference(const ComponentEventTemporalReference&) = delete;
    ComponentEventTemporalReference& operator=(const ComponentEventTemporalReference&) = delete;
    ComponentEventTemporalReference(ComponentEventTemporalReference&&) = delete;
    ComponentEventTemporalReference& operator=(ComponentEventTemporalReference&&) = delete;
};