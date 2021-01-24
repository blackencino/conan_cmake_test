#include "opengl_wrappers.h"

namespace graphics_utils {

//-----------------------------------------------------------------------------
Vertex_array::Vertex_array() {
    glGenVertexArrays(1, &id);
}

Vertex_array::~Vertex_array() {
    if (id != 0) {
        glDeleteVertexArrays(1, &id);
        id = 0;
    }
}

Vertex_array::Vertex_array(Vertex_array&& other)
  : id(other.id) {
    other.id = 0;
}

Vertex_array& Vertex_array::operator=(Vertex_array&& other) {
    if (id != 0) {
        glDeleteVertexArrays(1, &id);
        id = 0;
    }

    id = other.id;
    other.id = 0;

    return *this;
}

void bind(Vertex_array const& vao) {
    glBindVertexArray(vao.id);
}

void unbind(Vertex_array const& vao) {
    glBindVertexArray(0);
}


//-----------------------------------------------------------------------------
Buffer::Buffer() {
    glGenBuffers(1, &id);
}

Buffer::~Buffer() {
    if (id != 0) {
        glDeleteBuffers(1, &id);
        id = 0;
    }
}

Buffer::Buffer(Buffer&& other)
  : id(other.id) {
    other.id = 0;
}

Buffer& Buffer::operator=(Buffer&& other) {
    if (id != 0) {
        glDeleteBuffers(1, &id);
        id = 0;
    }

    id = other.id;
    other.id = 0;

    return *this;
}

void bind(Buffer& buffer, GLenum const target) {
    glBindBuffer(target, buffer.id);
}

void unbind(Buffer&, GLenum const target) {
    glBindBuffer(target, 0);
}

}  // namespace graphics_utils
