#version 450

// Inputs
in vec4 gl_FragCoord;
layout (location = 0) in vec2 iTexcoord;
layout (location = 1) in vec4 iColor;
layout (location = 2) in float iZindex;
layout (location = 3) in float iOpacity;

// Outputs
layout (location = 0) out vec4 result;

// Samplers
layout (binding = 0) uniform sampler2D s;

void main() {
    float r = iColor.x;
    float g = iColor.y;
    float b = iColor.z;
    float a = texture(s, iTexcoord.xy).x;
    result = vec4(r, g, b, a);
    //result *= opacity;
}
