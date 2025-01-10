#pragma once

#include "Renderer/RenderData.h"

#include "util.h"

struct Mesh
{
    UUID id;
    MeshData data;
};

class MeshLoader 
{
public:
    static std::shared_ptr<Mesh> loadMesh(const std::filesystem::path& filepath, const glm::mat4& t_mesh_world = glm::mat4(1.0f));

private:
    static void addNode(const aiScene* source, const aiNode* node, glm::mat4 t_node_world, const glm::mat4& t_mesh_world);

    static std::shared_ptr<Mesh> s_mesh;
};

class MeshLibrary
{
public:
    static void add(const UUID id, const std::shared_ptr<Mesh>& mesh);
    static std::shared_ptr<Mesh> load(const std::filesystem::path& filepath, const glm::mat4& t_mesh_world = glm::mat4(1.0f));

    static std::shared_ptr<Mesh> get(const UUID id);
    inline static bool exists(const UUID id) { return s_meshes.find(id) != s_meshes.end(); }

private:
    static std::unordered_map<UUID, std::shared_ptr<Mesh>> s_meshes;
};
