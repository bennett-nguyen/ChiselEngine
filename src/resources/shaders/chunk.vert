#version 410 core
layout (location = 0) in ivec3 in_pos;
layout (location = 1) in uint voxel_id;
layout (location = 2) in uint face_id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_color;

float shades[6] = float[6](
    1.0, 0.7,
    0.8, 0.6,
    0.84, 0.5
);

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0f);
    out_color = vec3(1.0f) * shades[face_id];
}