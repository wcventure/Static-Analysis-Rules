### WebView with geolocation activated (Android) 
**Bug Pattern:** ANDROID_GEOLOCATION

#### 描述：
建议用户对其地理位置进行确认。

It is suggested to ask the user for a confirmation about obtaining its geolocation.
- [x] 安全相关

#### 漏洞代码：

```
webView.setWebChromeClient(new WebChromeClient() {
    @Override
    public void onGeolocationPermissionsShowPrompt(String origin, GeolocationPermissions.Callback callback) {
        callback.invoke(origin, true, false);
    }
});
```

#### 解决方案

Limit the sampling of geolocation and ask the user for confirmation.
```
webView.setWebChromeClient(new WebChromeClient() {
    @Override
    public void onGeolocationPermissionsShowPrompt(String origin, GeolocationPermissions.Callback callback) {
        callback.invoke(origin, true, false);

        //Ask the user for confirmation
    }
});
```

##### Check级别：AST

#### 相关信息：

[CERT: DRD15-J. Consider privacy concerns when using Geolocation API](https://www.securecoding.cert.org/confluence/display/java/DRD15-J.+Consider+privacy+concerns+when+using+Geolocation+API)

[Wikipedia: W3C Geolocation API](http://en.wikipedia.org/wiki/W3C_Geolocation_API)

[W3C: Geolocation Specification](http://dev.w3.org/geo/api/spec-source.html)
