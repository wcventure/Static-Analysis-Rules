### Untrusted query string 
GET方法中的参数不安全  
**Bug Pattern:** SERVLET_QUERY_STRING  
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)
#### 描述：
The query string is the concatenation of the GET parameter names and values. Parameters other than those intended can be passed in.

For the URL request ==/app/servlet.htm?a=1&b=2==, the query string extract will be ==a=1&b=2==

Just as is true for individual parameter values retrieved via methods like ==HttpServletRequest.getParameter()==, the value obtained from ==HttpServletRequest.getQueryString()== should be considered unsafe. You may need to validate or sanitize anything pulled from the query string before passing it to sensitive APIs.  
- [x] 安全相关
#### 漏洞代码：

```
IntentFilter filter = new IntentFilter("com.example.URLHandler.openURL");
MyReceiver receiver = new MyReceiver();
registerReceiver(receiver, filter);
... 

public class UrlHandlerReceiver extends BroadcastReceiver {
@Override
public void onReceive(Context context, Intent intent) {
if("com.example.URLHandler.openURL".equals(intent.getAction())) {
String URL = intent.getStringExtra("URLToOpen");
int length = URL.length();
```
**check级别：** dataflow
#### 参考信息：
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)
