#pragma once

#include "graphics_utils/glm_common.h"
#include "graphics_utils/opengl_wrappers.h"
#include "graphics_utils/opengl_program.h"
#include "graphics_utils/camera.h"

#include <glad/glad.h>

#include <memory>

namespace opengl_cuda_test {

using namespace graphics_utils;

struct Uniform_locations {
    GLint camera_T_world = -1;
    GLint world_T_object = -1;
    GLint clip_T_camera = -1;
    GLint surface_diffuse_gain = -1;
    GLint surface_specular_gain = -1;
    GLint surface_specular_exponent = -1;
    GLint world_key_light_direction = -1;
    GLint key_light_rgb = -1;
    GLint world_fill_light_direction = -1;
    GLint fill_light_rgb = -1;
};

struct Light {
    glm::vec3 direction = glm::vec3{0.0f, -1.0f, 0.0f};
    glm::vec3 rgb = glm::vec3{1.0f, 1.0f, 1.0f};
};

struct Material {
    glm::vec3 diffuse_gain = glm::vec3{0.18f, 0.18f, 0.18f};
    glm::vec3 specular_gain = glm::vec3{0.9f, 0.9f, 0.9f};
    float specular_exponent = 90.0f;
};


struct Scene {
    std::unique_ptr<Vertex_array> cube_vertex_array;
    std::unique_ptr<Buffer> cube_positions_buffer;
    std::unique_ptr<Buffer> cube_normals_buffer;

    std::unique_ptr<Program> program;
    Uniform_locations uniform_locations;

    Camera camera;

    Light key_light;
    Light fill_light;

    Material cube_material;
    glm::mat4x4 world_T_cube = glm::mat4x4{1.0f};
};

Scene make_scene();
void draw(Scene const& scene, glm::dvec2 const& view_size);

} // namespace

