#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/ssao_pass.hpp>
#include <limitless/instances/abstract_instance.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/ms/blending.hpp>
#include <limitless/util/sorter.hpp>
#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>
#include <limitless/fx/effect_renderer.hpp>
#include "limitless/core/uniform/uniform.hpp"
#include "limitless/core/shader/shader_program.hpp"
#include <random>
#include <iostream>
#include <limitless/camera.hpp>
#include <limitless/pipeline/deferred/gbuffer_pass.hpp>
#include <limitless/pipeline/pipeline.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/uniform/uniform_setter.hpp>

using namespace Limitless;

SSAOPass::SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx)
    : PipelinePass(pipeline)
    , ssao {ctx} {
}

SSAOPass::SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx, glm::uvec2 frame_size)
    : PipelinePass(pipeline)
    , ssao {ctx, frame_size} {
}

void SSAOPass::draw([[maybe_unused]] Instances& instances, Context& ctx, [[maybe_unused]] const Assets& assets, [[maybe_unused]] const Camera& camera, [[maybe_unused]] UniformSetter& setter) {
    ssao.draw(ctx, assets, pipeline.get<DeferredFramebufferPass>().getDepth());
}

void SSAOPass::onFramebufferChange(glm::uvec2 size) {
    ssao.onFramebufferChange(size);
}

void SSAOPass::update(Scene &scene, Instances &instances, Context &ctx, const Camera &camera) {
    ssao.update(camera);
}

void SSAOPass::addSetter(Limitless::UniformSetter &setter) {
    setter.add([&](ShaderProgram& shader){
        shader.setUniform("ssao_texture", getResult());
    });
}

