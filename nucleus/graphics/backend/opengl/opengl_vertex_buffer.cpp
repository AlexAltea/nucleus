/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#pragma once

#include "nucleus/graphics/backend/opengl/opengl_vertex_buffer.h"

namespace gfx {

OpenGLVertexBuffer::OpenGLVertexBuffer() {
    glGenBuffers(1, &id);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &id);
}

void* OpenGLVertexBuffer::map() {
    void* address;

#if defined(GRAPHICS_OPENGL_GL45)
    address = glMapNamedBuffer(id, GL_MAP_WRITE_BIT);
#else
    glBindBuffer(GL_VERTEX_ARRAY);
    address = glMapBuffer(id, GL_MAP_WRITE_BIT);
#endif

    return address;
}

bool OpenGLVertexBuffer::unmap() {
    GLboolean result;

#if defined(GRAPHICS_OPENGL_GL45)
    result = glUnmapNamedBuffer(id);
#else
    glBindBuffer(GL_VERTEX_ARRAY);
    result = glUnmapBuffer(id);
#endif

    return result;
}

}  // namespace gfx
