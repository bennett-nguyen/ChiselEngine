#include "block_registry.hpp"

chisel::BlockRegistry& chisel::BlockRegistry::getInstance() {
    static BlockRegistry instance {};
    return instance;
}

chisel::BlockRegistry::BlockRegistry() {
    using namespace rapidjson;
    definitions.emplace_back(AIR_NAME, AIR_ID, std::vector<std::string>(), std::unordered_map<std::string, unsigned>());
    name_to_id.emplace(AIR_NAME, AIR_ID);
    id_to_name.emplace_back(AIR_NAME);

    FILE* fp = fopen("resources/block_definition.json", "rb");
    if (fp == nullptr) {
        std::cerr << "ERROR :: CANNOT READ resources/block_definition.json" << '\n';
        exit(1);
    }

    char read_buffer[65536];
    FileReadStream is(fp, read_buffer, sizeof(read_buffer));

    Document document;
    document.ParseStream(is);

    const Value::Array& definition_array = document["definitions"].GetArray();

    types::VoxelID id = 1;
    for (Value::ValueIterator entry_itr = definition_array.Begin(); entry_itr != definition_array.End(); ++entry_itr) {
        std::string name = (*entry_itr)["name"].GetString();
        const Value::Array& textures = (*entry_itr)["textures"].GetArray();
        const Value::Object& texture_face = (*entry_itr)["texture_face"].GetObject();

        std::vector<std::string> textures_path {};
        std::unordered_map<std::string, unsigned> texture_faces {};

        for (auto const& file_path : textures) {
            textures_path.emplace_back(file_path.GetString());
        }

        for (auto const& [face_str, _] : FACE_STRING_TO_ID) {
            texture_faces[face_str] = texture_face[face_str.c_str()].GetUint();
        }

        definitions.emplace_back(name, id, textures_path, texture_faces);
        name_to_id.emplace(name, id);
        id_to_name.push_back(name);
        id++;
    }

    fclose(fp);
}

chisel::BlockDefinition chisel::BlockRegistry::getDefinition(const std::string &name) const {
    const types::VoxelID id = name_to_id.at(name);
    return definitions.at(id);
}

const std::vector<chisel::BlockDefinition>& chisel::BlockRegistry::getAllDefinitions() const {
    return definitions;
}

