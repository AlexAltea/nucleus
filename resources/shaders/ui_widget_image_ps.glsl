#version 450

// Inputs
in vec4 gl_FragCoord;
layout (location = 0) in vec2 iTexcoord;
layout (location = 1) in float iZindex;
layout (location = 2) in float iOpacity;

// Outputs
layout (location = 0) out vec4 result;

// Samplers
layout (binding = 0) uniform sampler2D s;

void main() {
	result = texture(s, iTexcoord.xy);
    //result *= opacity;
}
