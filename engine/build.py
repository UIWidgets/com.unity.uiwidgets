import subprocess
import os
import sys

def runCmd(cmds, log = False):
    process = subprocess.Popen(
        cmds,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE
    )
    stdout, stderr = process.communicate()
    if log:
        print("out:", stdout)
        print("err:", stderr)
    return stdout, stderr

linkOnly = len(sys.argv) > 1
buildEngine = linkOnly and (sys.argv[1] == 'e')
FLUTTER_ROOT = os.environ['FLUTTER_ROOT']
SKIA_ROOT = FLUTTER_ROOT+"/third_party/skia"

clang = FLUTTER_ROOT+"/buildtools/mac-x64/clang/bin/clang++"

codeFiles = [
    "src/assets/asset_manager.cc",
    "src/assets/asset_manager.h",
    "src/assets/asset_resolver.h",
    "src/assets/directory_asset_bundle.cc",
    "src/assets/directory_asset_bundle.h",

    "src/common/settings.cc",
    "src/common/settings.h",
    "src/common/task_runners.cc",
    "src/common/task_runners.h",

    "src/flow/layers/backdrop_filter_layer.cc",
    "src/flow/layers/backdrop_filter_layer.h",
    "src/flow/layers/clip_path_layer.cc",
    "src/flow/layers/clip_path_layer.h",
    "src/flow/layers/clip_rect_layer.cc",
    "src/flow/layers/clip_rect_layer.h",
    "src/flow/layers/clip_rrect_layer.cc",
    "src/flow/layers/clip_rrect_layer.h",
    "src/flow/layers/color_filter_layer.cc",
    "src/flow/layers/color_filter_layer.h",
    "src/flow/layers/container_layer.cc",
    "src/flow/layers/container_layer.h",
    "src/flow/layers/image_filter_layer.cc",
    "src/flow/layers/image_filter_layer.h",
    "src/flow/layers/layer.cc",
    "src/flow/layers/layer.h",
    "src/flow/layers/layer_tree.cc",
    "src/flow/layers/layer_tree.h",
    "src/flow/layers/opacity_layer.cc",
    "src/flow/layers/opacity_layer.h",
    "src/flow/layers/performance_overlay_layer.cc",
    "src/flow/layers/performance_overlay_layer.h",
    "src/flow/layers/physical_shape_layer.cc",
    "src/flow/layers/physical_shape_layer.h",
    "src/flow/layers/picture_layer.cc",
    "src/flow/layers/picture_layer.h",
    "src/flow/layers/platform_view_layer.cc",
    "src/flow/layers/platform_view_layer.h",
    "src/flow/layers/shader_mask_layer.cc",
    "src/flow/layers/shader_mask_layer.h",
    "src/flow/layers/texture_layer.cc",
    "src/flow/layers/texture_layer.h",
    "src/flow/layers/transform_layer.cc",
    "src/flow/layers/transform_layer.h",
    "src/flow/compositor_context.cc",
    "src/flow/compositor_context.h",
    "src/flow/embedded_views.cc",
    "src/flow/embedded_views.h",
    "src/flow/instrumentation.cc",
    "src/flow/instrumentation.h",
    "src/flow/matrix_decomposition.cc",
    "src/flow/matrix_decomposition.h",
    "src/flow/paint_utils.cc",
    "src/flow/paint_utils.h",
    "src/flow/raster_cache.cc",
    "src/flow/raster_cache.h",
    "src/flow/raster_cache_key.cc",
    "src/flow/raster_cache_key.h",
    "src/flow/rtree.cc",
    "src/flow/rtree.h",
    "src/flow/skia_gpu_object.cc",
    "src/flow/skia_gpu_object.h",
    "src/flow/texture.cc",
    "src/flow/texture.h",

    "src/lib/ui/compositing/scene.cc",
    "src/lib/ui/compositing/scene.h",
    "src/lib/ui/compositing/scene_builder.cc",
    "src/lib/ui/compositing/scene_builder.h",


    "src/lib/ui/text/icu_util.h",
    "src/lib/ui/text/icu_util.cc",
    "src/lib/ui/text/asset_manager_font_provider.cc",
    "src/lib/ui/text/asset_manager_font_provider.h",
    "src/lib/ui/text/paragraph_builder.cc",
    "src/lib/ui/text/paragraph_builder.h",
    "src/lib/ui/text/font_collection.cc",
    "src/lib/ui/text/font_collection.h",
    "src/lib/ui/text/paragraph.cc",
    "src/lib/ui/text/paragraph.h",

    "src/lib/ui/painting/canvas.cc",
    "src/lib/ui/painting/canvas.h",
    "src/lib/ui/painting/codec.cc",
    "src/lib/ui/painting/codec.h",
    "src/lib/ui/painting/color_filter.cc",
    "src/lib/ui/painting/color_filter.h",
    "src/lib/ui/painting/engine_layer.cc",
    "src/lib/ui/painting/engine_layer.h",
    "src/lib/ui/painting/frame_info.cc",
    "src/lib/ui/painting/frame_info.h",
    "src/lib/ui/painting/gradient.cc",
    "src/lib/ui/painting/gradient.h",
    "src/lib/ui/painting/image.cc",
    "src/lib/ui/painting/image.h",
    "src/lib/ui/painting/image_decoder.cc",
    "src/lib/ui/painting/image_decoder.h",
    "src/lib/ui/painting/image_encoding.cc",
    "src/lib/ui/painting/image_encoding.h",
    "src/lib/ui/painting/image_filter.cc",
    "src/lib/ui/painting/image_filter.h",
    "src/lib/ui/painting/image_shader.cc",
    "src/lib/ui/painting/image_shader.h",
    "src/lib/ui/painting/matrix.cc",
    "src/lib/ui/painting/matrix.h",
    "src/lib/ui/painting/multi_frame_codec.cc",
    "src/lib/ui/painting/multi_frame_codec.h",
    "src/lib/ui/painting/path.cc",
    "src/lib/ui/painting/path.h",
    "src/lib/ui/painting/path_measure.cc",
    "src/lib/ui/painting/path_measure.h",
    "src/lib/ui/painting/paint.cc",
    "src/lib/ui/painting/paint.h",
    "src/lib/ui/painting/picture.cc",
    "src/lib/ui/painting/picture.h",
    "src/lib/ui/painting/picture_recorder.cc",
    "src/lib/ui/painting/picture_recorder.h",
    "src/lib/ui/painting/rrect.cc",
    "src/lib/ui/painting/rrect.h",
    "src/lib/ui/painting/shader.cc",
    "src/lib/ui/painting/shader.h",
    "src/lib/ui/painting/single_frame_codec.cc",
    "src/lib/ui/painting/single_frame_codec.h",
    # "src/lib/ui/painting/skottie.cc",
    # "src/lib/ui/painting/skottie.h",
    "src/lib/ui/painting/vertices.cc",
    "src/lib/ui/painting/vertices.h",

    "src/lib/ui/window/platform_message_response_mono.cc",
    "src/lib/ui/window/platform_message_response_mono.h",
    "src/lib/ui/window/platform_message_response.cc",
    "src/lib/ui/window/platform_message_response.h",
    "src/lib/ui/window/platform_message.cc",
    "src/lib/ui/window/platform_message.h",
    "src/lib/ui/window/pointer_data.cc",
    "src/lib/ui/window/pointer_data.h",
    "src/lib/ui/window/pointer_data_packet.cc",
    "src/lib/ui/window/pointer_data_packet.h",
    "src/lib/ui/window/pointer_data_packet_converter.cc",
    "src/lib/ui/window/pointer_data_packet_converter.h",
    "src/lib/ui/window/viewport_metrics.cc",
    "src/lib/ui/window/viewport_metrics.h",
    "src/lib/ui/window/window.cc",
    "src/lib/ui/window/window.h",

    "src/lib/ui/io_manager.h",
    "src/lib/ui/snapshot_delegate.h",
    "src/lib/ui/ui_mono_state.cc",
    "src/lib/ui/ui_mono_state.h",

    "src/runtime/mono_api.cc",
    "src/runtime/mono_api.h",
    "src/runtime/mono_isolate.cc",
    "src/runtime/mono_isolate.h",
    "src/runtime/mono_isolate_scope.cc",
    "src/runtime/mono_isolate_scope.h",
    "src/runtime/mono_microtask_queue.cc",
    "src/runtime/mono_microtask_queue.h",
    "src/runtime/mono_state.cc",
    "src/runtime/mono_state.h",
    "src/runtime/runtime_controller.cc",
    "src/runtime/runtime_controller.h",
    "src/runtime/runtime_delegate.cc",
    "src/runtime/runtime_delegate.h",
    "src/runtime/start_up.cc",
    "src/runtime/start_up.h",
    "src/runtime/window_data.cc",
    "src/runtime/window_data.h",

    "src/shell/common/animator.cc",
    "src/shell/common/animator.h",
    "src/shell/common/canvas_spy.cc",
    "src/shell/common/canvas_spy.h",
    "src/shell/common/engine.cc",
    "src/shell/common/engine.h",
    "src/shell/common/lists.h",
    "src/shell/common/lists.cc",
    "src/shell/common/persistent_cache.cc",
    "src/shell/common/persistent_cache.h",
    "src/shell/common/pipeline.cc",
    "src/shell/common/pipeline.h",
    "src/shell/common/platform_view.cc",
    "src/shell/common/platform_view.h",
    "src/shell/common/pointer_data_dispatcher.cc",
    "src/shell/common/pointer_data_dispatcher.h",
    "src/shell/common/rasterizer.cc",
    "src/shell/common/rasterizer.h",
    "src/shell/common/run_configuration.cc",
    "src/shell/common/run_configuration.h",
    "src/shell/common/shell.cc",
    "src/shell/common/shell.h",
    "src/shell/common/shell_io_manager.cc",
    "src/shell/common/shell_io_manager.h",
    "src/shell/common/surface.cc",
    "src/shell/common/surface.h",
    "src/shell/common/switches.cc",
    "src/shell/common/switches.h",
    "src/shell/common/thread_host.cc",
    "src/shell/common/thread_host.h",
    "src/shell/common/vsync_waiter.cc",
    "src/shell/common/vsync_waiter.h",
    "src/shell/common/vsync_waiter_fallback.cc",
    "src/shell/common/vsync_waiter_fallback.h",

    "src/shell/gpu/gpu_surface_delegate.h",
    "src/shell/gpu/gpu_surface_gl.cc",
    "src/shell/gpu/gpu_surface_gl.h",
    "src/shell/gpu/gpu_surface_gl_delegate.cc",
    "src/shell/gpu/gpu_surface_gl_delegate.h",
    "src/shell/gpu/gpu_surface_software.cc",
    "src/shell/gpu/gpu_surface_software.h",
    "src/shell/gpu/gpu_surface_software_delegate.cc",
    "src/shell/gpu/gpu_surface_software_delegate.h",



    "src/shell/platform/embedder/embedder.cc",
    "src/shell/platform/embedder/embedder.h",
    "src/shell/platform/embedder/embedder_engine.cc",
    "src/shell/platform/embedder/embedder_engine.h",
    "src/shell/platform/embedder/embedder_external_texture_gl.cc",
    "src/shell/platform/embedder/embedder_external_texture_gl.h",
    "src/shell/platform/embedder/embedder_external_view.cc",
    "src/shell/platform/embedder/embedder_external_view.h",
    "src/shell/platform/embedder/embedder_external_view_embedder.cc",
    "src/shell/platform/embedder/embedder_external_view_embedder.h",
    "src/shell/platform/embedder/embedder_layers.cc",
    "src/shell/platform/embedder/embedder_layers.h",
    "src/shell/platform/embedder/embedder_platform_message_response.cc",
    "src/shell/platform/embedder/embedder_platform_message_response.h",
    "src/shell/platform/embedder/embedder_render_target.cc",
    "src/shell/platform/embedder/embedder_render_target.h",
    "src/shell/platform/embedder/embedder_render_target_cache.cc",
    "src/shell/platform/embedder/embedder_render_target_cache.h",
    "src/shell/platform/embedder/embedder_surface.cc",
    "src/shell/platform/embedder/embedder_surface.h",
    "src/shell/platform/embedder/embedder_surface_gl.cc",
    "src/shell/platform/embedder/embedder_surface_gl.h",
    "src/shell/platform/embedder/embedder_surface_software.cc",
    "src/shell/platform/embedder/embedder_surface_software.h",
    "src/shell/platform/embedder/embedder_task_runner.cc",
    "src/shell/platform/embedder/embedder_task_runner.h",
    "src/shell/platform/embedder/embedder_thread_host.cc",
    "src/shell/platform/embedder/embedder_thread_host.h",
    "src/shell/platform/embedder/platform_view_embedder.cc",
    "src/shell/platform/embedder/platform_view_embedder.h",
    "src/shell/platform/embedder/vsync_waiter_embedder.cc",
    "src/shell/platform/embedder/vsync_waiter_embedder.h",

    "src/shell/platform/unity/gfx_worker_task_runner.cc",
    "src/shell/platform/unity/gfx_worker_task_runner.h",
    "src/shell/platform/unity/uiwidgets_system.h",

    "src/shell/platform/unity/unity_console.cc",
    "src/shell/platform/unity/unity_console.h",

    "src/shell/version/version.cc",
    "src/shell/version/version.h",

    "src/engine.cc",
    "src/platform_base.h"
]
objs = [
    # icudtl
    "icudtl.o",

    # libcxx
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.algorithm.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.any.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.bind.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.charconv.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.chrono.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.condition_variable.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.condition_variable_destructor.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.debug.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.exception.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.functional.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.future.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.hash.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.ios.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.iostream.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.locale.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.memory.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.mutex.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.mutex_destructor.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.new.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.optional.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.random.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.regex.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.shared_mutex.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.stdexcept.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.string.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.strstream.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.system_error.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.thread.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.typeinfo.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.utility.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.valarray.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.variant.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxx/src/libcxx.vector.o",
    # libcxxabi
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.abort_message.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_aux_runtime.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_default_handlers.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_demangle.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_exception_storage.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_guard.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_handlers.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_noexception.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_unexpected.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_vector.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.cxa_virtual.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.fallback_malloc.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.private_typeinfo.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.stdlib_exception.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.stdlib_stdexcept.o",
    FLUTTER_ROOT+"/out/android_debug_unopt/obj/third_party/libcxxabi/src/libcxxabi.stdlib_typeinfo.o",
]

# compile uiwidgets object
for codeFile in codeFiles:
    div = codeFile.rfind("/")+1
    dir = codeFile[0:div-1]
    fileName = codeFile[div::]
    div = fileName.index(".")
    name = fileName[0:div]
    extention = fileName[div::]
    if extention != ".cc":
        continue

    FLUTTER_ROOT = FLUTTER_ROOT+""
    clang = FLUTTER_ROOT + "/buildtools/mac-x64/clang/bin/clang++"
    mkdirScript = ["mkdir", "-p", "obj/"+dir]
    process = subprocess.Popen(
        mkdirScript, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdout, stderr = process.communicate()
    buildScript = [
        clang,
        "-MD",
        "-MF",
        "obj/"+dir+"/"+name+".o.d",
        "-DUSE_OPENSSL=1",
        "-DUSE_OPENSSL_CERTS=1",
        "-DANDROID",
        "-DHAVE_SYS_UIO_H",
        "-D__STDC_CONSTANT_MACROS",
        "-D__STDC_FORMAT_MACROS",
        "-D_FORTIFY_SOURCE=2",
        "-D__compiler_offsetof=__builtin_offsetof",
        "-Dnan=__builtin_nan",
        "-D__GNU_SOURCE=1",
        "-D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS",
        "-D_LIBCPP_ENABLE_THREAD_SAFETY_ANNOTATIONS",
        "-D_DEBUG",
        "-DU_USING_ICU_NAMESPACE=0",
        "-DU_ENABLE_DYLOAD=0",
        "-DUSE_CHROMIUM_ICU=1",
        "-DU_STATIC_IMPLEMENTATION",
        "-DICU_UTIL_DATA_IMPL=ICU_UTIL_DATA_FILE",
        "-DUCHAR_TYPE=uint16_t",
        "-DFLUTTER_RUNTIME_MODE_DEBUG=1",
        "-DFLUTTER_RUNTIME_MODE_PROFILE=2",
        "-DFLUTTER_RUNTIME_MODE_RELEASE=3",
        "-DFLUTTER_RUNTIME_MODE_JIT_RELEASE=4",
        "-DFLUTTER_RUNTIME_MODE=1",
        "-DFLUTTER_JIT_RUNTIME=1",

        # confige for rapidjson
        "-DUIWIDGETS_FORCE_ALIGNAS_8=\"1\"",
        "-DRAPIDJSON_HAS_STDSTRING",
        "-DRAPIDJSON_HAS_CXX11_RANGE_FOR",
        "-DRAPIDJSON_HAS_CXX11_RVALUE_REFS",
        "-DRAPIDJSON_HAS_CXX11_TYPETRAITS",
        "-DRAPIDJSON_HAS_CXX11_NOEXCEPT",
        "-DSK_ENABLE_SPIRV_VALIDATION",
        "-DSK_GAMMA_APPLY_TO_A8",
        "-DSK_GAMMA_EXPONENT=1.4",
        "-DSK_GAMMA_CONTRAST=0.0",
        "-DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1",
        "-DGR_TEST_UTILS=1",
        "-DSKIA_IMPLEMENTATION=1",
        "-DSK_GL",
        "-DSK_ENABLE_DUMP_GPU",
        "-DSK_SUPPORT_PDF",
        "-DSK_CODEC_DECODES_JPEG",
        "-DSK_ENCODE_JPEG",
        "-DSK_ENABLE_ANDROID_UTILS",
        "-DSK_USE_LIBGIFCODEC",
        "-DSK_HAS_HEIF_LIBRARY",
        "-DSK_CODEC_DECODES_PNG",
        "-DSK_ENCODE_PNG",
        "-DSK_CODEC_DECODES_RAW",
        "-DSK_ENABLE_SKSL_INTERPRETER",
        "-DSK_CODEC_DECODES_WEBP",
        "-DSK_ENCODE_WEBP",
        "-DSK_XML",


        "-DUIWIDGETS_ENGINE_VERSION=\"0.0\"",
        "-DSKIA_VERSION=\"0.0\"",
        "-DXML_STATIC",

        "-I.",
        "-Ithird_party",
        "-Isrc",
        "-I"+FLUTTER_ROOT,
        "-I"+FLUTTER_ROOT+"/third_party/rapidjson/include",
        "-I"+SKIA_ROOT,
        "-I"+SKIA_ROOT + "/include/third_party/vulkan",
        "-I"+FLUTTER_ROOT+"/flutter/third_party/txt/src",
        "-I" + FLUTTER_ROOT + "/third_party/harfbuzz/src",
        "-I" + SKIA_ROOT + "/third_party/externals/icu/source/common",

        # "-Igen",
        "-I"+FLUTTER_ROOT+"/third_party/libcxx/include",
        "-I"+FLUTTER_ROOT+"/third_party/libcxxabi/include",
        "-I"+FLUTTER_ROOT+"/third_party/icu/source/common",
        "-I"+FLUTTER_ROOT+"/third_party/icu/source/i18n",

        # ignore deprecated code
        "-Wno-deprecated-declarations",

        "-fno-strict-aliasing",
        "-march=armv7-a",
        "-mfloat-abi=softfp",
        "-mtune=generic-armv7-a",
        "-mthumb",
        "-fPIC",
        "-pipe",
        "-fcolor-diagnostics",
        "-ffunction-sections",
        "-funwind-tables",
        "-fno-short-enums",
        "-nostdinc++",
        "--target=arm-linux-androideabi",
        "-mfpu=neon",
        "-Wall",
        "-Wextra",
        "-Wendif-labels",
        "-Werror",
        "-Wno-missing-field-initializers",
        "-Wno-unused-parameter",
        "-Wno-unused-variable",
        # "-Wno-non-c-typedef-for-linkage",
        "-isystem"+FLUTTER_ROOT+"/third_party/android_tools/ndk/sources/android/support/include",
        "-isystem"+FLUTTER_ROOT +
        "/third_party/android_tools/ndk/sysroot/usr/include/arm-linux-androideabi",
        "-D__ANDROID_API__=16",
        # "-fvisibility=hidden",
        "--sysroot="+FLUTTER_ROOT+"/third_party/android_tools/ndk/sysroot",
        "-Wstring-conversion",
        # supress new line error
        # "-Wnewline-eof",
        "-O0",
        "-g2",
        "-fvisibility-inlines-hidden",
        "-std=c++17",
        "-fno-rtti",
        "-fno-exceptions",
        "-c",
        dir+"/" + name + extention,
        "-o",
        "obj/"+dir+"/"+name+".o"
    ]
    objs.append("obj/"+dir+"/"+name+".o")
    if linkOnly and (buildEngine == False):
        continue
    if buildEngine and name != 'engine':
        continue
    _, stderr = runCmd(buildScript, False)
    # process = subprocess.Popen(
    #     buildScript, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # stdout, stderr = process.communicate()
    # print(codeFile)
    if len(stderr) > 0:
        print(stdout)
        print(stderr)
        exit()

# delete .so file
soFile = "../Samples/UIWidgetsSamples_2019_4/Assets/Plugins/Android/libUIWidgets_d.so"
deleteCmd = ["rm", soFile]
runCmd(deleteCmd, False)

# link .so file
linkCmd = [
    clang,
    "-shared",
    # "-Wl,--version-script="+FLUTTER_ROOT+"/flutter/shell/platform/android/android_exports.lst",
    "-Wl,--fatal-warnings",
    "-fPIC",
    "-Wl,-z,noexecstack",
    "-Wl,-z,now",
    "-Wl,-z,relro",
    "-Wl,-z,defs",
    "--gcc-toolchain="+FLUTTER_ROOT + \
    "/third_party/android_tools/ndk/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64",
    "-Wl,--no-undefined",
    "-Wl,--exclude-libs,ALL",
    "-fuse-ld=lld",
    "-Wl,--icf=all",
    "--target=arm-linux-androideabi",
    "-nostdlib++",
    "-Wl,--warn-shared-textrel",
    "-nostdlib",
    "--sysroot="+FLUTTER_ROOT+"/third_party/android_tools/ndk/platforms/android-16/arch-arm",
    "-L"+FLUTTER_ROOT + \
    "/third_party/android_tools/ndk/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a",
    "-o",
    soFile,
    "-Wl,--build-id=sha1",
    "-Wl,-soname=libUIWidgets_d.so",
    "-Wl,--whole-archive",
]
linkCmd.extend(objs)
linkCmd.extend([

    FLUTTER_ROOT+"/out/android_debug_unopt/obj/flutter/third_party/txt/libtxt_lib.a",

    "-Wl,--no-whole-archive",

    # lib for atexit
    FLUTTER_ROOT+"/third_party/android_tools/ndk/platforms/android-16/arch-arm/usr/lib/crtbegin_so.o",
    "-landroid",
    "-lEGL",
    "-lGLESv2",
    "-landroid_support",
    "-lunwind",
    "-lgcc",
    "-lc",
    "-ldl",
    "-lm",
    "-llog",
    FLUTTER_ROOT+"/third_party/android_tools/ndk/platforms/android-16/arch-arm/usr/lib/crtend_so.o",
])

runCmd(linkCmd, True)