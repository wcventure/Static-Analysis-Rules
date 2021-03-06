###A malicious XSLT could be provided(JSP)

提供恶意的XSLT

**Bug Pattern：**JSP_XSLT
####描述：
“XSLT（可扩展样式表语言转换）是一种用于将XML文档转换为其他XML文档的语言”。[1]

有可能将恶意行为附加到这些样式表。 因此，如果攻击者可以控制样式表的内容或来源，他可能会触发远程代码执行。[2]

- [x] 安全相关

####风险代码：
```
<x:transform xml="${xmlData}" xslt="${xsltControlledByUser}" />

```


#### Check级别：AST,PDG

####解决方案：
The solution would be to make sure the stylesheet is loaded from a safe sources and make sure that vulnerabilities such as Path traversal [3][4] are not possible.

####相关信息：
[1]  [Wikipedia: XSLT (Extensible Stylesheet Language Transformations)]( https://en.wikipedia.org/wiki/XSLT)
[Offensive XSLT by Nicolas Gregoire]( https://prezi.com/y_fuybfudgnd/offensive-xslt/)
[2]  [From XSLT code execution to Meterpreter shells by Nicolas Gregoire]( http://www.agarri.fr/kom/archives/2012/07/02/from_xslt_code_execution_to_meterpreter_shells/index.html)
[XSLT Hacking Encyclopedia by Nicolas Gregoire]( http://xhe.myxwiki.org/xwiki/bin/view/Main/)
[Acunetix.com : The hidden dangers of XSLTProcessor - Remote XSL injection]( http://www.acunetix.com/blog/articles/the-hidden-dangers-of-xsltprocessor-remote-xsl-injection/)
[w3.org XSL Transformations (XSLT) Version 1.0 : w3c specification]( https://www.w3.org/TR/xslt)
[3]  [WASC: Path Traversal]( http://projects.webappsec.org/w/page/13246952/Path%20Traversal)
[4]  [OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)





