### XSSRequestWrapper is a weak XSS protection 
**Bug Pattern:** XSS_REQUEST_WRAPPER

[CWE-79: Improper Neutralization of Input During Web Page Generation ('Cross-site Scripting')](http://cwe.mitre.org/data/definitions/79.html)

#### 描述：
一个名为XSSRequestWrapper的HttpServletRequestWrapper的实现通过各种博客站点发布。([Stronger Anti Cross-Site Scripting (XSS) Filter for Java Web Apps](https://dzone.com/articles/stronger-anti-cross-site), [Anti cross-site scripting (XSS) filter for Java web apps](https://www.javacodegeeks.com/2012/07/anti-cross-site-scripting-xss-filter.html))。

由于一些原因，过滤功能很弱:

- 它只包含参数，而不是headers和side-channel输入。

- 替换链可以很容易地绕过(参见下面的例子)

- 它是非常特定的坏模式的黑色列表(而不是一个良好/有效输入的白色列表)

An implementation of HttpServletRequestWrapper called XSSRequestWrapper was published through various blog sites. ([Stronger Anti Cross-Site Scripting (XSS) Filter for Java Web Apps](https://dzone.com/articles/stronger-anti-cross-site), [Anti cross-site scripting (XSS) filter for Java web apps](https://www.javacodegeeks.com/2012/07/anti-cross-site-scripting-xss-filter.html))

The filtering is weak for a few reasons:

- It covers only parameters not headers and side-channel inputs
- The replace chain can be bypassed easily (see example below)
- It's a black list of very specific bad patterns (rather than a white list of good/valid input)

- [x] 安全相关

#### 漏洞代码：
```
<scrivbscript:pt>alert(1)</scrivbscript:pt>
```
之前的输入将被转换为"<script>alert(1)</script>"。删除"vbscript:"是在替换了"<script>.*</script>"之后。

The previous input will be transformed into "<script>alert(1)</script>". The removal of "vbscript:" is after the replacement of "<script>.*</script>".

#### 解决方案
For stronger protection, choose a solution that encodes characters automatically in the view (template, jsp, ...) following the XSS protection rules defined in the OWASP XSS Prevention Cheat Sheet.
##### Check级别：AST

#### 相关信息：

[WASC-8: Cross Site Scripting](http://projects.webappsec.org/w/page/13246920/Cross%20Site%20Scripting)

[OWASP: XSS Prevention Cheat Sheet](https://www.owasp.org/index.php/XSS_%28Cross_Site_Scripting%29_Prevention_Cheat_Sheet)

[OWASP: Top 10 2013-A3: Cross-Site Scripting (XSS)](https://www.owasp.org/index.php/Top_10_2013-A3-Cross-Site_Scripting_%28XSS%29)

[CWE-79: Improper Neutralization of Input During Web Page Generation ('Cross-site Scripting')](http://cwe.mitre.org/data/definitions/79.html)
