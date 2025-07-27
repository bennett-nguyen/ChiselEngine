#include "chunk.hpp"

#include "chunk_pool.hpp"

constexpr unsigned X_SIZE = 5,
                   Y_SIZE = 6,
                   Z_SIZE = 5,
                   FACE_ID_SIZE = 3,
                   VOXEL_ID_SIZE = 8;

const std::unordered_map<Direction, std::array<LocalPosition, 4>> FACE_VERTEX_VECTORS {
    {
        Direction::Top, {{
                LocalPosition(0, 1, 0),
                LocalPosition(1, 1, 0),
                LocalPosition(1, 1, 1),
                LocalPosition(0, 1, 1)
            }}
    },

    {
        Direction::Bottom, {{
            LocalPosition(0, 0, 0),
            LocalPosition(1, 0, 0),
            LocalPosition(1, 0, 1),
            LocalPosition(0, 0, 1)
        }}
    },

    {
        Direction::North, {{
            LocalPosition(1, 0, 0),
            LocalPosition(1, 1, 0),
            LocalPosition(1, 1, 1),
            LocalPosition(1, 0, 1)
        }}
    },

    {
        Direction::South, {{
            LocalPosition(0, 0, 0),
            LocalPosition(0, 1, 0),
            LocalPosition(0, 1, 1),
            LocalPosition(0, 0, 1)
        }}
    },

    {
        Direction::East, {{
            LocalPosition(0, 0, 1),
            LocalPosition(0, 1, 1),
            LocalPosition(1, 1, 1),
            LocalPosition(1, 0, 1)
        }}
    },

    {
        Direction::West, {{
            LocalPosition(0, 0, 0),
            LocalPosition(0, 1, 0),
            LocalPosition(1, 1, 0),
            LocalPosition(1, 0, 0)
        }}
    }
};

enum class Plane : unsigned {
    XY,
    XZ,
    YZ,
};

inline void appendBits(unsigned &base, const unsigned data, const unsigned size) {
    base <<= size;
    base |= data;
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

Vertex::Vertex(const int vertex_index, const LocalPosition &voxel_origin, const Direction face_id, const VoxelID voxel_id) {
    const LocalPosition VERTEX_POSITION = voxel_origin + FACE_VERTEX_VECTORS.at(face_id).at(vertex_index);

    packed_data = packData(
        VERTEX_POSITION,
        static_cast<unsigned>(face_id),
        voxel_id
    );
}

void Chunk::fetchNeighbors(const ChunkNeighbors &neighbors) {
    this->neighbors = neighbors;
}

bool Chunk::isChunkVisible(const std::vector<glm::vec4>& frustum_planes) const {
    if (isEmpty() or not isBuilt()) return false;

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

bool Chunk::isVoidInNextTo(const Direction direction, const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;

    switch (direction) {
        case Direction::East:
            if (not isVoxelAtChunkBoundaryEast(voxel_origin)) break;
            if (nullptr == neighbors.east) return false;
            if (neighbors.east->isEmpty()) return true;
            neighbor_voxel.z = 0;
            return neighbors.east->isVoidAt(neighbor_voxel);

        case Direction::West:
            if (not isVoxelAtChunkBoundaryWest(voxel_origin)) break;
            if (nullptr == neighbors.west) return false;
            if (neighbors.west->isEmpty()) return true;
            neighbor_voxel.z = Constant::CHUNK_SIZE-1;
            return neighbors.west->isVoidAt(neighbor_voxel);

        case Direction::North:
            if (not isVoxelAtChunkBoundaryNorth(voxel_origin)) break;
            if (nullptr == neighbors.north) return false;
            if (neighbors.north->isEmpty()) return true;
            neighbor_voxel.x = 0;
            return neighbors.north->isVoidAt(neighbor_voxel);

        case Direction::South:
            if (not isVoxelAtChunkBoundarySouth(voxel_origin)) break;
            if (nullptr == neighbors.south) return false;
            if (neighbors.south->isEmpty()) return true;
            neighbor_voxel.x = Constant::CHUNK_SIZE-1;
            return neighbors.south->isVoidAt(neighbor_voxel);

        case Direction::Top:
            if (not isVoxelAtChunkBoundaryTop(voxel_origin)) break;
            return true;

        case Direction::Bottom:
            if (not isVoxelAtChunkBoundaryBottom(voxel_origin)) break;
            return true;

        default:
            throw std::invalid_argument("Chunk Error: Invalid direction\n");
    }

    neighbor_voxel += DIRECTION_VECTORS.at(direction);
    return isVoidAt(neighbor_voxel);
}

VoxelID Chunk::getVoxelID(const LocalPosition local) const {
    return voxel_ids.at(Conversion::toIndex(local));
}

void Chunk::setVoxelIDAtPosition(const VoxelID voxel_id, const LocalPosition local) {
    voxel_ids.at(Conversion::toIndex(local)) = voxel_id;
}

void Chunk::setPosition(const ChunkPosition position) {
    this->position = position;
}

void Chunk::updateBoundingBoxWithCubeFace(const Direction face, const LocalPosition &voxel_origin) {
    const glm::vec3 v0 = voxel_origin + FACE_VERTEX_VECTORS.at(face).at(0);
    const glm::vec3 v1 = voxel_origin + FACE_VERTEX_VECTORS.at(face).at(1);
    const glm::vec3 v2 = voxel_origin + FACE_VERTEX_VECTORS.at(face).at(2);
    const glm::vec3 v3 = voxel_origin + FACE_VERTEX_VECTORS.at(face).at(3);

    const glm::vec3 min_v = glm::min(v0, v1, v2, v3);
    const glm::vec3 max_v = glm::max(v0, v1, v2, v3);

    bounding_box.vmin = glm::min(bounding_box.vmin, min_v);
    bounding_box.vmax = glm::max(bounding_box.vmax, max_v);
}

void Chunk::translateBoundingBox() {
    bounding_box.vmax += glm::vec3(1.0f);
    const glm::mat4 chunk_model = Conversion::toChunkModel(position);
    bounding_box.vmin = glm::vec3(chunk_model * glm::vec4(bounding_box.vmin.x, bounding_box.vmin.y, bounding_box.vmin.z, 1.0f));
    bounding_box.vmax = glm::vec3(chunk_model * glm::vec4(bounding_box.vmax.x, bounding_box.vmax.y, bounding_box.vmax.z, 1.0f));
}

unsigned heightMap(unsigned num_iterations, float x, float z, float persistence, float scale, unsigned low, unsigned high) {
    float max_amp = 0;
    float amp = 1;
    float freq = scale;
    float noise = 0;

    for (unsigned i = 0; i < num_iterations; ++i) {
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
                setEmpty(false);
            }
        }
    }
}

void Chunk::buildMesh() {
    if (isEmpty()) return;

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

                if (isVoidInNextTo(Direction::Top, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::Top, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::Top, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::Top, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::Top, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::Top, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+3, index+2, index, index+2, index+1 });
                    index += 4;
                }

                if (isVoidInNextTo(Direction::Bottom, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::Bottom, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::Bottom, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::Bottom, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::Bottom, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::Bottom, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+3, index, index+1, index+2 });
                    index += 4;
                }

                if (isVoidInNextTo(Direction::North, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::North, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::North, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::North, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::North, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::North, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }

                if (isVoidInNextTo(Direction::South, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::South, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::South, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::South, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::South, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::South, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                if (isVoidInNextTo(Direction::East, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::East, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::East, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::East, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::East, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::East, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                if (isVoidInNextTo(Direction::West, voxel_origin)) {
                    vtx0 = { 0, voxel_origin, Direction::West, voxel_id };
                    vtx1 = { 1, voxel_origin, Direction::West, voxel_id };
                    vtx2 = { 2, voxel_origin, Direction::West, voxel_id };
                    vtx3 = { 3, voxel_origin, Direction::West, voxel_id };

                    updateBoundingBoxWithCubeFace(Direction::West, voxel_origin);
                    mesh.vertices.insert(mesh.vertices.end(),{ vtx0, vtx1, vtx2, vtx3 });
                    mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }
            }
        }
    }

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

    setBuilt(true);
}

void Chunk::destroyMesh() {
    if (not isBuilt()) return;

    glDeleteVertexArrays(1, &mesh.vao);
    glDeleteBuffers(1, &mesh.ssbo_vertices);
    glDeleteBuffers(1, &mesh.ebo);
    mesh.vertices.clear();
    mesh.indices.clear();

    setBuilt(false);
}

void Chunk::resetVoxels() {
    std::fill(std::begin(voxel_ids), std::end(voxel_ids), 0);
    setEmpty(true);
}

void Chunk::render() const {
    if (not isBuilt()) return;

    glBindVertexArray(mesh.vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mesh.ssbo_vertices);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
}

bool Chunk::isBuilt() const {
    return is_built;
}

bool Chunk::isEmpty() const {
    return is_empty;
}

void Chunk::setBuilt(const bool state) {
    is_built = state;
}

void Chunk::setEmpty(const bool state) {
    is_empty = state;
}
