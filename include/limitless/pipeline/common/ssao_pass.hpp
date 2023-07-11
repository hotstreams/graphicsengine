#pragma once

#include <limitless/pipeline/pipeline_pass.hpp>
#include <limitless/core/framebuffer.hpp>
#include <limitless/postprocessing/ssao.hpp>

namespace Limitless {
    class SSAOPass final : public PipelinePass {
    private:
        SSAO ssao;
    public:
        SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx);
        SSAOPass(Pipeline& pipeline, ContextEventObserver& ctx, glm::uvec2 frame_size);

        std::shared_ptr<Texture> getResult() override { return ssao.getFramebuffer().get(FramebufferAttachment::Color0).texture; }

        void addSetter(Limitless::UniformSetter &setter) override;

        void update(Limitless::Scene &scene, Limitless::Instances &instances, Limitless::Context &ctx, const Limitless::Camera &camera) override;

        void draw(Instances& instances, Context& ctx, const Assets& assets, const Camera& camera, UniformSetter& setter) override;

        void onFramebufferChange(glm::uvec2 size) override;
    };
}
