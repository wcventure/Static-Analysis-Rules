### Potentially sensitive data in a cookie  
存放在cookie中的信息不能包含敏感信息或者和session有关的信息。在大多数例子中，敏感的信息应该存放在session中并且通过用户的session cookie调用。如果敏感信息存放在cookie中，则很容易被攻击者窃取。  
**Bug Pattern:** COOKIE_USAGE  
[CWE-315: Cleartext Storage of Sensitive Information in a Cookie](http://cwe.mitre.org/data/definitions/315.html)
#### 描述：
The information stored in a custom cookie should not be sensitive or related to the session. In most cases, sensitive data should only be stored in session and referenced by the user's session cookie. See HttpSession (HttpServletRequest.getSession())

Custom cookies can be used for information that needs to live longer than and is independent of a specific session.

- [x] 安全相关  
#### 漏洞代码：

```
response.addCookie( new Cookie("userAccountID", acctID);
```

####  check级别：AST
#### 参考信息：

[CWE-315: Cleartext Storage of Sensitive Information in a Cookie](http://cwe.mitre.org/data/definitions/315.html)
