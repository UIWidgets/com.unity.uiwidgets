#pragma once

// OpenGL ES and EGL includes
//#include <EGL/eglext.h>
//#include <EGL/eglplatform.h>
//#include <EGL/egl.h>
#include <GL/gl.h>
#include <GL/glx.h>

#ifdef Status
#undef Status
#endif

#ifdef Bool
#undef Bool
#endif

#ifdef Success
#undef Success
#endif

#include <cstdint>

#include "Unity/IUnityInterface.h"
#include "flutter/fml/macros.h"

#include <include/gpu/GrContext.h>
#include <src/gpu/gl/GrGLDefines.h>

#include "cassert"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#include "include/core/SkTextBlob.h"
#include "include/effects/SkPerlinNoiseShader.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/gl/GrGLTypes.h"

#include "Unity/IUnityGraphics.h"

//#include <GL/glx.h>

namespace uiwidgets
{

  class UnitySurfaceManager
  {
  public:
    static void GetUnityContext();

    UnitySurfaceManager(IUnityInterfaces *unity_interfaces);
    ~UnitySurfaceManager();

    GLuint CreateRenderSurface(void *native_texture_ptr);
    void DestroyRenderSurface();

    bool ClearCurrent();

    bool MakeCurrent(GLXDrawable drawable);

    bool MakeResourceCurrent();

    FML_DISALLOW_COPY_AND_ASSIGN(UnitySurfaceManager);

  private:
    bool Initialize(IUnityInterfaces *unity_interfaces);
    void CleanUp();

    GLXContext gl_context_;
    GLXContext gl_resource_context_;
    XVisualInfo* gl_config_;

    bool initialize_succeeded_;

    GLuint fbo_ = 0;
  };

} // namespace uiwidgets
