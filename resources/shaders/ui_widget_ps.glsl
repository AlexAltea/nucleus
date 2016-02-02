#version 450

in vec4 gl_FragCoord;
in vec4 color;
out vec4 result;

layout (binding = 0) uniform sampler2D s;

void main() {
    result = texture(s, gl_FragCoord.xy);
}
