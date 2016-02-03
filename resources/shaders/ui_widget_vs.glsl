#version 450

layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec4 iColor;
layout(location = 2) in vec2 iTexcoord;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec2 oTexcoord;

void main() {
    gl_Position = vec4(iPosition, 0.0, 1.0);
    oColor = iColor;
    oTexcoord = iTexcoord;
}
