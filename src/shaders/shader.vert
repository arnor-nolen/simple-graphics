#version 450 core

in vec3 position;
in vec3 vertex_color;
in vec2 vertex_uv;

out vec3 fragment_color;
out vec2 fragment_uv;

uniform mat4 mvp_matrix;

void main() {
  fragment_color = vertex_color;
  fragment_uv = vertex_uv;

  // gl_Position is a special variable
  gl_Position = mvp_matrix * vec4(position, 1.0);
}