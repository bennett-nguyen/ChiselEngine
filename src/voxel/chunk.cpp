#include "chunk.hpp"

constexpr std::array<FaceID, 6> ACCEPTABLE_FACE_VALUE = { Top, Bottom, North, South, East, West };

constexpr int X_SIZE = 5,
              Y_SIZE = 6,
              Z_SIZE = 5,
              FACE_ID_SIZE = 3,
              VOXEL_ID_SIZE = 8;

inline void appendBits(unsigned &base, const unsigned data, const int size) {
    base <<= size;
    base |= data;
}

[[nodiscard]] bool isFaceAcceptable(const FaceID face) {
    return std::any_of(ACCEPTABLE_FACE_VALUE.begin(), ACCEPTABLE_FACE_VALUE.end(),
        [&face](const FaceID valid_face){ return valid_face == face; });
}

[[nodiscard]] glm::uvec3 getVertexPositionOfFaceByIndex(const FaceID face, const int vertex_index, const glm::uvec3 &voxel_origin) {
    if (!isFaceAcceptable(face)) {
        throw std::invalid_argument("Face Enum Error: invalid face enum: " + std::to_string(face));
    }

    if (vertex_index < 0 || vertex_index > 3) {
        throw std::invalid_argument("Vertex Index Error: index out of range: " + std::to_string(vertex_index));
    }

    glm::uvec3 vertex_vector {};

    if (Top == face) {
        switch (vertex_index) {
            case 0:
                vertex_vector = {0,1,0};
                break;
            case 1:
                vertex_vector = {1,1,0};
                break;
            case 2:
                vertex_vector = {1,1,1};
                break;
            case 3:
                vertex_vector = {0,1,1};
                break;
        }
    } else if (Bottom == face) {
        switch (vertex_index) {
            case 0:
                vertex_vector = {0,0,0};
                break;
            case 1:
                vertex_vector = {1,0,0};
                break;
            case 2:
                vertex_vector = {1,0,1};
                break;
            case 3:
                vertex_vector = {0,0,1};
                break;
        }
    } else if (North == face) {
        switch (vertex_index) {
            case 0:
                vertex_vector = {1,0,0};
                break;
            case 1:
                vertex_vector = {1,1,0};
                break;
            case 2:
                vertex_vector = {1,1,1};
                break;
            case 3:
                vertex_vector = {1,0,1};
                break;
        }
    } else if (South == face) {
        switch (vertex_index) {
            case 0:
                vertex_vector = {0,0,0};
                break;
            case 1:
                vertex_vector = {0,1,0};
                break;
            case 2:
                vertex_vector = {0,1,1};
                break;
            case 3:
                vertex_vector = {0,0,1};
                break;
        }
    } else if (East == face) {
        switch (vertex_index) {
            case 0:
                vertex_vector = {0,0,1};
                break;
            case 1:
                vertex_vector = {0,1,1};
                break;
            case 2:
                vertex_vector = {1,1,1};
                break;
            case 3:
                vertex_vector = {1,0,1};
                break;
        }
    } else {
        switch (vertex_index) {
            case 0:
                vertex_vector = {0,0,0};
                break;
            case 1:
                vertex_vector = {0,1,0};
                break;
            case 2:
                vertex_vector = {1,1,0};
                break;
            case 3:
                vertex_vector = {1,0,0};
                break;
        }
    }

    return voxel_origin + vertex_vector;
}

[[nodiscard]] GLuint packData(const glm::uvec3 &vertex_position, const unsigned face_id, const VoxelID voxel_id) {
    GLuint packed_data = 0;
    appendBits(packed_data, vertex_position.x, X_SIZE);
    appendBits(packed_data, vertex_position.y, Y_SIZE);
    appendBits(packed_data, vertex_position.z, Z_SIZE);
    appendBits(packed_data, face_id, FACE_ID_SIZE);
    appendBits(packed_data, voxel_id, VOXEL_ID_SIZE);

    return packed_data;
}

Vertex::Vertex(const int vertex_index, const LocalPosition &voxel_origin, const FaceID face_id, const VoxelID voxel_id) {
    packed_data = packData(
        getVertexPositionOfFaceByIndex(face_id, vertex_index, voxel_origin),
        face_id,
        voxel_id
    );
}

bool Chunk::isChunkVisible(const std::vector<glm::vec4>& frustum_planes) const {
    const glm::vec3& vmin = bounding_box.vmin;
    const glm::vec3& vmax = bounding_box.vmax;

    for (auto const &g : frustum_planes) {
        if ((glm::dot(g, glm::vec4(vmin.x, vmin.y, vmin.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmax.x, vmin.y, vmin.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmin.x, vmax.y, vmin.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmax.x, vmax.y, vmin.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmin.x, vmin.y, vmax.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmax.x, vmin.y, vmax.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmin.x, vmax.y, vmax.z, 1.0f)) < 0.0) &&
            (glm::dot(g, glm::vec4(vmax.x, vmax.y, vmax.z, 1.0f)) < 0.0))
        {
            return false;
        }
    }

    return true;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryEast(const LocalPosition local) {
    return Constant::CHUNK_SIZE-1 == local.z;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryWest(const LocalPosition local) {
    return 0 == local.z;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryNorth(const LocalPosition local) {
    return Constant::CHUNK_SIZE-1 == local.x;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundarySouth(const LocalPosition local) {
    return 0 == local.x;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryTop(const LocalPosition local) {
    return Constant::CHUNK_HEIGHT-1 == local.y;
}

[[nodiscard]] inline bool isVoxelAtChunkBoundaryBottom(const LocalPosition local) {
    return 0 == local.y;
}

bool Chunk::isVoidAt(const LocalPosition local) const {
    return 0 == getVoxelID(local);
}

bool Chunk::isVoidEast(const LocalPosition voxel_origin, const Chunk *ptr_east_neighbor) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundaryEast(voxel_origin)) {
        if (nullptr == ptr_east_neighbor) return true;

        neighbor_voxel.z = 0;
        return ptr_east_neighbor->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel.z++;
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidWest(const LocalPosition voxel_origin, const Chunk *ptr_west_neighbor) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundaryWest(voxel_origin)) {
        if (nullptr == ptr_west_neighbor) return true;

        neighbor_voxel.z = Constant::CHUNK_SIZE-1;
        return ptr_west_neighbor->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel.z--;
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidNorth(const LocalPosition voxel_origin, const Chunk *ptr_north_neighbor) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundaryNorth(voxel_origin)) {
        if (nullptr == ptr_north_neighbor) return true;

        neighbor_voxel.x = 0;
        return ptr_north_neighbor->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel.x++;
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidSouth(const LocalPosition voxel_origin, const Chunk *ptr_south_neighbor) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundarySouth(voxel_origin)) {
        if (nullptr == ptr_south_neighbor) return true;

        neighbor_voxel.x = Constant::CHUNK_SIZE-1;
        return ptr_south_neighbor->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel.x--;
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTop(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundaryTop(voxel_origin)) {
        // check for neighboring chunk
        return true;
    }

    neighbor_voxel.y++;
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottom(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;

    if (isVoxelAtChunkBoundaryBottom(voxel_origin)) {
        // check for neighboring chunk
        return false;
    }

    neighbor_voxel.y--;
    return isVoidAt(neighbor_voxel);
}

VoxelID Chunk::getVoxelID(const LocalPosition local) const {
    return ptr_voxels[Conversion::toIndex(local)];
}

void Chunk::setVoxelIDAtPosition(const VoxelID voxel_id, const LocalPosition local) const {
    ptr_voxels[Conversion::toIndex(local)] = voxel_id;
}

void Chunk::updateBoundingBoxWithCubeFace(const FaceID face, const LocalPosition &voxel_origin) {
    const glm::vec3 v0 = getVertexPositionOfFaceByIndex(face, 0, voxel_origin);
    const glm::vec3 v1 = getVertexPositionOfFaceByIndex(face, 1, voxel_origin);
    const glm::vec3 v2 = getVertexPositionOfFaceByIndex(face, 2, voxel_origin);
    const glm::vec3 v3 = getVertexPositionOfFaceByIndex(face, 3, voxel_origin);

    const glm::vec3 min_v = glm::min(v0, v1, v2, v3);
    const glm::vec3 max_v = glm::max(v0, v1, v2, v3);

    bounding_box.vmin = glm::min(bounding_box.vmin, min_v);
    bounding_box.vmax = glm::max(bounding_box.vmax, max_v);
}

void Chunk::translateBoundingBox() {
    bounding_box.vmax += glm::vec3(1.0f);
    const glm::mat4 chunk_model = getChunkModel();
    bounding_box.vmin = glm::vec3(chunk_model * glm::vec4(bounding_box.vmin.x, bounding_box.vmin.y, bounding_box.vmin.z, 1.0f));
    bounding_box.vmax = glm::vec3(chunk_model * glm::vec4(bounding_box.vmax.x, bounding_box.vmax.y, bounding_box.vmax.z, 1.0f));
}

unsigned heightMap(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high) {
    float max_amp = 0;
    float amp = 1;
    float freq = scale;
    float noise = 0;

    for(unsigned i = 0; i < num_iterations; ++i) {
        noise += glm::simplex(glm::vec2(x, z) * freq) * amp;
        max_amp += amp;
        amp *= persistence;
        freq *= 2;
    }

    noise /= max_amp;
    noise = noise * (static_cast<float>(high) - static_cast<float>(low)) / 2 + (static_cast<float>(high) + static_cast<float>(low)) / 2;
    return static_cast<unsigned>(noise);
}

void Chunk::buildVoxels() {
    ptr_voxels = new VoxelID[Constant::CHUNK_VOLUME] {};

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            unsigned y_level = heightMap(6, float((int) x + position.x * (int) Constant::CHUNK_SIZE),
                                         float((int) z + position.z * (int) Constant::CHUNK_SIZE), 0.6f, 0.007f,
                                         0, Constant::CHUNK_HEIGHT);

            for (unsigned y = 0; y <= y_level; y++) {
                const LocalPosition local { x, y, z };
                VoxelID voxel_id;

                if (y <= 15) {
                    voxel_id = 6;
                } else if (y < y_level || y <= 20) {
                    voxel_id = 1;
                } else if (y == y_level) {
                    voxel_id = 2;
                } else {
                    voxel_id = 0;
                }

                setVoxelIDAtPosition(voxel_id, local);
            }
        }
    }
}

void Chunk::buildMesh(const Chunk *ptr_north_neighbor,
                      const Chunk *ptr_south_neighbor,
                      const Chunk *ptr_east_neighbor,
                      const Chunk *ptr_west_neighbor) {
    GLuint index = 0;

    // Setting them to an extreme point so we can update them later
    bounding_box.vmin = glm::vec3(10000.0f);
    bounding_box.vmax = glm::vec3(-10000.0f);

    Vertex vtx0 {}, vtx1 {}, vtx2 {}, vtx3 {};

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            for (unsigned y = 0; y < Constant::CHUNK_HEIGHT; y++) {
                LocalPosition voxel_origin = { x, y, z };
                if (isVoidAt(voxel_origin)) continue;
                const VoxelID voxel_id = getVoxelID(voxel_origin);

                if (isVoidTop(voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Top, voxel_id };
                    vtx1 = { 1, voxel_origin, Top, voxel_id };
                    vtx2 = { 2, voxel_origin, Top, voxel_id };
                    vtx3 = { 3, voxel_origin, Top, voxel_id };

                    updateBoundingBoxWithCubeFace(Top, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+3, index+2, index, index+2, index+1 });
                    index += 4;
                }

                if (isVoidBottom(voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Bottom, voxel_id };
                    vtx1 = { 1, voxel_origin, Bottom, voxel_id };
                    vtx2 = { 2, voxel_origin, Bottom, voxel_id };
                    vtx3 = { 3, voxel_origin, Bottom, voxel_id };

                    updateBoundingBoxWithCubeFace(Bottom, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+3, index, index+1, index+2 });
                    index += 4;
                }

                if (isVoidNorth(voxel_origin, ptr_north_neighbor)) {
                    vtx0 = { 0, voxel_origin, North, voxel_id };
                    vtx1 = { 1, voxel_origin, North, voxel_id };
                    vtx2 = { 2, voxel_origin, North, voxel_id };
                    vtx3 = { 3, voxel_origin, North, voxel_id };

                    updateBoundingBoxWithCubeFace(North, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }

                if (isVoidSouth(voxel_origin, ptr_south_neighbor)) {
                    vtx0 = { 0, voxel_origin, South, voxel_id };
                    vtx1 = { 1, voxel_origin, South, voxel_id };
                    vtx2 = { 2, voxel_origin, South, voxel_id };
                    vtx3 = { 3, voxel_origin, South, voxel_id };

                    updateBoundingBoxWithCubeFace(South, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                if (isVoidEast(voxel_origin, ptr_east_neighbor)) {
                    vtx0 = { 0, voxel_origin, East, voxel_id };
                    vtx1 = { 1, voxel_origin, East, voxel_id };
                    vtx2 = { 2, voxel_origin, East, voxel_id };
                    vtx3 = { 3, voxel_origin, East, voxel_id };

                    updateBoundingBoxWithCubeFace(East, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                if (isVoidWest(voxel_origin, ptr_west_neighbor)) {
                    vtx0 = { 0, voxel_origin, West, voxel_id };
                    vtx1 = { 1, voxel_origin, West, voxel_id };
                    vtx2 = { 2, voxel_origin, West, voxel_id };
                    vtx3 = { 3, voxel_origin, West, voxel_id };

                    updateBoundingBoxWithCubeFace(West, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }
            }
        }
    }

    if (mesh.vertices.empty()) return;

    translateBoundingBox();

    glCreateBuffers(1, &mesh.ssbo_vertices);
    glCreateBuffers(1, &mesh.ebo);
    glCreateVertexArrays(1, &mesh.vao);

    const auto VERTEX_BUFFER_SIZE = static_cast<GLsizeiptr>(mesh.vertices.size() * sizeof(Vertex));
    const auto VERTEX_DATA = reinterpret_cast<void *>(mesh.vertices.data());
    const auto ELEMENT_BUFFER_SIZE = static_cast<GLsizeiptr>(mesh.indices.size() * sizeof(GLuint));
    const auto ELEMENT_DATA = reinterpret_cast<void *>(mesh.indices.data());
    glNamedBufferStorage(mesh.ssbo_vertices, VERTEX_BUFFER_SIZE, VERTEX_DATA, 0);
    glNamedBufferStorage(mesh.ebo, ELEMENT_BUFFER_SIZE, ELEMENT_DATA, 0);
    glVertexArrayElementBuffer(mesh.vao, mesh.ebo);
}

void Chunk::destroyMesh() {
    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.ssbo_vertices);
    glDeleteBuffers(1, &mesh.ebo);
    mesh.vertices.clear();
    mesh.indices.clear();
}

void Chunk::destroyChunk() {
    destroyMesh();
    delete[] ptr_voxels;
}

void Chunk::render() const {
    glBindVertexArray(mesh.vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mesh.ssbo_vertices);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
}

glm::mat4 Chunk::getChunkModel() const {
    glm::mat4 model(1.0f);
    const ChunkPosition chunk_position_to_world = Conversion::chunkToWorld(position);
    model = glm::translate(model, glm::vec3(chunk_position_to_world));
    return model;
}
