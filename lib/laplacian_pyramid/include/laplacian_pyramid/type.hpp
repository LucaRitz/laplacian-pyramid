#pragma once

#include "macro_definition.hpp"

namespace laplacian {
    struct EXPORT_LAPLACIAN_PYRAMID Type {
        explicit Type(int value = 0);

        int _value;
    };
}