### Untrusted Content-Type header 
HTTP头中的Content-Type可被用户篡改
**Bug Pattern:** SERVLET_CONTENT_TYPE  
#### 描述：
The HTTP header Content-Type can be controlled by the client. As such, its value should not be used in any security critical decisions.
- [x] 安全相关
#### 代码:

```
Cookie[] cookies = request.getCookies();
for (int i =0; i< cookies.length; i++) {
Cookie c = cookies[i];
if (c.getName().equals("role")) {
userRole = c.getValue();
}
}
```
**check级别**: AST
#### 参考信息：
[CWE-807: Untrusted Inputs in a Security Decision](http://cwe.mitre.org/data/definitions/807.html)
