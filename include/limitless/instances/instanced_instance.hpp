#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>

namespace Limitless {
    class InstancedInstance : public Instance {
    protected:
        // contains all instanced models
        std::vector<std::shared_ptr<ModelInstance>> instances;

        // contains instances to be drawn in current frame
        std::vector<std::shared_ptr<ModelInstance>> visible_instances;

        // contains model matrices for each ModelInstance
        std::shared_ptr<Buffer> buffer;

        std::vector<Data> current_instance_data;

        void updateInstanceBuffer();
    public:
        InstancedInstance();
        ~InstancedInstance() override = default;

        InstancedInstance(const InstancedInstance& rhs);
        InstancedInstance(InstancedInstance&&) noexcept = default;

        std::unique_ptr<Instance> clone() noexcept override;

        void add(const std::shared_ptr<ModelInstance>& instance);
        void remove(uint64_t id);

        void update(const Camera &camera) override;

        auto& getInstances() noexcept { return instances; }
        auto& getVisibleInstances() noexcept { return visible_instances; }
        auto& getBuffer() noexcept { return buffer ; }

        /**
         *  Sets visible instances to specified subset
         */
        void setVisible(const std::vector<std::shared_ptr<ModelInstance>>& visible);
    };
}