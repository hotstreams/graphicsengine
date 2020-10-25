#pragma once

#include <material_builder.hpp>
#include <custom_material.hpp>

namespace GraphicsEngine {
    class CustomMaterialBuilder : public MaterialBuilder {
    private:
        std::unique_ptr<CustomMaterial> material;
    public:
        CustomMaterialBuilder() = default;
        ~CustomMaterialBuilder() override = default;

        CustomMaterialBuilder& setFragmentCode(std::string fs_code) noexcept;
        CustomMaterialBuilder& setVertexCode(std::string vs_code) noexcept;
        CustomMaterialBuilder& addUniform(std::string name, Uniform* uniform);

        CustomMaterialBuilder& create(std::string name);
        std::shared_ptr<Material> build(const ModelShaders& model_shaders = {ModelShader::Model}, const MaterialShaders& material_shaders = {MaterialShader::Default});
    };
}