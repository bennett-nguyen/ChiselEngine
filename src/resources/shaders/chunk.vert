#version 410 core
layout (location = 0) in ivec3 in_pos;
layout (location = 1) in uint voxel_id;
layout (location = 2) in uint face_id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_color;

vec3 hash31(float p) {
    vec3 p3 = fract(vec3(p * 21.2) * vec3(0.1031, 0.103, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05;
}

float shades[6] = float[6](
    1.0, 0.7,
    0.8, 0.6,
    0.84, 0.5
);

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0f);
    out_color = hash31(voxel_id) * shades[face_id];
}