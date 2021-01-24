#include "graphics_utils/camera.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace graphics_utils {

static auto rotate_vector(double const rx,
                          double const ry,
                          glm::dvec3 const& v) {
    return glm::rotateY(glm::rotateX(v, rx), ry);
}

std::array<double, 2> auto_clipping_planes(Camera const& camera,
                                           daabb3 const& bounds) {
    auto const bsize = size(bounds);
    auto const tiny = 0.0001 * std::min(std::min(bsize.x, bsize.y), bsize.z);

    auto clip_near = std::numeric_limits<double>::max();
    auto clip_far = std::numeric_limits<double>::min();

    auto const to_target =
      rotate_vector(camera.rotation.x,
                    camera.rotation.y,
                    glm::dvec3{0.0, 0.0, -camera.center_of_interest});
    auto const target = camera.translation + to_target;
    auto const to_target_dir = glm::normalize(to_target);

    for (auto const bx : {bounds.min.x, bounds.max.x}) {
        for (auto const by : {bounds.min.y, bounds.max.y}) {
            for (auto const bz : {bounds.min.z, bounds.max.z}) {
                auto const dp = glm::dvec3{bx, by, bz} - camera.translation;
                auto const proj = glm::dot(dp, to_target_dir);
                clip_near = std::min(proj, clip_near);
                clip_far = std::max(proj, clip_far);
            }
        }
    }

    clip_near -= tiny;
    clip_far += tiny;
    clip_near = std::clamp(clip_near, tiny, 1.0e30);
    clip_far = std::clamp(clip_far, tiny, 1.0e30);

    // EMLD_ASSERT(clip_far > clip_near, "Bad clipping");

    return {clip_near, clip_far};
}

Camera frame(Camera const& camera, daabb3 const& bounds) {
    auto const r = 0.5 * glm::length(size(bounds));

    auto const g = (1.1 * r) / std::sin(camera.fovy * 0.5);
    auto const cen = center(bounds);
    return look_at(camera, cen + glm::dvec3{0.0, 0.0, g}, cen);
}

Camera look_at(Camera const& camera,
               glm::dvec3 const& eye,
               glm::dvec3 const& at) {
    auto const to_target = at - eye;
    auto const xz_len = std::hypot(to_target.x, to_target.z);

    auto const rotation_x = std::atan2(to_target.y, xz_len);
    auto const rotation_y = std::atan2(-to_target.x, -to_target.z);

    return {glm::dvec3{rotation_x, rotation_y,
                       0.0},
            eye,
            glm::length(to_target),
            camera.fovy,
            camera.clip};
}

glm::dmat4x4 model_view_matrix(Camera const& camera) {
    auto const world_T_camera =
      glm::translate(camera.translation) *
      glm::rotate(camera.rotation.y, glm::dvec3{0.0, 1.0, 0.0}) *
      glm::rotate(camera.rotation.x, glm::dvec3{1.0, 0.0, 0.0}) * 
        glm::rotate(camera.rotation.z, glm::dvec3{0.0, 0.0, 1.0});

    return glm::inverse(world_T_camera);

    //return //glm::rotate(-camera.rotation.z, glm::dvec3{0.0, 0.0, 1.0}) *
    //       //glm::rotate(-camera.rotation.x, glm::dvec3{1.0, 0.0, 0.0}) *
    //       glm::rotate(-camera.rotation.y, glm::dvec3{0.0, 1.0, 0.0}) *
    //       glm::translate(-camera.translation);
}

glm::dmat4x4 projection_matrix(Camera const& camera, glm::dvec2 const& view_size) {
    auto const aspect = view_size.x / view_size.y;
    return glm::perspective<double>(
      camera.fovy, aspect, camera.clip[0], camera.clip[1]);
}

Camera track(Camera const& camera, glm::dvec2 const& point, glm::dvec2 const& view_size) {
    auto const ds = rotate_vector(
      camera.rotation.x, camera.rotation.y, glm::dvec3{1.0, 0.0, 0.0});
    auto const dt = rotate_vector(
      camera.rotation.x, camera.rotation.y, glm::dvec3{0.0, 1.0, 0.0});

    auto mult_s = 2.0 * camera.center_of_interest * std::tan(camera.fovy / 2.0);
    auto const mult_t = mult_s / view_size.y;
    mult_s /= view_size.x;

    auto const s = -mult_s * point.x;
    auto const t = mult_t * point.y;

    return {camera.rotation,
            camera.translation + (s * ds) + (t * dt),
            camera.center_of_interest,
            camera.fovy,
            camera.clip};
}

Camera dolly(Camera const& camera,
             glm::dvec2 const& point,
             glm::dvec2 const& view_size,
             double const dolly_speed) {
    auto const to_target =
      rotate_vector(camera.rotation.x,
                    camera.rotation.y,
                    glm::dvec3{0.0, 0.0, -camera.center_of_interest});
    auto const target = camera.translation + to_target;
    auto const to_target_dir = glm::normalize(to_target);

    auto const t = point.x / view_size.x;
    double dolly_by = 1.0 - std::exp(-dolly_speed * t);

    // EMLD_ASSERT(std::abs(dolly_by) < 1.0, "bad dolly");
    dolly_by *= camera.center_of_interest;
    auto const new_translation =
      camera.translation + (dolly_by * to_target_dir);

    return {camera.rotation,
            new_translation,
            glm::length(new_translation - target),
            camera.fovy,
            camera.clip};
}

Camera rotate(Camera const& camera,
              glm::dvec2 const& point,
              glm::dvec2 const& view_size,
              double const rotate_speed) {
    auto const to_target =
      rotate_vector(camera.rotation.x,
                    camera.rotation.y,
                    glm::dvec3{0.0, 0.0, -camera.center_of_interest});

    auto const target = camera.translation + to_target;

    auto const new_rotation =
      camera.rotation - rotate_speed * glm::dvec3{point.y / view_size.y,
                                                  point.x / view_size.x,
                                                  0.0};

    auto const new_to_target =
      rotate_vector(new_rotation.x,
                    new_rotation.y,
                    glm::dvec3{0.0, 0.0, -camera.center_of_interest});

    return {new_rotation,
            target - new_to_target,
            camera.center_of_interest,
            camera.fovy,
            camera.clip};
}

}  // namespace graphics_utils
