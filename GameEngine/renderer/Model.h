#pragma once

#include "RendererCore.h"

#include "tiny_gltf.h"

#include <memory>
#include <filesystem>

namespace egkr
{
    struct Primitive
    {
        uint32_t startIndex{};
        uint32_t indexCount{};
        int32_t materialIndex{};
    };

    struct Mesh
    {
        std::vector<Primitive> primtives{};
    };

    struct Material
    {
        glm::vec4 baseColourFactor{ 1.f };
        uint32_t baseColourTextureIndex{};
    };

    struct Image
    {
        Texture2D texture{};
        vk::DescriptorSet descriptor{};
    };

    class Node : public  std::enable_shared_from_this<Node>
    {
    public:
        using SharedPtr = std::shared_ptr<Node>;
        static SharedPtr create() noexcept;

        void set_parent(SharedPtr parent) noexcept;
        void add_primitive(Primitive&& primitive) noexcept;
        void add_child(Node::SharedPtr child) noexcept;

        [[nodiscard]] bool has_primitives() const noexcept
        {
            return !_mesh.primtives.empty();
        }

        [[nodiscard]] auto get_matrix() const noexcept
        {
            return _matrix;
        }

        [[nodiscard]] auto get_parent() const noexcept
        {
            return _parent;
        }

        [[nodiscard]] auto get_primitives() const noexcept
        {
            return _mesh.primtives;
        }

        [[nodiscard]] auto get_children() const noexcept
        {
            return _children;
        }

    private:
        Node() = default;
        SharedPtr _parent{};
        std::vector<SharedPtr> _children{};
        Mesh _mesh{};
        glm::mat4 _matrix{1.F};
    };

    class Model : public std::enable_shared_from_this<Model>
    {
    public:
        using SharedPtr = std::shared_ptr<Model>;
        static SharedPtr create() noexcept;
        static SharedPtr createGLTF(std::filesystem::path filepath) noexcept;
        void draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout) const;

    private:
        Model() = default;
        void draw_node(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout, Node::SharedPtr node) const;

        void load_image(const tinygltf::Model& model);
        void load_textures(const tinygltf::Model& model);
        void load_materials(const tinygltf::Model& model);
        void load_nodes(const tinygltf::Node& gltfNode, const tinygltf::Model& model, Node::SharedPtr node, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer);
    private:
        std::vector<Node::SharedPtr> _nodes{};
        std::vector<Image> _images{};
        std::vector<int32_t> _imageIndex{};
        std::vector<Material> _materials{};

        Buffer indices{};
        Buffer vertices{};

    };
}