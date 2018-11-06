### Potential XSS in Servlet 
**Bug Pattern:** XSS_SERVLET

[CWE-79: Improper Neutralization of Input During Web Page Generation ('Cross-site Scripting'))](http://cwe.mitre.org/data/definitions/79.html)

#### 描述：
一个潜在的XSS被发现。它可以用于在客户机的浏览器中执行不需要的JavaScript。(见参考文献)

A potential XSS was found. It could be used to execute unwanted JavaScript in a client's browser. (See references)

- [x] 安全相关

#### 漏洞代码：
```
protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
    String input1 = req.getParameter("input1");
    [...]
    resp.getWriter().write(input1);
}
```

#### 解决方案
```
protected void doGet(HttpServletRequest req, HttpServletResponse resp) throws ServletException, IOException {
    String input1 = req.getParameter("input1");
    [...]
    resp.getWriter().write(Encode.forHtml(input1));
}
```
The best defense against XSS is context sensitive output encoding like the example above. There are typically 4 contexts to consider: HTML, JavaScript, CSS (styles), and URLs. Please follow the XSS protection rules defined in the OWASP XSS Prevention Cheat Sheet, which explains these defenses in significant detail.

Note that this XSS in Servlet rule looks for similar issues, but looks for them in a different way than the existing 'XSS: Servlet reflected cross site scripting vulnerability' and 'XSS: Servlet reflected cross site scripting vulnerability in error page' rules in FindBugs.

##### Check级别：AST

#### 相关信息：
[WASC-8: Cross Site Scripting](http://projects.webappsec.org/w/page/13246920/Cross%20Site%20Scripting)

[OWASP: XSS Prevention Cheat Sheet](https://www.owasp.org/index.php/XSS_%28Cross_Site_Scripting%29_Prevention_Cheat_Sheet)

[OWASP: Top 10 2013-A3: Cross-Site Scripting (XSS)](https://www.owasp.org/index.php/Top_10_2013-A3-Cross-Site_Scripting_%28XSS%29)

[CWE-79: Improper Neutralization of Input During Web Page Generation ('Cross-site Scripting'))](http://cwe.mitre.org/data/definitions/79.html)

[OWASP Java Encoder](https://code.google.com/p/owasp-java-encoder/)