#version 460 core

struct VertexData {
    int position[3]; float tex_coords[2]; uint voxel_id; uint face_id;
};

layout(binding = 0, std430) restrict readonly buffer Vertices {
    VertexData in_vertices[];
};

vec3 getPosition(int vertex_id) {
    return vec3(
        in_vertices[vertex_id].position[0],
        in_vertices[vertex_id].position[1],
        in_vertices[vertex_id].position[2]
    );
}

vec2 getTexCoords(int vertex_id) {
    return vec2(
        in_vertices[vertex_id].tex_coords[0],
        in_vertices[vertex_id].tex_coords[1]
    );
}

uint getVoxelID(int vertex_id) {
    return in_vertices[vertex_id].voxel_id;
}

uint getFaceID(int vertex_id) {
    return in_vertices[vertex_id].face_id;
}

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 out_color;
out vec2 tex_coords;
flat out uint voxel_id;
flat out uint face_id;

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
    voxel_id = getVoxelID(gl_VertexID);
    face_id = getFaceID(gl_VertexID);
    tex_coords = getTexCoords(gl_VertexID);
    gl_Position = projection * view * model * vec4(getPosition(gl_VertexID), 1.0f);
    out_color = hash31(getVoxelID(gl_VertexID)) * shades[getFaceID(gl_VertexID)];
}