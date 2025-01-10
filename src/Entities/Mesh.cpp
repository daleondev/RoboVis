// #include "pch.h"

// #include "Mesh.h"

// #include "Renderer/Renderer.h"

// #include "ImGui/ImGuiLayer.h"

// #include "Util/geometry.h"

// Mesh::Mesh(const aiScene* source, const glm::mat4& t_mesh_world)
// {
//     if (ShaderLibrary::exists("Color"))
//         m_shader = ShaderLibrary::get("Color");
//     else
//         m_shader = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/Color", "Color");

//     if (ShaderLibrary::exists("FlatColor"))
//         m_shaderBB = ShaderLibrary::get("FlatColor");
//     else
//         m_shaderBB = ShaderLibrary::load("/home/david/Schreibtisch/RoboVis/src/Shaders/FlatColor", "FlatColor");

//     glm::mat4 t_node_world(1.0f);
//     addNode(source, source->mRootNode, t_node_world, t_mesh_world);

//     const size_t numVertices = std::accumulate(m_meshData.begin(), m_meshData.end(), 0, [](const size_t sum, const MeshData& meshData) {
//         return sum + meshData.vertices.size();
//     });

//     m_triData = std::make_shared<TriangulationData>();
//     m_triData->vertices.resize(numVertices);
//     for (const auto& meshData : m_meshData)
//         m_triData->indices.insert(m_triData->indices.end(), meshData.indices.begin(), meshData.indices.end());
//     updateTriangulationData();

//     createBuffers();
// }

// Mesh::~Mesh() = default;

// void Mesh::draw(const Camera& camera, const bool drawBB)
// {
//     draw(camera);

//     if (m_visible && drawBB)
//         drawBoundingBox(camera);
// }

// void Mesh::draw(const Camera& camera)
// {
//     if (!m_visible)
//         return;

//     updateMvp(camera);

//     // for (const auto& va : m_vertexArrays)
//     //     Renderer::draw(m_shader, va);
// }

// void Mesh::updateTriangulationData()
// {
//     m_limitsX[0] = m_limitsY[0] = m_limitsZ[0] = std::numeric_limits<float>::max();
//     m_limitsX[1] = m_limitsY[1] = m_limitsZ[1] = std::numeric_limits<float>::min();

//     size_t i = 0;
//     for (const auto& meshData : m_meshData) {
//         for (const auto& vertices : meshData.vertices) {
//             m_triData->vertices[i] = glm::vec4{vertices.pos, 1.0f} * m_model;

//             m_limitsX[0] = std::min(m_limitsX[0], m_triData->vertices[i].x);
//             m_limitsX[1] = std::max(m_limitsX[1], m_triData->vertices[i].x);
//             m_limitsY[0] = std::min(m_limitsY[0], m_triData->vertices[i].y);
//             m_limitsY[1] = std::max(m_limitsY[1], m_triData->vertices[i].y);
//             m_limitsZ[0] = std::min(m_limitsZ[0], m_triData->vertices[i].z);
//             m_limitsZ[1] = std::max(m_limitsZ[1], m_triData->vertices[i++].z);
//         }
//     }

//     updateBoundingBox();
// }

// bool Mesh::rayIntersection(const std::tuple<glm::vec3, glm::vec3>& ray_world, glm::vec3& p_hit_world, float& minDist) const
// {
//     const auto& [v_ray_world, p_ray_world] = ray_world;

//     bool hit = false;
//     for (const auto& indices : m_bbData.indices) {
//         std::array<glm::vec3, 3> p_vertices_world;
//         for (size_t i = 0; i < 3; ++i)
//             p_vertices_world[i] = m_bbData.vertices[indices[i]];

//         const auto[n_tri_world, p_tri_world] = trianglePlane(p_vertices_world);
//         glm::vec3 p_hitTmp_world;                               
//         if (intersectionLinePlane(n_tri_world, p_tri_world, v_ray_world, p_ray_world, p_hitTmp_world) && 
//             pointInTriangle(n_tri_world, p_tri_world, p_vertices_world, p_hitTmp_world)) {   

//             hit = true;
//             break;
//         }
//     }

//     if (!hit)
//         return false;

//     return EntityOld::rayIntersection(ray_world, p_hit_world, minDist);
// }

// void Mesh::updateBoundingBox()
// {
//     m_bbData.vertices[0] = glm::vec3{m_limitsX[0], m_limitsY[0], m_limitsZ[0]};
//     m_bbData.vertices[1] = glm::vec3{m_limitsX[1], m_limitsY[0], m_limitsZ[0]};
//     m_bbData.vertices[2] = glm::vec3{m_limitsX[1], m_limitsY[1], m_limitsZ[0]};
//     m_bbData.vertices[3] = glm::vec3{m_limitsX[0], m_limitsY[1], m_limitsZ[0]};
//     m_bbData.vertices[4] = glm::vec3{m_limitsX[0], m_limitsY[0], m_limitsZ[1]};
//     m_bbData.vertices[5] = glm::vec3{m_limitsX[1], m_limitsY[0], m_limitsZ[1]};
//     m_bbData.vertices[6] = glm::vec3{m_limitsX[1], m_limitsY[1], m_limitsZ[1]};
//     m_bbData.vertices[7] = glm::vec3{m_limitsX[0], m_limitsY[1], m_limitsZ[1]};
// }

// void Mesh::drawBoundingBox(const Camera& camera)
// {
//     const glm::mat4 mvp = camera.getProjection() * camera.getView() * glm::transpose(m_model);

//     m_shaderBB->bind();
//     m_shaderBB->uploadMat4("u_mvp", mvp);
//     m_shaderBB->uploadVec4("u_color", {0.0f, 1.0f, 0.0, 1.0f});
//     // Renderer::draw(m_shaderBB, m_vertexArrayBB);
// }

// void Mesh::addNode(const aiScene* source, const aiNode* node, glm::mat4 t_node_world, const glm::mat4& t_mesh_world)
// {
//     glm::mat4 t_curr_world = convertMat4<aiMatrix4x4, glm::mat4>(node->mTransformation);
//     setMat4Translation(t_curr_world, 1000.0f*getMat4Translation(t_curr_world));

//     t_node_world = t_node_world*t_curr_world;

//     for (size_t i = 0; i < node->mNumMeshes; ++i) {
//         MeshData meshData;
//         const auto& meshSource = source->mMeshes[node->mMeshes[i]];

//         glm::vec4 color(0.4f, 0.4f, 0.4f, 1.0f);
//         if (source->mNumMaterials > 0 && meshSource->mMaterialIndex < source->mNumMaterials) {
//             uint32_t size = 4; 
//             source->mMaterials[meshSource->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE, glm::value_ptr(color), &size);
//         }
//         else 
//             LOG_WARN << "Material not valid: " << meshSource->mMaterialIndex;

//         meshData.vertices.resize(meshSource->mNumVertices);
//         for (size_t j = 0; j < meshData.vertices.size(); ++j) {
//             auto& vertex = meshData.vertices[j];
//             auto& vertSource = meshSource->mVertices[j];

//             glm::vec3 p_vertex_world;
//             p_vertex_world.x = 1000.0f*vertSource.x;
//             p_vertex_world.y = 1000.0f*vertSource.y;
//             p_vertex_world.z = 1000.0f*vertSource.z;  

//             p_vertex_world = glm::vec4(p_vertex_world, 1.0f) * t_node_world * t_mesh_world;

//             vertex.pos = p_vertex_world;
//             vertex.color = color;
//         }

//         meshData.indices.resize(meshSource->mNumFaces);
//         for (size_t j = 0; j < meshData.indices.size(); ++j) {
//             auto& indices = meshData.indices[j];
//             auto& indexSource = meshSource->mFaces[j];

//             if (indexSource.mNumIndices != 3)
//                 continue;

//             for (size_t k = 0; k < 3; ++k)
//                 indices[k] = indexSource.mIndices[k];
//         }

//         m_meshData.push_back(meshData);
//     }

//     for (size_t i = 0; i < node->mNumChildren; ++i)
//         addNode(source, node->mChildren[i], t_node_world, t_mesh_world);
// }

// void Mesh::createBuffers()
// {
//     m_vertexArrays.resize(m_meshData.size());
//     for (size_t i = 0; i < m_vertexArrays.size(); ++i) {
//         auto& vertices = m_meshData[i].vertices;
//         auto& indices = m_meshData[i].indices;
//         auto& vertexArray = m_vertexArrays[i];

//         std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
//         vertexBuffer->allocate(reinterpret_cast<float*>(vertices.data()), vertices.size() * sizeof(MeshData::Vertex)/sizeof(float));
//         BufferLayout layout = {
//             { ShaderDataType::Float3, "a_position" },
//             { ShaderDataType::Float4, "a_color" }
//         };
//         vertexBuffer->setLayout(layout);
//         vertexArray.addVertexBuffer(vertexBuffer);

//         std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
//         indexBuffer->allocate(indices.data()->data(), 3*indices.size());
//         vertexArray.setIndexBuffer(indexBuffer);
//     }

//     std::shared_ptr<VertexBuffer> vertexBuffer = std::make_shared<VertexBuffer>();
//     vertexBuffer->allocate(reinterpret_cast<const GLfloat*>(m_bbData.vertices.data()), m_bbData.vertices.size() * 3);
//     BufferLayout layout = {
//         { ShaderDataType::Float3, "a_position" }
//     };
//     vertexBuffer->setLayout(layout);
//     m_vertexArrayBB.addVertexBuffer(vertexBuffer);

//     std::shared_ptr<IndexBuffer> indexBuffer = std::make_shared<IndexBuffer>();
//     indexBuffer->allocate(reinterpret_cast<const GLushort*>(m_bbData.indices.data()), 3*m_bbData.indices.size());
//     m_vertexArrayBB.setIndexBuffer(indexBuffer);
// }