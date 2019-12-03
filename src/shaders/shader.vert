#version 150 core

uniform mat4 mvp_matrix;

in vec2 position;
in vec3 color;

out vec3 Color;

void main() {
  Color = color;
  gl_Position = mvp_matrix * vec4(position, 0.0, 1.0);
}