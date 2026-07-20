struct VertexData {
    uint packed_data;
};

layout(binding = 0, std430) restrict readonly buffer Vertices {
    VertexData in_vertices[];
};

layout(binding = 0, std140) uniform ViewProjection {
    mat4 view;
    mat4 projection;
};

vec3 hash31(float p) {
    vec3 p3 = fract(vec3(p * 21.2) * vec3(0.1031, 0.103, 0.0973));
    p3 += dot(p3, p3.yxz + 33.33);
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05;
}

vec3 position;
uint ao_id, face_id, voxel_id;

uint popBits(uint base, int size) {
    return base >> size;
}

void unpack(int vertex_id) {
    uint packed_data = in_vertices[vertex_id].packed_data;

    voxel_id = bitfieldExtract(packed_data, 0, VOXEL_ID_SIZE);
    packed_data = popBits(packed_data, VOXEL_ID_SIZE);

    face_id  = bitfieldExtract(packed_data, 0, FACE_ID_SIZE);
    packed_data = popBits(packed_data, FACE_ID_SIZE);

    ao_id    = bitfieldExtract(packed_data, 0, AO_ID_SIZE);
    packed_data = popBits(packed_data, AO_ID_SIZE);

    uint z = bitfieldExtract(packed_data, 0, Z_SIZE);
    packed_data = popBits(packed_data, Z_SIZE);

    uint y = bitfieldExtract(packed_data, 0, Y_SIZE);
    packed_data = popBits(packed_data, Y_SIZE);

    uint x = bitfieldExtract(packed_data, 0, X_SIZE);

    position = vec3(x, y, z);
}

uniform mat4 model;

out vec2  fs_uv_coords;
out float fs_shades;

flat out uint fs_face_id;
flat out uint fs_voxel_id;

void main() {
    unpack(gl_VertexID);

    fs_face_id  = face_id;
    fs_voxel_id = voxel_id;

    int uv_index = gl_VertexID % 4 + int(face_id) * 4;
    fs_uv_coords = uv_coords[uv_indices[uv_index]];
    fs_shades =  shades[face_id] * ao[ao_id];

    gl_Position = projection * view * model * vec4(position, 1.0f);
}
