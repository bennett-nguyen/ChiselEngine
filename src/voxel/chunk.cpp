#include "chunk.hpp"

void buildBoundingBox(Chunk *ptr_chunk) {
    const std::vector<Vertex>& mesh_vertices = ptr_chunk->mesh.vertices;

    glm::vec3 vmin = mesh_vertices[0].position;
    glm::vec3 vmax = vmin;

    for (size_t i = 1; i < ptr_chunk->mesh.vertices.size(); i++) {
        const glm::vec3& current = mesh_vertices[i].position;
        vmin = glm::min(vmin, current);
        vmax = glm::max(vmax, current);
    }

    vmax += glm::vec3(1.0f);

    // Transform it to match with the location in world space
    const glm::mat4 chunk_model = getChunkModel(ptr_chunk);
    vmin = glm::vec3(chunk_model * glm::vec4(vmin.x, vmin.y, vmin.z, 1.0f));
    vmax = glm::vec3(chunk_model * glm::vec4(vmax.x, vmax.y, vmax.z, 1.0f));

    ptr_chunk->bounding_box.vmin = vmin;
    ptr_chunk->bounding_box.vmax = vmax;
}

bool isChunkVisible(const Chunk *ptr_chunk, const std::vector<glm::vec4>& frustum_planes) {
    const glm::vec3& vmin = ptr_chunk->bounding_box.vmin;
    const glm::vec3& vmax = ptr_chunk->bounding_box.vmax;

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

bool isVoid(const glm::uvec3 local_position, const Chunk *ptr_chunk) {
    return 0 == ptr_chunk->ptr_voxels[Conversion::toIndex(local_position)];
}

bool isVoidEast(glm::uvec3 local_position, const Chunk *ptr_chunk, const Chunk *ptr_echunk) {
    if (Constant::CHUNK_SIZE-1 == local_position.z) {
        if (nullptr == ptr_echunk) return true;

        local_position.z = 0;
        return isVoid(local_position, ptr_echunk);
    }

    local_position.z += 1;
    return isVoid(local_position, ptr_chunk);
}

bool isVoidWest(glm::uvec3 local_position, const Chunk *ptr_chunk, const Chunk *ptr_wchunk) {
    if (0 == local_position.z) {
        if (nullptr == ptr_wchunk) return true;

        local_position.z = Constant::CHUNK_SIZE-1;
        return isVoid(local_position, ptr_wchunk);
    }

    local_position.z -= 1;
    return isVoid(local_position, ptr_chunk);
}

bool isVoidNorth(glm::uvec3 local_position, const Chunk *ptr_chunk, const Chunk *ptr_nchunk) {
    if (Constant::CHUNK_SIZE-1 == local_position.x) {
        if (nullptr == ptr_nchunk) return true;

        local_position.x = 0;
        return isVoid(local_position, ptr_nchunk);
    }

    local_position.x += 1;
    return isVoid(local_position, ptr_chunk);
}

bool isVoidSouth(glm::uvec3 local_position, const Chunk *ptr_chunk, const Chunk *ptr_schunk) {
    if (0 == local_position.x) {
        if (nullptr == ptr_schunk) return true;

        local_position.x = Constant::CHUNK_SIZE-1;
        return isVoid(local_position, ptr_schunk);
    }

    local_position.x -= 1;
    return isVoid(local_position, ptr_chunk);
}

bool isVoidTop(glm::uvec3 local_position, const Chunk *ptr_chunk) {
    if (Constant::CHUNK_HEIGHT-1 == local_position.y) {
        // check for neighboring chunk
        return true;
    }

    return isVoid(local_position + glm::uvec3(0, 1, 0), ptr_chunk);
}

bool isVoidBottom(glm::uvec3 local_position, const Chunk *ptr_chunk) {
    if (0 == local_position.y) {
        // check for neighboring chunk
        return false;
    }

    return isVoid(local_position - glm::uvec3(0, 1, 0), ptr_chunk);
}

unsigned getVoxelID(const Chunk *ptr_chunk, const glm::uvec3 local_position) {
    return ptr_chunk->ptr_voxels[Conversion::toIndex(local_position)];
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

void buildVoxels(Chunk *ptr_chunk) {
    ptr_chunk->ptr_voxels = new unsigned[Constant::CHUNK_VOLUME] {};

    unsigned y_level;
    unsigned voxel_id = 1;

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            y_level = heightMap(6, float((int)x + ptr_chunk->position.x * (int)Constant::CHUNK_SIZE),
                float((int)z + ptr_chunk->position.z * (int)Constant::CHUNK_SIZE), 0.6f, 0.007f, 0, Constant::CHUNK_HEIGHT);

            for (unsigned y = 0; y <= y_level; y++) {
                ptr_chunk->ptr_voxels[Conversion::toIndex(x, y, z)] = voxel_id;
                voxel_id %= 5;
                voxel_id++;
            }
        }
    }
}

void buildMesh(Chunk *ptr_chunk,
                Chunk *ptr_nchunk,
                Chunk *ptr_schunk,
                Chunk *ptr_echunk,
                Chunk *ptr_wchunk) {
    GLuint index = 0;

    for (unsigned x = 0; x < Constant::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Constant::CHUNK_SIZE; z++) {
            for (unsigned y = 0; y < Constant::CHUNK_HEIGHT; y++) {
                const auto position = glm::uvec3(x, y, z);
                const unsigned voxel_id = getVoxelID(ptr_chunk, position);
                if (0 == voxel_id) continue;

                const int X_VERT = static_cast<int>(x);
                const int Y_VERT = static_cast<int>(y);
                const int Z_VERT = static_cast<int>(z);

                // Top
                if (isVoidTop(position, ptr_chunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT,   Y_VERT+1, Z_VERT,   1.0f, 0.0f, voxel_id, Top),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT,   0.0f, 0.0f, voxel_id, Top),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT+1, 0.0f, 1.0f, voxel_id, Top),
                        Vertex(X_VERT,   Y_VERT+1, Z_VERT+1, 1.0f, 1.0f, voxel_id, Top)
                    });

                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+3, index+2, index, index+2, index+1 });
                    index += 4;
                }

                // Bottom
                if (isVoidBottom(position, ptr_chunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT,   Y_VERT, Z_VERT,   0.0f, 0.0f, voxel_id, Bottom),
                        Vertex(X_VERT+1, Y_VERT, Z_VERT,   1.0f, 0.0f, voxel_id, Bottom),
                        Vertex(X_VERT+1, Y_VERT, Z_VERT+1, 1.0f, 1.0f, voxel_id, Bottom),
                        Vertex(X_VERT,   Y_VERT, Z_VERT+1, 0.0f, 1.0f, voxel_id, Bottom)
                    });

                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+2, index+3, index, index+1, index+2 });
                    index += 4;
                }

                // North
                if (isVoidNorth(position, ptr_chunk, ptr_nchunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT+1, Y_VERT,   Z_VERT,   1.0f, 0.0f, voxel_id, North),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT,   1.0f, 1.0f, voxel_id, North),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT+1, 0.0f, 1.0f, voxel_id, North),
                        Vertex(X_VERT+1, Y_VERT,   Z_VERT+1, 0.0f, 0.0f, voxel_id, North)
                    });


                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }

                // South
                if (isVoidSouth(position, ptr_chunk, ptr_schunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT, Y_VERT,   Z_VERT,   0.0f, 0.0f, voxel_id, South),
                        Vertex(X_VERT, Y_VERT+1, Z_VERT,   0.0f, 1.0f, voxel_id, South),
                        Vertex(X_VERT, Y_VERT+1, Z_VERT+1, 1.0f, 1.0f, voxel_id, South),
                        Vertex(X_VERT, Y_VERT,   Z_VERT+1, 1.0f, 0.0f, voxel_id, South)
                    });

                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                // East
                if (isVoidEast(position, ptr_chunk, ptr_echunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT,   Y_VERT,   Z_VERT+1, 0.0f, 0.0f, voxel_id, East),
                        Vertex(X_VERT,   Y_VERT+1, Z_VERT+1, 0.0f, 1.0f, voxel_id, East),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT+1, 1.0f, 1.0f, voxel_id, East),
                        Vertex(X_VERT+1, Y_VERT,   Z_VERT+1, 1.0f, 0.0f, voxel_id, East)
                    });

                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+2, index+1, index, index+3, index+2 });
                    index += 4;
                }

                // West
                if (isVoidWest(position, ptr_chunk, ptr_wchunk)) {
                    ptr_chunk->mesh.vertices.insert(ptr_chunk->mesh.vertices.end(), {
                        Vertex(X_VERT,   Y_VERT,   Z_VERT, 1.0f, 0.0f, voxel_id, West),
                        Vertex(X_VERT,   Y_VERT+1, Z_VERT, 1.0f, 1.0f, voxel_id, West),
                        Vertex(X_VERT+1, Y_VERT+1, Z_VERT, 0.0f, 1.0f, voxel_id, West),
                        Vertex(X_VERT+1, Y_VERT,   Z_VERT, 0.0f, 0.0f, voxel_id, West)
                    });

                    ptr_chunk->mesh.indices.insert(ptr_chunk->mesh.indices.end(), { index, index+1, index+2, index, index+2, index+3 });
                    index += 4;
                }
            }
        }
    }

    glCreateBuffers(1, &ptr_chunk->mesh.ssbo_vertices);
    glCreateBuffers(1, &ptr_chunk->mesh.ebo);
    glCreateVertexArrays(1, &ptr_chunk->mesh.vao);

    const auto VERTEX_BUFFER_SIZE = static_cast<GLsizeiptr>(ptr_chunk->mesh.vertices.size() * sizeof(Vertex));
    const auto VERTEX_DATA = reinterpret_cast<void *>(ptr_chunk->mesh.vertices.data());
    const auto ELEMENT_BUFFER_SIZE = static_cast<GLsizeiptr>(ptr_chunk->mesh.indices.size() * sizeof(GLuint));
    const auto ELEMENT_DATA = reinterpret_cast<void *>(ptr_chunk->mesh.indices.data());
    glNamedBufferStorage(ptr_chunk->mesh.ssbo_vertices, VERTEX_BUFFER_SIZE, VERTEX_DATA, 0);
    glNamedBufferStorage(ptr_chunk->mesh.ebo, ELEMENT_BUFFER_SIZE, ELEMENT_DATA, 0);
    glVertexArrayElementBuffer(ptr_chunk->mesh.vao, ptr_chunk->mesh.ebo);
}

void destroyChunk(Chunk *ptr_chunk) {
    destroyMesh(ptr_chunk);
    delete[] ptr_chunk->ptr_voxels;
}

void destroyMesh(Chunk *ptr_chunk) {
    glDeleteVertexArrays(1, &ptr_chunk->mesh.vao);
    glDeleteBuffers(1, &ptr_chunk->mesh.ssbo_vertices);
    glDeleteBuffers(1, &ptr_chunk->mesh.ebo);
    ptr_chunk->mesh.vertices.clear();
    ptr_chunk->mesh.indices.clear();
}

void renderChunk(const Chunk *ptr_chunk) {
    glBindVertexArray(ptr_chunk->mesh.vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(ptr_chunk->mesh.indices.size()), GL_UNSIGNED_INT, 0);
}

glm::mat4 getChunkModel(const Chunk *ptr_chunk) {
    glm::mat4 model(1.0f);
    const glm::ivec3 chunk_position_to_world = Conversion::chunkToWorld(ptr_chunk->position);
    model = glm::translate(model, glm::vec3(chunk_position_to_world));
    return model;
}