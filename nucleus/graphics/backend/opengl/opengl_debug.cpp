/**
 * (c) 2015 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_debug.h"
#include "nucleus/logger/logger.h"

namespace gfx {
namespace opengl {

void OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam) {
#ifdef GRAPHICS_OPENGL_GL
    const char* sourceName = "(Unknown)";
    switch (source) {
    case GL_DEBUG_SOURCE_API_ARB:
        sourceName = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:
        sourceName = "Window system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:
        sourceName = "Shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:
        sourceName = "Third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION_ARB:
        sourceName = "Application"; break;
    case GL_DEBUG_SOURCE_OTHER_ARB:
        sourceName = "Other"; break;
    }

    const char* typeName = "(Unknown)";
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeName = "Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeName = "Deprecated behavior"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeName = "Undefined behavior"; break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeName = "Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeName = "Performance"; break;
    case GL_DEBUG_TYPE_OTHER:
        typeName = "Other"; break;
    case GL_DEBUG_TYPE_MARKER:
        typeName = "Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeName = "Push group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeName = "Pop group"; break;
    }

    const char* severityName = "(Unknown)";
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH_ARB:
        severityName = "High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM_ARB:
        severityName = "Medium"; break;
    case GL_DEBUG_SEVERITY_LOW_ARB:
        severityName = "Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severityName = "Notification"; break;
    }

    logger.error(LOG_GRAPHICS, "OpenGL Debug Message (Source: %s, Type: %s, Severity: %s, ID: %d):\n%s",
        sourceName, typeName, severityName, id, message);
#endif
}

}  // namespace opengl
}  // namespace gfx
