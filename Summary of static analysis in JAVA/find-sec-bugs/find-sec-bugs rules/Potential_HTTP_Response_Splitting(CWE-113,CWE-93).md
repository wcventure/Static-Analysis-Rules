### Potential HTTP Response Splitting 
**Bug Pattern:** HTTP_RESPONSE_SPLITTING

[CWE-113: Improper Neutralization of CRLF Sequences in HTTP Headers ('HTTP Response Splitting')](http://cwe.mitre.org/data/definitions/113.html)

[CWE-93: Improper Neutralization of CRLF Sequences ('CRLF Injection')](http://cwe.mitre.org/data/definitions/93.html)

#### 描述：
当HTTP请求包含了意外的CR或者LF字符时，服务器可能会响应输出流，而输出流被解释为两个不同的HTTP响应（而不是一个）。攻击者可以控制第二个响应和挂载攻击，如跨站脚本攻击和缓存中毒攻击。根据OWASP的说法，这个问题在几乎所有现代Java EE 应用服务器中得到了修复，但是它仍然可以更好地验证输入。如果您担心这个风险，您应该在关注的平台上进行测试，查看其底层平台是否允许CR或者LF字符注入到header中。这个弱点优先级比SQL注入优先级低，如果您使用的是易受攻击的平台，也请检查低优先级的警告。

When an HTTP request contains unexpected CR and LF characters, the server may respond with an output stream that is interpreted as two different HTTP responses (instead of one). An attacker can control the second response and mount attacks such as cross-site scripting and cache poisoning attacks. According to OWASP, the issue has been fixed in virtually all modern Java EE application servers, but it is still better to validate the input. If you are concerned about this risk, you should test on the platform of concern to see if the underlying platform allows for CR or LF characters to be injected into headers. This weakness is reported with lower priority than SQL injection etc., if you are using a vulnerable platform, please check low-priority warnings too. evaluation.

- [x] 安全相关

#### 漏洞代码：

```
String author = request.getParameter(AUTHOR_PARAMETER);
// ...
Cookie cookie = new Cookie("author", author);
response.addCookie(cookie);
```
##### Check级别：AST

#### 相关信息：

[OWASP: HTTP Response Splitting](https://www.owasp.org/index.php/HTTP_Response_Splitting)

[CWE-113: Improper Neutralization of CRLF Sequences in HTTP Headers ('HTTP Response Splitting')](http://cwe.mitre.org/data/definitions/113.html)

[CWE-93: Improper Neutralization of CRLF Sequences ('CRLF Injection')](http://cwe.mitre.org/data/definitions/93.html)