### Potential HTTP Response Splitting 
**Bug Pattern:** CRLF_INJECTION_LOGS

[CWE-117: Improper Output Neutralization for Logs](http://cwe.mitre.org/data/definitions/117.html)

[CWE-93: Improper Neutralization of CRLF Sequences ('CRLF Injection')](http://cwe.mitre.org/data/definitions/93.html)

#### 描述：
当来源不可信的数据放入日志记录器中，而不是被正确地中和时，攻击者可以伪造日志条目或者包含恶意内容。插入错误条目可以歪曲统计数据，分散管理员的注意力，甚至将另一方牵扯到恶意行为中。如果日志文件是自动运行的，攻击者可以通过破坏文件格式或者注入意外字符导致文件无法使用。攻击者也可以注入代码或其他命令到日志文件中，并利用日志运行工具中的漏洞（像命令注入或XSS）。

When data from an untrusted source is put into a logger and not neutralized correctly, an attacker could forge log entries or include malicious content. Inserted false entries could be used to skew statistics, distract the administrator or even to implicate another party in the commission of a malicious act. If the log file is processed automatically, the attacker can render the file unusable by corrupting the format of the file or injecting unexpected characters. An attacker may also inject code or other commands into the log file and take advantage of a vulnerability in the log processing utility (e.g. command injection or XSS).

- [x] 安全相关

#### 漏洞代码：

```
String val = request.getParameter("user");
String metadata = request.getParameter("metadata");
[...]
if(authenticated) {
    log.info("User " + val + " (" + metadata + ") was authenticated successfully");
}
else {
    log.info("User " + val + " (" + metadata + ") was not authenticated");
}
```
一个恶意用户可以发送带有值的元数参数：
```
"Firefox) was authenticated successfully\r\n[INFO] User bbb (Internet Explorer"
```
#### 解决方案:

你可以手动对每个参数进行消毒：
```
log.info("User " + val.replaceAll("[\r\n]","") + " (" + userAgent.replaceAll("[\r\n]","") + ") was not authenticated");
```
你也可以配置logger服务，以替换所有代表消息事件的新行。下面是使用[replace function](https://logback.qos.ch/manual/layouts.html#replace)返回的示例配置:
```
<pattern>%-5level - %replace(%msg){'[\r\n]', ''}%n</pattern>
```
[OWASP Security Logging](https://github.com/javabeanz/owasp-security-logging)项目记录了Logback和Log4j的实现。

##### Check级别：AST

#### 相关信息：

[CWE-117: Improper Output Neutralization for Logs](http://cwe.mitre.org/data/definitions/117.html)

[CWE-93: Improper Neutralization of CRLF Sequences ('CRLF Injection')](http://cwe.mitre.org/data/definitions/93.html)

[replace function](https://logback.qos.ch/manual/layouts.html#replace)

[OWASP Security Logging](https://github.com/javabeanz/owasp-security-logging)