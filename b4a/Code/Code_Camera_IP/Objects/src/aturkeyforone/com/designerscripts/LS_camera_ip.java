package aturkeyforone.com.designerscripts;
import anywheresoftware.b4a.objects.TextViewWrapper;
import anywheresoftware.b4a.objects.ImageViewWrapper;
import anywheresoftware.b4a.BA;


public class LS_camera_ip{

public static void LS_general(java.util.LinkedHashMap<String, anywheresoftware.b4a.keywords.LayoutBuilder.ViewWrapperAndAnchor> views, int width, int height, float scale) {
anywheresoftware.b4a.keywords.LayoutBuilder.setScaleRate(0.3);
//BA.debugLineNum = 3;BA.debugLine="WebView1.SetLeftAndRight(0, 100%x)"[camera_ip/General script]
views.get("webview1").vw.setLeft((int)(0d));
views.get("webview1").vw.setWidth((int)((100d / 100 * width) - (0d)));
//BA.debugLineNum = 4;BA.debugLine="WebView1.SetTopAndBottom(0, 75%y)"[camera_ip/General script]
views.get("webview1").vw.setTop((int)(0d));
views.get("webview1").vw.setHeight((int)((75d / 100 * height) - (0d)));

}
}