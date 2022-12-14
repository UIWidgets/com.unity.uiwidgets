#include "vsync_waiter_fallback.h"

#include "flutter/fml/logging.h"

namespace uiwidgets {
namespace {

static fml::TimePoint SnapToNextTick(fml::TimePoint value,
                                     fml::TimePoint tick_phase,
                                     fml::TimeDelta tick_interval) {
  fml::TimeDelta offset = (tick_phase - value) % tick_interval;
  if (offset != fml::TimeDelta::Zero())
    offset = offset + tick_interval;
  return value + offset;
}

}  // namespace

VsyncWaiterFallback::VsyncWaiterFallback(TaskRunners task_runners)
    : VsyncWaiter(std::move(task_runners)), phase_(fml::TimePoint::Now()) {}

VsyncWaiterFallback::~VsyncWaiterFallback() = default;

// |VsyncWaiter|
void VsyncWaiterFallback::AwaitVSync() {
  constexpr fml::TimeDelta kSingleFrameInterval =
      fml::TimeDelta::FromSecondsF(1.0 / 60.0);

  auto next =
      SnapToNextTick(fml::TimePoint::Now(), phase_, kSingleFrameInterval);

  FireCallback(next, next + kSingleFrameInterval);
}

}  // namespace flutter
