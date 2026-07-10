#include "block_textures.hpp"

chisel::BlockTextures& chisel::BlockTextures::getInstance() {
    static BlockTextures instance {};
    return instance;
}

chisel::BlockTextures::BlockTextures() {
    using namespace rapidjson;

    constexpr int TEXTURE_FACE_WIDTH = 32;
    constexpr int TEXTURE_FACE_HEIGHT = 32;
    constexpr int TEXTURE_ARRAY_WIDTH = TEXTURE_FACE_WIDTH * 6;
    constexpr int TEXTURE_ARRAY_HEIGHT = TEXTURE_FACE_HEIGHT;

    const BlockRegistry& registry = BlockRegistry::getInstance();

    glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &texture_array);
    glCreateSamplers(1, &sampler);

    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    const auto ARRAY_DEPTH = static_cast<GLsizei>(registry.getAllDefinitions().size());
    glTextureStorage3D(texture_array, 4, GL_RGBA8, TEXTURE_ARRAY_WIDTH, TEXTURE_ARRAY_HEIGHT, ARRAY_DEPTH);

    FILE* fp = fopen("resources/block_definition.json", "rb");
    if (fp == nullptr) {
        std::cerr << "ERROR :: CANNOT READ resources/block_definition.json" << '\n';
        exit(1);
    }

    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    Document document;
    document.ParseStream(is);
    const Value::Object& missing_textures = document["missing_textures"].GetObject();

    const Value::Array& textures = missing_textures["textures"].GetArray();
    const Value::Object& texture_faces = missing_textures["texture_face"].GetObject();

    int width {};
    int height {};
    int num_ch {};
    for (auto const& [face_str, face_id] : FACE_STRING_TO_ID) {
        unsigned face_idx = texture_faces[face_str.c_str()].GetUint();
        std::string file_path = textures[face_idx].GetString();

        auto xoffset = static_cast<GLint>(face_id) * 32;

        unsigned char* bytes = stbi_load(file_path.c_str(), &width, &height, &num_ch, 0);
        glTextureSubImage3D(texture_array, 0, xoffset, 0, 0, TEXTURE_FACE_WIDTH, TEXTURE_FACE_HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        stbi_image_free(bytes);
    }

    for (auto const& definition : registry.getAllDefinitions()) {
        if (definition.name == AIR_NAME) continue;

        for (auto const& [face_str, face_id] : FACE_STRING_TO_ID) {
            unsigned face_idx = definition.texture_faces.at(face_str);
            std::string file_path = definition.textures.at(face_idx);

            auto xoffset = static_cast<GLint>(face_id) * 32;

            unsigned char* bytes = stbi_load(file_path.c_str(), &width, &height, &num_ch, 0);
            glTextureSubImage3D(texture_array, 0, xoffset, 0, definition.voxel_id, TEXTURE_FACE_WIDTH, TEXTURE_FACE_HEIGHT, 1, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
            stbi_image_free(bytes);
        }
    }

    glGenerateTextureMipmap(texture_array);
    fclose(fp);
}

chisel::BlockTextures::~BlockTextures() {
    glDeleteSamplers(1, &sampler);
    glDeleteTextures(1, &texture_array);
}

void chisel::BlockTextures::bind() const {
    glBindTextureUnit(0, texture_array);
    glBindSampler(0, sampler);
}

void chisel::BlockTextures::unbind() {
    glBindTextureUnit(0, 0);
    glBindSampler(0, 0);
}
