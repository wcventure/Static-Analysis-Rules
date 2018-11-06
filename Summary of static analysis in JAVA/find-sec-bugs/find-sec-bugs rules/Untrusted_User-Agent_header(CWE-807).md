### Untrusted User-Agent header
HTTP头中的"User-Agent"字段可以很容易地被用户更改  
**Bug Pattern:** COOKIE_USAGE  
[CWE-807: Untrusted Inputs in a Security Decision](http://cwe.mitre.org/data/definitions/807.html)
#### 描述：
The header "User-Agent" can easily be spoofed by the client. Adopting different behaviors based on the User-Agent (for crawler UA) is not recommended.


#### 代码：

```
String Agent = request.getHeader("User-Agent");
```
#### check级别: AST

