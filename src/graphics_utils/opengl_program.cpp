#include "graphics_utils/opengl_program.h"

#include <array>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <string>

namespace graphics_utils {

//-----------------------------------------------------------------------------
Shader_stage::Shader_stage(std::string const& name,
                           GLenum const type,
                           std::string const& source) {
    std::array<GLchar const*, 1> source_array{
      static_cast<GLchar const*>(source.c_str())};

    id = glCreateShader(type);
    if (id == 0) {
        throw std::runtime_error{"Could not create shader: " + name};
    }

    glShaderSource(id, 1, source_array.data(), nullptr);
    glCompileShader(id);
    GLint compiled = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<GLchar> log;
        log.resize(length + 1);
        glGetShaderInfoLog(id, length, &length, log.data());

        auto const error_string =
          "Compilation error in shader: " +
          std::string{static_cast<const char*>(log.data())};
        std::cerr << "Compilation error: " << error_string << std::endl;
        throw std::runtime_error{error_string};
    }
}

Shader_stage::Shader_stage(Shader_stage&& other)
  : id(other.id) {
    other.id = 0;
}

Shader_stage& Shader_stage::operator=(Shader_stage&& other) {
    if (id != 0) {
        glDeleteShader(id);
        id = 0;
    }

    id = other.id;
    other.id = 0;
    return *this;
}

Shader_stage::~Shader_stage() {
    if (id) {
        glDeleteShader(id);
        id = 0;
    }
}

//-----------------------------------------------------------------------------
Program::Program(std::string const& name,
                 std::string const& vertex_source,
                 std::string const& geometry_source,
                 std::string const& fragment_source) {
    // Do the program creation.
    id = glCreateProgram();
    if (id == 0) {
        throw std::runtime_error{"Couldn't allocate GLSL program: " + name};
    }

    vertex_stage = std::make_unique<Shader_stage>(
      name + "::vertex", GL_VERTEX_SHADER, vertex_source);
    glAttachShader(id, vertex_stage->id);
    if (!geometry_source.empty()) {
        geometry_stage = std::make_unique<Shader_stage>(
          name + "::geometry", GL_GEOMETRY_SHADER, geometry_source);
        glAttachShader(id, geometry_stage->id);
    }
    fragment_stage = std::make_unique<Shader_stage>(
      name + "::fragment", GL_FRAGMENT_SHADER, fragment_source);
    glAttachShader(id, fragment_stage->id);

    glLinkProgram(id);

    GLint linked = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);

        std::vector<GLchar> log;
        log.resize(length + 1);
        glGetProgramInfoLog(id, length, &length, log.data());
        throw std::runtime_error{
          "Linking error in program: " + name + " " +
          std::string{static_cast<GLchar const*>(log.data())}};
    }
}

Program::Program(Program&& other)
  : id(other.id)
  , vertex_stage(std::move(other.vertex_stage))
  , geometry_stage(std::move(other.geometry_stage))
  , fragment_stage(std::move(other.fragment_stage)) {
    other.id = 0;
}

Program& Program::operator=(Program&& other) {
    if (id != 0) {
        glDeleteProgram(id);
        id = 0;
    }

    vertex_stage = std::move(other.vertex_stage);
    geometry_stage = std::move(other.geometry_stage);
    fragment_stage = std::move(other.fragment_stage);
    return *this;
}

Program::~Program() {
    if (id) {
        glDeleteProgram(id);
        id = 0;
    }
}

void bind(Program& program) {
    glUseProgram(program.id);
}

void unbind(Program&) {
    glUseProgram(0);
}

}  // namespace graphics_utils
