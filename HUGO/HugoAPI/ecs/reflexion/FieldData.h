#pragma once

#include "HugoError.h"
#include "HugoParsingVariant.h"

template<typename Cmp>
struct IFieldData {
    virtual HUGO::ErrorOrVoid Set(Cmp& obj, const HVariant& value) const = 0;
    virtual ~IFieldData() = default;
};

template<typename Cmp, typename T>
struct FieldData: IFieldData<Cmp> {
    T Cmp::*member_ptr;

    FieldData(T Cmp::*ptr) : member_ptr(ptr) {}

    HUGO::ErrorOrVoid Set(Cmp& component, const HVariant& value) const override
    {
        if (auto val = std::get_if<T>(&value)) {
            component.*member_ptr = *val;
            return HUGO::ErrorOrVoid();
        }
        else {
            HUGO::StringError err;
            err.errorMessage = "Tipo incorrecto para campo";
            return HUGO::ErrorOrVoid(err);
        }
    }
};