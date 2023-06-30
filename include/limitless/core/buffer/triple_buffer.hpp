#pragma once

#include <limitless/core/buffer/buffer.hpp>
#include <array>
#include <memory>

namespace Limitless {
    class TripleBuffer : public Buffer {
    private:
        std::array<std::shared_ptr<Buffer>, 3> buffers;
        mutable uint8_t curr_index;
    public:
        explicit TripleBuffer(decltype(buffers) buffers) noexcept;

        void clearSubData(GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void* data) const noexcept override;
        void clearData(GLenum internalformat, GLenum format, GLenum type, const void* data) const noexcept override;
        void bufferSubData(GLintptr offset, size_t sub_size, const void* data) const noexcept override;
        void mapData(const void* data, size_t data_size) override;

        void bindBufferRangeAs(Type target, GLuint index, GLintptr offset) const noexcept override;
        void bindBufferRange(GLuint index, GLintptr offset) const noexcept override;
        void bindBaseAs(Type target, GLuint index) const noexcept override;
        void bindBase(GLuint index) const noexcept override;
        void bindAs(Type target) const noexcept override;
        void bind() const noexcept override;

        void fence() noexcept override;
        void waitFence() noexcept override;

        [[nodiscard]] void* mapBufferRange(GLintptr offset, GLsizeiptr size) const override;
        void unmapBuffer() const noexcept override;

        [[nodiscard]] GLuint getId() const noexcept override;
        [[nodiscard]] Type getType() const noexcept override;
        [[nodiscard]] size_t getSize() const noexcept override;
        [[nodiscard]] const std::variant<Usage, Storage>& getUsageFlags() const noexcept override;
        [[nodiscard]] const std::variant<MutableAccess, ImmutableAccess>& getAccess() const noexcept override;
    };
}