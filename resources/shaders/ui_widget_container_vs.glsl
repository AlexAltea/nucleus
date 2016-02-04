#version 450

layout (location = 0) in vec2 iPosition;
layout (location = 1) in vec4 iColor;
layout (location = 2) in float iZindex;
layout (location = 3) in float iOpacity;

layout (location = 0) out vec4 oColor;
layout (location = 1) out float oZindex;
layout (location = 2) out float oOpacity;

void main() {
    gl_Position = vec4(iPosition, 0.0, 1.0);
    oColor = iColor;
    oZindex = iZindex;
    oOpacity = iOpacity;
}
