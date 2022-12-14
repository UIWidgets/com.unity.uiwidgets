#include "embedder_platform_message_response.h"

#include "flutter/fml/make_copyable.h"

namespace uiwidgets {

EmbedderPlatformMessageResponse::EmbedderPlatformMessageResponse(
    fml::RefPtr<fml::TaskRunner> runner, const Callback& callback)
    : runner_(std::move(runner)), callback_(callback) {}

EmbedderPlatformMessageResponse::~EmbedderPlatformMessageResponse() = default;

// |PlatformMessageResponse|
void EmbedderPlatformMessageResponse::Complete(
    std::unique_ptr<fml::Mapping> data) {
  if (!data) {
    CompleteEmpty();
    return;
  }

  runner_->PostTask(
      fml::MakeCopyable([data = std::move(data), callback = callback_]() {
        callback(data->GetMapping(), data->GetSize());
      }));
}

// |PlatformMessageResponse|
void EmbedderPlatformMessageResponse::CompleteEmpty() {
  Complete(std::make_unique<fml::NonOwnedMapping>(nullptr, 0u));
}

}  // namespace uiwidgets
