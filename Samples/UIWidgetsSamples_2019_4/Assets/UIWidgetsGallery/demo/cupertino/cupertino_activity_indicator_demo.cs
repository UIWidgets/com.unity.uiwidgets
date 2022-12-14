using Unity.UIWidgets.cupertino;
using Unity.UIWidgets.widgets;

namespace UIWidgetsGallery.gallery {
    public class CupertinoProgressIndicatorDemo : StatelessWidget {
        public static string routeName = "/cupertino/progress_indicator";
        public CupertinoProgressIndicatorDemo(){}
        public override Widget build(BuildContext context) {
            return new CupertinoPageScaffold(
                navigationBar: new CupertinoNavigationBar(
                    previousPageTitle: "Cupertino",
                    middle: new Text("Activity Indicator")
           
                ), 
                child: new Center(
                    child: new CupertinoActivityIndicator()
                )
            );
        }
    }
}