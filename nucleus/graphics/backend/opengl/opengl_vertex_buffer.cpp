/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "nucleus/graphics/backend/opengl/opengl_vertex_buffer.h"
#include "nucleus/logger/logger.h"

#define checkBackendError(name) { \
    GLenum error = glGetError(); \
    if (error != GL_NO_ERROR) { \
        logger.error(LOG_GPU, "Something went wrong in %s. Error code: %x", name, error); \
    } \
}

namespace gfx {
namespace opengl {

OpenGLVertexBuffer::OpenGLVertexBuffer(GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkBackendError("OpenGLVertexBuffer::OpenGLVertexBuffer");
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    glDeleteBuffers(1, &id);
    checkBackendError("OpenGLVertexBuffer::~OpenGLVertexBuffer");
}

void* OpenGLVertexBuffer::map() {
    void* address;

#if defined(GRAPHICS_OPENGL_GL45)
    address = glMapNamedBuffer(id, GL_WRITE_ONLY);
#else
    glBindBuffer(GL_ARRAY_BUFFER, id);
    address = glMapBufferRange(GL_ARRAY_BUFFER, 0/*TODO*/, 0/*TODO*/, GL_WRITE_ONLY);
#endif

    checkBackendError("OpenGLVertexBuffer::map");
    return address;
}

bool OpenGLVertexBuffer::unmap() {
    GLboolean result;

#if defined(GRAPHICS_OPENGL_GL45)
    result = glUnmapNamedBuffer(id);
#else
    glBindBuffer(GL_ARRAY_BUFFER, id);
    result = glUnmapBuffer(GL_ARRAY_BUFFER);
#endif

    checkBackendError("OpenGLVertexBuffer::unmap");
    return result;
}

}  // namespace opengl
}  // namespace gfx
