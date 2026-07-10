#ifndef BLOCK_REGISTRY_HPP
#define BLOCK_REGISTRY_HPP

#include <iostream>
#include <cstdint>
#include <string>
#include <array>
#include <unordered_map>
#include <vector>
#include <fstream>

#include "block_textures.hpp"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

namespace chisel::types {
    using VoxelID = uint16_t;
}

namespace chisel {
    const std::string AIR_NAME = "chisel::air";
    constexpr types::VoxelID AIR_ID = 0;

    struct BlockDefinition {
        std::string name {};
        types::VoxelID voxel_id {};

        std::vector<std::string> textures {};
        std::unordered_map<std::string, unsigned> texture_faces {};

        BlockDefinition(const std::string& name, const uint16_t voxel_id, const std::vector<std::string>& textures, const std::unordered_map<std::string, unsigned>& texture_faces)
            : name(name), voxel_id(voxel_id), textures(std::move(textures)), texture_faces(std::move(texture_faces)){}
    };

    class BlockRegistry {
        std::vector<BlockDefinition> definitions {};
        std::unordered_map<std::string, types::VoxelID> name_to_id {};
        std::vector<std::string> id_to_name {};

        BlockRegistry();
    public:
        static BlockRegistry& getInstance();
        ~BlockRegistry() = default;

        BlockDefinition getDefinition(const std::string &name) const;
        const std::vector<BlockDefinition>& getAllDefinitions() const;

        BlockRegistry(const BlockRegistry&)            = delete;
        BlockRegistry& operator=(const BlockRegistry&) = delete;
        BlockRegistry(BlockRegistry&&)                 = delete;
        BlockRegistry& operator=(BlockRegistry&&)      = delete;
    };
}

#endif
