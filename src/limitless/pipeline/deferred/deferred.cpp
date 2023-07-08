#include <limitless/pipeline/deferred/deferred.hpp>

#include <limitless/core/context_observer.hpp>
#include <limitless/ms/blending.hpp>

#include <limitless/pipeline/common/sceneupdate_pass.hpp>
#include <limitless/pipeline/deferred/effectupdate_pass.hpp>
#include <limitless/pipeline/common/shadow_pass.hpp>
#include <limitless/pipeline/common/skybox_pass.hpp>
#include <limitless/pipeline/forward/postprocessing_pass.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <limitless/pipeline/deferred/deferred_lighting_pass.hpp>
#include <limitless/pipeline/deferred/translucent_pass.hpp>
#include <limitless/pipeline/deferred/gbuffer_pass.hpp>
#include <limitless/pipeline/deferred/depth_pass.hpp>
#include <limitless/scene.hpp>
#include <limitless/pipeline/common/bloom_pass.hpp>
#include <limitless/pipeline/deferred/composite_pass.hpp>
#include <limitless/pipeline/common/ssao_pass.hpp>
#include <limitless/pipeline/common/fxaa_pass.hpp>
#include <limitless/pipeline/common/quad_pass.hpp>
#include <limitless/pipeline/common/render_debug_pass.hpp>
#include <limitless/core/texture/texture_builder.hpp>
#include <limitless/pipeline/common/dof_pass.hpp>
#include <limitless/pipeline/common/ssr_pass.hpp>
#include <limitless/pipeline/common/outline_pass.hpp>
#include <iostream>

using namespace Limitless;

Deferred::Deferred(ContextEventObserver& ctx, glm::uvec2 size, const RenderSettings& settings, RenderTarget& _target)
    : Pipeline {size, _target} {
    build(ctx, settings);
}

void Deferred::update(ContextEventObserver& ctx, const RenderSettings& settings) {
    clear();
    build(ctx, settings);
}

void Deferred::build(ContextEventObserver& ctx, const RenderSettings& settings) {
    /*
     * Updates scene and GPU scene buffer
     */
    add<SceneUpdatePass>(ctx);

    auto& fx = add<EffectUpdatePass>(ctx);

    /*
     * Fills shadow maps
     */
    if (settings.cascade_shadow_maps) {
        add<DirectionalShadowPass>(ctx, settings, fx.getRenderer());
    }

    /*
     * Prepares GBUFFER
     */
    add<DeferredFramebufferPass>(size);

    /*
     * Renders opaque objects to DeferredFramebufferPass's depth buffer
     */
    add<DepthPass>(fx.getRenderer());

    /*
     * Fills GBUFFER with opaque objects
     */
    add<GBufferPass>(fx.getRenderer());

    /*
     * Renders skybox
     */
    add<SkyboxPass>();

    /*
     * TODO: ref
     */
    if (settings.screen_space_ambient_occlusion) {
        add<SSAOPass>(ctx, size);
    }

    add<SSRPass>(ctx, size);

    /*
     * Makes lighting calculations (with AO) from GBUFFER and renders result in another framebuffer
     */
    add<DeferredLightingPass>(size);

    /*
     * Copies lighting result and renders translucent objects on-top of it
     */
    add<TranslucentPass>(fx.getRenderer(), size, get<DeferredFramebufferPass>().getDepth());

    /*
     *  Applies bloom to previous result image
     */
    if (settings.bloom) {
        add<BloomPass>(size);
    }

    /*
     *  Combines shaded translucent result and bloom
     */
    add<CompositePass>(size);

    //TODO: restore
//    add<OutlinePass>();

//    if (settings.fast_approximate_antialiasing) {
//        add<FXAAPass>(size);
//    }

    //TODO: refactor move to postprocessing
//    if (settings.depth_of_field) {
//        add<DoFPass>(ctx);
//    }

    add<ScreenPass>(*target);
}
