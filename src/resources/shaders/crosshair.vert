#version 410 core
layout (location = 0) in vec2 in_pos;

uniform mat4 model;
uniform mat4 ortho_projection;

void main() {
    gl_Position = ortho_projection * model * vec4(in_pos, 0.0, 1.0);
}
