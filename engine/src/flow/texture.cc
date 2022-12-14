#include "flow/texture.h"

namespace uiwidgets {

Texture::Texture(int64_t id) : id_(id) {}

Texture::~Texture() = default;

TextureRegistry::TextureRegistry() = default;

void TextureRegistry::RegisterTexture(std::shared_ptr<Texture> texture) {
  if (!texture) {
    return;
  }
  mapping_[texture->Id()] = texture;
}

void TextureRegistry::UnregisterTexture(int64_t id) {
  auto found = mapping_.find(id);
  if (found == mapping_.end()) {
    return;
  }
  found->second->OnTextureUnregistered();
  mapping_.erase(found);
}

void TextureRegistry::OnGrContextCreated() {
  for (auto& it : mapping_) {
    it.second->OnGrContextCreated();
  }
}

void TextureRegistry::OnGrContextDestroyed() {
  for (auto& it : mapping_) {
    it.second->OnGrContextDestroyed();
  }
}

std::shared_ptr<Texture> TextureRegistry::GetTexture(int64_t id) {
  auto it = mapping_.find(id);
  return it != mapping_.end() ? it->second : nullptr;
}

}  // namespace uiwidgets
