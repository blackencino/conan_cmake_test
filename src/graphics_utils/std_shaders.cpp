#include "std_shaders.h"

namespace graphics_utils {

static std::string g_shader_header =
  R"SHADER(
#version 410
)SHADER";

static std::string g_key_fill_lighting =
  R"SHADER(
uniform vec3 surface_diffuse_gain;
uniform vec3 surface_specular_gain;
uniform float surface_specular_exponent;

uniform vec3 world_key_light_direction;
uniform vec3 key_light_rgb;

uniform vec3 world_fill_light_direction;
uniform vec3 fill_light_rgb;

float specular(vec3 Vn, vec3 Nn, vec3 Ln, float m) {
    vec3 H = normalize(Ln + Vn);
    float d = max(dot(Nn, H), 0.0);
    return pow( d, m );
}

float diffuse(vec3 Nn, vec3 Ln) {
    float d = dot(Nn, Ln);
    return clamp(d, 0, 1);
}

vec3 lighting(vec3 camera_view_direction,
              vec3 camera_normal,
              vec3 camera_light_direction,
              vec3 light_rgb,
              vec3 surface_diffuse_gain,
              vec3 surface_specular_gain,
              float surface_specular_exponent) {
    vec3 Nn = normalize(camera_normal);
    vec3 Ln = normalize(-camera_light_direction);
    vec3 Vn = normalize(camera_view_direction);
    vec3 diff = surface_diffuse_gain * diffuse(Nn, Ln);
    vec3 spec = surface_specular_gain * specular(Vn,
                                                 Nn,
                                                 Ln,
                                                 surface_specular_exponent);

    return light_rgb * (diff + spec);
}

vec3 gamma_correct(vec3 col, float gamma) {
    float inv_g = 1.0/gamma;
    return vec3(pow(max(col.r, 0.0), inv_g),
                pow(max(col.g, 0.0), inv_g),
                pow(max(col.b, 0.0), inv_g));
}
)SHADER";

//------------------------------------------------------------------------------
std::string std_vertex_shader_P_N() {
    return g_shader_header +

           R"SHADER(
layout(location = 0) in vec3 object_position;
layout(location = 1) in vec3 object_normal;

out Vertex_data {
    vec3 camera_position;
    vec3 camera_normal;
    vec3 rgb;
} out_data;

// These together make modelview
uniform mat4x4 camera_T_world;
uniform mat4x4 world_T_object;

// Projection
uniform mat4x4 clip_T_camera;

void main() {
    mat4x4 camera_T_object = camera_T_world * world_T_object;
    mat3x3 camera_Tn_object = inverse(transpose(mat3(camera_T_object)));

    vec4 camera_hposition = camera_T_object * vec4(object_position, 1);
    out_data.camera_position = vec3(camera_hposition) / camera_hposition.w;
    out_data.camera_normal = normalize(camera_Tn_object * object_normal);
    out_data.rgb = vec3(1, 1, 1);

    gl_Position = clip_T_camera * camera_hposition;
}
)SHADER";
}

//------------------------------------------------------------------------------
std::string key_fill_fragment_shader() {
    return g_shader_header + g_key_fill_lighting +
           R"SHADER(
in Vertex_data {
    vec3 camera_position;
    vec3 camera_normal;
    vec3 rgb;
} in_data;

out vec4 fragment_color;

uniform mat4x4 camera_T_world;

void main() {
    mat3x3 camera_Tr_world = mat3x3(camera_T_world);
    vec3 camera_view_direction = -normalize(in_data.camera_position);

    vec3 Ci = vec3(0, 0, 0);

    // fill
    vec3 camera_fill_light_direction = normalize(camera_Tr_world * world_fill_light_direction);
    Ci += lighting(camera_view_direction,
                   in_data.camera_normal,
                   camera_fill_light_direction,
                   fill_light_rgb,
                   in_data.rgb * surface_diffuse_gain,
                   surface_specular_gain,
                   surface_specular_exponent);

    // key
    vec3 camera_key_light_direction = normalize(camera_Tr_world * world_key_light_direction);
    Ci += lighting(camera_view_direction,
                   in_data.camera_normal,
                   camera_key_light_direction,
                   key_light_rgb,
                   in_data.rgb * surface_diffuse_gain,
                   surface_specular_gain,
                   surface_specular_exponent);

    Ci = gamma_correct(Ci, 2.2);
    fragment_color = vec4(Ci, 1);
}
)SHADER";
}

//-----------------------------------------------------------------------------
std::string instanced_sprite_vertex_shader() {
    return g_shader_header +

           R"SHADER(
layout(location = 0) in vec2 quad_corner;
layout(location = 1) in vec3 center;
layout(location = 2) in vec3 in_rgb;

out Vertex_data {
    vec3 camera_position;
    vec2 plane_offset;
    vec3 rgb;
} out_data;

uniform float object_radius;

// These together make modelview
uniform mat4x4 camera_T_world;
uniform mat4x4 world_T_object;

// Projection
uniform mat4x4 clip_T_camera;

#define SQRT_3 1.73205080757

void main() {
    mat4x4 camera_T_object = camera_T_world * world_T_object;
    mat3x3 camera_Tr_object = mat3x3(camera_T_object);

    float camera_Tsu_object = length(camera_Tr_object * vec3(1, 1, 1)) / SQRT_3; 
    float camera_radius = camera_Tsu_object * object_radius;

    out_data.plane_offset = quad_corner;
    
    vec4 camera_center_hposition = camera_T_object * vec4(center, 1);
    vec3 camera_center_position = vec3(camera_center_hposition) / camera_center_hposition.w;

    vec3 camera_position = camera_center_position + 0.5 * camera_radius * vec3(quad_corner, 0);
    out_data.camera_position = camera_position;

    vec4 camera_hposition = vec4(camera_position, 1);
    out_data.rgb = in_rgb;

    gl_Position = clip_T_camera * camera_hposition;
}

)SHADER";
}

//-----------------------------------------------------------------------------
std::string instanced_sprite_key_fill_fragment_shader() {
    return g_shader_header + g_key_fill_lighting +
           R"SHADER(
in Vertex_data {
    vec3 camera_position;
    vec2 plane_offset;
    vec3 rgb;
} in_data;

out vec4 fragment_color;

uniform mat4x4 camera_T_world;

void main() {
    // calculate normal from texture coordinates
    float mag = length(in_data.plane_offset);
    if (mag > 1.0) discard;

    vec3 camera_normal = normalize(vec3(in_data.plane_offset, sqrt(1.0 - mag)));

    // The normal is currently in the coordinate space of the sprite
    // which is close enough the the coordinate space of the camera. It's not exactly right,
    // but this is a cheap approximation either way.

    mat3x3 camera_Tr_world = mat3x3(camera_T_world);
    vec3 camera_view_direction = -normalize(in_data.camera_position);

    vec3 Ci = vec3(0, 0, 0);

    // fill
    vec3 camera_fill_light_direction = normalize(camera_Tr_world * world_fill_light_direction);
    Ci += lighting(camera_view_direction,
                   camera_normal,
                   camera_fill_light_direction,
                   fill_light_rgb,
                   in_data.rgb * surface_diffuse_gain,
                   surface_specular_gain,
                   surface_specular_exponent);

    // key
    vec3 camera_key_light_direction = normalize(camera_Tr_world * world_key_light_direction);
    Ci += lighting(camera_view_direction,
                   camera_normal,
                   camera_key_light_direction,
                   key_light_rgb,
                   in_data.rgb * surface_diffuse_gain,
                   surface_specular_gain,
                   surface_specular_exponent);

    Ci = gamma_correct(Ci, 2.2);
    fragment_color = vec4(Ci, 1);
}
)SHADER";
}

}  // namespace graphics_utils
