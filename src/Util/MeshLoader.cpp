#include "MeshLoader.h"

#include "Log.h"
#include "geometry.h"

std::shared_ptr<Mesh> MeshLoader::s_mesh;

std::shared_ptr<Mesh> MeshLoader::loadMesh(const std::filesystem::path& filepath, const glm::mat4& t_mesh_world)
{
    // check mesh file
    if (aiIsExtensionSupported(filepath.extension().c_str()) == AI_FALSE) {
        LOG_ERROR << "Invalid mesh-file: " << filepath;
        return {};
    }    
    s_mesh = std::make_shared<Mesh>();

    // load mesh data
    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_COLLADA_IGNORE_UP_DIRECTION, 1);
    const aiScene* source = importer.ReadFile(filepath.c_str(), aiProcessPreset_TargetRealtime_Fast);  
    glm::mat4 t_node_world(1.0f);
    addNode(source, source->mRootNode, t_node_world, t_mesh_world);
    importer.FreeScene();

    s_mesh->id = uuid();
    return s_mesh;
}

void MeshLoader::addNode(const aiScene* source, const aiNode* node, glm::mat4 t_node_world, const glm::mat4& t_mesh_world)
{
    glm::mat4 t_curr_world = convertMat4<aiMatrix4x4, glm::mat4>(node->mTransformation);
    setMat4Translation(t_curr_world, getMat4Translation(t_curr_world));

    t_node_world = t_node_world*t_curr_world;

    size_t indexStart = s_mesh->data.vertices.size();
    for (size_t i = 0; i < node->mNumMeshes; ++i) {
        const auto& meshSource = source->mMeshes[node->mMeshes[i]];

        glm::vec4 color(0.4f, 0.4f, 0.4f, 1.0f);
        if (source->mNumMaterials > 0 && meshSource->mMaterialIndex < source->mNumMaterials) {
            uint32_t size = 4; 
            source->mMaterials[meshSource->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, glm::value_ptr(color), &size);
        }
        else 
            LOG_WARN << "Material not valid: " << meshSource->mMaterialIndex;

        for (size_t j = 0; j < meshSource->mNumVertices; ++j) {
            auto& vertSource = meshSource->mVertices[j];

            glm::vec3 p_vertex_world;
            p_vertex_world.x = vertSource.x;
            p_vertex_world.y = vertSource.y;
            p_vertex_world.z = vertSource.z;  
            p_vertex_world = glm::vec4(p_vertex_world, 1.0f) * t_node_world * t_mesh_world;

            s_mesh->data.vertices.emplace_back(p_vertex_world, color);
        }

        for (size_t j = 0; j < meshSource->mNumFaces; ++j) {
            std::array<uint32_t, 3> indices;
            auto& indexSource = meshSource->mFaces[j];

            if (indexSource.mNumIndices != 3)
                continue;

            for (size_t k = 0; k < 3; ++k)
                indices[k] = indexStart + indexSource.mIndices[k];

            s_mesh->data.indices.emplace_back(indices);
        }
    }

    for (size_t i = 0; i < node->mNumChildren; ++i)
        addNode(source, node->mChildren[i], t_node_world, t_mesh_world);
}

// --------------------------------------------------

std::unordered_map<UUID, std::shared_ptr<Mesh>> MeshLibrary::s_meshes;

void MeshLibrary::add(const UUID id, const std::shared_ptr<Mesh>& mesh)
{
    assert(s_meshes.find(id) == s_meshes.end() && "Mesh already exists");
    s_meshes[id] = mesh;
}

std::shared_ptr<Mesh> MeshLibrary::load(const std::filesystem::path& filepath, const glm::mat4& t_mesh_world)
{
    std::shared_ptr<Mesh> mesh = MeshLoader::loadMesh(filepath, t_mesh_world);
    if (!mesh)
        return {};

    add(mesh->id, mesh);
    return mesh;
}

std::shared_ptr<Mesh> MeshLibrary::get(const UUID id)
{
    assert(s_meshes.find(id) != s_meshes.end() && "texture not found");
    return s_meshes[id];
}