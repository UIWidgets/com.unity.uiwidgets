#include "flow/layers/clip_rrect_layer.h"

namespace uiwidgets {

ClipRRectLayer::ClipRRectLayer(const SkRRect& clip_rrect, Clip clip_behavior)
    : clip_rrect_(clip_rrect), clip_behavior_(clip_behavior) {
  FML_DCHECK(clip_behavior != Clip::none);
}

void ClipRRectLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  TRACE_EVENT0("uiwidgets", "ClipRRectLayer::Preroll");

  SkRect previous_cull_rect = context->cull_rect;
  SkRect clip_rrect_bounds = clip_rrect_.getBounds();
  children_inside_clip_ = context->cull_rect.intersect(clip_rrect_bounds);
  if (children_inside_clip_) {
    TRACE_EVENT_INSTANT0("uiwidgets", "children inside clip rect");

    Layer::AutoPrerollSaveLayerState save =
        Layer::AutoPrerollSaveLayerState::Create(context, UsesSaveLayer());
    context->mutators_stack.PushClipRRect(clip_rrect_);
    SkRect child_paint_bounds = SkRect::MakeEmpty();
    PrerollChildren(context, matrix, &child_paint_bounds);

    if (child_paint_bounds.intersect(clip_rrect_bounds)) {
      set_paint_bounds(child_paint_bounds);
    }
    context->mutators_stack.Pop();
  }
  context->cull_rect = previous_cull_rect;
}

void ClipRRectLayer::Paint(PaintContext& context) const {
  TRACE_EVENT0("uiwidgets", "ClipRRectLayer::Paint");
  FML_DCHECK(needs_painting());

  if (!children_inside_clip_) {
    TRACE_EVENT_INSTANT0("uiwidgets",
                         "children not inside clip rect, skipping");
    return;
  }

  SkAutoCanvasRestore save(context.internal_nodes_canvas, true);
  context.internal_nodes_canvas->clipRRect(clip_rrect_,
                                           clip_behavior_ != Clip::hardEdge);

  if (UsesSaveLayer()) {
    context.internal_nodes_canvas->saveLayer(paint_bounds(), nullptr);
  }
  PaintChildren(context);
  if (UsesSaveLayer()) {
    context.internal_nodes_canvas->restore();
  }
}

}  // namespace uiwidgets
