#include "chunk.hpp"

constexpr unsigned X_SIZE = 5,
                   Y_SIZE = 5,
                   Z_SIZE = 5,
                   AO_ID_SIZE = 2,
                   FACE_ID_SIZE = 3,
                   VOXEL_ID_SIZE = 8;

inline void appendBits(unsigned &base, const unsigned data, const unsigned size) {
    base <<= size;
    base |= data;
}

bool isVoxelAdjacentToChunkInXAxis(const LocalPosition voxel_origin, const Direction expected_adjacent) {
    if (isVoxelAtChunkBoundaryNorth(voxel_origin) and Direction::North == expected_adjacent) {
        return true;
    }

    if (isVoxelAtChunkBoundarySouth(voxel_origin) and Direction::South == expected_adjacent) {
        return true;
    }

    return false;
}

bool isVoxelAdjacentToChunkInYAxis(const LocalPosition voxel_origin, const Direction expected_adjacent) {
    if (isVoxelAtChunkBoundaryTop(voxel_origin) and Direction::Top == expected_adjacent) {
        return true;
    }

    if (isVoxelAtChunkBoundaryBottom(voxel_origin) and Direction::Bottom == expected_adjacent) {
        return true;
    }

    return false;
}

bool isVoxelAdjacentToChunkInZAxis(const LocalPosition voxel_origin, const Direction expected_adjacent) {
    if (isVoxelAtChunkBoundaryEast(voxel_origin) and Direction::East == expected_adjacent) {
        return true;
    }

    if (isVoxelAtChunkBoundaryWest(voxel_origin) and Direction::West == expected_adjacent) {
        return true;
    }

    return false;
}

Vertex::Vertex(const int vertex_index, const LocalPosition &voxel_origin, const unsigned ao_id, const Direction face_direction, const VoxelID voxel_id) {
    const unsigned VERTEX_FACE_ID = FACE_DIRECTION_TO_ID.at(face_direction);
    const LocalPosition VERTEX_POSITION = voxel_origin + FACE_VERTICES.at(face_direction).at(vertex_index);

    appendBits(packed_data, VERTEX_POSITION.x, X_SIZE);
    appendBits(packed_data, VERTEX_POSITION.y, Y_SIZE);
    appendBits(packed_data, VERTEX_POSITION.z, Z_SIZE);
    appendBits(packed_data, ao_id, AO_ID_SIZE);
    appendBits(packed_data, VERTEX_FACE_ID, FACE_ID_SIZE);
    appendBits(packed_data, voxel_id, VOXEL_ID_SIZE);
}

void Chunk::buildVoxels() {
    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            glm::vec3 voxel_position = Conversion::toWorld(LocalPosition(x, 0, z), position);
            voxel_position /= static_cast<float>(Constant::CHUNK_SIZE);

            const unsigned y_level = heightMap(voxel_position);

            for (unsigned y = 0; y <= y_level; y++) {
                const LocalPosition local { x, y, z };
                VoxelID voxel_id {};

                if (y <= 15) {
                    voxel_id = 6;
                } else if (y < y_level || y <= 20) {
                    voxel_id = 1;
                } else if (y == y_level) {
                    voxel_id = 2;
                }

                setVoxelIDAtPosition(voxel_id, local);
                setEmpty(false);
            }
        }
    }
}

void Chunk::resetVoxels() {
    std::fill(std::begin(voxel_ids), std::end(voxel_ids), 0);
    setEmpty(true);
}

void Chunk::buildMesh() {
    if (isEmpty()) return;

    GLuint index = 0;
    std::array<unsigned, 4>&& AO {};

    bounding_box.reset();

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            for (unsigned y = 0; y < Constant::CHUNK_HEIGHT; y++) {
                LocalPosition voxel_origin = { x, y, z };
                if (isVoidAt(voxel_origin)) continue;
                const VoxelID voxel_id = getVoxelID(voxel_origin);

                if (isVoidTopNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::Top, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+3, index+2, index, index+2, index+1 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+3, index+2, index+1, index, index+3 });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::Top, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::Top, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::Top, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::Top, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::Top, voxel_origin);

                    index += 4;
                }

                if (isVoidBottomNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::Bottom, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+2, index+3, index, index+1, index+2 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+2, index+3, index+1, index+3, index });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::Bottom, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::Bottom, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::Bottom, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::Bottom, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::Bottom, voxel_origin);

                    index += 4;
                }

                if (isVoidNorthNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::North, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+2, index+3, index+1, index+3, index });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::North, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::North, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::North, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::North, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::North, voxel_origin);

                    index += 4;
                }

                if (isVoidSouthNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::South, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+3, index+2, index+1, index, index+3 });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::South, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::South, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::South, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::South, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::South, voxel_origin);

                    index += 4;
                }

                if (isVoidEastNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::East, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+3, index+2, index+1, index, index+3 });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::East, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::East, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::East, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::East, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::East, voxel_origin);

                    index += 4;
                }

                if (isVoidWestNeighbor(voxel_origin)) {
                    AO = getVertexAO(Direction::West, voxel_origin);

                    if (AO.at(0) + AO.at(2) > AO.at(1) + AO.at(3)) {
                        mesh.indices.insert(mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    } else {
                        mesh.indices.insert(mesh.indices.end(), { index+1, index+2, index+3, index+1, index+3, index });
                    }

                    mesh.vertices.emplace_back(0, voxel_origin, AO.at(0), Direction::West, voxel_id);
                    mesh.vertices.emplace_back(1, voxel_origin, AO.at(1), Direction::West, voxel_id);
                    mesh.vertices.emplace_back(2, voxel_origin, AO.at(2), Direction::West, voxel_id);
                    mesh.vertices.emplace_back(3, voxel_origin, AO.at(3), Direction::West, voxel_id);

                    bounding_box.updateWithCubeFace(Direction::West, voxel_origin);

                    index += 4;
                }
            }
        }
    }

    bounding_box.translate(position);

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

void Chunk::render() const {
    if (not isBuilt()) return;

    glBindVertexArray(mesh.vao);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mesh.ssbo_vertices);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, 0);
}

bool Chunk::isChunkVisible(const std::array<glm::vec4, 6>& frustum_planes) const {
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

bool Chunk::isVoidAt(const LocalPosition local) const {
    return 0 == getVoxelID(local);
}

VoxelID Chunk::getVoxelID(const LocalPosition local) const {
    return voxel_ids.at(Conversion::toIndex(local));
}

void Chunk::setVoxelIDAtPosition(const VoxelID voxel_id, const LocalPosition local) {
    voxel_ids.at(Conversion::toIndex(local)) = voxel_id;
}

std::array<unsigned, 4> Chunk::getVertexAO(const Direction face, const LocalPosition voxel_origin) const {
    std::array<unsigned, 4> face_ao {};

    unsigned a = 1;
    unsigned b = 1;
    unsigned c = 1;
    unsigned d = 1;
    unsigned e = 1;
    unsigned f = 1;
    unsigned g = 1;
    unsigned h = 1;

    if (Direction::Top == face) {
        a = isVoidTopSouthNeighbor(voxel_origin);
        b = isVoidTopSouthWestNeighbor(voxel_origin);
        c = isVoidTopWestNeighbor(voxel_origin);
        d = isVoidTopNorthWestNeighbor(voxel_origin);
        e = isVoidTopNorthNeighbor(voxel_origin);
        f = isVoidTopNorthEastNeighbor(voxel_origin);
        g = isVoidTopEastNeighbor(voxel_origin);
        h = isVoidTopSouthEastNeighbor(voxel_origin);
    } else if (Direction::Bottom == face) {
        a = isVoidBottomSouthNeighbor(voxel_origin);
        b = isVoidBottomSouthWestNeighbor(voxel_origin);
        c = isVoidBottomWestNeighbor(voxel_origin);
        d = isVoidBottomNorthWestNeighbor(voxel_origin);
        e = isVoidBottomNorthNeighbor(voxel_origin);
        f = isVoidBottomNorthEastNeighbor(voxel_origin);
        g = isVoidBottomEastNeighbor(voxel_origin);
        h = isVoidBottomSouthEastNeighbor(voxel_origin);
    } else if (Direction::North == face) {
        a = isVoidBottomNorthNeighbor(voxel_origin);
        b = isVoidBottomNorthWestNeighbor(voxel_origin);
        c = isVoidNorthWestNeighbor(voxel_origin);
        d = isVoidTopNorthWestNeighbor(voxel_origin);
        e = isVoidTopNorthNeighbor(voxel_origin);
        f = isVoidTopNorthEastNeighbor(voxel_origin);
        g = isVoidNorthEastNeighbor(voxel_origin);
        h = isVoidBottomNorthEastNeighbor(voxel_origin);
    } else if (Direction::South == face) {
        a = isVoidBottomSouthNeighbor(voxel_origin);
        b = isVoidBottomSouthWestNeighbor(voxel_origin);
        c = isVoidSouthWestNeighbor(voxel_origin);
        d = isVoidTopSouthWestNeighbor(voxel_origin);
        e = isVoidTopSouthNeighbor(voxel_origin);
        f = isVoidTopSouthEastNeighbor(voxel_origin);
        g = isVoidSouthEastNeighbor(voxel_origin);
        h = isVoidBottomSouthEastNeighbor(voxel_origin);
    } else if (Direction::East == face) {
        a = isVoidBottomEastNeighbor(voxel_origin);
        b = isVoidBottomSouthEastNeighbor(voxel_origin);
        c = isVoidSouthEastNeighbor(voxel_origin);
        d = isVoidTopSouthEastNeighbor(voxel_origin);
        e = isVoidTopEastNeighbor(voxel_origin);
        f = isVoidTopNorthEastNeighbor(voxel_origin);
        g = isVoidNorthEastNeighbor(voxel_origin);
        h = isVoidBottomNorthEastNeighbor(voxel_origin);
    } else if (Direction::West == face) {
        a = isVoidBottomWestNeighbor(voxel_origin);
        b = isVoidBottomSouthWestNeighbor(voxel_origin);
        c = isVoidSouthWestNeighbor(voxel_origin);
        d = isVoidTopSouthWestNeighbor(voxel_origin);
        e = isVoidTopWestNeighbor(voxel_origin);
        f = isVoidTopNorthWestNeighbor(voxel_origin);
        g = isVoidNorthWestNeighbor(voxel_origin);
        h = isVoidBottomNorthWestNeighbor(voxel_origin);
    }

    face_ao.at(0) = a + b + c;
    face_ao.at(1) = c + d + e;
    face_ao.at(2) = e + f + g;
    face_ao.at(3) = g + h + a;

    return face_ao;
}

bool Chunk::isVoidEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_EAST) {
        if (nullptr == neighbors.east or neighbors.east->isEmpty()) return true;
        neighbor_voxel.z = 0;
        return neighbors.east->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_WEST) {
        if (nullptr == neighbors.west or neighbors.west->isEmpty()) return true;
        neighbor_voxel.z = Constant::CHUNK_SIZE-1;
        return neighbors.west->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidNorthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);

    if (IS_ADJACENT_NORTH) {
        if (nullptr == neighbors.north or neighbors.north->isEmpty()) return true;
        neighbor_voxel.x = 0;
        return neighbors.north->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidSouthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);

    if (IS_ADJACENT_SOUTH) {
        if (nullptr == neighbors.south or neighbors.south->isEmpty()) return true;
        neighbor_voxel.x = Constant::CHUNK_SIZE-1;
        return neighbors.south->isVoidAt(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);

    if (IS_ADJACENT_TOP) {
        return true;
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);

    if (IS_ADJACENT_BOTTOM) {
        return true;
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidNorthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_NORTH and IS_ADJACENT_EAST) {
        if (nullptr == neighbors.north_east or neighbors.north_east->isEmpty()) return true;
        neighbor_voxel.x = 0;
        neighbor_voxel.z = 0;
        return neighbors.north_east->isVoidAt(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidNorthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_NORTH and IS_ADJACENT_WEST) {
        if (nullptr == neighbors.north_west or neighbors.north_west->isEmpty()) return true;
        neighbor_voxel.x = 0;
        neighbor_voxel.z = Constant::CHUNK_SIZE-1;
        return neighbors.north_west->isVoidAt(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidSouthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_EAST) {
        if (nullptr == neighbors.south_east or neighbors.south_east->isEmpty()) return true;
        neighbor_voxel.x = Constant::CHUNK_SIZE-1;
        neighbor_voxel.z = 0;
        return neighbors.south_east->isVoidAt(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidSouthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_WEST) {
        if (nullptr == neighbors.south_west or neighbors.south_west->isEmpty()) return true;
        neighbor_voxel.x = Constant::CHUNK_SIZE-1;
        neighbor_voxel.z = Constant::CHUNK_SIZE-1;
        return neighbors.south_west->isVoidAt(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopNorthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);

    if (IS_ADJACENT_TOP and IS_ADJACENT_NORTH) {
        return true;
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::North);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopSouthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);

    if (IS_ADJACENT_TOP and IS_ADJACENT_SOUTH) {
        return true;
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::South);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_TOP and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_TOP and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopNorthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_TOP and IS_ADJACENT_NORTH and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidTopEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidTopNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidNorthEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::East);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::East);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::North);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::North | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopNorthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_TOP and IS_ADJACENT_NORTH and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidTopWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidTopNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidNorthWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::West);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::West);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::North);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::North | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopSouthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_TOP and IS_ADJACENT_SOUTH and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidTopEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidTopSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidSouthEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::East);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::East);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::South);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::South | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidTopSouthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_TOP = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Top);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_TOP and IS_ADJACENT_SOUTH and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidTopWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP and IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidTopSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top);
        return isVoidSouthWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_TOP) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::West);
        return isVoidTopNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::West);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::South);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Top | Direction::South | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomNorthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_NORTH) {
        return true;
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::North);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomSouthNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_SOUTH) {
        return true;
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::South);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomNorthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_NORTH and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidBottomEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidBottomNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidNorthEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::East);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::East);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::North);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::North | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomNorthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_NORTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::North);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_NORTH and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North);
        return isVoidBottomWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidBottomNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidNorthWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::North | Direction::West);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_NORTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::West);
        return isVoidNorthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::North);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::North | Direction::West);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomSouthEastNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_EAST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::East);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_SOUTH and IS_ADJACENT_EAST) {
        return true;
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidBottomEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::East);
        return isVoidBottomSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidSouthEastNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::East);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::East);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_EAST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::South);
        return isVoidEastNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::South | Direction::East);
    return isVoidAt(neighbor_voxel);
}

bool Chunk::isVoidBottomSouthWestNeighbor(const LocalPosition voxel_origin) const {
    LocalPosition neighbor_voxel = voxel_origin;
    const bool IS_ADJACENT_SOUTH = isVoxelAdjacentToChunkInXAxis(voxel_origin, Direction::South);
    const bool IS_ADJACENT_BOTTOM = isVoxelAdjacentToChunkInYAxis(voxel_origin, Direction::Bottom);
    const bool IS_ADJACENT_WEST = isVoxelAdjacentToChunkInZAxis(voxel_origin, Direction::West);

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_SOUTH and IS_ADJACENT_WEST) {
        return true;
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South);
        return isVoidBottomWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM and IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::West);
        return isVoidBottomSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH and IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom);
        return isVoidSouthWestNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_BOTTOM) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::South | Direction::West);
        return isVoidBottomNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_SOUTH) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::West);
        return isVoidSouthNeighbor(neighbor_voxel);
    }

    if (IS_ADJACENT_WEST) {
        neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::South);
        return isVoidWestNeighbor(neighbor_voxel);
    }

    neighbor_voxel += CHUNK_NEIGHBORS_DIRECTION.at(Direction::Bottom | Direction::South | Direction::West);
    return isVoidAt(neighbor_voxel);
}

void Chunk::fetchNeighbors(const ChunkNeighbors &neighbors) {
    this->neighbors = neighbors;
}

void Chunk::setPosition(const ChunkPosition position) {
    this->position = position;
}

void Chunk::preload() {
    constexpr unsigned RESERVED_NUM_FACES = 8192;
    mesh.vertices.reserve(RESERVED_NUM_FACES * 4);
    mesh.indices.reserve(RESERVED_NUM_FACES * 6);
}
