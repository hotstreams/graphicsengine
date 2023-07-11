#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/shader_storage.hpp>
#include "limitless/core/shader/shader_program.hpp"
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/core/skeletal_stream.hpp>
#include <iostream>

using namespace Limitless;

constexpr auto SKELETAL_BUFFER_NAME = "bone_buffer";

void SkeletalInstance::initializeBuffer() {
    BufferBuilder builder;
    bone_buffer = builder.setTarget(Buffer::Type::ShaderStorage)
            .setUsage(Buffer::Usage::DynamicDraw)
            .setAccess(Buffer::MutableAccess::WriteOrphaning)
            .setData(bone_transform.data())
            .setDataSize(bone_transform.size() * sizeof(glm::mat4))
            .build();
}

void SkeletalInstance::updateAnimationFrame() {
    if (!animation || paused) {
        return;
    }

    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    auto& bones = skeletal.getBones();
    const Animation& anim = *animation;

    const auto current_time = std::chrono::steady_clock::now();
    if (last_time == std::chrono::time_point<std::chrono::steady_clock>()) {
        last_time = current_time;
    }
    const auto delta_time = current_time - last_time;
    animation_duration += delta_time;
    last_time = current_time;
    const auto animation_time = glm::mod(animation_duration.count() * anim.tps, anim.duration);

    std::function<void(const Tree<uint32_t>&, const glm::mat4&)> node_traversal;
    node_traversal = [&](const Tree<uint32_t>& node, const glm::mat4& parent_mat) {
        auto anim_node = findAnimationNode(bones[*node]);

        auto local_transform = !bones[*node].isFake() ? bones[*node].node_transform : glm::mat4(1.f);

        if (anim_node) {
            glm::vec3 scale = anim_node->scalingLerp(animation_time);
            glm::vec3 position = anim_node->positionLerp(animation_time);
            auto rotation = anim_node->rotationLerp(animation_time);

            auto translate = glm::translate(glm::mat4(1.f), position);
            auto rotate = glm::mat4_cast(rotation);
            auto scale_mat = glm::scale(glm::mat4(1.f), scale);

            local_transform =  translate * rotate * scale_mat;
        }

        auto transform = parent_mat * local_transform;
        bone_transform[*node] = skeletal.getGlobalInverseMatrix() * transform * bones[*node].offset_matrix;

        for (const auto& n : node) {
            node_traversal(n, transform);
        }
    };

    try {
        node_traversal(skeletal.getSkeletonTree(), glm::mat4(1.f));
    } catch (const std::exception& e) {
        throw std::runtime_error("Wrong TPS/duration. " + std::string(e.what()));
    }

    bone_buffer->mapData(bone_transform.data(), sizeof(glm::mat4) * bone_transform.size());
}

const AnimationNode* SkeletalInstance::findAnimationNode(const Bone& bone) const noexcept {
    if (animation) {
        for (auto& node : animation->nodes) {
            if (&node.bone == &bone) {
                return &node;
            }
        }
    }

    return nullptr;
}

SkeletalInstance::SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position)
    : ModelInstance(InstanceType::Skeletal, std::move(m), position) {
    //TODO: check type
    auto& skeletal = dynamic_cast<SkeletalModel&>(*model);

    bone_transform.resize(skeletal.getBones().size(), glm::mat4(1.0f));
    initializeBuffer();
}

SkeletalInstance::SkeletalInstance(const SkeletalInstance& rhs) noexcept
    : ModelInstance {rhs}
    , SocketAttachment {rhs}
    , bone_transform {rhs.bone_transform}
    , animation {rhs.animation}
    , paused {rhs.paused}
    , last_time {rhs.last_time}
    , animation_duration {rhs.animation_duration} {
    initializeBuffer();
}


std::unique_ptr<AbstractInstance> SkeletalInstance::clone() noexcept {
    return std::make_unique<SkeletalInstance>(*this);
}

void SkeletalInstance::update(Context& context, const Camera& camera) {
    updateAnimationFrame();

    SocketAttachment::updateSocketAttachments();

    ModelInstance::update(context, camera);
}

SkeletalInstance& SkeletalInstance::play(const std::string& name) {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();

    const auto found = std::find_if(animations.begin(), animations.end(), [&] (const auto& anim) { return name == anim.name; });
    if (found == animations.end()) {
        throw no_such_animation("with name " + name);
    } else {
        animation = &(*found);
        animation_duration = std::chrono::seconds(0);
        last_time = std::chrono::time_point<std::chrono::steady_clock>();
    }

    return *this;
}

SkeletalInstance& SkeletalInstance::pause() noexcept {
    paused = true;
    return *this;
}

SkeletalInstance& SkeletalInstance::resume() noexcept {
    paused = false;
    return *this;
}

SkeletalInstance& SkeletalInstance::stop() noexcept {
    animation = nullptr;
    return *this;
}

void SkeletalInstance::draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_setter) {
    if (hidden) {
        return;
    }

	bone_buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SKELETAL_BUFFER_NAME));

    // iterates over all meshes
    for (auto& [name, mesh] : meshes) {
        mesh.draw(ctx, assets, pass, shader_type, final_matrix, blending, uniform_setter);
    }

    bone_buffer->fence();
}

glm::vec3 SkeletalInstance::getSkinnedVertexPosition(const std::shared_ptr<AbstractMesh>& mesh, size_t vertex_index) const {
    const auto& skinned_mesh = dynamic_cast<SkinnedVertexStream<VertexNormalTangent>&>(dynamic_cast<Mesh&>(*mesh).getVertexStream());

    const auto& bone_weight = skinned_mesh.getBoneWeights().at(vertex_index);
    const auto& vertex = skinned_mesh.getVertices().at(vertex_index);

    auto transform = bone_transform[bone_weight.bone_index[0]] * bone_weight.weight[0];
    transform     += bone_transform[bone_weight.bone_index[1]] * bone_weight.weight[1];
    transform     += bone_transform[bone_weight.bone_index[2]] * bone_weight.weight[2];
    transform     += bone_transform[bone_weight.bone_index[3]] * bone_weight.weight[3];

    auto matrix = final_matrix;
    matrix *= transform;

    return matrix * glm::vec4(vertex.position, 1.0);
}

const std::vector<Animation>& SkeletalInstance::getAllAnimations() const noexcept {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& animations = skeletal.getAnimations();
    return animations;
}

const std::vector<Bone>& SkeletalInstance::getAllBones() const noexcept {
    const auto& skeletal = dynamic_cast<SkeletalModel&>(*model);
    const auto& bones = skeletal.getBones();
    return bones;
}


