#version 150

in vec4 i0; // Vertex
in vec4 i1; // Color
out vec4 o0;  // Color

void main() {
    gl_Position = i0;
    o0 = i1;
}
