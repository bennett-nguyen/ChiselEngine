#version 460 core

struct VertexData {
    float position[2]; float tex_coords[2];
};

layout(binding = 1, std430) restrict readonly buffer QuadVertices {
    VertexData in_vertices[];
};

vec2 getPosition(int vertex_id) {
    return vec2(
        in_vertices[vertex_id].position[0],
        in_vertices[vertex_id].position[1]
    );
}

vec2 getTexCoords(int vertex_id) {
    return vec2(
        in_vertices[vertex_id].tex_coords[0],
        in_vertices[vertex_id].tex_coords[1]
    );
}

out vec2 TexCoords;

void main() {
    gl_Position = vec4(getPosition(gl_VertexID), 0.0, 1.0);
    TexCoords = getTexCoords(gl_VertexID);
}