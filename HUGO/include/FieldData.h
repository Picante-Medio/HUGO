#pragma once

#include "HugoError.h"

template<typename Cmp>
struct IFieldData {
    virtual ErrorOrVoid Set(Cmp& obj, const Variant& value) const = 0;
    virtual ~IFieldData() = default;
};

template<typename Cmp, typename T>
struct FieldData: IFieldData<Cmp> {
    T Cmp::*member_ptr;

    FieldData(T Cmp::*ptr) : member_ptr(ptr) {}

    ErrorOrVoid Set(Cmp& component, const Variant& value) const override
    {
        if (auto val = std::get_if<T>(&value)) {
            component.*member_ptr = *val;
            return ErrorOrVoid();
        }
        else {
            StringError err;
            err.errorMessage = "Tipo incorrecto para campo";
            return ErrorOrVoid(err);
        }
    }
};