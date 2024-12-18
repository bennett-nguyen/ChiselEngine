#version 410 core
layout (location = 0) in vec2 in_pos;
layout (location = 1) in vec2 in_tex_coords;

uniform mat4 model;
uniform mat4 ortho_projection;

out vec2 TexCoords;

void main() {
    TexCoords = in_tex_coords;
    gl_Position = ortho_projection * model * vec4(in_pos, 0.0, 1.0);
}
