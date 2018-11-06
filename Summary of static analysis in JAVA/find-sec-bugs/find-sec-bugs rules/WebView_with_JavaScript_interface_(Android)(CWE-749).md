### WebView with JavaScript interface (Android)
**Bug Pattern:** ANDROID_WEB_VIEW_JAVASCRIPT_INTERFACE

[CWE-749: Exposed Dangerous Method or Function](https://cwe.mitre.org/data/definitions/749.html)

#### 描述：
使用JavaScript接口可以将WebView暴露给有风险的API。如果在WebView中触发了XSS，则可以通过恶意JavaScript代码调用该类。

The use of JavaScript Interface could expose the WebView to risky API. If an XSS is triggered in the WebView, the class could be called by the malicious JavaScript code.
- [x] 安全相关

#### 漏洞代码：

```
WebView myWebView = (WebView) findViewById(R.id.webView);

myWebView.addJavascriptInterface(new FileWriteUtil(this), "fileWriteUtil");

WebSettings webSettings = myWebView.getSettings();
webSettings.setJavaScriptEnabled(true);

[...]
class FileWriteUtil {
    Context mContext;

    FileOpenUtil(Context c) {
        mContext = c;
    }

    public void writeToFile(String data, String filename, String tag) {
        [...]
    }
}
```

##### Check级别：AST

#### 相关信息：
[Android Official Doc: WebView.addJavascriptInterface()](http://developer.android.com/reference/android/webkit/WebView.html#addJavascriptInterface%28java.lang.Object,%20java.lang.String%29)

[CWE-749: Exposed Dangerous Method or Function](https://cwe.mitre.org/data/definitions/749.html)