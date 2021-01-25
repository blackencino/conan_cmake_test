#include "graphics.h"

#include "graphics_utils/scoped_binding.h"
#include "graphics_utils/std_shaders.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <random>
#include <vector>

namespace opengl_cuda_test {

static constexpr int CUBE_VERTEX_COUNT = 36;
static constexpr int POINT_COUNT = 4096;
static constexpr uint64_t SEED = 54321;
static constexpr float POINT_RADIUS = 0.1f;

Scene make_scene() {
    Scene scene;

    std::mt19937_64 rand_gen{SEED};
    std::uniform_real_distribution<float> xyz_dist{0.0f, 5.0f};
    std::uniform_real_distribution<float> rgb_dist{0.1f, 0.9f};

    scene.points_centers.resize(POINT_COUNT);
    scene.points_rgbs.resize(POINT_COUNT);
    for (int i = 0; i < POINT_COUNT; ++i) {
        scene.points_centers[i] = glm::vec4{
          xyz_dist(rand_gen), xyz_dist(rand_gen), xyz_dist(rand_gen), 0.0f};
        scene.points_rgbs[i] = glm::vec4{
          rgb_dist(rand_gen), rgb_dist(rand_gen), rgb_dist(rand_gen), 1.0f};
    }

    std::vector<glm::vec2> quad_corners = {
      {-1.0f, -1.0f}, {1.0f, -1.0f}, {-1.0f, 1.0f}, {1.0f, 1.0f}};

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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(0);
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
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            glEnableVertexAttribArray(1);
        }
    }

    scene.points_vertex_array = std::make_unique<Vertex_array>();
    scene.points_quad_corners_buffer = std::make_unique<Buffer>();
    scene.points_centers_buffer = std::make_unique<Buffer>();
    scene.points_rgbs_buffer = std::make_unique<Buffer>();

    // Create points
    {
        auto const vertex_array_binding =
          make_scoped_binding(*(scene.points_vertex_array));

        // Quad corners
        {
            auto const quad_corners_buffer_binding =
              make_scoped_binding(*(scene.points_quad_corners_buffer),
                                  static_cast<GLenum>(GL_ARRAY_BUFFER));

            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizei>(4 * sizeof(glm::vec2)),
                         static_cast<GLvoid const*>(quad_corners.data()),
                         GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            glVertexAttribDivisor(0, 0);
        }

        // centers
        {
            auto const centers_buffer_binding =
              make_scoped_binding(*(scene.points_centers_buffer),
                                  static_cast<GLenum>(GL_ARRAY_BUFFER));

            glBufferData(
              GL_ARRAY_BUFFER,
              static_cast<GLsizei>(scene.points_centers.size() *
                                   sizeof(glm::vec4)),
              static_cast<GLvoid const*>(scene.points_centers.data()),
              GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(
              1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
            glVertexAttribDivisor(1, 1);
        }

        // rgbs
        {
            auto const rgbs_buffer_binding =
              make_scoped_binding(*(scene.points_rgbs_buffer),
                                  static_cast<GLenum>(GL_ARRAY_BUFFER));

            glBufferData(GL_ARRAY_BUFFER,
                         static_cast<GLsizei>(scene.points_rgbs.size() *
                                              sizeof(glm::vec4)),
                         static_cast<GLvoid const*>(scene.points_rgbs.data()),
                         GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(2);
            glVertexAttribPointer(
              2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
            glVertexAttribDivisor(2, 1);
        }
    }

    // Camera
    scene.camera =
      look_at(Camera{}, glm::dvec3{3.0, 1.0, 5.0}, glm::dvec3{0.0, 0.0, 0.0});

    // Create program
    scene.cube_program = std::make_unique<Program>(
      "std_key_fill", std_vertex_shader_P_N(), "", key_fill_fragment_shader());

    scene.cube_uniform_locations.camera_T_world =
      glGetUniformLocation(scene.cube_program->id, "camera_T_world");
    scene.cube_uniform_locations.world_T_object =
      glGetUniformLocation(scene.cube_program->id, "world_T_object");
    scene.cube_uniform_locations.clip_T_camera =
      glGetUniformLocation(scene.cube_program->id, "clip_T_camera");
    scene.cube_uniform_locations.surface_diffuse_gain =
      glGetUniformLocation(scene.cube_program->id, "surface_diffuse_gain");
    scene.cube_uniform_locations.surface_specular_gain =
      glGetUniformLocation(scene.cube_program->id, "surface_specular_gain");
    scene.cube_uniform_locations.surface_specular_exponent =
      glGetUniformLocation(scene.cube_program->id, "surface_specular_exponent");
    scene.cube_uniform_locations.world_key_light_direction =
      glGetUniformLocation(scene.cube_program->id, "world_key_light_direction");
    scene.cube_uniform_locations.key_light_rgb =
      glGetUniformLocation(scene.cube_program->id, "key_light_rgb");
    scene.cube_uniform_locations.world_fill_light_direction =
      glGetUniformLocation(scene.cube_program->id,
                           "world_fill_light_direction");
    scene.cube_uniform_locations.fill_light_rgb =
      glGetUniformLocation(scene.cube_program->id, "fill_light_rgb");

    // Create program
    scene.points_program =
      std::make_unique<Program>("std_key_fill",
                                instanced_sprite_vertex_shader(),
                                "",
                                instanced_sprite_key_fill_fragment_shader());

    scene.points_uniform_locations.camera_T_world =
      glGetUniformLocation(scene.points_program->id, "camera_T_world");
    scene.points_uniform_locations.world_T_object =
      glGetUniformLocation(scene.points_program->id, "world_T_object");
    scene.points_uniform_locations.clip_T_camera =
      glGetUniformLocation(scene.points_program->id, "clip_T_camera");

    scene.points_uniform_locations.object_radius =
      glGetUniformLocation(scene.points_program->id, "object_radius");

    scene.points_uniform_locations.surface_diffuse_gain =
      glGetUniformLocation(scene.points_program->id, "surface_diffuse_gain");
    scene.points_uniform_locations.surface_specular_gain =
      glGetUniformLocation(scene.points_program->id, "surface_specular_gain");
    scene.points_uniform_locations.surface_specular_exponent =
      glGetUniformLocation(scene.points_program->id,
                           "surface_specular_exponent");
    scene.points_uniform_locations.world_key_light_direction =
      glGetUniformLocation(scene.points_program->id,
                           "world_key_light_direction");
    scene.points_uniform_locations.key_light_rgb =
      glGetUniformLocation(scene.points_program->id, "key_light_rgb");
    scene.points_uniform_locations.world_fill_light_direction =
      glGetUniformLocation(scene.points_program->id,
                           "world_fill_light_direction");
    scene.points_uniform_locations.fill_light_rgb =
      glGetUniformLocation(scene.points_program->id, "fill_light_rgb");

    scene.key_light.direction = -glm::normalize(glm::vec3{5.0f, 5.0f, 10.0f});
    scene.key_light.rgb = glm::vec3{1.0f, 1.0f, 1.0f};

    scene.fill_light.direction =
      -glm::normalize(glm::vec3{-5.0f, -5.0f, 10.0f});
    scene.fill_light.rgb = glm::vec3{0.1f, 0.2f, 0.6f};

    scene.points_material.specular_exponent = 7.0f;
    scene.points_material.specular_gain = glm::vec3{0.05f, 0.05f, 0.05f};

    return scene;
}

void draw_cube(Scene const& scene,
               glm::dvec2 const& view_size,
               glm::mat4x4 const& camera_T_world,
               glm::mat4x4 const& world_T_camera) {
    auto const program_bind = make_scoped_binding(*(scene.cube_program));

    // Camera (global)
    glUniformMatrix4fv(scene.cube_uniform_locations.camera_T_world,
                       1,
                       GL_FALSE,
                       glm::value_ptr(camera_T_world));

    glUniformMatrix4fv(
      scene.cube_uniform_locations.clip_T_camera,
      1,
      GL_FALSE,
      glm::value_ptr(glm::mat4x4{projection_matrix(scene.camera, view_size)}));

    // Lights (global)
    auto vtransform = [](glm::mat4x4 const& m, glm::vec3 const& v) {
        return glm::vec3{m * glm::vec4{v, 0.0f}};
    };

    glUniform3fv(
      scene.cube_uniform_locations.world_key_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.key_light.direction)));
    glUniform3fv(scene.cube_uniform_locations.key_light_rgb,
                 1,
                 glm::value_ptr(scene.key_light.rgb));

    glUniform3fv(
      scene.cube_uniform_locations.world_fill_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.fill_light.direction)));
    glUniform3fv(scene.cube_uniform_locations.fill_light_rgb,
                 1,
                 glm::value_ptr(scene.fill_light.rgb));

    // Cube transform
    glUniformMatrix4fv(scene.cube_uniform_locations.world_T_object,
                       1,
                       GL_FALSE,
                       glm::value_ptr(scene.world_T_cube));

    // Cube material
    glUniform3fv(scene.cube_uniform_locations.surface_diffuse_gain,
                 1,
                 glm::value_ptr(scene.cube_material.diffuse_gain));
    glUniform3fv(scene.cube_uniform_locations.surface_specular_gain,
                 1,
                 glm::value_ptr(scene.cube_material.specular_gain));
    glUniform1f(scene.cube_uniform_locations.surface_specular_exponent,
                scene.cube_material.specular_exponent);

    // Draw cube
    auto const cube_bind = make_scoped_binding(*(scene.cube_vertex_array));
    glDrawArrays(GL_TRIANGLES, 0, CUBE_VERTEX_COUNT);
}

void draw_points(Scene const& scene,
                 glm::dvec2 const& view_size,
                 glm::mat4x4 const& camera_T_world,
                 glm::mat4x4 const& world_T_camera) {
    auto const program_bind = make_scoped_binding(*(scene.points_program));

    // Camera (global)
    glUniformMatrix4fv(scene.points_uniform_locations.camera_T_world,
                       1,
                       GL_FALSE,
                       glm::value_ptr(camera_T_world));

    glUniformMatrix4fv(
      scene.points_uniform_locations.clip_T_camera,
      1,
      GL_FALSE,
      glm::value_ptr(glm::mat4x4{projection_matrix(scene.camera, view_size)}));

    glUniform1f(scene.points_uniform_locations.object_radius, POINT_RADIUS);

    // Lights (global)
    auto vtransform = [](glm::mat4x4 const& m, glm::vec3 const& v) {
        return glm::vec3{m * glm::vec4{v, 0.0f}};
    };

    glUniform3fv(
      scene.points_uniform_locations.world_key_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.key_light.direction)));
    glUniform3fv(scene.points_uniform_locations.key_light_rgb,
                 1,
                 glm::value_ptr(scene.key_light.rgb));

    glUniform3fv(
      scene.points_uniform_locations.world_fill_light_direction,
      1,
      glm::value_ptr(vtransform(world_T_camera, scene.fill_light.direction)));
    glUniform3fv(scene.points_uniform_locations.fill_light_rgb,
                 1,
                 glm::value_ptr(scene.fill_light.rgb));

    // points transform
    glUniformMatrix4fv(scene.points_uniform_locations.world_T_object,
                       1,
                       GL_FALSE,
                       glm::value_ptr(scene.world_T_points));

    // points material
    glUniform3fv(scene.points_uniform_locations.surface_diffuse_gain,
                 1,
                 glm::value_ptr(scene.points_material.diffuse_gain));
    glUniform3fv(scene.points_uniform_locations.surface_specular_gain,
                 1,
                 glm::value_ptr(scene.points_material.specular_gain));
    glUniform1f(scene.points_uniform_locations.surface_specular_exponent,
                scene.points_material.specular_exponent);

    // Draw points
    auto const points_bind = make_scoped_binding(*(scene.points_vertex_array));
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, POINT_COUNT);
}

void draw(Scene const& scene, glm::dvec2 const& view_size) {
    // Camera (global)
    auto const camera_T_world = glm::mat4x4{model_view_matrix(scene.camera)};
    auto const world_T_camera = glm::mat4x4{1.0f};
    //glm::inverse(camera_T_world);

    glEnable(GL_DEPTH_TEST);
    draw_cube(scene, view_size, camera_T_world, world_T_camera);
    draw_points(scene, view_size, camera_T_world, world_T_camera);
}

}  // namespace opengl_cuda_test
