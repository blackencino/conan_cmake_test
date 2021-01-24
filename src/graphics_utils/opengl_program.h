#pragma once

#include <glad/glad.h>

#include <memory>
#include <string>

namespace graphics_utils {

//-----------------------------------------------------------------------------
struct Shader_stage {
    GLuint id = 0;
    Shader_stage(std::string const& name,
                 GLenum const type,
                 std::string const& source);
    ~Shader_stage();
    Shader_stage(Shader_stage&& other);
    Shader_stage& operator=(Shader_stage&& other);

private:
    Shader_stage() = delete;
    Shader_stage(Shader_stage const&) = delete;
    Shader_stage& operator=(Shader_stage const&) = delete;
};

//-----------------------------------------------------------------------------
struct Program {
    GLuint id = 0;
    Program(std::string const& name,
            std::string const& vertex_source,
            std::string const& geometry_source,
            std::string const& fragment_source);
    Program(Program&& other);
    Program& operator=(Program&& other);
    ~Program();

    std::unique_ptr<Shader_stage> vertex_stage;
    std::unique_ptr<Shader_stage> geometry_stage;
    std::unique_ptr<Shader_stage> fragment_stage;

private:
    Program() = delete;
    Program(Program const&) = delete;
    Program& operator=(Program const&) = delete;
};

void bind(Program& program);
void unbind(Program& program);

}  // namespace graphics_utils