#pragma once

#include "flutter/fml/macros.h"
#include "shell/gpu/gpu_surface_gl.h"
#include "shell/platform/embedder/embedder_external_view_embedder.h"
#include "shell/platform/embedder/embedder_surface.h"

namespace uiwidgets {

class EmbedderSurfaceGL final : public EmbedderSurface,
                                public GPUSurfaceGLDelegate {
 public:
  struct GLDispatchTable {
    std::function<bool(void)> gl_make_current_callback;           // required
    std::function<bool(void)> gl_clear_current_callback;          // required
    std::function<bool(void)> gl_present_callback;                // required
    std::function<intptr_t(void)> gl_fbo_callback;                // required
    std::function<bool(void)> gl_make_resource_current_callback;  // optional
    std::function<SkMatrix(void)>
        gl_surface_transformation_callback;              // optional
    std::function<void*(const char*)> gl_proc_resolver;  // optional
  };

  EmbedderSurfaceGL(
      GLDispatchTable gl_dispatch_table, bool fbo_reset_after_present,
      std::unique_ptr<EmbedderExternalViewEmbedder> external_view_embedder);

  ~EmbedderSurfaceGL() override;

 private:
  bool valid_ = false;
  GLDispatchTable gl_dispatch_table_;
  bool fbo_reset_after_present_;

  std::unique_ptr<EmbedderExternalViewEmbedder> external_view_embedder_;

  // |EmbedderSurface|
  bool IsValid() const override;

  // |EmbedderSurface|
  std::unique_ptr<Surface> CreateGPUSurface() override;

  // |EmbedderSurface|
  sk_sp<GrContext> CreateResourceContext() const override;

  // |GPUSurfaceGLDelegate|
  bool GLContextMakeCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextClearCurrent() override;

  // |GPUSurfaceGLDelegate|
  bool GLContextPresent() override;

  // |GPUSurfaceGLDelegate|
  intptr_t GLContextFBO() const override;

  // |GPUSurfaceGLDelegate|
  bool GLContextFBOResetAfterPresent() const override;

  // |GPUSurfaceGLDelegate|
  SkMatrix GLContextSurfaceTransformation() const override;

  // |GPUSurfaceGLDelegate|
  ExternalViewEmbedder* GetExternalViewEmbedder() override;

  // |GPUSurfaceGLDelegate|
  GLProcResolver GetGLProcResolver() const override;

  FML_DISALLOW_COPY_AND_ASSIGN(EmbedderSurfaceGL);
};

}  // namespace uiwidgets
