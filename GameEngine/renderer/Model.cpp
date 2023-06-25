#include "Model.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace egkr
{
    Node::SharedPtr Node::create() noexcept
    {
        return std::shared_ptr<Node>(new Node());
    }

    void Node::set_parent(SharedPtr parent) noexcept
    {
        _parent = parent;
    }

    void Node::add_primitive(Primitive&& primitive) noexcept
    {
        _mesh.primtives.emplace_back(primitive);
    }

    void Node::add_child(Node::SharedPtr child) noexcept
    {
        _children.emplace_back(child);
    }

    Model::SharedPtr Model::create() noexcept
    {
        return std::shared_ptr<Model>(new Model());
    }

    void Model::load_image(const tinygltf::Model& tiny)
    {
        _images.resize(tiny.images.size());

        for (size_t i{ 0 }; i < _images.size(); ++i)
        {
            auto& image = tiny.images[i];
            if (image.component == 3)
            {

            }
        }
    }

    void Model::load_textures(const tinygltf::Model& model)
    {
        _imageIndex.resize(model.textures.size());

        for (int i = 0; i < _imageIndex.size(); ++i)
        {
            _imageIndex[i] = model.textures[i].source;
        }

    }

    void Model::load_materials(const tinygltf::Model& model)
    {
        _materials.resize(model.materials.size());

        for (size_t i{ 0 }; i < _materials.size(); ++i)
        {
            const auto& material = model.materials[i];
            if (material.values.contains("baseColorFactor"))
            {
                _materials[i].baseColourFactor = glm::make_vec4(material.values.at("baseColorFactor").ColorFactor().data());
            }

            if (material.values.contains("baseColorTexture"))
            {
                _materials[i].baseColourTextureIndex = material.values.at("baseColorTexture").TextureIndex();
            }

        }
    }

    void Model::load_nodes(const tinygltf::Node& gltfNode, const tinygltf::Model& model, Node::SharedPtr parent, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer)
    {
        auto node = Node::create();
        node->set_parent(parent);

        auto matrix = glm::mat4(1.f);

        if (gltfNode.translation.size() == 3)
        {
            matrix = glm::translate(matrix, glm::vec3(glm::make_vec3(gltfNode.translation.data())));
        }

        if (gltfNode.rotation.size() == 4)
        {
            glm::quat q = glm::make_quat(gltfNode.rotation.data());
            matrix *= glm::mat4(q);
        }

        if (gltfNode.scale.size() == 3)
        {
            matrix = glm::scale(matrix, glm::vec3(glm::make_vec3(gltfNode.scale.data())));
        }

        if (gltfNode.matrix.size() == 16)
        {
            matrix = glm::make_mat4x4(gltfNode.matrix.data());
        }

        if (!gltfNode.children.empty())
        {
            for (size_t i{ 0 }; i < gltfNode.children.size(); ++i)
            {
                load_nodes(model.nodes[gltfNode.children[i]], model, node, indexBuffer, vertexBuffer);
            }
        }

        if (gltfNode.mesh > -1)
        {
            const auto& mesh = model.meshes[gltfNode.mesh];
            for (const auto& primitive : mesh.primitives)
            {
                auto firstIndex = static_cast<uint32_t>(indexBuffer.size());
                auto vertexStart = static_cast<uint32_t>(vertexBuffer.size());
                uint32_t indexCount{ 0 };

                const float* positionBuffer{};
                const float* normalsBuffer{};
                const float* texCoordsBuffer{};
                size_t vertexCount{};

                {
                    if (primitive.attributes.contains("POSITION"))
                    {
                        const auto& accessor = model.accessors[primitive.attributes.find("POSITION")->second];
                        const auto& view = model.bufferViews[accessor.bufferView];
                        positionBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                        vertexCount = accessor.count;
                    }

                    if (primitive.attributes.contains("NORMAL"))
                    {
                        const auto& accessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                        const auto& view = model.bufferViews[accessor.bufferView];
                        normalsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
                    }

                    if (primitive.attributes.contains("TEXCOORD_0"))
                    {
                        const auto& accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                        const auto& view = model.bufferViews[accessor.bufferView];
                        texCoordsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));

                    }

                    for (size_t v{ 0 }; v < vertexCount; ++v)
                    {
                        Vertex vert{};
                        vert.pos = glm::vec4(glm::make_vec3(&positionBuffer[v * 3]), 1.F);
                        vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.F)));
                        vert.tex = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.F);
                        vert.colour = glm::vec3(1.F);
                        vertexBuffer.emplace_back(vert);
                    }
                }

                {
                    const auto& accessor = model.accessors[primitive.indices];
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[bufferView.buffer];

                    indexCount += static_cast<uint32_t>(accessor.count);

                    switch (accessor.componentType)
                    {
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
                    {
                        auto buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index{ 0 }; index < accessor.count; ++index)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
                    {
                        auto buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index{ 0 }; index < accessor.count; ++index)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
                    {
                        auto buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
                        for (size_t index{ 0 }; index < accessor.count; ++index)
                        {
                            indexBuffer.emplace_back(buf[index] + vertexStart);
                        }
                        break;
                    }
                    default:
                        ENGINE_ASSERT(false, "Index type {0} not supported", accessor.componentType);
                        return;
                    }
                }

                node->add_primitive({ firstIndex, indexCount, primitive.material });
            }

        }

        if (parent != nullptr)
        {
            parent->add_child(node);
        }
        else
        {
            _nodes.emplace_back(node);
        }

    }

    void Model::draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout) const
    {
        vk::DeviceSize offsets[1] = { 0 };
        commandBuffer.bindVertexBuffers(0, vertices.buffer, offsets);
        commandBuffer.bindIndexBuffer(indices.buffer, 0, vk::IndexType::eUint32);

        for (auto& node : _nodes)
        {
            draw_node(commandBuffer, pipelineLayout, node);
        }
    }

    void Model::draw_node(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, Node::SharedPtr node) const
    {
        if (node->has_primitives())
        {
            auto nodeMatrix = node->get_matrix();
            auto currentParent = node->get_parent();

            while (currentParent)
            {
                nodeMatrix = currentParent->get_matrix() * nodeMatrix;
                currentParent = currentParent->get_parent();
            }

            commandBuffer.pushConstants(pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &nodeMatrix);

            for (const auto& primitive : node->get_primitives())
            {
                if (primitive.indexCount > 0 && !_imageIndex.empty())
                {
                    auto texture = _imageIndex[_materials[primitive.materialIndex].baseColourTextureIndex];
                    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 1, _images[texture].descriptor, nullptr);
                }
                    commandBuffer.drawIndexed(primitive.indexCount, 1, primitive.startIndex, 0, 0);
            }
        }
        for (const auto& child : node->get_children())
        {
            draw_node(commandBuffer, pipelineLayout, child);
        }
    }

    Model::SharedPtr Model::createGLTF(std::filesystem::path filepath) noexcept
    {
        tinygltf::TinyGLTF loader;
        std::string err{};
        std::string warn{};

        tinygltf::Model gltfModel{};
        bool loadSuccessful = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, filepath.string());
        if (!warn.empty())
        {
            ENGINE_ASSERT(false, "Failed to create model, {0}", warn);
        }
        if (!err.empty())
        {
            ENGINE_ASSERT(false, "Failed to create model, {0}", err);
        }

        if (!loadSuccessful)
        {
            ENGINE_ASSERT(false, "Failed to load model");
        }

        std::vector<uint32_t> indexBuffer{};
        std::vector<Vertex> vertexBuffer{};

        auto model = create();
        model->load_image(gltfModel);
        model->load_textures(gltfModel);
        model->load_materials(gltfModel);

        const auto& scene = gltfModel.scenes[0];
        for (size_t i{ 0 }; i < scene.nodes.size(); ++i)
        {
            const auto& n = gltfModel.nodes[scene.nodes[i]];
            model->load_nodes(n, gltfModel, nullptr, indexBuffer, vertexBuffer);
        }

        auto vertexBufferSize = vertexBuffer.size() * sizeof(Vertex);
        auto indexBufferSize = indexBuffer.size() * sizeof(uint32_t);

        BufferProperties vertexStagingProperties{ vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent };
        Buffer vertexStaging{vertexStagingProperties};
        vertexStaging.map(vertexBuffer.data());

        BufferProperties indexStagingProperties{ indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent };
        Buffer indexStaging{indexStagingProperties};
        indexStaging.map(indexBuffer.data());

		BufferProperties vertexBufferProperties =
		{
			.size = vertexBufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

        model->vertices = Buffer(vertexBufferProperties);
		model->vertices.copy(vertexStaging);

		BufferProperties indexBufferProperties =
		{
			.size = indexBufferSize,
			.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			.memoryProperties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		};

        model->indices = Buffer(indexBufferProperties);
		model->indices.copy(indexStaging);

		vertexStaging.destroy();
		indexStaging.destroy();
        
        return model;
    }
}