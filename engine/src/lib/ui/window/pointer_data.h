#pragma once

#include <stdint.h>

namespace uiwidgets {

static constexpr int kPointerDataFieldCount = 29;
static constexpr int kBytesPerField = sizeof(int64_t);

enum PointerButtonMouse : int64_t {
  kPointerButtonMousePrimary = 1 << 0,
  kPointerButtonMouseSecondary = 1 << 1,
  kPointerButtonMouseMiddle = 1 << 2,
  kPointerButtonMouseBack = 1 << 3,
  kPointerButtonMouseForward = 1 << 4,
};

enum PointerButtonTouch : int64_t {
  kPointerButtonTouchContact = 1 << 0,
};

enum PointerButtonStylus : int64_t {
  kPointerButtonStylusContact = 1 << 0,
  kPointerButtonStylusPrimary = 1 << 1,
  kPointerButtonStylusSecondary = 1 << 2,
};

#ifdef UIWIDGETS_FORCE_ALIGNAS_8
struct alignas(8) PointerData {
#else
struct alignas(4) PointerData {
#endif
  enum class Change : int64_t {
    kCancel,
    kAdd,
    kRemove,
    kHover,
    kDown,
    kMove,
    kUp,
    kMouseDown,
    kMouseUp,
  };

  enum class DeviceKind : int64_t {
    kTouch,
    kMouse,
    kStylus,
    kInvertedStylus,    
    kKeyboard,
  };

  enum class SignalKind : int64_t {
    kNone,
    kScroll,
  };

  int64_t time_stamp;
  Change change;
  DeviceKind kind;
  SignalKind signal_kind;
  int64_t device;
  int64_t pointer_identifier;
  double physical_x;
  double physical_y;
  double physical_delta_x;
  double physical_delta_y;
  int64_t buttons;
  int64_t modifier;
  int64_t obscured;
  int64_t synthesized;
  double pressure;
  double pressure_min;
  double pressure_max;
  double distance;
  double distance_max;
  double size;
  double radius_major;
  double radius_minor;
  double radius_min;
  double radius_max;
  double orientation;
  double tilt;
  int64_t platformData;
  double scroll_delta_x;
  double scroll_delta_y;

  void Clear();
};

}  // namespace uiwidgets
