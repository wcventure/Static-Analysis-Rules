### ECB mode is insecure
**Bug Pattern:** HTTPONLY_COOKIE

#### 描述：
一个新的cookie是在没有HttpOnly标志的情况下创建的。HttpOnly标志是一个指向浏览器的指令，以确保cookie不会被恶意脚本所破坏。当用户是“跨站点脚本”的目标时，攻击者将从获取会话id得到极大的好处。

A new cookie is created without the HttpOnly flag set. The HttpOnly flag is a directive to the browser to make sure that the cookie can not be red by malicious script. When a user is the target of a "Cross-Site Scripting", the attacker would benefit greatly from getting the session id for example.

- [x] 安全相关

#### 漏洞代码：
```
Cookie cookie = new Cookie("email",userName);
response.addCookie(cookie);
```

#### 解决方案（Specific configuration）

```
Cookie cookie = new Cookie("email",userName);
cookie.setSecure(true);
cookie.setHttpOnly(true); //HttpOnly flag
```

#### 解决方案（Servlet 3.0 configuration）

```
<web-app xmlns="http://java.sun.com/xml/ns/javaee" version="3.0">
[...]
<session-config>
 <cookie-config>
  <http-only>true</http-only>
  <secure>true</secure>
 </cookie-config>
</session-config>
</web-app>
```
##### Check级别：AST

#### 相关信息：
[Coding Horror blog: Protecting Your Cookies: HttpOnly](http://blog.codinghorror.com/protecting-your-cookies-httponly/)

[OWASP: HttpOnly](https://www.owasp.org/index.php/HttpOnly)

[Rapid7: Missing HttpOnly Flag From Cookie](https://www.rapid7.com/db/vulnerabilities/http-cookie-http-only-flag)