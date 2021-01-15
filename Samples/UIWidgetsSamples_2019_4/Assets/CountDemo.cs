using System.Collections.Generic;
using Unity.UIWidgets.cupertino;
using Unity.UIWidgets.engine2;
using Unity.UIWidgets.ui;
using Unity.UIWidgets.widgets;
using UnityEngine.UI;
using Text = Unity.UIWidgets.widgets.Text;
using ui_ = Unity.UIWidgets.widgets.ui_;
using TextStyle = Unity.UIWidgets.painting.TextStyle;

namespace UIWidgetsSample
{
    public class CountDemo : UIWidgetsPanel
    {
        protected void OnEnable()
        {
            base.OnEnable();
        }

        protected override void main()
        {
            ui_.runApp(new MyApp());
        }

        class MyApp : StatelessWidget
        {
            public override Widget build(BuildContext context)
            {
                return new CupertinoApp(
                    home: new CounterApp()
                );
            }
        }
    }

    internal class CounterApp : StatefulWidget
    {
        public override State createState()
        {
            return new CountDemoState();
        }
    }

    internal class CountDemoState : State<CounterApp>
    {
        private int count = 0;

        public override Widget build(BuildContext context)
        {
            return new Container(
                color: Color.fromARGB(255, 255, 0, 0),
                child: new Column(children: new List<Widget>()
                    {
                        new Text($"count: {count}", style: new TextStyle(color: Color.fromARGB(255, 0 ,0 ,255))),
                        new CupertinoButton(
                            onPressed: () =>
                            {
                                setState(() =>
                                {
                                    count++;
                                });
                            },
                            child: new Container(
                                color: Color.fromARGB(255,0 , 255, 0),
                                width: 100,
                                height: 40
                            )
                        ),
                    }
                )
            );
        }
    }
}