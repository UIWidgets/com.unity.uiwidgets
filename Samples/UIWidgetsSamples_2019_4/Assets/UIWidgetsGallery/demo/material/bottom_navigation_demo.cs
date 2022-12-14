using System.Collections.Generic;
using System.Linq;
using uiwidgets;
using UIWidgetsGallery.gallery;
using Unity.UIWidgets.animation;
using Unity.UIWidgets.material;
using Unity.UIWidgets.painting;
using Unity.UIWidgets.scheduler;
using Unity.UIWidgets.ui;
using Unity.UIWidgets.widgets;

namespace UIWidgetsGallery.demo.material
{
    internal class NavigationIconView
    {
        internal NavigationIconView(
            Widget icon = null,
            Widget activeIcon = null,
            string title = null,
            Color color = null,
            TickerProvider vsync = null
        )
        {
            this._icon = icon;
            this._color = color;
            this._title = title;
            this.item = new BottomNavigationBarItem(
                icon: icon,
                activeIcon: activeIcon,
                title: new Text(title),
                backgroundColor: color
            );
            this.controller = new AnimationController(
                duration: ThemeUtils.kThemeAnimationDuration,
                vsync: vsync);

            this._animation = this.controller.drive(new CurveTween(
                curve: new Interval(0.5f, 1.0f, curve: Curves.fastOutSlowIn)
            ));
        }

        public readonly Widget _icon;
        public readonly Color _color;
        public readonly string _title;
        public readonly BottomNavigationBarItem item;
        public readonly AnimationController controller;
        private Animation<float> _animation;

        internal FadeTransition transition(BottomNavigationBarType type, BuildContext context)
        {
            Color iconColor;
            if (type == BottomNavigationBarType.shifting)
            {
                iconColor = this._color;
            }
            else
            {
                ThemeData themeData = Theme.of(context);
                iconColor = themeData.brightness == Brightness.light
                    ? themeData.primaryColor
                    : themeData.accentColor;
            }

            return new FadeTransition(
                opacity: this._animation,
                child: new SlideTransition(
                    position: this._animation.drive(
                        new OffsetTween(
                            begin: new Offset(0.0f, 0.02f), // Slightly down.
                            end: Offset.zero
                        )
                    ),
                    child: new IconTheme(
                        data: new IconThemeData(
                            color: iconColor,
                            size: 120.0f
                        ),
                        child: this._icon
                    )
                )
            );
        }
    }

    internal class CustomIcon : StatelessWidget
    {
        public override Widget build(BuildContext context)
        {
            IconThemeData iconTheme = IconTheme.of(context);
            return new Container(
                margin: EdgeInsets.all(4.0f),
                width: iconTheme.size - 8.0f,
                height: iconTheme.size - 8.0f,
                color: iconTheme.color
            );
        }
    }

    internal class CustomInactiveIcon : StatelessWidget
    {
        public override Widget build(BuildContext context)
        {
            IconThemeData iconTheme = IconTheme.of(context);
            return new Container(
                margin: EdgeInsets.all(4.0f),
                width: iconTheme.size - 8.0f,
                height: iconTheme.size - 8.0f,
                decoration: new BoxDecoration(
                    border: Border.all(color: iconTheme.color, width: 2.0f)
                )
            );
        }
    }

    internal class BottomNavigationDemo : StatefulWidget
    {
        public static readonly string routeName = "/material/bottom_navigation";

        public override State createState()
        {
            return new _BottomNavigationDemoState();
        }
    }

    internal class _BottomNavigationDemoState : TickerProviderStateMixin<BottomNavigationDemo>
    {
        private int _currentIndex = 0;
        private BottomNavigationBarType _type = BottomNavigationBarType.shifting;
        private List<NavigationIconView> _navigationViews;

        public override void initState()
        {
            base.initState();
            this._navigationViews = new List<NavigationIconView>
            {
                new NavigationIconView(
                    icon: new Icon(Icons.access_alarm),
                    title: "Alarm",
                    color: Colors.deepPurple,
                    vsync: this
                ),
                new NavigationIconView(
                    activeIcon: new CustomIcon(),
                    icon: new CustomInactiveIcon(),
                    title: "Box",
                    color: Colors.deepOrange,
                    vsync: this
                ),
                new NavigationIconView(
                    activeIcon: new Icon(Icons.cloud),
                    icon: new Icon(Icons.cloud_queue),
                    title: "Cloud",
                    color: Colors.teal,
                    vsync: this
                ),
                new NavigationIconView(
                    activeIcon: new Icon(Icons.favorite),
                    icon: new Icon(Icons.favorite_border),
                    title: "Favorites",
                    color: Colors.indigo,
                    vsync: this
                ),
                new NavigationIconView(
                    icon: new Icon(Icons.event_available),
                    title: "Event",
                    color: Colors.pink,
                    vsync: this
                )
            };

            this._navigationViews[this._currentIndex].controller.setValue(1.0f);
        }


        public override void dispose()
        {
            foreach (NavigationIconView view in this._navigationViews)
                view.controller.dispose();
            base.dispose();
        }

        private Widget _buildTransitionsStack()
        {
            List<FadeTransition> transitions = new List<FadeTransition>();

            foreach (NavigationIconView view in this._navigationViews)
                transitions.Add(view.transition(this._type, this.context));

            // We want to have the newly animating (fading in) views on top.
            transitions.Sort((FadeTransition a, FadeTransition b) =>
            {
                Animation<float> aAnimation = a.opacity;
                Animation<float> bAnimation = b.opacity;
                float aValue = aAnimation.value;
                float bValue = bAnimation.value;
                return aValue.CompareTo(bValue);
            });

            return new Stack(children: new List<Widget>(transitions));
        }

        public override Widget build(BuildContext context)
        {
            BottomNavigationBar botNavBar = new BottomNavigationBar(
                items: this._navigationViews
                    .Select<NavigationIconView, BottomNavigationBarItem>((NavigationIconView navigationView) =>
                        navigationView.item)
                    .ToList(),
                currentIndex: this._currentIndex,
                type: this._type,
                onTap: (int index) =>
                {
                    this.setState(() =>
                    {
                        this._navigationViews[this._currentIndex].controller.reverse();
                        this._currentIndex = index;
                        this._navigationViews[this._currentIndex].controller.forward();
                    });
                }
            );

            return new Scaffold(
                appBar: new AppBar(
                    title: new Text("Bottom navigation"),
                    actions: new List<Widget>
                    {
                        new MaterialDemoDocumentationButton(BottomNavigationDemo.routeName),
                        new PopupMenuButton<BottomNavigationBarType>(
                            onSelected: (BottomNavigationBarType value) =>
                            {
                                this.setState(() => { this._type = value; });
                            },
                            itemBuilder: (BuildContext subContext) => new List<PopupMenuEntry<BottomNavigationBarType>>
                            {
                                new PopupMenuItem<BottomNavigationBarType>(
                                    value: BottomNavigationBarType.fix,
                                    child: new Text("Fixed")
                                ),
                                new PopupMenuItem<BottomNavigationBarType>(
                                    value: BottomNavigationBarType.shifting,
                                    child: new Text("Shifting")
                                )
                            }
                        )
                    }
                ),
                body: new Center(
                    child: this._buildTransitionsStack()
                ),
                bottomNavigationBar: botNavBar
            );
        }
    }
}