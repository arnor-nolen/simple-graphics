#version 410 core

layout(location = 0) in vec3 position;
layout(location = 3) in vec3 vertex_color;
layout(location = 6) in vec2 vertex_uv;

out vec3 fragment_color;
out vec2 fragment_uv;

uniform mat4 mvp_matrix;

void main() {
  fragment_color = vertex_color;
  fragment_uv = vertex_uv;

  // gl_Position is a special variable
  gl_Position = mvp_matrix * vec4(position, 1.0);
}