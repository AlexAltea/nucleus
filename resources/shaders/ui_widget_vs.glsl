#version 450

in vec2 i0; // Vertex
in vec4 i1; // Color
out vec4 o0;  // Color

void main() {
    gl_Position = vec4(i0, 0.0, 1.0);
    o0 = i1;
}
