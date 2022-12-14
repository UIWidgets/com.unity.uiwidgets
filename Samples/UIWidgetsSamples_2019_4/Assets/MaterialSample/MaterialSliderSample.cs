using System.Collections.Generic;
using Unity.UIWidgets.engine;
using Unity.UIWidgets.material;
using Unity.UIWidgets.painting;
using Unity.UIWidgets.ui;
using Unity.UIWidgets.widgets;
using UnityEngine;
using ui_ = Unity.UIWidgets.widgets.ui_;

namespace UIWidgetsSample {
    
    public class MaterialSliderSample : UIWidgetsPanel {

        protected override void main() {
            ui_.runApp(new MaterialApp(
                showPerformanceOverlay: false,
                home: new MaterialSliderWidget()));
        }

        protected new void OnEnable() {
            base.OnEnable();
        }
    }
    
    public class MaterialSliderWidget : StatefulWidget {
        public override State createState() {
            return new MaterialSliderState();
        }
    }

    public class MaterialSliderState : State<MaterialSliderWidget> {

        float _value = 0.8f;

        void onChanged(float value) {
            this.setState(() => { this._value = value; });
        }

        public override Widget build(BuildContext context) {
            return new Scaffold(
                appBar: new AppBar(
                    title: new Text("Slider and Indicators")),
                body: new Column(
                    children: new List<Widget> {
                        new Padding(
                            padding: EdgeInsets.only(top: 100.0f),
                            child: new Container(
                            child: new Slider(
                                divisions: 10,
                                min: 0.4f,
                                label: "Here",
                                value: this._value,
                                onChanged: this.onChanged))
                            )
                    }
                )
            );
        }
    }

}