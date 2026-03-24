#pragma once

#include "ComponentEvent.h"
#include "EcsTypes.h"

namespace HUGO {

template<typename T>
class Mutable {

    std::shared_ptr<ComponentEvent<T>> updateEvent;
    T& reference;
    EntityID entity;

public:

    Mutable(std::shared_ptr<SafeComponentEventDispatcher<T>> updateEvent, T& reference, EntityID entity) 
    : updateEvent(updateEvent), reference(reference), entity(entity) {}

    ~Mutable() {
        updateEvent->dispatch(reference, entity);
    }

    T& operator->() {
        return reference;
    }

    Mutable(const Mutable&) = delete;
    Mutable(Mutable&&) = delete;
    Mutable& operator=(const Mutable&) = delete;
    Mutable& operator=(Mutable&&) = delete;
};

}