#define RAPIDJSON_HAS_STDSTRING 1
#include "shell.h"

#include <future>
#include <memory>
#include <sstream>
#include <vector>

#include "assets/directory_asset_bundle.h"
#include "flutter/fml/file.h"
//#include "flutter/fml/icu_util.h"
#include "flutter/fml/log_settings.h"
#include "flutter/fml/logging.h"
#include "flutter/fml/make_copyable.h"
#include "flutter/fml/message_loop.h"
#include "flutter/fml/paths.h"
#include "flutter/fml/trace_event.h"
#include "flutter/fml/unique_fd.h"
#include "include/core/SkGraphics.h"
#include "include/utils/SkBase64.h"
#include "lib/ui/text/icu_util.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "runtime/start_up.h"
#include "shell/common/engine.h"
#include "shell/common/persistent_cache.h"
//#include "shell/common/skia_event_tracer_impl.h"
//#include "shell/common/switches.h"
#include "shell/common/vsync_waiter.h"
#include "shell/platform/embedder/embedder_task_runner.h"

namespace uiwidgets {

constexpr char kSkiaChannel[] = "uiwidgets/skia";
constexpr char kSystemChannel[] = "uiwidgets/system";
constexpr char kTypeKey[] = "type";
constexpr char kFontChange[] = "fontsChange";

std::unique_ptr<Shell> Shell::CreateShellOnPlatformThread(
    TaskRunners task_runners, const WindowData window_data, Settings settings,
    const Shell::CreateCallback<PlatformView>& on_create_platform_view,
    const Shell::CreateCallback<Rasterizer>& on_create_rasterizer) {
  if (!task_runners.IsValid()) {
    FML_LOG(ERROR) << "Task runners to run the shell were invalid.";
    return nullptr;
  }

  auto shell = std::unique_ptr<Shell>(new Shell(task_runners, settings));

  // Create the rasterizer on the raster thread.
  std::promise<std::unique_ptr<Rasterizer>> rasterizer_promise;
  auto rasterizer_future = rasterizer_promise.get_future();
  std::promise<fml::WeakPtr<SnapshotDelegate>> snapshot_delegate_promise;
  auto snapshot_delegate_future = snapshot_delegate_promise.get_future();
  fml::TaskRunner::RunNowOrPostTask(
      task_runners.GetRasterTaskRunner(), [&rasterizer_promise,  //
                                           &snapshot_delegate_promise,
                                           on_create_rasterizer,  //
                                           shell = shell.get()    //
  ]() {
        TRACE_EVENT0("uiwidgets", "ShellSetupGPUSubsystem");
        std::unique_ptr<Rasterizer> rasterizer(on_create_rasterizer(*shell));
        snapshot_delegate_promise.set_value(rasterizer->GetSnapshotDelegate());
        rasterizer_promise.set_value(std::move(rasterizer));
      });

  // Create the platform view on the platform thread (this thread).
  auto platform_view = on_create_platform_view(*shell.get());
  if (!platform_view || !platform_view->GetWeakPtr()) {
    snapshot_delegate_future.get();
    rasterizer_future.get();
    return nullptr;
  }

  // Ask the platform view for the vsync waiter. This will be used by the engine
  // to create the animator.
  auto vsync_waiter = platform_view->CreateVSyncWaiter();
  if (!vsync_waiter) {
    snapshot_delegate_future.get();
    rasterizer_future.get();
    return nullptr;
  }

  // Create the IO manager on the IO thread. The IO manager must be initialized
  // first because it has state that the other subsystems depend on. It must
  // first be booted and the necessary references obtained to initialize the
  // other subsystems.
  std::promise<std::unique_ptr<ShellIOManager>> io_manager_promise;
  auto io_manager_future = io_manager_promise.get_future();
  std::promise<fml::WeakPtr<ShellIOManager>> weak_io_manager_promise;
  auto weak_io_manager_future = weak_io_manager_promise.get_future();
  std::promise<fml::RefPtr<SkiaUnrefQueue>> unref_queue_promise;
  auto unref_queue_future = unref_queue_promise.get_future();
  auto io_task_runner = shell->GetTaskRunners().GetIOTaskRunner();

  // TODO(gw280): The WeakPtr here asserts that we are derefing it on the
  // same thread as it was created on. We are currently on the IO thread
  // inside this lambda but we need to deref the PlatformView, which was
  // constructed on the platform thread.
  //
  // https://github.com/flutter/flutter/issues/42948
  fml::TaskRunner::RunNowOrPostTask(
      io_task_runner,
      [&io_manager_promise,                                               //
       &weak_io_manager_promise,                                          //
       &unref_queue_promise,                                              //
       platform_view = platform_view->GetWeakPtr(),                       //
       io_task_runner,                                                    //
       is_backgrounded_sync_switch = shell->GetIsGpuDisabledSyncSwitch()  //
  ]() {
        TRACE_EVENT0("uiwidgets", "ShellSetupIOSubsystem");
        auto io_manager = std::make_unique<ShellIOManager>(
            platform_view.getUnsafe()->CreateResourceContext(),
            is_backgrounded_sync_switch, io_task_runner);
        weak_io_manager_promise.set_value(io_manager->GetWeakPtr());
        unref_queue_promise.set_value(io_manager->GetSkiaUnrefQueue());
        io_manager_promise.set_value(std::move(io_manager));
      });

  // Send dispatcher_maker to the engine constructor because shell won't have
  // platform_view set until Shell::Setup is called later.
  auto dispatcher_maker = platform_view->GetDispatcherMaker();

  // Create the engine on the UI thread.
  std::promise<std::unique_ptr<Engine>> engine_promise;
  auto engine_future = engine_promise.get_future();
  fml::TaskRunner::RunNowOrPostTask(
      shell->GetTaskRunners().GetUITaskRunner(),
      fml::MakeCopyable([&engine_promise,                         //
                         shell = shell.get(),                     //
                         &dispatcher_maker,                       //
                         &window_data,                            //
                         vsync_waiter = std::move(vsync_waiter),  //
                         &weak_io_manager_future,                 //
                         &snapshot_delegate_future,               //
                         &unref_queue_future                      //
  ]() mutable {
        TRACE_EVENT0("uiwidgets", "ShellSetupUISubsystem");
        const auto& task_runners = shell->GetTaskRunners();

        // The animator is owned by the UI thread but it gets its vsync pulses
        // from the platform.
        auto animator = std::make_unique<Animator>(*shell, task_runners,
                                                   std::move(vsync_waiter));

        engine_promise.set_value(std::make_unique<Engine>(
            *shell,                         //
            dispatcher_maker,               //
            task_runners,                   //
            window_data,                    //
            shell->GetSettings(),           //
            std::move(animator),            //
            weak_io_manager_future.get(),   //
            unref_queue_future.get(),       //
            snapshot_delegate_future.get()  //
            ));
      }));

  if (!shell->Setup(std::move(platform_view),  //
                    engine_future.get(),       //
                    rasterizer_future.get(),   //
                    io_manager_future.get())   //
  ) {
    return nullptr;
  }

  return shell;
}

static void RecordStartupTimestamp() {
  if (engine_main_enter_ts == 0) {
    engine_main_enter_ts = Mono_TimelineGetMicros();
  }
}

static void Tokenize(const std::string& input,
                     std::vector<std::string>* results, char delimiter) {
  std::istringstream ss(input);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    results->push_back(token);
  }
}

// Though there can be multiple shells, some settings apply to all components in
// the process. These have to be setup before the shell or any of its
// sub-components can be initialized. In a perfect world, this would be empty.
// TODO(chinmaygarde): The unfortunate side effect of this call is that settings
// that cause shell initialization failures will still lead to some of their
// settings being applied.
static void PerformInitializationTasks(const Settings& settings) {
  {
    fml::LogSettings log_settings;
    log_settings.min_log_level =
        settings.verbose_logging ? fml::LOG_INFO : fml::LOG_ERROR;
    fml::SetLogSettings(log_settings);
  }

  static std::once_flag gShellSettingsInitialization = {};
  std::call_once(gShellSettingsInitialization, [&settings] {
    RecordStartupTimestamp();

    if (settings.trace_skia) {
      // InitSkiaEventTracer(settings.trace_skia);
    }

    if (!settings.trace_whitelist.empty()) {
      std::vector<std::string> prefixes;
      Tokenize(settings.trace_whitelist, &prefixes, ',');
      fml::tracing::TraceSetWhitelist(prefixes);
    }

    if (!settings.skia_deterministic_rendering_on_cpu) {
      SkGraphics::Init();
    } else {
      FML_DLOG(INFO) << "Skia deterministic rendering is enabled.";
    }

    if (settings.icu_initialization_required) {
      if (settings.icu_data_path.length() > 0) {
        uiwidgets::icu::InitializeICU(settings.icu_data_path);
      } else if (settings.icu_mapper) {
        uiwidgets::icu::InitializeICUFromMapping(settings.icu_mapper());
      } else {
        FML_DLOG(WARNING) << "Skipping ICU initialization in the shell.";
      }
    }
  });
}

std::unique_ptr<Shell> Shell::Create(
    TaskRunners task_runners, Settings settings,
    const Shell::CreateCallback<PlatformView>& on_create_platform_view,
    const Shell::CreateCallback<Rasterizer>& on_create_rasterizer) {
  return Shell::Create(std::move(task_runners),                //
                       WindowData{/* default window data */},  //
                       std::move(settings),                    //
                       std::move(on_create_platform_view),     //
                       std::move(on_create_rasterizer)         //
  );
}

std::unique_ptr<Shell> Shell::Create(
    TaskRunners task_runners, const WindowData window_data, Settings settings,
    const Shell::CreateCallback<PlatformView>& on_create_platform_view,
    const Shell::CreateCallback<Rasterizer>& on_create_rasterizer) {
  PerformInitializationTasks(settings);
  PersistentCache::SetCacheSkSL(settings.cache_sksl);

  TRACE_EVENT0("uiwidgets", "Shell::CreateWithSnapshots");

  if (!task_runners.IsValid() || !on_create_platform_view ||
      !on_create_rasterizer) {
    return nullptr;
  }

  fml::AutoResetWaitableEvent latch;
  std::unique_ptr<Shell> shell;
  fml::TaskRunner::RunNowOrPostTask(
      task_runners.GetPlatformTaskRunner(),
      fml::MakeCopyable([&latch,                                  //
                         &shell,                                  //
                         task_runners = std::move(task_runners),  //
                         window_data,                             //
                         settings,                                //
                         on_create_platform_view,                 //
                         on_create_rasterizer                     //
  ]() mutable {
        shell = CreateShellOnPlatformThread(std::move(task_runners),  //
                                            window_data,              //
                                            settings,                 //
                                            on_create_platform_view,  //
                                            on_create_rasterizer      //
        );
        latch.Signal();
      }));
  latch.Wait();
  return shell;
}

Shell::Shell(TaskRunners task_runners, Settings settings)
    : task_runners_(std::move(task_runners)),
      settings_(std::move(settings)),
      is_gpu_disabled_sync_switch_(new fml::SyncSwitch()),
      weak_factory_(this),
      weak_factory_gpu_(nullptr) {
  FML_DCHECK(task_runners_.IsValid());
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  // Generate a WeakPtrFactory for use with the raster thread. This does not
  // need to wait on a latch because it can only ever be used from the raster
  // thread from this class, so we have ordering guarantees.
  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetRasterTaskRunner(), fml::MakeCopyable([this]() mutable {
        this->weak_factory_gpu_ =
            std::make_unique<fml::WeakPtrFactory<Shell>>(this);
      }));
}

Shell::~Shell() {
  PersistentCache::GetCacheForProcess()->RemoveWorkerTaskRunner(
      task_runners_.GetIOTaskRunner());

  fml::AutoResetWaitableEvent ui_latch, gpu_latch, platform_latch, io_latch;

  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetUITaskRunner(),
      fml::MakeCopyable(
          [engine = std::move(engine_), &ui_latch,
           task_runner = task_runners_.GetUITaskRunner()]() mutable {
            engine.reset();
            ui_latch.Signal();

            // assume it's always EmbedderTaskRunner
            static_cast<EmbedderTaskRunner*>(task_runner.get())->Terminate();
          }));
  ui_latch.Wait();

  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetRasterTaskRunner(),
      fml::MakeCopyable(
          [rasterizer = std::move(rasterizer_),
           weak_factory_gpu = std::move(weak_factory_gpu_), &gpu_latch,
           task_runner = task_runners_.GetRasterTaskRunner()]() mutable {
            rasterizer.reset();
            weak_factory_gpu.reset();
            gpu_latch.Signal();

            // assume it's always EmbedderTaskRunner
            static_cast<EmbedderTaskRunner*>(task_runner.get())->Terminate();
          }));
  gpu_latch.Wait();

  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetIOTaskRunner(),
      fml::MakeCopyable([io_manager = std::move(io_manager_),
                         platform_view = platform_view_.get(),
                         &io_latch]() mutable {
        io_manager.reset();
        if (platform_view) {
          platform_view->ReleaseResourceContext();
        }
        io_latch.Signal();
      }));

  io_latch.Wait();

  // The platform view must go last because it may be holding onto platform side
  // counterparts to resources owned by subsystems running on other threads. For
  // example, the NSOpenGLContext on the Mac.
  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetPlatformTaskRunner(),
      fml::MakeCopyable([platform_view = std::move(platform_view_),
                         &platform_latch]() mutable {
        platform_view.reset();
        platform_latch.Signal();
      }));
  platform_latch.Wait();
}

void Shell::NotifyLowMemoryWarning() const {
  // This does not require a current isolate but does require a running VM.
  // Since a valid shell will not be returned to the embedder without a valid
  // DartVMRef, we can be certain that this is a safe spot to assume a VM is
  // running.
  // ::Dart_NotifyLowMemory();

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr()]() {
        if (rasterizer) {
          rasterizer->NotifyLowMemoryWarning();
        }
      });
  // The IO Manager uses resource cache limits of 0, so it is not necessary
  // to purge them.
}

void Shell::RunEngine(RunConfiguration run_configuration) {
  RunEngine(std::move(run_configuration), nullptr);
}

void Shell::RunEngine(
    RunConfiguration run_configuration,
    const std::function<void(Engine::RunStatus)>& result_callback) {
  auto result = [platform_runner = task_runners_.GetPlatformTaskRunner(),
                 result_callback](Engine::RunStatus run_result) {
    if (!result_callback) {
      return;
    }
    platform_runner->PostTask(
        [result_callback, run_result]() { result_callback(run_result); });
  };
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetUITaskRunner(),
      fml::MakeCopyable(
          [run_configuration = std::move(run_configuration),
           weak_engine = weak_engine_, result]() mutable {
            if (!weak_engine) {
              FML_LOG(ERROR)
                  << "Could not launch engine with configuration - no engine.";
              result(Engine::RunStatus::Failure);
              return;
            }
            auto run_result = weak_engine->Run(std::move(run_configuration));
            if (run_result == Engine::RunStatus::Failure) {
              FML_LOG(ERROR) << "Could not launch engine with configuration.";
            }
            result(run_result);
          }));
}

bool Shell::IsSetup() const { return is_setup_; }

bool Shell::Setup(std::unique_ptr<PlatformView> platform_view,
                  std::unique_ptr<Engine> engine,
                  std::unique_ptr<Rasterizer> rasterizer,
                  std::unique_ptr<ShellIOManager> io_manager) {
  if (is_setup_) {
    return false;
  }

  if (!platform_view || !engine || !rasterizer || !io_manager) {
    return false;
  }

  platform_view_ = std::move(platform_view);
  engine_ = std::move(engine);
  rasterizer_ = std::move(rasterizer);
  io_manager_ = std::move(io_manager);

  // The weak ptr must be generated in the platform thread which owns the unique
  // ptr.
  weak_engine_ = engine_->GetWeakPtr();
  weak_rasterizer_ = rasterizer_->GetWeakPtr();
  weak_platform_view_ = platform_view_->GetWeakPtr();

  fml::TaskRunner::RunNowOrPostTask(task_runners_.GetUITaskRunner(),
                                    [engine = weak_engine_] {
                                      if (engine) {
                                        engine->SetupDefaultFontManager();
                                      }
                                    });

  is_setup_ = true;

  PersistentCache::GetCacheForProcess()->AddWorkerTaskRunner(
      task_runners_.GetIOTaskRunner());

  PersistentCache::GetCacheForProcess()->SetIsDumpingSkp(
      settings_.dump_skp_on_shader_compilation);

  // TODO(gw280): The WeakPtr here asserts that we are derefing it on the
  // same thread as it was created on. Shell is constructed on the platform
  // thread but we need to call into the Engine on the UI thread, so we need
  // to use getUnsafe() here to avoid failing the assertion.
  //
  // https://github.com/flutter/flutter/issues/42947
  display_refresh_rate_ = weak_engine_.getUnsafe()->GetDisplayRefreshRate();

  return true;
}

const Settings& Shell::GetSettings() const { return settings_; }

const TaskRunners& Shell::GetTaskRunners() const { return task_runners_; }

fml::WeakPtr<Rasterizer> Shell::GetRasterizer() const {
  FML_DCHECK(is_setup_);
  return weak_rasterizer_;
}

fml::WeakPtr<Engine> Shell::GetEngine() {
  FML_DCHECK(is_setup_);
  return weak_engine_;
}

fml::WeakPtr<PlatformView> Shell::GetPlatformView() {
  FML_DCHECK(is_setup_);
  return weak_platform_view_;
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewCreated(std::unique_ptr<Surface> surface) {
  TRACE_EVENT0("uiwidgets", "Shell::OnPlatformViewCreated");
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  // Note:
  // This is a synchronous operation because certain platforms depend on
  // setup/suspension of all activities that may be interacting with the GPU in
  // a synchronous fashion.
  fml::AutoResetWaitableEvent latch;
  auto raster_task =
      fml::MakeCopyable([&waiting_for_first_frame = waiting_for_first_frame_,
                         rasterizer = rasterizer_->GetWeakPtr(),  //
                         surface = std::move(surface),            //
                         &latch]() mutable {
        if (rasterizer) {
          rasterizer->Setup(std::move(surface));
        }

        waiting_for_first_frame.store(true);

        // Step 3: All done. Signal the latch that the platform thread is
        // waiting on.
        latch.Signal();
      });

  // The normal flow executed by this method is that the platform thread is
  // starting the sequence and waiting on the latch. Later the UI thread posts
  // raster_task to the raster thread which signals the latch. If the raster and
  // the platform threads are the same this results in a deadlock as the
  // raster_task will never be posted to the plaform/raster thread that is
  // blocked on a latch. To avoid the described deadlock, if the raster and the
  // platform threads are the same, should_post_raster_task will be false, and
  // then instead of posting a task to the raster thread, the ui thread just
  // signals the latch and the platform/raster thread follows with executing
  // raster_task.
  bool should_post_raster_task = task_runners_.GetRasterTaskRunner() !=
                                 task_runners_.GetPlatformTaskRunner();

  auto ui_task = [engine = engine_->GetWeakPtr(),                            //
                  raster_task_runner = task_runners_.GetRasterTaskRunner(),  //
                  raster_task, should_post_raster_task,
                  &latch  //
  ] {
    if (engine) {
      engine->OnOutputSurfaceCreated();
    }
    // Step 2: Next, tell the raster thread that it should create a surface for
    // its rasterizer.
    if (should_post_raster_task) {
      fml::TaskRunner::RunNowOrPostTask(raster_task_runner, raster_task);
    } else {
      // See comment on should_post_raster_task, in this case we just unblock
      // the platform thread.
      latch.Signal();
    }
  };

  // Threading: Capture platform view by raw pointer and not the weak pointer.
  // We are going to use the pointer on the IO thread which is not safe with a
  // weak pointer. However, we are preventing the platform view from being
  // collected by using a latch.
  auto* platform_view = platform_view_.get();

  FML_DCHECK(platform_view);

  bool should_post_ui_task =
      task_runners_.GetUITaskRunner() != task_runners_.GetPlatformTaskRunner();

  auto io_task = [io_manager = io_manager_->GetWeakPtr(), platform_view,
                  ui_task_runner = task_runners_.GetUITaskRunner(), ui_task,
                  should_post_ui_task, &latch] {
    if (io_manager && !io_manager->GetResourceContext()) {
      io_manager->NotifyResourceContextAvailable(
          platform_view->CreateResourceContext());
    }

    // Step 1: Next, post a task on the UI thread to tell the engine that it has
    // an output surface.
    if (should_post_ui_task) {
      fml::TaskRunner::RunNowOrPostTask(ui_task_runner, ui_task);
    } else {
      latch.Signal();
    }
  };

  fml::TaskRunner::RunNowOrPostTask(task_runners_.GetIOTaskRunner(), io_task);

  latch.Wait();

  if (!should_post_ui_task) {
    ui_task();
    latch.Wait();
  }

  if (!should_post_raster_task) {
    // See comment on should_post_raster_task, in this case the raster_task
    // wasn't executed, and we just run it here as the platform thread
    // is the raster thread.
    raster_task();
    latch.Wait();
  }
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewDestroyed() {
  TRACE_EVENT0("uiwidgets", "Shell::OnPlatformViewDestroyed");
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  // Note:
  // This is a synchronous operation because certain platforms depend on
  // setup/suspension of all activities that may be interacting with the GPU in
  // a synchronous fashion.

  fml::AutoResetWaitableEvent latch;

  auto io_task = [io_manager = io_manager_.get(), &latch]() {
    // Execute any pending Skia object deletions while GPU access is still
    // allowed.
    io_manager->GetIsGpuDisabledSyncSwitch()->Execute(
        fml::SyncSwitch::Handlers().SetIfFalse(
            [&] { io_manager->GetSkiaUnrefQueue()->Drain(); }));
    // Step 3: All done. Signal the latch that the platform thread is waiting
    // on.
    latch.Signal();
  };

  auto raster_task = [rasterizer = rasterizer_->GetWeakPtr(),
                      io_task_runner = task_runners_.GetIOTaskRunner(),
                      io_task]() {
    if (rasterizer) {
      rasterizer->Teardown();
    }

    // Step 2: Next, tell the IO thread to complete its remaining work.
    fml::TaskRunner::RunNowOrPostTask(io_task_runner, io_task);
  };

  // The normal flow executed by this method is that the platform thread is
  // starting the sequence and waiting on the latch. Later the UI thread posts
  // raster_task to the raster thread triggers signaling the latch(on the IO
  // thread). If the raster and the platform threads are the same this results
  // in a deadlock as the raster_task will never be posted to the plaform/raster
  // thread that is blocked on a latch.  To avoid the described deadlock, if the
  // raster and the platform threads are the same, should_post_raster_task will
  // be false, and then instead of posting a task to the raster thread, the ui
  // thread just signals the latch and the platform/raster thread follows with
  // executing raster_task.
  bool should_post_raster_task = task_runners_.GetRasterTaskRunner() !=
                                 task_runners_.GetPlatformTaskRunner();

  auto ui_task = [engine = engine_->GetWeakPtr(),
                  ui_task_runner = task_runners_.GetUITaskRunner(),
                  raster_task_runner = task_runners_.GetRasterTaskRunner(),
                  raster_task, should_post_raster_task, &latch]() {
    if (engine) {
      engine->OnOutputSurfaceDestroyed();
    }

    // Step 1: Next, tell the raster thread that its rasterizer should suspend
    // access to the underlying surface.
    if (should_post_raster_task) {
      fml::TaskRunner::RunNowOrPostTask(raster_task_runner, raster_task);
    } else {
      // See comment on should_post_raster_task, in this case we just unblock
      // the platform thread.
      latch.Signal();
    }
  };

  // Step 0: Post a task onto the UI thread to tell the engine that its output
  // surface is about to go away.
  fml::TaskRunner::RunNowOrPostTask(task_runners_.GetUITaskRunner(), ui_task);
  latch.Wait();

  if (!should_post_raster_task) {
    // See comment on should_post_raster_task, in this case the raster_task
    // wasn't executed, and we just run it here as the platform thread
    // is the raster thread.
    raster_task();
    latch.Wait();
  }
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewSetViewportMetrics(const ViewportMetrics& metrics) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  // This is the formula Android uses.
  // https://android.googlesource.com/platform/frameworks/base/+/master/libs/hwui/renderthread/CacheManager.cpp#41
  size_t max_bytes = metrics.physical_width * metrics.physical_height * 12 * 4;
  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), max_bytes] {
        if (rasterizer) {
          rasterizer->SetResourceCacheMaxBytes(max_bytes, false);
        }
      });

  task_runners_.GetUITaskRunner()->PostTask(
      [engine = engine_->GetWeakPtr(), metrics]() {
        if (engine) {
          engine->SetViewportMetrics(metrics);
        }
      });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewDispatchPlatformMessage(
    fml::RefPtr<PlatformMessage> message) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  task_runners_.GetUITaskRunner()->PostTask(
      [engine = engine_->GetWeakPtr(), message = std::move(message)] {
        if (engine) {
          engine->DispatchPlatformMessage(std::move(message));
        }
      });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewDispatchPointerDataPacket(
    std::unique_ptr<PointerDataPacket> packet) {
  TRACE_EVENT0("uiwidgets", "Shell::OnPlatformViewDispatchPointerDataPacket");
  TRACE_FLOW_BEGIN("uiwidgets", "PointerEvent", next_pointer_flow_id_);
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());
  task_runners_.GetUITaskRunner()->PostTask(
      fml::MakeCopyable([engine = weak_engine_, packet = std::move(packet),
                         flow_id = next_pointer_flow_id_]() mutable {
        if (engine) {
          engine->DispatchPointerDataPacket(std::move(packet), flow_id);
        }
      }));
  next_pointer_flow_id_++;
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewSetAccessibilityFeatures(int32_t flags) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  task_runners_.GetUITaskRunner()->PostTask(
      [engine = engine_->GetWeakPtr(), flags] {
        if (engine) {
          engine->SetAccessibilityFeatures(flags);
        }
      });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewRegisterTexture(std::shared_ptr<Texture> texture) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), texture] {
        if (rasterizer) {
          if (auto* registry = rasterizer->GetTextureRegistry()) {
            registry->RegisterTexture(texture);
          }
        }
      });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewUnregisterTexture(int64_t texture_id) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), texture_id]() {
        if (rasterizer) {
          if (auto* registry = rasterizer->GetTextureRegistry()) {
            registry->UnregisterTexture(texture_id);
          }
        }
      });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewMarkTextureFrameAvailable(int64_t texture_id) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  // Tell the rasterizer that one of its textures has a new frame available.
  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), texture_id]() {
        auto* registry = rasterizer->GetTextureRegistry();

        if (!registry) {
          return;
        }

        auto texture = registry->GetTexture(texture_id);

        if (!texture) {
          return;
        }

        texture->MarkNewFrameAvailable();
      });

  // Schedule a new frame without having to rebuild the layer tree.
  task_runners_.GetUITaskRunner()->PostTask([engine = engine_->GetWeakPtr()]() {
    if (engine) {
      engine->ScheduleFrame(false);
    }
  });
}

// |PlatformView::Delegate|
void Shell::OnPlatformViewSetNextFrameCallback(const fml::closure& closure) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), closure = closure]() {
        if (rasterizer) {
          rasterizer->SetNextFrameCallback(std::move(closure));
        }
      });
}

// |Animator::Delegate|
void Shell::OnAnimatorBeginFrame(fml::TimePoint frame_time) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetUITaskRunner()->RunsTasksOnCurrentThread());

  if (engine_) {
    engine_->BeginFrame(frame_time);
  }
}

// |Animator::Delegate|
void Shell::OnAnimatorNotifyIdle(int64_t deadline) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetUITaskRunner()->RunsTasksOnCurrentThread());

  if (engine_) {
    engine_->NotifyIdle(deadline);
  }
}

// |Animator::Delegate|
void Shell::OnAnimatorDraw(fml::RefPtr<Pipeline<LayerTree>> pipeline) {
  FML_DCHECK(is_setup_);

  task_runners_.GetRasterTaskRunner()->PostTask(
      [&waiting_for_first_frame = waiting_for_first_frame_,
       &waiting_for_first_frame_condition = waiting_for_first_frame_condition_,
       rasterizer = rasterizer_->GetWeakPtr(),
       pipeline = std::move(pipeline)]() {
        if (rasterizer) {
          rasterizer->Draw(pipeline);

          if (waiting_for_first_frame.load()) {
            waiting_for_first_frame.store(false);
            waiting_for_first_frame_condition.notify_all();
          }
        }
      });
}

// |Animator::Delegate|
void Shell::OnAnimatorDrawLastLayerTree() {
  FML_DCHECK(is_setup_);

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr()]() {
        if (rasterizer) {
          rasterizer->DrawLastLayerTree();
        }
      });
}

// |Engine::Delegate|
void Shell::OnEngineHandlePlatformMessage(
    fml::RefPtr<PlatformMessage> message) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetUITaskRunner()->RunsTasksOnCurrentThread());

  if (message->channel() == kSkiaChannel) {
    HandleEngineSkiaMessage(std::move(message));
    return;
  }

  task_runners_.GetPlatformTaskRunner()->PostTask(
      [view = platform_view_->GetWeakPtr(), message = std::move(message)]() {
        if (view) {
          view->HandlePlatformMessage(std::move(message));
        }
      });
}

void Shell::HandleEngineSkiaMessage(fml::RefPtr<PlatformMessage> message) {
  const auto& data = message->data();

  rapidjson::Document document;
  document.Parse(reinterpret_cast<const char*>(data.data()), data.size());
  if (document.HasParseError() || !document.IsObject()) return;
  auto root = document.GetObject();
  auto method = root.FindMember("method");
  if (method->value != "Skia.setResourceCacheMaxBytes") return;
  auto args = root.FindMember("args");
  if (args == root.MemberEnd() || !args->value.IsInt()) return;

  task_runners_.GetRasterTaskRunner()->PostTask(
      [rasterizer = rasterizer_->GetWeakPtr(), max_bytes = args->value.GetInt(),
       response = std::move(message->response())] {
        if (rasterizer) {
          rasterizer->SetResourceCacheMaxBytes(static_cast<size_t>(max_bytes),
                                               true);
        }
        if (response) {
          // The framework side expects this to be valid json encoded as a list.
          // Return `[true]` to signal success.
          std::vector<uint8_t> data = {'[', 't', 'r', 'u', 'e', ']'};
          response->Complete(
              std::make_unique<fml::DataMapping>(std::move(data)));
        }
      });
}

// |Engine::Delegate|
void Shell::OnPreEngineRestart() {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetUITaskRunner()->RunsTasksOnCurrentThread());

  fml::AutoResetWaitableEvent latch;
  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetPlatformTaskRunner(),
      [view = platform_view_->GetWeakPtr(), &latch]() {
        if (view) {
          view->OnPreEngineRestart();
        }
        latch.Signal();
      });
  // This is blocking as any embedded platform views has to be flushed before
  // we re-run the Dart code.
  latch.Wait();
}

void Shell::SetNeedsReportTimings(bool value) { needs_report_timings_ = value; }

void Shell::ReportTimings() {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetRasterTaskRunner()->RunsTasksOnCurrentThread());

  auto timings = std::move(unreported_timings_);
  unreported_timings_ = {};
  task_runners_.GetUITaskRunner()->PostTask([timings, engine = weak_engine_] {
    if (engine) {
      engine->ReportTimings(std::move(timings));
    }
  });
}

size_t Shell::UnreportedFramesCount() const {
  // Check that this is running on the raster thread to avoid race conditions.
  FML_DCHECK(task_runners_.GetRasterTaskRunner()->RunsTasksOnCurrentThread());
  FML_DCHECK(unreported_timings_.size() % FrameTiming::kCount == 0);
  return unreported_timings_.size() / FrameTiming::kCount;
}

void Shell::OnFrameRasterized(const FrameTiming& timing) {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetRasterTaskRunner()->RunsTasksOnCurrentThread());

  // The C++ callback defined in settings.h and set by Flutter runner. This is
  // independent of the timings report to the Dart side.
  if (settings_.frame_rasterized_callback) {
    settings_.frame_rasterized_callback(timing);
  }

  if (!needs_report_timings_) {
    return;
  }

  for (auto phase : FrameTiming::kPhases) {
    unreported_timings_.push_back(
        timing.Get(phase).ToEpochDelta().ToMicroseconds());
  }

  // In tests using iPhone 6S with profile mode, sending a batch of 1 frame or a
  // batch of 100 frames have roughly the same cost of less than 0.1ms. Sending
  // a batch of 500 frames costs about 0.2ms. The 1 second threshold usually
  // kicks in before we reaching the following 100 frames threshold. The 100
  // threshold here is mainly for unit tests (so we don't have to write a
  // 1-second unit test), and make sure that our vector won't grow too big with
  // future 120fps, 240fps, or 1000fps displays.
  //
  // In the profile/debug mode, the timings are used by development tools which
  // require a latency of no more than 100ms. Hence we lower that 1-second
  // threshold to 100ms because performance overhead isn't that critical in
  // those cases.
  if (!first_frame_rasterized_ || UnreportedFramesCount() >= 100) {
    first_frame_rasterized_ = true;
    ReportTimings();
  } else if (!frame_timings_report_scheduled_) {
#if UIWIDGETS_RELEASE
    constexpr int kBatchTimeInMilliseconds = 1000;
#else
    constexpr int kBatchTimeInMilliseconds = 100;
#endif

    // Also make sure that frame times get reported with a max latency of 1
    // second. Otherwise, the timings of last few frames of an animation may
    // never be reported until the next animation starts.
    frame_timings_report_scheduled_ = true;
	
	task_runners_.GetUITaskRunner()->PostDelayedTask(
		[self = this]() {
			self->task_runners_.GetRasterTaskRunner()->PostTask(
				[self2 = self->weak_factory_gpu_->GetWeakPtr()]() {
				if (!self2.get()) {
					return;
				}
				self2->frame_timings_report_scheduled_ = false;
				if (self2->UnreportedFramesCount() > 0) {
					self2->ReportTimings();
				}
			});
		},
		fml::TimeDelta::FromMilliseconds(kBatchTimeInMilliseconds)
	);   
  }
}

fml::Milliseconds Shell::GetFrameBudget() {
  if (display_refresh_rate_ > 0) {
    return fml::RefreshRateToFrameBudget(display_refresh_rate_.load());
  } else {
    return fml::kDefaultFrameBudget;
  }
}

Rasterizer::Screenshot Shell::Screenshot(
    Rasterizer::ScreenshotType screenshot_type, bool base64_encode) {
  TRACE_EVENT0("uiwidgets", "Shell::Screenshot");
  fml::AutoResetWaitableEvent latch;
  Rasterizer::Screenshot screenshot;
  fml::TaskRunner::RunNowOrPostTask(
      task_runners_.GetRasterTaskRunner(), [&latch,                        //
                                            rasterizer = GetRasterizer(),  //
                                            &screenshot,                   //
                                            screenshot_type,               //
                                            base64_encode                  //
  ]() {
        if (rasterizer) {
          screenshot = rasterizer->ScreenshotLastLayerTree(screenshot_type,
                                                           base64_encode);
        }
        latch.Signal();
      });
  latch.Wait();
  return screenshot;
}

fml::Status Shell::WaitForFirstFrame(fml::TimeDelta timeout) {
  FML_DCHECK(is_setup_);
  if (task_runners_.GetUITaskRunner()->RunsTasksOnCurrentThread() ||
      task_runners_.GetRasterTaskRunner()->RunsTasksOnCurrentThread()) {
    return fml::Status(fml::StatusCode::kFailedPrecondition,
                       "WaitForFirstFrame called from thread that can't wait "
                       "because it is responsible for generating the frame.");
  }

  std::unique_lock<std::mutex> lock(waiting_for_first_frame_mutex_);
  bool success = waiting_for_first_frame_condition_.wait_for(
      lock, std::chrono::milliseconds(timeout.ToMilliseconds()),
      [&waiting_for_first_frame = waiting_for_first_frame_] {
        return !waiting_for_first_frame.load();
      });
  if (success) {
    return fml::Status();
  } else {
    return fml::Status(fml::StatusCode::kDeadlineExceeded, "timeout");
  }
}

bool Shell::ReloadSystemFonts() {
  FML_DCHECK(is_setup_);
  FML_DCHECK(task_runners_.GetPlatformTaskRunner()->RunsTasksOnCurrentThread());

  if (!engine_) {
    return false;
  }
  // engine_->GetFontCollection().GetFontCollection()->SetupDefaultFontManager();
  // engine_->GetFontCollection().GetFontCollection()->ClearFontFamilyCache();
  // After system fonts are reloaded, we send a system channel message
  // to notify flutter framework.
  rapidjson::Document document;
  document.SetObject();
  auto& allocator = document.GetAllocator();
  rapidjson::Value message_value;
  message_value.SetString(kFontChange, allocator);
  document.AddMember(kTypeKey, message_value, allocator);

  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  document.Accept(writer);
  std::string message = buffer.GetString();
  fml::RefPtr<PlatformMessage> fontsChangeMessage =
      fml::MakeRefCounted<PlatformMessage>(
          kSystemChannel, std::vector<uint8_t>(message.begin(), message.end()),
          nullptr);

  OnPlatformViewDispatchPlatformMessage(fontsChangeMessage);
  return true;
}

std::shared_ptr<fml::SyncSwitch> Shell::GetIsGpuDisabledSyncSwitch() const {
  return is_gpu_disabled_sync_switch_;
}

}  // namespace uiwidgets
