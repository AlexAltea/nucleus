/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_vp.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_vp.h"
#include "nucleus/gpu/opengl/opengl_renderer.h"

#include <sstream>

// OpenGL dependencies
#include <GL/glew.h>

bool OpenGLVertexProgram::decompile(rsx_vp_t program)
{
    std::stringstream source;

    source
        << "#version 330"
        << "void main() {"
        << "}";

    return true;
}

bool OpenGLVertexProgram::compile()
{
    // Check if the shader was already compiled
    if (m_id != 0) {
        glDeleteShader(m_id);
    }

    m_id = glCreateShader(GL_VERTEX_SHADER);

    // Compile shader
    const GLchar* sourceString = m_shader.data();
	const GLint sourceLength = m_shader.length();
    glShaderSource(m_id, 1, &sourceString, &sourceLength);
    glCompileShader(m_id);

    // Check if shader compiled succesfully
    GLint status;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        nucleus.log.error(LOG_GPU, "OpenGLVertexProgram::compile: Can't compile shader");
        return false;
    }

    return true;
}
