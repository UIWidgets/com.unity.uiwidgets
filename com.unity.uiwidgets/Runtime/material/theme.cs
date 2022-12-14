using System;
using Unity.UIWidgets.animation;
using Unity.UIWidgets.foundation;
using Unity.UIWidgets.ui;
using Unity.UIWidgets.widgets;

namespace Unity.UIWidgets.material {
    public static class ThemeUtils {
        public static readonly TimeSpan kThemeAnimationDuration = new TimeSpan(0, 0, 0, 0, 200);
    }

    public class Theme : StatelessWidget {
        public Theme(
            Key key = null,
            ThemeData data = null,
            bool isMaterialAppTheme = false,
            Widget child = null
        ) : base(key: key) {
            D.assert(child != null);
            D.assert(data != null);
            this.data = data;
            this.isMaterialAppTheme = isMaterialAppTheme;
            this.child = child;
        }

        public readonly ThemeData data;

        public readonly bool isMaterialAppTheme;

        readonly Widget child;

        static readonly ThemeData _kFallbackTheme = ThemeData.fallback();

        public static ThemeData of(BuildContext context, bool shadowThemeOnly = false) {
            _InheritedTheme inheritedTheme = context.dependOnInheritedWidgetOfExactType<_InheritedTheme>();
            if (shadowThemeOnly) {
                if (inheritedTheme == null || inheritedTheme.theme.isMaterialAppTheme) {
                    return null;
                }

                return inheritedTheme.theme.data;
            }

            MaterialLocalizations localizations = MaterialLocalizations.of(context);
            ScriptCategory category = ScriptCategory.englishLike;
            ThemeData theme = inheritedTheme?.theme?.data ?? _kFallbackTheme;
            return ThemeData.localize(theme, theme.typography.geometryThemeFor(category));
        }

        public override Widget build(BuildContext context) {
            return new _InheritedTheme(
                theme: this,
                child: new IconTheme(
                    data: data.iconTheme,
                    child: child
                )
            );
        }

        public override void debugFillProperties(DiagnosticPropertiesBuilder properties) {
            base.debugFillProperties(properties);
            properties.add(new DiagnosticsProperty<ThemeData>("data", data, showName: false));
        }
    }


    class _InheritedTheme : InheritedTheme {
        public _InheritedTheme(
            Key key = null,
            Theme theme = null,
            Widget child = null) : base(key: key, child: child) {
            D.assert(theme != null);
            D.assert(child != null);
            this.theme = theme;
        }

        public readonly Theme theme;

        public override Widget wrap(BuildContext context, Widget child) {
            _InheritedTheme ancestorTheme = context.findAncestorWidgetOfExactType<_InheritedTheme>();
            return ReferenceEquals(this, ancestorTheme) ? child : new Theme(data: theme.data, child: child);
        }

        public override bool updateShouldNotify(InheritedWidget old) {
            return theme.data != ((_InheritedTheme) old).theme.data;
        }
    }

    class ThemeDataTween : Tween<ThemeData> {
        public ThemeDataTween(
            ThemeData begin = null,
            ThemeData end = null
        ) : base(begin: begin, end: end) {
        }

        public override ThemeData lerp(float t) {
            return ThemeData.lerp(begin, end, t);
        }
    }

    class AnimatedTheme : ImplicitlyAnimatedWidget {
        public AnimatedTheme(
            Key key = null,
            ThemeData data = null,
            bool isMaterialAppTheme = false,
            Curve curve = null,
            TimeSpan? duration = null,
            VoidCallback onEnd = null,
            Widget child = null
        ) : base(key: key, curve: curve ?? Curves.linear, duration: duration ?? ThemeUtils.kThemeAnimationDuration, onEnd: onEnd) {
            D.assert(child != null);
            D.assert(data != null);
            this.data = data;
            this.isMaterialAppTheme = isMaterialAppTheme;
            this.child = child;
        }


        public readonly ThemeData data;

        public readonly bool isMaterialAppTheme;

        public readonly Widget child;

        public override State createState() {
            return new _AnimatedThemeState();
        }
    }


    class _AnimatedThemeState : AnimatedWidgetBaseState<AnimatedTheme> {
        ThemeDataTween _data;

        protected override void forEachTween(TweenVisitor visitor) {
            _data = (ThemeDataTween) visitor.visit(this, _data, widget.data,
                (ThemeData value) => new ThemeDataTween(begin: value));
        }

        public override Widget build(BuildContext context) {
            return new Theme(
                isMaterialAppTheme: widget.isMaterialAppTheme,
                child: widget.child,
                data: _data.evaluate(animation)
            );
        }

        public override void debugFillProperties(DiagnosticPropertiesBuilder description) {
            base.debugFillProperties(description);
            description.add(
                new DiagnosticsProperty<ThemeDataTween>("data", _data, showName: false, defaultValue: null));
        }
    }
}