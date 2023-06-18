#include <limitless/core/bindless_texture.hpp>
#include <limitless/core/context_state.hpp>

using namespace Limitless;

BindlessTexture::BindlessTexture(ExtensionTexture* _texture)
    : texture {_texture} {
}

void BindlessTexture::generateId() noexcept {
    texture->generateId();
}

void BindlessTexture::makeBindless() noexcept {
    if (!handle) {
        handle = glGetTextureHandleARB(texture->getId());
    }
}

BindlessTexture::~BindlessTexture() {
    makeNonResident();

    if (handle) {
        if (auto *state = ContextState::getState(glfwGetCurrentContext()); state) {
            state->texture_resident.erase(handle);
        }
    }
}

void BindlessTexture::makeResident() noexcept {
    makeBindless();
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (!state->texture_resident[handle]) {
            glMakeTextureHandleResidentARB(glGetTextureHandleARB(texture->getId()));
            state->texture_resident[handle] = true;
        }
    }
}

void BindlessTexture::makeNonResident() noexcept {
    if (!handle) return;
    if (auto* state = ContextState::getState(glfwGetCurrentContext()); state) {
        if (state->texture_resident[handle]) {
            glMakeTextureHandleNonResidentARB(handle);
            state->texture_resident[handle] = false;
        }
    }
}

BindlessTexture *BindlessTexture::clone() const {
    return new BindlessTexture(texture->clone());
}

void BindlessTexture::texStorage2D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec2 size) noexcept {
    makeNonResident();
    texture->texStorage2D(target, levels, internal_format, size);
}

void BindlessTexture::texStorage3D(GLenum target, GLsizei levels, GLenum internal_format, glm::uvec3 size) noexcept {
    makeNonResident();
    texture->texStorage3D(target, levels, internal_format, size);
}

void BindlessTexture::texImage2D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec2 size, bool border, const void *data) noexcept {
    makeNonResident();
    texture->texImage2D(target, levels, internal_format, format, type, size, border, data);
}

void BindlessTexture::texImage3D(GLenum target, GLsizei levels, GLenum internal_format, GLenum format, GLenum type, glm::uvec3 size, bool border, const void *data) noexcept {
    makeNonResident();
    texture->texImage3D(target, levels, internal_format, format, type, size, border, data);
}

void BindlessTexture::texSubImage2D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, glm::uvec2 size, GLenum format, GLenum type, const void *data) noexcept {
    makeNonResident();
    texture->texSubImage2D(target, levels, xoffset, yoffset, size, format, type, data);
}

void BindlessTexture::texSubImage3D(GLenum target, GLsizei levels, GLint xoffset, GLint yoffset, GLint zoffset, glm::uvec3 size, GLenum format, GLenum type, const void *data) noexcept {
    makeNonResident();
    texture->texSubImage3D(target, levels, xoffset, yoffset, zoffset, size, format, type, data);
}

void BindlessTexture::generateMipMap(GLenum target) noexcept {
    makeNonResident();
    texture->generateMipMap(target);
}

void BindlessTexture::bind(GLenum target, GLuint index) const noexcept {
    texture->bind(target, index);
}

void BindlessTexture::accept(TextureVisitor& visitor) noexcept {
    visitor.visit(*this);
}

BindlessTexture::BindlessTexture(BindlessTexture&& rhs) noexcept {
    std::swap(handle, rhs.handle);
    std::swap(texture, rhs.texture);
}

BindlessTexture& BindlessTexture::operator=(BindlessTexture&& rhs) noexcept {
    std::swap(handle, rhs.handle);
    std::swap(texture, rhs.texture);
    return *this;
}

BindlessTexture& BindlessTexture::setMinFilter(GLenum target, GLenum filter) {
    makeNonResident();
    texture->setMinFilter(target, filter);
    return *this;
}

BindlessTexture& BindlessTexture::setMagFilter(GLenum target, GLenum filter) {
    makeNonResident();
    texture->setMagFilter(target, filter);
    return *this;
}

BindlessTexture& BindlessTexture::setAnisotropicFilter(GLenum target, GLfloat value) {
    makeNonResident();
    texture->setAnisotropicFilter(target, value);
    return *this;
}

BindlessTexture& BindlessTexture::setBorderColor(GLenum target, float *color) {
    makeNonResident();
    texture->setBorderColor(target, color);
    return *this;
}

BindlessTexture& BindlessTexture::setWrapS(GLenum target, GLenum wrap) {
    makeNonResident();
    texture->setWrapS(target, wrap);
    return *this;
}

BindlessTexture& BindlessTexture::setWrapT(GLenum target, GLenum wrap) {
    makeNonResident();
    texture->setWrapT(target, wrap);
    return *this;
}

BindlessTexture& BindlessTexture::setWrapR(GLenum target, GLenum wrap) {
    makeNonResident();
    texture->setWrapR(target, wrap);
    return *this;
}

void BindlessTexture::compressedTexImage2D(GLenum target, GLint level, GLenum internal_format, glm::uvec2 size, bool border, const void *data, std::size_t bytes) noexcept {
    makeNonResident();
    texture->compressedTexImage2D(target, level, internal_format, size, border, data, bytes);
}

void BindlessTexture::compressedTexImage3D(GLenum target, GLint level, GLenum internal_format, glm::uvec3 size, bool border, const void *data, std::size_t bytes) noexcept {
    makeNonResident();
    texture->compressedTexImage3D(target, level, internal_format, size, border, data, bytes);
}

void BindlessTexture::compressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum internal_format, glm::uvec2 size, const void *data, std::size_t bytes) noexcept {
    makeNonResident();
    texture->compressedTexSubImage2D(target, level, xoffset, yoffset, internal_format, size, data, bytes);
}

void BindlessTexture::compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum internal_format, glm::uvec3 size, const void *data, std::size_t bytes) noexcept {
    makeNonResident();
    texture->compressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, internal_format, size, data, bytes);
}
