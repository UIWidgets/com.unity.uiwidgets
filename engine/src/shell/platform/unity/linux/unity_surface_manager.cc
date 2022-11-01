
#include "unity_surface_manager.h"

#include <flutter/fml/logging.h>
//#include <EGL/egl.h>

//#include <GL/gl.h>
//#include <GL/glx.h>
#include <GL/glext.h>

#include "src/shell/common/shell_io_manager.h"
#include "src/shell/gpu/gpu_surface_delegate.h"
#include "src/shell/gpu/gpu_surface_gl_delegate.h"
namespace uiwidgets
{

  static Display* gl_display_;
  static GLXContext gl_unity_context_;

  template <class T>
  using GLXResult = std::pair<bool, T>;

  UnitySurfaceManager::UnitySurfaceManager(IUnityInterfaces *unity_interfaces)
      : gl_context_(NULL),
        gl_resource_context_(NULL)
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
    return glXMakeContextCurrent(gl_display_, NULL, NULL,
                          NULL) == True;
  }

  bool UnitySurfaceManager::MakeCurrent(GLXDrawable drawable)
  {
    return glXMakeContextCurrent(gl_display_, drawable, drawable, gl_context_) ==
           True;
  }

  bool UnitySurfaceManager::MakeResourceCurrent()
  {
    return glXMakeContextCurrent(gl_display_, NULL, NULL, gl_resource_context_) == True;
  }

  static GLXResult<XVisualInfo*> ChooseEGLConfiguration(Display* display)
  {
    int attributes[] = {
      GLX_RGBA,
      GLX_RED_SIZE, 8,
      GLX_GREEN_SIZE, 8,
      GLX_BLUE_SIZE, 8,
      GLX_DEPTH_SIZE, 0,
      GLX_STENCIL_SIZE, 0,
      NULL
    };

    XVisualInfo* glx_config = glXChooseVisual(display, 0, attributes);

    if (glx_config == NULL)
    {
      return {false, nullptr};
    }

    return {True, glx_config};
  }

  static GLXResult<GLXContext> CreateContext(Display* display,
                                             XVisualInfo* config,
                                             GLXContext share = NULL)
  {
    GLXContext context = glXCreateContext(display, config, share, True);
    return {context != NULL, context};
  }

  void UnitySurfaceManager::GetUnityContext()
  {
    if(gl_unity_context_ != NULL){
      return;
    }

    gl_display_ = glXGetCurrentDisplay();
    gl_unity_context_ = glXGetCurrentContext();

    FML_CHECK(gl_display_ != NULL)
        << "Renderer type is invalid";
  }

  bool UnitySurfaceManager::Initialize(IUnityInterfaces *unity_interfaces)
  {
    FML_CHECK(gl_display_ != NULL)
        << "Renderer type is invalid";

    // Initialize the display connection.
    //FML_CHECK(__glXInitialize(gl_display_) == True)
    //    << "Renderer type is invalid";

    auto valid_ = true;

    bool success = false;

    std::tie(success, gl_config_) = ChooseEGLConfiguration(gl_display_);
    FML_CHECK(success) << "Could not choose an EGL configuration.";

    std::tie(success, gl_context_) = CreateContext(gl_display_, gl_config_, gl_unity_context_);

    std::tie(success, gl_resource_context_) = CreateContext(gl_display_, gl_config_, gl_context_);

    return success;
  }

  void UnitySurfaceManager::CleanUp()
  {
    if (gl_display_ != NULL &&
        gl_resource_context_ != NULL)
    {
      glXDestroyContext(gl_display_, gl_resource_context_);
      gl_resource_context_ = NULL;
    }
    if (gl_display_ != NULL && gl_context_ != NULL)
    {
      glXDestroyContext(gl_display_, gl_context_);
      gl_context_ = NULL;
    }
  }

} // namespace uiwidgets
