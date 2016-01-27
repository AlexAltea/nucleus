#version 150

in vec4 gl_FragCoord;
in vec4 color;
out vec4 result;

void main() {
    result = color;
}
