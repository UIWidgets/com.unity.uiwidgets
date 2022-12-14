using System.Collections.Generic;
using Unity.UIWidgets.engine;
using Unity.UIWidgets.ui;
using Unity.UIWidgets.widgets;
using ui_ = Unity.UIWidgets.widgets.ui_;

namespace UIWidgetsSample {
    public class PageViewSample : UIWidgetsPanel {
            
        protected override void main()
        {
            ui_.runApp(new MyApp());
        }
        
        class MyApp : StatelessWidget
        {
            public override Widget build(BuildContext context)
            {
                return new WidgetsApp(
                    color: Color.white,
                    home: new Container(
                        width: 200,
                        height: 400,
                        child: new PageView(
                            children: new List<Widget>() {
                                new Container(
                                    color: new Color(0xFFE91E63)
                                ),
                                new Container(
                                    color: new Color(0xFF00BCD4)
                                ),
                                new Container(
                                    color: new Color(0xFF673AB7)
                                )
                            }
                        )),
                    pageRouteBuilder: (settings, builder) =>
                        new PageRouteBuilder(
                            settings: settings,
                            pageBuilder: (Buildcontext, animation, secondaryAnimation) => builder(context)
                        )
                );
            }
        }
    }
}