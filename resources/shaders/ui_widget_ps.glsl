#version 450

in vec4 gl_FragCoord;
layout (location = 0) in vec4 iColor;
layout (location = 1) in vec2 iTexcoord;

out vec4 result;

layout (binding = 0) uniform sampler2D s;

void main() {
    result = texture(s, iTexcoord.xy);
}
