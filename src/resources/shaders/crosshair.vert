#version 460 core

struct VertexData {
    float position[2]; float tex_coords[2];
};

layout(binding = 1, std430) restrict readonly buffer Vertices {
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

uniform mat4 model;
uniform mat4 ortho_projection;

out vec2 TexCoords;

void main() {
    TexCoords = getTexCoords(gl_VertexID);
    gl_Position = ortho_projection * model * vec4(getPosition(gl_VertexID), 0.0, 1.0);
}
