#pragma once

#include "ComponentEvent.h"
#include "EcsTypes.h"

namespace HUGO {

    template<typename T>
    class Mutable {

        std::shared_ptr<ComponentEvent<T>> updateEvent;
        T& reference;
        bool moved = false;

    public:

        Mutable() = delete;
        Mutable(const Mutable&) = delete;
        Mutable& operator=(const Mutable&) = delete;
        Mutable& operator=(Mutable&&) = delete;

        Mutable(Mutable&& other): updateEvent(other.updateEvent), reference(other.reference), moved(false) {
            other.moved = true;
        }

        Mutable(std::shared_ptr<ComponentEvent<T>> updateEvent, T& reference) 
        : updateEvent(updateEvent), reference(reference) {}

        ~Mutable() {
            if (!moved) 
                updateEvent->dispatch(reference);
        }

        T* operator->() {
            return &reference;
        }


    };

}
