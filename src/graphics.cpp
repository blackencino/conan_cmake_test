#include "graphics.h"

#include "graphics_utils/scoped_binding.h"
#include "graphics_utils/std_shaders.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace opengl_cuda_test {

static constexpr int CUBE_VERTEX_COUNT = 36;

Scene make_scene() {
    std::vector<int> a{{5, 4, 3, 2}};

    Scene scene;

    // clang-format off
    std::vector<glm::vec3> cube_vertices_unwound = {
        // -Z Face
        {-0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f, -0.5f},
         {0.5f,  0.5f, -0.5f},
         {0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},

        // +Z Face
        {-0.5f, -0.5f,  0.5f},
         {0.5f, -0.5f,  0.5f},
         {0.5f,  0.5f,  0.5f},
         {0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},

        // -X Face
        {-0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},

        // X Face
         {0.5f,  0.5f,  0.5f},
         {0.5f,  0.5f, -0.5f},
         {0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f,  0.5f},
         {0.5f,  0.5f,  0.5f},

         // -Y Face
        {-0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f, -0.5f},
         {0.5f, -0.5f,  0.5f},
         {0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f},

        // Y Face
        {-0.5f,  0.5f, -0.5f},
         {0.5f,  0.5f, -0.5f},
         {0.5f,  0.5f,  0.5f},
         {0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f}
    };

    std::vector<glm::vec3> cube_normals_unwound = {
        // -Z Face
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},
        {0.0f, 0.0f, -1.0f},

        // +Z Face
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},
        {0.0f, 0.0f, 1.0f},

        // -X Face
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},
        {-1.0f, 0.0f, 0.0f},

        // X Face
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},

         // -Y Face
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},
        {0.0f, -1.0f, 0.0f},

        // Y Face
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}
    };
    // clang-format on

    scene.cube_vertex_array = std::make_unique<Vertex_array>();
    scene.cube_positions_buffer = std::make_unique<Buffer>();
    scene.cube_normals_buffer = std::make_unique<Buffer>();

    // Create cube in vao
    {
        auto const vertex_array_binding =
          make_scoped_binding(*(scene.cube_vertex_array));

        // Bind positions to vertex_array
        {
            auto const position_buffer_binding =
              make_scoped_binding(*(scene.cube_positions_buffer),
                                  static_cast<GLenum>(GL_ARRAY_BUFFER));
            glBufferData(
              GL_ARRAY_BUFFER,
              static_cast<GLsizei>(cube_vertices_unwound.size() *
                                   sizeof(glm::vec3)),
              static_cast<GLvoid const*>(cube_vertices_unwound.data()),
              GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }

        // Bind normals to vertex array
        {
            auto const normal_buffer_binding =
              make_scoped_binding(*(scene.cube_normals_buffer),
                                  static_cast<GLenum>(GL_ARRAY_BUFFER));
            glBufferData(
              GL_ARRAY_BUFFER,
              static_cast<GLsizei>(cube_normals_unwound.size() *
                                   sizeof(glm::vec3)),
              static_cast<GLvoid const*>(cube_normals_unwound.data()),
              GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }
    }

    // Camera
    scene.camera =
      look_at(Camera{}, glm::dvec3{3.0, 1.0, 5.0}, glm::dvec3{0.0, 0.0, 0.0});

    // Create program
    scene.program = std::make_unique<Program>(
      "std_key_fill", std_vertex_shader_P_N(), "", key_fill_fragment_shader());

    scene.uniform_locations.camera_T_world =
      glGetUniformLocation(scene.program->id, "camera_T_world");
    scene.uniform_locations.world_T_object =
      glGetUniformLocation(scene.program->id, "world_T_object");
    scene.uniform_locations.clip_T_camera =
      glGetUniformLocation(scene.program->id, "clip_T_camera");
    scene.uniform_locations.surface_diffuse_gain =
      glGetUniformLocation(scene.program->id, "surface_diffuse_gain");
    scene.uniform_locations.surface_specular_gain =
      glGetUniformLocation(scene.program->id, "surface_specular_gain");
    scene.uniform_locations.surface_specular_exponent =
      glGetUniformLocation(scene.program->id, "surface_specular_exponent");
    scene.uniform_locations.world_key_light_direction =
      glGetUniformLocation(scene.program->id, "world_key_light_direction");
    scene.uniform_locations.key_light_rgb =
      glGetUniformLocation(scene.program->id, "key_light_rgb");
    scene.uniform_locations.world_fill_light_direction =
      glGetUniformLocation(scene.program->id, "world_fill_light_direction");
    scene.uniform_locations.fill_light_rgb =
      glGetUniformLocation(scene.program->id, "fill_light_rgb");

    scene.key_light.direction = -glm::normalize(glm::vec3{10.0f, 10.0f, 10.0f});
    scene.fill_light.direction =
      -glm::normalize(glm::vec3{-5.0f, -5.0f, 10.0f});
    scene.fill_light.rgb = glm::vec3{0.1f, 0.2f, 0.6f};

    return scene;
}

void draw(Scene const& scene, glm::dvec2 const& view_size) {
    auto const program_bind = make_scoped_binding(*(scene.program));

    // Camera (global)
    auto const camera_T_world = glm::mat4x4{model_view_matrix(scene.camera)};
    auto const world_T_camera = glm::inverse(camera_T_world);
    glUniformMatrix4fv(scene.uniform_locations.camera_T_world,
                       1,
                       GL_FALSE,
                       glm::value_ptr(camera_T_world));

    glUniformMatrix4fv(
      scene.uniform_locations.clip_T_camera,
      1,
      GL_FALSE,
      glm::value_ptr(glm::mat4x4{projection_matrix(scene.camera, view_size)}));

    // Lights (global)
    auto vtransform = [](glm::mat4x4 const& m, glm::vec3 const& v) {
        return glm::vec3{m * glm::vec4{v, 0.0f}};
    };

    glUniform3fv(
      scene.uniform_locations.world_key_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.key_light.direction)));
    glUniform3fv(scene.uniform_locations.key_light_rgb,
                 1,
                 glm::value_ptr(scene.key_light.rgb));

    glUniform3fv(
      scene.uniform_locations.world_fill_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.fill_light.direction)));
    glUniform3fv(scene.uniform_locations.fill_light_rgb,
                 1,
                 glm::value_ptr(scene.fill_light.rgb));

    // Cube transform
    glUniformMatrix4fv(scene.uniform_locations.world_T_object,
                       1,
                       GL_FALSE,
                       glm::value_ptr(scene.world_T_cube));

    // Cube material
    glUniform3fv(scene.uniform_locations.surface_diffuse_gain,
                 1,
                 glm::value_ptr(scene.cube_material.diffuse_gain));
    glUniform3fv(scene.uniform_locations.surface_specular_gain,
                 1,
                 glm::value_ptr(scene.cube_material.specular_gain));
    glUniform1f(scene.uniform_locations.surface_specular_exponent,
                scene.cube_material.specular_exponent);

    // Draw cube
    auto const cube_bind = make_scoped_binding(*(scene.cube_vertex_array));
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

}  // namespace opengl_cuda_test
