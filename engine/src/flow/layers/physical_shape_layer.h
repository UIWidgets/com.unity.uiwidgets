#pragma once

#include "flow/layers/container_layer.h"

namespace uiwidgets {

class PhysicalShapeLayer : public ContainerLayer {
 public:
  PhysicalShapeLayer(SkColor color, SkColor shadow_color, float elevation,
                     const SkPath& path, Clip clip_behavior);

  static SkRect ComputeShadowBounds(const SkRect& bounds, float elevation,
                                    float pixel_ratio);
  static void DrawShadow(SkCanvas* canvas, const SkPath& path, SkColor color,
                         float elevation, bool transparentOccluder,
                         SkScalar dpr);

  void Preroll(PrerollContext* context, const SkMatrix& matrix) override;

  void Paint(PaintContext& context) const override;

  bool UsesSaveLayer() const {
    return clip_behavior_ == Clip::antiAliasWithSaveLayer;
  }

  float total_elevation() const { return total_elevation_; }

 private:
  SkColor color_;
  SkColor shadow_color_;
  float elevation_ = 0.0f;
  float total_elevation_ = 0.0f;
  SkPath path_;
  bool isRect_;
  SkRRect frameRRect_;
  Clip clip_behavior_;
};

}  // namespace uiwidgets
