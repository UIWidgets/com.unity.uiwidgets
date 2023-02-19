
#include "unity_surface_manager.h"

#include <flutter/fml/logging.h>
#include <EGL/egl.h>

#include <GL/gl.h>
#include <stdio.h>
//#include <GL/glx.h>
//#include <GL/glext.h>

#include "src/shell/common/shell_io_manager.h"
#include "src/shell/gpu/gpu_surface_delegate.h"
#include "src/shell/gpu/gpu_surface_gl_delegate.h"
namespace uiwidgets
{

  static EGLDisplay egl_display_;
  static EGLContext egl_unity_context_;



  template <class T>
  using EGLResult = std::pair<bool, T>;

  UnitySurfaceManager::UnitySurfaceManager(IUnityInterfaces *unity_interfaces)
      : egl_context_(EGL_NO_CONTEXT),
        egl_resource_context_(EGL_NO_CONTEXT),
        unity_previous_gl_context_(EGL_NO_CONTEXT)
  {
    initialize_succeeded_ = Initialize(unity_interfaces);
  }

  UnitySurfaceManager::~UnitySurfaceManager() { CleanUp(); }

  GLuint UnitySurfaceManager::CreateRenderSurface(void *native_texture_ptr)
  {
    GLint old_framebuffer_binding;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &old_framebuffer_binding);
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    GLuint gltex = (GLuint)(size_t)(native_texture_ptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gltex, 0);
    FML_CHECK(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    glBindFramebuffer(GL_FRAMEBUFFER, old_framebuffer_binding);

    return fbo_;
  }

  void UnitySurfaceManager::DestroyRenderSurface()
  {
    FML_DCHECK(fbo_ != 0);
    glDeleteFramebuffers(1, &fbo_);
    fbo_ = 0;
  }

  bool UnitySurfaceManager::ClearCurrent()
  {
    //THIS API will crash the Editor since it might mistakenly clear the Unity's internal context
    //We should find out a way to fix it though
    bool suc = eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                          EGL_NO_CONTEXT) == EGL_TRUE;
    
    if (unity_previous_gl_context_ != EGL_NO_CONTEXT)
    {
      eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE, unity_previous_gl_context_);
      unity_previous_gl_context_ = EGL_NO_CONTEXT;
    }

    return suc;
  }

  bool UnitySurfaceManager::MakeCurrent(const EGLSurface surface)
  {
    EGLContext _pre_context = eglGetCurrentContext();
    if (_pre_context != EGL_NO_CONTEXT && _pre_context != egl_context_ &&
      _pre_context != egl_resource_context_)
    {
      unity_previous_gl_context_ = _pre_context;
    }

    return eglMakeCurrent(egl_display_, surface, surface, egl_context_) ==
           EGL_TRUE;
  }

  bool UnitySurfaceManager::MakeResourceCurrent()
  {
    EGLContext _pre_context = eglGetCurrentContext();
    if (_pre_context != EGL_NO_CONTEXT && _pre_context != egl_context_ &&
      _pre_context != egl_resource_context_)
    {
      unity_previous_gl_context_ = _pre_context;
    }

    return eglMakeCurrent(egl_display_, EGL_NO_SURFACE, EGL_NO_SURFACE,
                          egl_resource_context_) == EGL_TRUE;
  }

  static EGLResult<EGLConfig> ChooseEGLConfiguration(EGLDisplay display)
  {
    EGLint attributes[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT, //EGL_STREAM_BIT_KHR, //EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_CONFORMANT, EGL_OPENGL_BIT,
        EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        // EGL_DEPTH_SIZE, 24,
        // EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    EGLint config_count = 0;
    EGLConfig egl_config = nullptr;

    

    if (eglChooseConfig(display, attributes, &egl_config, 1, &config_count) !=
        EGL_TRUE)
    {
      EGLint errCode = eglGetError();

      printf("%d", errCode);

      return {false, nullptr};
    }

    bool success = config_count > 0 && egl_config != nullptr;

    return {success, success ? egl_config : nullptr};
  }

  static EGLResult<EGLSurface> CreateContext(EGLDisplay display,
                                             EGLConfig config,
                                             EGLContext share = EGL_NO_CONTEXT)
  {
    EGLint attributes[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    EGLContext context = eglCreateContext(display, config, share, attributes);

    return {context != EGL_NO_CONTEXT, context};
  }

  void UnitySurfaceManager::GetUnityContext()
  {
    if(egl_unity_context_ != nullptr){
      return;
    }
    egl_display_ = eglGetCurrentDisplay();
    egl_unity_context_ = eglGetCurrentContext();
    FML_CHECK(egl_display_ != EGL_NO_DISPLAY)
        << "Renderer type is invalid";
  }

  bool UnitySurfaceManager::Initialize(IUnityInterfaces *unity_interfaces)
  {
    FML_CHECK(egl_display_ != EGL_NO_DISPLAY)
        << "Renderer type is invalid";

    // Initialize the display connection.
    FML_CHECK(eglInitialize(egl_display_, nullptr, nullptr) == EGL_TRUE)
        << "Renderer type is invalid";

    auto valid_ = true;

    bool success = false;

    std::tie(success, egl_config_) = ChooseEGLConfiguration(egl_display_);
    FML_CHECK(success) << "Could not choose an EGL configuration.";

    std::tie(success, egl_context_) = CreateContext(egl_display_, egl_config_, egl_unity_context_);

    std::tie(success, egl_resource_context_) = CreateContext(egl_display_, egl_config_, egl_context_);

    return success;
  }

  void UnitySurfaceManager::CleanUp()
  {
    if (egl_display_ != EGL_NO_DISPLAY &&
        egl_resource_context_ != EGL_NO_CONTEXT)
    {
      eglDestroyContext(egl_display_, egl_resource_context_);
      egl_resource_context_ = EGL_NO_CONTEXT;
    }
    if (egl_display_ != EGL_NO_DISPLAY && egl_context_ != EGL_NO_CONTEXT)
    {
      eglDestroyContext(egl_display_, egl_context_);
      egl_context_ = EGL_NO_CONTEXT;
    }
  }

} // namespace uiwidgets
