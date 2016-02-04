#version 450

// Inputs
in vec4 gl_FragCoord;
layout (location = 0) in vec2 iTexcoord;
layout (location = 1) in vec4 iColor;
layout (location = 2) in float iType;
layout (location = 3) in float iZindex;
layout (location = 4) in float iOpacity;

// Outputs
layout (location = 0) out vec4 result;

// Samplers
layout (binding = 0) uniform sampler2D s;

#define TYPE_CONTAINER  0
#define TYPE_IMAGE      1
#define TYPE_TEXT       2

void main() {
    if (iType == TYPE_CONTAINER) {
        result = iColor;
    }
    else if (iType == TYPE_IMAGE) {
        result = texture(s, iTexcoord.xy);
    }
    else if (iType == TYPE_TEXT) {
        result = iColor;
        result.w = texture(s, iTexcoord.xy).r;
    }
    //result *= opacity;
}
