cmake_minimum_required(VERSION 3.10)

project(UIWidgetsPlugin VERSION 2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

#set(CMAKE_VERBOSE_MAKEFILE ON)

set(FLUTTER_ROOT /Users/xingweizhu/WorkSpace/UnityApps/UIWidget/temp/engine_denpendencies/engine/src)
set(SKIA_ROOT ${FLUTTER_ROOT}/third_party/skia)

set(UIWidgetsInc
    src/assets/asset_manager.h
    src/assets/asset_resolver.h
    src/assets/directory_asset_bundle.h

    src/common/settings.h
    src/common/task_runners.h

    src/flow/layers/backdrop_filter_layer.h
    src/flow/layers/clip_path_layer.h
    src/flow/layers/clip_rect_layer.h
    src/flow/layers/clip_rrect_layer.h
    src/flow/layers/color_filter_layer.h
    src/flow/layers/container_layer.h
    src/flow/layers/image_filter_layer.h
    src/flow/layers/layer.h
    src/flow/layers/layer_tree.h
    src/flow/layers/opacity_layer.h
    src/flow/layers/performance_overlay_layer.h
    src/flow/layers/physical_shape_layer.h
    src/flow/layers/picture_layer.h
    src/flow/layers/platform_view_layer.h
    src/flow/layers/shader_mask_layer.h
    src/flow/layers/texture_layer.h
    src/flow/layers/transform_layer.h
    src/flow/compositor_context.h
    src/flow/embedded_views.h
    src/flow/instrumentation.h
    src/flow/matrix_decomposition.h
    src/flow/paint_utils.h
    src/flow/raster_cache.h
    src/flow/raster_cache_key.h
    src/flow/rtree.h
    src/flow/skia_gpu_object.h
    src/flow/texture.h

    src/lib/ui/compositing/scene.h
    src/lib/ui/compositing/scene_builder.h


    src/lib/ui/text/icu_util.h
    src/lib/ui/text/asset_manager_font_provider.h
    src/lib/ui/text/paragraph_builder.h
    src/lib/ui/text/font_collection.h
    src/lib/ui/text/paragraph.h

    src/lib/ui/painting/canvas.h
    src/lib/ui/painting/codec.h
    src/lib/ui/painting/color_filter.h
    src/lib/ui/painting/engine_layer.h
    src/lib/ui/painting/frame_info.h
    src/lib/ui/painting/gradient.h
    src/lib/ui/painting/image.h
    src/lib/ui/painting/image_decoder.h
    src/lib/ui/painting/image_encoding.h
    src/lib/ui/painting/image_filter.h
    src/lib/ui/painting/image_shader.h
    src/lib/ui/painting/matrix.h
    src/lib/ui/painting/multi_frame_codec.h
    src/lib/ui/painting/path.h
    src/lib/ui/painting/path_measure.h
    src/lib/ui/painting/paint.h
    src/lib/ui/painting/picture.h
    src/lib/ui/painting/picture_recorder.h
    src/lib/ui/painting/rrect.h
    src/lib/ui/painting/shader.h
    src/lib/ui/painting/single_frame_codec.h
    src/lib/ui/painting/skottie.h
    src/lib/ui/painting/vertices.h

    src/lib/ui/window/platform_message_response_mono.h
    src/lib/ui/window/platform_message_response.h
    src/lib/ui/window/platform_message.h
    src/lib/ui/window/pointer_data.h
    src/lib/ui/window/pointer_data_packet.h
    src/lib/ui/window/pointer_data_packet_converter.h
    src/lib/ui/window/viewport_metrics.h
    src/lib/ui/window/window.h

    src/lib/ui/io_manager.h
    src/lib/ui/snapshot_delegate.h
    src/lib/ui/ui_mono_state.h

    src/runtime/mono_api.h
    src/runtime/mono_isolate.h
    src/runtime/mono_isolate_scope.h
    src/runtime/mono_microtask_queue.h
    src/runtime/mono_state.h
    src/runtime/runtime_controller.h
    src/runtime/runtime_delegate.h
    src/runtime/start_up.h
    src/runtime/window_data.h

    src/shell/common/animator.h
    src/shell/common/canvas_spy.h
    src/shell/common/engine.h
    src/shell/common/lists.h
    src/shell/common/persistent_cache.h
    src/shell/common/pipeline.h
    src/shell/common/platform_view.h
    src/shell/common/pointer_data_dispatcher.h
    src/shell/common/rasterizer.h
    src/shell/common/run_configuration.h
    src/shell/common/shell.h
    src/shell/common/shell_io_manager.h
    src/shell/common/surface.h
    src/shell/common/switches.h
    src/shell/common/thread_host.h
    src/shell/common/vsync_waiter.h
    src/shell/common/vsync_waiter_fallback.h

    src/shell/gpu/gpu_surface_delegate.h
    src/shell/gpu/gpu_surface_gl.h
    src/shell/gpu/gpu_surface_gl_delegate.h
    src/shell/gpu/gpu_surface_software.h
    src/shell/gpu/gpu_surface_software_delegate.h

    src/shell/platform/embedder/embedder.h
    src/shell/platform/embedder/embedder_engine.h
    src/shell/platform/embedder/embedder_external_texture_gl.h
    src/shell/platform/embedder/embedder_external_view.h
    src/shell/platform/embedder/embedder_external_view_embedder.h
    src/shell/platform/embedder/embedder_layers.h
    src/shell/platform/embedder/embedder_platform_message_response.h
    src/shell/platform/embedder/embedder_render_target.h
    src/shell/platform/embedder/embedder_render_target_cache.h
    src/shell/platform/embedder/embedder_surface.h
    src/shell/platform/embedder/embedder_surface_gl.h
    src/shell/platform/embedder/embedder_surface_software.h
    src/shell/platform/embedder/embedder_task_runner.h
    src/shell/platform/embedder/embedder_thread_host.h
    src/shell/platform/embedder/platform_view_embedder.h
    src/shell/platform/embedder/vsync_waiter_embedder.h

    src/shell/platform/unity/gfx_worker_task_runner.h
    src/shell/platform/unity/uiwidgets_system.h

              
    src/shell/platform/unity/unity_console.h

    src/shell/version/version.h

    src/platform_base.h
)

set(UIWidgetsMacInc
    src/shell/platform/unity/darwin/macos/uiwidgets_panel.h
    src/shell/platform/unity/darwin/macos/uiwidgets_system.h
    src/shell/platform/unity/darwin/macos/cocoa_task_runner.h
    src/shell/platform/unity/darwin/macos/unity_surface_manager.h
    src/shell/platform/unity/darwin/macos/unity_external_texture_gl.h
)

set(UIWidgetsMacSrc 
        src/shell/platform/unity/darwin/macos/uiwidgets_panel.mm
        src/shell/platform/unity/darwin/macos/uiwidgets_system.mm 
        src/shell/platform/unity/darwin/macos/cocoa_task_runner.cc    
        src/shell/platform/unity/darwin/macos/unity_surface_manager.mm  
        src/shell/platform/unity/darwin/macos/unity_external_texture_gl.mm          
)


set(UIWidgetsSrc 

    src/assets/asset_manager.cc
    src/assets/directory_asset_bundle.cc

    src/common/settings.cc
    src/common/task_runners.cc

    src/flow/layers/backdrop_filter_layer.cc
    src/flow/layers/clip_path_layer.cc
    src/flow/layers/clip_rect_layer.cc
    src/flow/layers/clip_rrect_layer.cc
    src/flow/layers/color_filter_layer.cc
    src/flow/layers/container_layer.cc
    src/flow/layers/image_filter_layer.cc
    src/flow/layers/layer.cc
    src/flow/layers/layer_tree.cc
    src/flow/layers/opacity_layer.cc
    src/flow/layers/performance_overlay_layer.cc
    src/flow/layers/physical_shape_layer.cc
    src/flow/layers/picture_layer.cc
    src/flow/layers/platform_view_layer.cc
    src/flow/layers/shader_mask_layer.cc
    src/flow/layers/texture_layer.cc
    src/flow/layers/transform_layer.cc
    src/flow/compositor_context.cc
    src/flow/embedded_views.cc
    src/flow/instrumentation.cc
    src/flow/matrix_decomposition.cc
    src/flow/paint_utils.cc
    src/flow/raster_cache.cc
    src/flow/raster_cache_key.cc
    src/flow/rtree.cc
    src/flow/skia_gpu_object.cc
    src/flow/texture.cc

    src/lib/ui/compositing/scene.cc
    src/lib/ui/compositing/scene_builder.cc


    src/lib/ui/text/icu_util.cc
    src/lib/ui/text/asset_manager_font_provider.cc
    src/lib/ui/text/paragraph_builder.cc
    src/lib/ui/text/font_collection.cc
    src/lib/ui/text/paragraph.cc

    src/lib/ui/painting/canvas.cc
    src/lib/ui/painting/codec.cc
    src/lib/ui/painting/color_filter.cc
    src/lib/ui/painting/engine_layer.cc
    src/lib/ui/painting/frame_info.cc
    src/lib/ui/painting/gradient.cc
    src/lib/ui/painting/image.cc
    src/lib/ui/painting/image_decoder.cc
    src/lib/ui/painting/image_encoding.cc
    src/lib/ui/painting/image_filter.cc
    src/lib/ui/painting/image_shader.cc
    src/lib/ui/painting/matrix.cc
    src/lib/ui/painting/multi_frame_codec.cc
    src/lib/ui/painting/path.cc
    src/lib/ui/painting/path_measure.cc
    src/lib/ui/painting/paint.cc
    src/lib/ui/painting/picture.cc
    src/lib/ui/painting/picture_recorder.cc
    src/lib/ui/painting/rrect.cc
    src/lib/ui/painting/shader.cc
    src/lib/ui/painting/single_frame_codec.cc
    src/lib/ui/painting/skottie.cc
    src/lib/ui/painting/vertices.cc

    src/lib/ui/window/platform_message_response_mono.cc
    src/lib/ui/window/platform_message_response.cc
    src/lib/ui/window/platform_message.cc
    src/lib/ui/window/pointer_data.cc
    src/lib/ui/window/pointer_data_packet.cc
    src/lib/ui/window/pointer_data_packet_converter.cc
    src/lib/ui/window/viewport_metrics.cc
    src/lib/ui/window/window.cc

    src/lib/ui/ui_mono_state.cc

    src/runtime/mono_api.cc
    src/runtime/mono_isolate.cc
    src/runtime/mono_isolate_scope.cc
    src/runtime/mono_microtask_queue.cc
    src/runtime/mono_state.cc
    src/runtime/runtime_controller.cc
    src/runtime/runtime_delegate.cc
    src/runtime/start_up.cc
    src/runtime/window_data.cc

    src/shell/common/animator.cc
    src/shell/common/canvas_spy.cc
    src/shell/common/engine.cc
    src/shell/common/lists.cc
    src/shell/common/persistent_cache.cc
    src/shell/common/pipeline.cc
    src/shell/common/platform_view.cc
    src/shell/common/pointer_data_dispatcher.cc
    src/shell/common/rasterizer.cc
    src/shell/common/run_configuration.cc
    src/shell/common/shell.cc
    src/shell/common/shell_io_manager.cc
    src/shell/common/surface.cc
    src/shell/common/switches.cc
    src/shell/common/thread_host.cc
    src/shell/common/vsync_waiter.cc
    src/shell/common/vsync_waiter_fallback.cc

    src/shell/gpu/gpu_surface_gl.cc
    src/shell/gpu/gpu_surface_gl_delegate.cc
    src/shell/gpu/gpu_surface_software.cc
    src/shell/gpu/gpu_surface_software_delegate.cc

    src/shell/platform/embedder/embedder.cc
    src/shell/platform/embedder/embedder_engine.cc
    src/shell/platform/embedder/embedder_external_texture_gl.cc
    src/shell/platform/embedder/embedder_external_view.cc
    src/shell/platform/embedder/embedder_external_view_embedder.cc
    src/shell/platform/embedder/embedder_layers.cc
    src/shell/platform/embedder/embedder_platform_message_response.cc
    src/shell/platform/embedder/embedder_render_target.cc
    src/shell/platform/embedder/embedder_render_target_cache.cc
    src/shell/platform/embedder/embedder_surface.cc
    src/shell/platform/embedder/embedder_surface_gl.cc
    src/shell/platform/embedder/embedder_surface_software.cc
    src/shell/platform/embedder/embedder_task_runner.cc
    src/shell/platform/embedder/embedder_thread_host.cc
    src/shell/platform/embedder/platform_view_embedder.cc
    src/shell/platform/embedder/vsync_waiter_embedder.cc

    src/shell/platform/unity/gfx_worker_task_runner.cc
              
    src/shell/platform/unity/unity_console.cc

    src/shell/version/version.cc

    src/engine.cc
)

add_library(uiwidgets SHARED ${UIWidgetsSrc} ${UIWidgetsMacSrc} ${UIWidgetsInc} ${UIWidgetsMacInc})


#includes
target_include_directories(uiwidgets PUBLIC 
                                third_party
                                src
                                ${PROJECT_SOURCE_DIR}
                                ${FLUTTER_ROOT}
                                ${FLUTTER_ROOT}/third_party/rapidjson/include
                                ${SKIA_ROOT}
                                ${FLUTTER_ROOT}/flutter/third_party/txt/src
                                ${FLUTTER_ROOT}/third_party/harfbuzz/src
                                ${SKIA_ROOT}/third_party/externals/icu/source/common
                                ${FLUTTER_ROOT}/third_party/icu/source/common
                                ${FLUTTER_ROOT}/third_party/icu/source/i18n
)


#custom compilation flag area
target_compile_options(uiwidgets PUBLIC -Wno-c++11-narrowing -fno-exceptions -fno-rtti -g -fno-strict-aliasing -ffunction-sections -fdata-sections -fmessage-length=0 -pipe)

target_compile_options(uiwidgets PUBLIC -MD -MF -I. -Ithird_party -Isrc -I${FLUTTER_ROOT} -I${FLUTTER_ROOT}/third_party/rapidjson/include -I${SKIA_ROOT} -I${FLUTTER_ROOT}/flutter/third_party/txt/src -I${FLUTTER_ROOT}/third_party/harfbuzz/src -I${SKIA_ROOT}/third_party/externals/icu/source/common -I${FLUTTER_ROOT}/third_party/icu/source/common -I${FLUTTER_ROOT}/third_party/icu/source/i18n -fvisibility-inlines-hidden)


#custom definitions
target_compile_definitions(uiwidgets PUBLIC "UIWIDGETS_ENGINE_VERSION=\"0.0\"" "SKIA_VERSION=\"0.0\"")
target_compile_definitions(uiwidgets PUBLIC "UIWIDGETS_FORCE_ALIGNAS_8=\"1\"")
target_compile_definitions(uiwidgets PUBLIC "UIWIDGETS_FORCE_DISABLE_PERSISTENTCACHE=\"1\"")
target_compile_definitions(uiwidgets PUBLIC _ITERATOR_DEBUG_LEVEL=2 _HAS_ITERATOR_DEBUGGING=1 _SECURE_SCL=1)

target_compile_definitions(uiwidgets PUBLIC USE_OPENSSL=1 __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS _FORTIFY_SOURCE=2 _LIBCPP_DISABLE_AVAILABILITY=1 _LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS _DEBUG FLUTTER_RUNTIME_MODE_DEBUG=1 FLUTTER_RUNTIME_MODE_PROFILE=2 FLUTTER_RUNTIME_MODE_RELEASE=3 FLUTTER_RUNTIME_MODE_JIT_RELEASE=4 FLUTTER_RUNTIME_MODE=1 FLUTTER_JIT_RUNTIME=1)

target_compile_definitions(uiwidgets PUBLIC SK_ENABLE_SPIRV_VALIDATION SK_ASSUME_GL=1 SK_ENABLE_API_AVAILABLE SK_GAMMA_APPLY_TO_A8 GR_OP_ALLOCATE_USE_NEW SK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1 GR_TEST_UTILS=1 SKIA_IMPLEMENTATION=1 SK_GL SK_ENABLE_DUMP_GPU SK_SUPPORT_PDF SK_CODEC_DECODES_JPEG SK_ENCODE_JPEG SK_ENABLE_ANDROID_UTILS SK_USE_LIBGIFCODEC SK_HAS_HEIF_LIBRARY SK_CODEC_DECODES_PNG SK_ENCODE_PNG SK_CODEC_DECODES_RAW SK_ENABLE_SKSL_INTERPRETER SKVM_JIT_WHEN_POSSIBLE SK_CODEC_DECODES_WEBP SK_ENCODE_WEBP SK_XML)

target_compile_definitions(uiwidgets PUBLIC SK_USING_THIRD_PARTY_ICU U_USING_ICU_NAMESPACE=0
U_ENABLE_DYLOAD=0 USE_CHROMIUM_ICU=1 U_STATIC_IMPLEMENTATION ICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_STATIC)

target_compile_definitions(uiwidgets PUBLIC RAPIDJSON_HAS_STDSTRING RAPIDJSON_HAS_CXX11_RANGE_FOR RAPIDJSON_HAS_CXX11_RVALUE_REFS RAPIDJSON_HAS_CXX11_TYPETRAITS RAPIDJSON_HAS_CXX11_NOEXCEPT USE_OPENSSL=1 __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS _FORTIFY_SOURCE=2 _LIBCPP_DISABLE_AVAILABILITY=1 _LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS _LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS _DEBUG)


#custom link
target_link_libraries(uiwidgets "-framework Foundation")
target_link_libraries(uiwidgets "-framework ApplicationServices")
target_link_libraries(uiwidgets "-framework OpenGL")
target_link_libraries(uiwidgets "-framework AppKit")
target_link_libraries(uiwidgets "-framework CoreVideo")
target_link_libraries(uiwidgets ${FLUTTER_ROOT}/out/host_debug_unopt/obj/flutter/third_party/txt/libtxt_lib.a)