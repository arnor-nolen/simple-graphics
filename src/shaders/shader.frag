#version 410 core

sample in vec3 fragment_color;
in vec2 fragment_uv;

out vec4 program_color;

uniform sampler2D tex;

void main() {
  program_color = texture(tex, fragment_uv) * vec4(fragment_color, 1.0);
}