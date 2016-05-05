#version 450

layout (location = 0) in vec2 iPosition;
layout (location = 1) in vec2 iTexcoord;
layout (location = 2) in float iZindex;
layout (location = 3) in float iOpacity;
layout (location = 4) in float iOpacityMin;

layout (location = 0) out vec2 oTexcoord;
layout (location = 1) out float oZindex;
layout (location = 2) out float oOpacity;
layout (location = 3) out float oOpacityMin;

void main() {
    gl_Position = vec4(iPosition, 0.0, 1.0);
    oTexcoord = iTexcoord;
    oZindex = iZindex;
    oOpacity = iOpacity;
    oOpacityMin = iOpacityMin;
}
