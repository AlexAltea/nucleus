/**
 * (c) 2014 Nucleus project. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "opengl_vp.h"
#include "nucleus/format.h"
#include "nucleus/emulator.h"
#include "nucleus/gpu/rsx_vp.h"
#include "nucleus/gpu/opengl/opengl_renderer.h"

// OpenGL dependencies
#include <GL/glew.h>

// Input register
struct vp_input_register_t {
    const char* glReg;
    const char* rsxReg;
};

const vp_input_register_t input_regs[] = {
    // OpenGL custom inputs
    { "rsx_Position",      "v0"  },
    { "rsx_SkinWeights",   "v1"  },
    { "rsx_Normal",        "v2"  },
    { "rsx_DiffuseColor",  "v3"  },
    { "rsx_SpecularColor", "v4"  },
    { "rsx_Fog",           "v5"  },
    { "rsx_PointSize",     "v6"  },
    { "rsx_Texture0",      "v8"  },
    { "rsx_Texture1",      "v9"  },
    { "rsx_Texture2",      "v10" },
    { "rsx_Texture3",      "v11" },
    { "rsx_Texture4",      "v12" },
    { "rsx_Texture5",      "v13" },
    { "rsx_Texture6",      "v14" },
    { "rsx_Texture7",      "v15" },
};

// Output register
struct vp_output_register_t {
    const char* glReg;
    const char* rsxReg;
    bool predefined;
};

const vp_output_register_t output_regs[] = {
    // OpenGL predefined outputs
    { "gl_Position",            "o[0]",   true },
    { "gl_PointSize",           "o[6].x", true },
    { "gl_ClipDistance[0]",     "o[5].y", true },
    { "gl_ClipDistance[1]",     "o[5].z", true },
    { "gl_ClipDistance[2]",     "o[5].w", true },
    { "gl_ClipDistance[3]",     "o[6].y", true },
    { "gl_ClipDistance[4]",     "o[6].z", true },
    { "gl_ClipDistance[5]",     "o[6].w", true },

    // OpenGL custom outputs
    { "rsx_BackDiffuseColor",   "o[1]",   false },
    { "rsx_BackSpecularColor",  "o[2]",   false },
    { "rsx_FrontDiffuseColor",  "o[3]",   false },
    { "rsx_FrontSpecularColor", "o[4]",   false },
    { "rsx_Fog",                "o[5].x", false },
    { "rsx_Texture0",           "o[7]",   false },
    { "rsx_Texture1",           "o[8]",   false },
    { "rsx_Texture2",           "o[9]",   false },
    { "rsx_Texture3",           "o[10]",  false },
    { "rsx_Texture4",           "o[11]",  false },
    { "rsx_Texture5",           "o[12]",  false },
    { "rsx_Texture6",           "o[13]",  false },
    { "rsx_Texture7",           "o[14]",  false },
    { "rsx_Texture8",           "o[15]",  false },
    { "rsx_Texture9",           "o[6]",   false },
};

bool OpenGLVertexProgram::decompile(rsx_vp_t program)
{
    // Header
    m_shader =
        "#version 330\n"

        // Vertex input registers
        "attribute vec4 v0;\n"
        "attribute vec4 v1;\n"
        "attribute vec4 v2;\n"
        "attribute vec4 v3;\n"
        "attribute vec4 v4;\n"
        "attribute vec4 v5;\n"
        "attribute vec4 v6;\n"
        "attribute vec4 v7;\n"
        "attribute vec4 v8;\n"
        "attribute vec4 v9;\n"
        "attribute vec4 v10;\n"
        "attribute vec4 v11;\n"
        "attribute vec4 v12;\n"
        "attribute vec4 v13;\n"
        "attribute vec4 v14;\n"
        "attribute vec4 v15;\n"

        // Vertex output registers
        "vec4 o[16];\n"

        // Shader body
        "void main() {\n";

    // Write Output registers
    for (const auto& reg : output_regs) {
        m_shader += format("\t%s = %s;\n", reg.glReg, reg.rsxReg);
    }

    m_shader += "}\n";

    // TEMPORARY
    m_shader = R"(
#version 330
layout (location = 0) in vec4 v0;
layout (location = 3) in vec4 v3;
out vec4 diff_color;
void main() {
	gl_Position = v0;
	diff_color = v3;
}
    )";

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
