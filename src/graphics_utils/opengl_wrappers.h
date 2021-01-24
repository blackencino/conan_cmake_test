#pragma once

#include <glad/glad.h>

namespace graphics_utils {

//-----------------------------------------------------------------------------
struct Vertex_array {
    GLuint id = 0;
    Vertex_array();
    ~Vertex_array();

    Vertex_array(Vertex_array&& other);
    Vertex_array& operator=(Vertex_array&& other);

private:
    Vertex_array(Vertex_array const&) = delete;
    Vertex_array& operator=(Vertex_array const&) = delete;
};

void bind(Vertex_array const& vao);
void unbind(Vertex_array const& vao);

//-----------------------------------------------------------------------------
struct Buffer {
    GLuint id = 0;
    Buffer();
    ~Buffer();

    Buffer(Buffer&& other);
    Buffer& operator=(Buffer&& other);

private:
    Buffer(Buffer const&) = delete;
    Buffer& operator=(Buffer const&) = delete;
};

void bind(Buffer& buffer, GLenum const target);
void unbind(Buffer&, GLenum const target);

} // namespace
