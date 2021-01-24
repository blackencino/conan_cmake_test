#pragma once

// From:
// https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#include "graphics_utils/glm_common.h"
#include "graphics_utils/aabb.h"

#include <array>

namespace graphics_utils {

//------------------------------------------------------------------------------
// 3D Camera
struct Camera {
    // Euler angles are bad, we should fix this.
    glm::dvec3 rotation = glm::dvec3{0.0, 0.0, 0.0};
    glm::dvec3 translation = glm::dvec3{0.0, 0.0, 0.0};

    double center_of_interest = 15.0;
    double fovy = glm::radians(45.0);
    std::array<double, 2> clip = {0.1, 1.0e30};
};

glm::dmat4x4 model_view_matrix(Camera const& camera);
glm::dmat4x4 projection_matrix(Camera const& camera, glm::dvec2 const& view_size);

std::array<double, 2> auto_clipping_planes(Camera const& camera, daabb3 const& bounds);
Camera track(Camera const& camera, glm::dvec2 const& point, glm::dvec2 const& view_size);
Camera dolly(Camera const& camera,
               glm::dvec2 const& point,
               glm::dvec2 const& view_size,
               double const dollySpeed = 5.0);
Camera rotate(Camera const& camera,
                glm::dvec2 const& point,
                glm::dvec2 const& view_size,
                double const rotateSpeed = glm::radians(400.0));
Camera frame(Camera const& camera, daabb3 const& bounds);
Camera look_at(Camera const& camera, glm::dvec3 const& eye, glm::dvec3 const& at);

}  // namespace graphics_utils
