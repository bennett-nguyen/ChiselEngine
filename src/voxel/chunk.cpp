#include "chunk.hpp"

std::array<unsigned, 4> get_vertices_indices(Faces face) {
    switch (face) {
        case X_NEG:
            return std::array<unsigned, 4>({ 1, 3, 5, 7 });
        case X_POS:
            return std::array<unsigned, 4>({ 2, 4, 6, 8 });
        case Y_NEG:
            return std::array<unsigned, 4>({ 1, 2, 3, 4 });
        case Y_POS:
            return std::array<unsigned, 4>({ 5, 6, 7, 8 });
        case Z_NEG:
            return std::array<unsigned, 4>({ 1, 2, 5, 6 });
        case Z_POS:
            return std::array<unsigned, 4>({ 3, 4, 7, 8 });
    }
}

std::array<unsigned, 6> get_face_indices(Faces face) {
    switch (face) {
        case X_NEG:
            return std::array<unsigned, 6>({ 1, 3, 5, 3, 5, 7 });
        case X_POS:
            return std::array<unsigned, 6>({ 2, 4, 6, 4, 6, 8 });
        case Y_NEG:
            return std::array<unsigned, 6>({ 1, 2, 3, 2, 3, 4 });
        case Y_POS:
            return std::array<unsigned, 6>({ 5, 6, 7, 6, 7, 8 });
        case Z_NEG:
            return std::array<unsigned, 6>({ 1, 2, 5, 2, 5, 6 });
        case Z_POS:
            return std::array<unsigned, 6>({ 3, 4, 7, 4, 7, 8 });
    }
}

const unsigned Chunk::CHUNK_SIZE = 16;
const unsigned Chunk::CHUNK_HEIGHT = 60;
const unsigned Chunk::CHUNK_AREA = Chunk::CHUNK_SIZE * Chunk::CHUNK_SIZE;

Chunk::Chunk(glm::vec3 position) {
    this->chunk_position = position;
}

Chunk::~Chunk() {
    delete[] this->pBlocks;
}

void Chunk::load_chunk() {
    this->pBlocks = new Block[Chunk::CHUNK_AREA * Chunk::CHUNK_HEIGHT];
    this->chunk_mesh.load();

    for (unsigned x = 0; x < Chunk::CHUNK_SIZE; x++) {
        for (unsigned z = 0; z < Chunk::CHUNK_SIZE; z++) {
            glm::vec3 chunk_pos = this->chunk_position;
            unsigned y_level = unsigned((glm::simplex(glm::vec2(x+chunk_pos.x, z+chunk_pos.z) * 0.02f) + 1.0f) / 2.0f * CHUNK_HEIGHT);
            for (unsigned y = 0; y <= y_level; y++) {
                this->get_pblock(x, y, z)->set_active(true);
            }
        }
    }

    this->_is_loaded = true;
}

void Chunk::unload_chunk() {
    this->chunk_mesh.destroy();
    delete[] this->pBlocks;
    this->_is_loaded = _is_setup = false;
}

void Chunk::setup_chunk() {
    this->create_mesh();
    this->chunk_mesh.setup();
    this->_is_setup = true;
}

void Chunk::rebuild_mesh() {
    this->chunk_mesh.destroy();
    this->chunk_mesh.load();
    this->create_mesh();
    this->chunk_mesh.setup();
}


size_t Chunk::get_block_idx(unsigned x, unsigned y, unsigned z) {
    return x + Chunk::CHUNK_SIZE * z + Chunk::CHUNK_AREA * y;
}

Block* Chunk::get_pblock(size_t idx) {
    return &this->pBlocks[idx];
}

Block* Chunk::get_pblock(unsigned x, unsigned y, unsigned z) {
    return &this->pBlocks[this->get_block_idx(x, y, z)];
}

void Chunk::create_mesh() {
    std::array<bool, 6> faces;
    for (unsigned y = 0; y < Chunk::CHUNK_HEIGHT; y++) {
        for (unsigned x = 0; x < Chunk::CHUNK_SIZE; x++) {
            for (unsigned z = 0; z < Chunk::CHUNK_SIZE; z++) {
                Block* current_pblock = this->get_pblock(x, y, z);
                if (!current_pblock->is_active()) continue;
                faces.fill(true);

                faces[X_NEG] = x <= 0 || !this->get_pblock(x-1, y, z)->is_active();
                faces[Y_NEG] = y <= 0 || !this->get_pblock(x, y-1, z)->is_active();
                faces[Z_NEG] = z <= 0 || !this->get_pblock(x, y, z-1)->is_active();

                faces[X_POS] = x+1 >= Chunk::CHUNK_SIZE   || !this->get_pblock(x+1, y, z)->is_active();
                faces[Y_POS] = y+1 >= Chunk::CHUNK_HEIGHT || !this->get_pblock(x, y+1, z)->is_active();
                faces[Z_POS] = z+1 >= Chunk::CHUNK_SIZE   || !this->get_pblock(x, y, z+1)->is_active();

                if (faces[X_NEG] || faces[Y_NEG] || faces[Z_NEG] ||
                        faces[X_POS] || faces[Y_POS] || faces[Z_POS]) {
                    this->create_cube(x, y, z, faces);
                }
            }
        }
    }
}

void Chunk::create_cube(unsigned x, unsigned y, unsigned z, std::array<bool, 6> faces) {
    float fx0 = (float)x + this->chunk_position.x;
    float fy0 = (float)y + this->chunk_position.y;
    float fz0 = (float)z + this->chunk_position.z;
    float fx1 = fx0 + Block::BLOCK_RENDER_SIZE;
    float fy1 = fy0 + Block::BLOCK_RENDER_SIZE;
    float fz1 = fz0 + Block::BLOCK_RENDER_SIZE;

    Vertex p1(fx0, fy0, fz0);
    Vertex p2(fx1, fy0, fz0);
    Vertex p3(fx0, fy0, fz1);
    Vertex p4(fx1, fy0, fz1);
    Vertex p5(fx0, fy1, fz0);
    Vertex p6(fx1, fy1, fz0);
    Vertex p7(fx0, fy1, fz1);
    Vertex p8(fx1, fy1, fz1);

    std::array<std::pair<GLuint, Vertex>, 8> is_chosen_vert = {
        std::make_pair(0, p1),
        std::make_pair(0, p2),
        std::make_pair(0, p3),
        std::make_pair(0, p4),
        std::make_pair(0, p5),
        std::make_pair(0, p6),
        std::make_pair(0, p7),
        std::make_pair(0, p8)
    };

    std::vector<Vertex> added_verts;
    std::vector<GLuint> *vp_chunk_mesh_indices = &this->chunk_mesh.indices;
    std::vector<Vertex> *vp_chunk_mesh_vertices = &this->chunk_mesh.vertices;

    for (int face = Faces::_BEGIN; face != Faces::_END; ++face) {
        for (auto const &idx : get_vertices_indices(static_cast<Faces>(face))) {
            if (faces[static_cast<Faces>(face)]) {
                is_chosen_vert[idx-1].first = 1;
            }
        }
    }

    GLuint index = (GLuint)vp_chunk_mesh_vertices->size();

    for (size_t i = 0; i < 8; i++) {
        if (0 == is_chosen_vert[i].first) continue;
        is_chosen_vert[i].first = index;
        added_verts.push_back(is_chosen_vert[i].second);
        index++;
    }

    vp_chunk_mesh_vertices->insert(vp_chunk_mesh_vertices->cend(), added_verts.cbegin(), added_verts.cend());

    for (int face = Faces::_BEGIN; face != Faces::_END; ++face) {
        if (!faces[static_cast<Faces>(face)]) continue;
        for (auto const &idx : get_face_indices(static_cast<Faces>(face))) {
            vp_chunk_mesh_indices->push_back(is_chosen_vert[idx-1].first);
        }
    }
}

void Chunk::render(ShaderProgram &shader_program, glm::vec4 voxel_fill, glm::vec4 voxel_outline) {
    this->chunk_mesh.draw(shader_program, voxel_fill, voxel_outline);
}

bool Chunk::is_loaded() {
    return this->_is_loaded;
}

bool Chunk::is_setup() {
    return this->_is_setup;
}

glm::vec3 Chunk::get_chunk_pos() {
    return this->chunk_position;
}