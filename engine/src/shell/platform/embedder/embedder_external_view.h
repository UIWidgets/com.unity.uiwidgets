#pragma once

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include "flow/embedded_views.h"
#include "flutter/fml/hash_combine.h"
#include "flutter/fml/macros.h"
#include "include/core/SkPictureRecorder.h"
#include "shell/common/canvas_spy.h"
#include "shell/platform/embedder/embedder_render_target.h"

namespace uiwidgets {

class EmbedderExternalView {
 public:
  using PlatformViewID = int64_t;
  struct ViewIdentifier {
    std::optional<PlatformViewID> platform_view_id;

    ViewIdentifier() {}

    ViewIdentifier(PlatformViewID view_id) : platform_view_id(view_id) {}

    struct Hash {
      constexpr std::size_t operator()(const ViewIdentifier& desc) const {
        if (!desc.platform_view_id.has_value()) {
          return fml::HashCombine();
        }

        return fml::HashCombine(desc.platform_view_id.value());
      }
    };

    struct Equal {
      constexpr bool operator()(const ViewIdentifier& lhs,
                                const ViewIdentifier& rhs) const {
        return lhs.platform_view_id == rhs.platform_view_id;
      }
    };
  };

  struct RenderTargetDescriptor {
    ViewIdentifier view_identifier;
    SkISize surface_size;

    RenderTargetDescriptor(ViewIdentifier p_view_identifier,
                           SkISize p_surface_size)
        : view_identifier(p_view_identifier), surface_size(p_surface_size) {}

    struct Hash {
      constexpr std::size_t operator()(
          const RenderTargetDescriptor& desc) const {
        return fml::HashCombine(desc.surface_size.width(),
                                desc.surface_size.height(),
                                ViewIdentifier::Hash{}(desc.view_identifier));
      }
    };

    struct Equal {
      bool operator()(const RenderTargetDescriptor& lhs,
                      const RenderTargetDescriptor& rhs) const {
        return lhs.surface_size == rhs.surface_size &&
               ViewIdentifier::Equal{}(lhs.view_identifier,
                                       rhs.view_identifier);
      }
    };
  };

  using ViewIdentifierSet =
      std::unordered_set<ViewIdentifier, ViewIdentifier::Hash,
                         ViewIdentifier::Equal>;

  using PendingViews =
      std::unordered_map<ViewIdentifier, std::unique_ptr<EmbedderExternalView>,
                         ViewIdentifier::Hash, ViewIdentifier::Equal>;

  EmbedderExternalView(const SkISize& frame_size,
                       const SkMatrix& surface_transformation);

  EmbedderExternalView(const SkISize& frame_size,
                       const SkMatrix& surface_transformation,
                       ViewIdentifier view_identifier,
                       std::unique_ptr<EmbeddedViewParams> params);

  ~EmbedderExternalView();

  bool IsRootView() const;

  bool HasPlatformView() const;

  bool HasEngineRenderedContents() const;

  ViewIdentifier GetViewIdentifier() const;

  const EmbeddedViewParams* GetEmbeddedViewParams() const;

  RenderTargetDescriptor CreateRenderTargetDescriptor() const;

  SkCanvas* GetCanvas() const;

  SkISize GetRenderSurfaceSize() const;

  bool Render(const EmbedderRenderTarget& render_target);

 private:
  const SkISize render_surface_size_;
  const SkMatrix surface_transformation_;
  ViewIdentifier view_identifier_;
  std::unique_ptr<EmbeddedViewParams> embedded_view_params_;
  std::unique_ptr<SkPictureRecorder> recorder_;
  std::unique_ptr<CanvasSpy> canvas_spy_;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderExternalView);
};

}  // namespace uiwidgets
