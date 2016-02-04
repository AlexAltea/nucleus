#version 450

// Inputs
in vec4 gl_FragCoord;
layout (location = 0) in vec4 iColor;
layout (location = 1) in float iZindex;
layout (location = 2) in float iOpacity;

// Outputs
layout (location = 0) out vec4 result;

void main() {
    result = iColor;
    //result *= opacity;
}
