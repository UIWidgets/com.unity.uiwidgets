/*
    WARNING!!!: GENERATED BY CODE, DO NOT CHANGE !!!!!!
    A file with the suffix .gen.cs is automatically generate from its corresponding template file with suffix .njk using nunjucks.
    If you want to update this file, please edit the corresponding .njk file first and execute the following cmd at the Scripts folder:
    "node uiwidgets-cli.js codegen"
*/


using System.Collections.Generic;
using Unity.UIWidgets.rendering;

namespace Unity.UIWidgets.widgets {

 
    public abstract class ViewportNotificationMixinNotification : Notification {
        public int depth {
            get { return _depth; }
        }

        int _depth = 0;

        protected override bool visitAncestor(Element element) {
            if (element is RenderObjectElement && element.renderObject is RenderAbstractViewport) {
                _depth += 1;
            }

            return base.visitAncestor(element);
        }

        protected override void debugFillDescription(List<string> description) {
            base.debugFillDescription(description);
            description.Add(string.Format("depth: {0} ({1})",
                _depth, _depth == 0 ? "local" : "remote"));
        }
    }


 
    public abstract class ViewportNotificationMixinLayoutChangedNotification : LayoutChangedNotification {
        public int depth {
            get { return _depth; }
        }

        int _depth = 0;

        protected override bool visitAncestor(Element element) {
            if (element is RenderObjectElement && element.renderObject is RenderAbstractViewport) {
                _depth += 1;
            }

            return base.visitAncestor(element);
        }

        protected override void debugFillDescription(List<string> description) {
            base.debugFillDescription(description);
            description.Add(string.Format("depth: {0} ({1})",
                _depth, _depth == 0 ? "local" : "remote"));
        }
    }

}