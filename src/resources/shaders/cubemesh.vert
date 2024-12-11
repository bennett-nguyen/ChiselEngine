#version 410 core
layout (location = 0) in vec3 in_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int block_interaction_mode;
out vec3 out_color;

vec3 colors[2] = vec3[2](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    out_color = colors[block_interaction_mode];
    gl_Position = projection * view * model * vec4(in_pos, 1.0);
}