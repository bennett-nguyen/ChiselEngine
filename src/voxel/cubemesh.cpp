#include "cubemesh.hpp"

void addVertices(GLuint index, std::array<CubeMeshVertex, 24> &v, CubeMeshVertex v0, CubeMeshVertex v1, CubeMeshVertex v2, CubeMeshVertex v3) {
    v[index] = v0;
    v[index+1] = v1;
    v[index+2] = v2;
    v[index+3] = v3;
}

void addIndices(unsigned &array_index, GLuint &index, std::array<GLuint, 36> &v, FaceID face) {
    if (Top == face) {
        v[array_index]   = index;
        v[array_index+1] = index+3;
        v[array_index+2] = index+2;
        v[array_index+3] = index;
        v[array_index+4] = index+2;
        v[array_index+5] = index+1;
    } else if (Bottom == face) {
        v[array_index]   = index;
        v[array_index+1] = index+2;
        v[array_index+2] = index+3;
        v[array_index+3] = index;
        v[array_index+4] = index+1;
        v[array_index+5] = index+2;
    } else if (North == face || West == face) {
        v[array_index]   = index;
        v[array_index+1] = index+1;
        v[array_index+2] = index+2;
        v[array_index+3] = index;
        v[array_index+4] = index+2;
        v[array_index+5] = index+3;
    } else if (South == face || East == face) {
        v[array_index]   = index;
        v[array_index+1] = index+2;
        v[array_index+2] = index+1;
        v[array_index+3] = index;
        v[array_index+4] = index+3;
        v[array_index+5] = index+2;
    }

    array_index += 6;
    index += 4;
}

CubeMesh::CubeMesh()
    : m_mesh_model(glm::mat4(1.0f)),
        m_shader_program(ShaderProgram("resources/shaders/cubemesh.vert",
            "resources/shaders/cubemesh.frag"))
    {

    GLuint index = 0;
    unsigned indices_array_index = 0;
    CubeMeshVertex v0, v1, v2, v3;
    const float X_MIN = -(0.5f+Constant::CUBE_MESH_SCALE), Y_MIN = -(0.5+Constant::CUBE_MESH_SCALE), Z_MIN = -(0.5f+Constant::CUBE_MESH_SCALE);
    const float X_MAX = X_MIN+1.0f+Constant::CUBE_MESH_SCALE, Y_MAX = Y_MIN+1.0f+Constant::CUBE_MESH_SCALE, Z_MAX = Z_MIN+1.0f+Constant::CUBE_MESH_SCALE;

    // Top
    v0.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MIN);
    v1.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MIN);
    v2.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MAX);
    v3.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MAX);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, Top);

    // Bottom
    v0.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MIN);
    v1.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MIN);
    v2.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MAX);
    v3.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MAX);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, Bottom);

    // North
    v0.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MIN);
    v1.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MIN);
    v2.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MAX);
    v3.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MAX);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, North);

    // South
    v0.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MIN);
    v1.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MIN);
    v2.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MAX);
    v3.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MAX);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, South);

    // East
    v0.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MAX);
    v1.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MAX);
    v2.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MAX);
    v3.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MAX);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, East);

    // West
    v0.m_pos = glm::vec3(X_MIN, Y_MIN, Z_MIN);
    v1.m_pos = glm::vec3(X_MIN, Y_MAX, Z_MIN);
    v2.m_pos = glm::vec3(X_MAX, Y_MAX, Z_MIN);
    v3.m_pos = glm::vec3(X_MAX, Y_MIN, Z_MIN);
    addVertices(index, m_vertices_data, v0, v1, v2, v3);
    addIndices(indices_array_index, index, m_indices_data, West);

    m_mesh_vao.genBuffer();
    m_mesh_vbo.genBuffer();
    m_mesh_ebo.genBuffer();

    m_mesh_vao.bind();
    m_mesh_ebo.bind();
    m_mesh_vbo.bind();

    m_mesh_vbo.bufferData((GLsizeiptr)(m_vertices_data.size() * sizeof(CubeMeshVertex)), m_vertices_data.data(), GL_STATIC_DRAW);
    m_mesh_vbo.attrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeMeshVertex), (void*)getOffsetOfCubeMeshPos());
    m_mesh_vbo.enable_attrib_array(0);

    m_mesh_ebo.bufferData((GLsizeiptr)(m_indices_data.size() * sizeof(GLuint)), m_indices_data.data(), GL_STATIC_DRAW);
}

void CubeMesh::render(glm::mat4 view, glm::mat4 projection, glm::vec3 position) {
    m_shader_program.activate();

    glm::mat4 model;
    model = glm::translate(m_mesh_model, position + 0.5f + Constant::CUBE_MESH_SCALE * 0.5f);

    m_shader_program.uniformMat4f("model", 1, GL_FALSE, model);
    m_shader_program.uniformMat4f("view", 1, GL_FALSE, view);
    m_shader_program.uniformMat4f("projection", 1, GL_FALSE, projection);

    m_mesh_vao.bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}