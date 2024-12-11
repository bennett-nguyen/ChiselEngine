#version 410 core
layout (location = 0) in ivec3 in_pos;
layout (location = 1) in uint voxel_id;
layout (location = 2) in uint face_id;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Dirt,
// Grass,
// CobbleStone,
// Stone,
// Sand,

vec3 colors[6] = vec3[6](
    vec3(0.0, 0.0, 0.0),
    vec3(0.61,0.46,0.33),
    vec3(0.24,0.57,0.25),
    vec3(0.66,0.66,0.66),
    vec3(0.50,0.50,0.50),
    vec3(0.76,0.70,0.50)
);


out vec3 out_color;

vec3 hash31(float p) {
    vec3 p3 = fract(vec3(p * 21.2) * vec3(0.1031, 0.103, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05;
}

float shades[6] = float[6](
    1.0, 0.7,
    0.8, 0.6,
    0.84, 0.8
);

void main() {
    gl_Position = projection * view * model * vec4(in_pos, 1.0f);
    out_color = colors[voxel_id] * shades[face_id];
}