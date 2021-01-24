#pragma once

#include "graphics_utils/glm_common.h"

namespace graphics_utils {

//-----------------------------------------------------------------------------
// Ultra bare-bones axis-aligned bounding box.

template <typename T>
struct taabb3;

template <>
struct taabb3<float> {
    glm::vec3 min;
    glm::vec3 max;
};

template <>
struct taabb3<double> {
    glm::dvec3 min;
    glm::dvec3 max;
};

using aabb3 = taabb3<float>;
using daabb3 = taabb3<double>;

template <typename T>
auto size(taabb3<T> const& bounds) {
    return bounds.max - bounds.min;
}

template <typename T>
auto center(taabb3<T> const& bounds) {
    return (bounds.min + bounds.max) / T(2);
}

}  // namespace graphics_utils
