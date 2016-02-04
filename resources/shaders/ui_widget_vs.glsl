#version 450

layout (location = 0) in vec2 iPosition;
layout (location = 1) in vec2 iTexcoord;
layout (location = 2) in vec4 iColor;
layout (location = 3) in float iType;
layout (location = 4) in float iZindex;
layout (location = 5) in float iOpacity;

layout (location = 0) out vec2 oTexcoord;
layout (location = 1) out vec4 oColor;
layout (location = 2) out float oType;
layout (location = 3) out float oZindex;
layout (location = 4) out float oOpacity;

void main() {
    gl_Position = vec4(iPosition, 0.0, 1.0);
    oTexcoord = iTexcoord;
    oColor = iColor;
    oType = iType;
    oZindex = iZindex;
    oOpacity = iOpacity;
}
