### Potential code injection when using OGNL expression
动态表达式会导致代码注入
**Bug Pattern:** OGNL_INJECTION  
- [x] 安全相关
#### 描述：
A expression is built with a dynamic value. The source of the value(s) should be verified to avoid that unfiltered values fall into this risky code evaluation.

#### 漏洞代码：

```
public void getUserProperty(String property) {
  [...]
  //The first argument is the dynamic expression.
  return ognlUtil.getValue("user."+property, ctx, root, String.class);
}
```
In general, method evaluating OGNL expression should not received user input. It is intented to be used in static configurations and JSP.

#### Check级别：AST
#### 相关信息：
1. [HP Enterprise: Struts 2 OGNL Expression Injections by Alvaro MuÃ±oz](https://community.saas.hpe.com/t5/Security-Research/Struts-2-OGNL-Expression-Injections/ba-p/288881)
2. [Gotham Digital Science: An Analysis Of CVE-2017-5638](https://blog.gdssecurity.com/labs/2017/3/27/an-analysis-of-cve-2017-5638.html)
3. [Apache Struts2: Vulnerability S2-016](http://struts.apache.org/docs/s2-016.html)
4. [Apache Struts 2 Documentation: OGNL](https://struts.apache.org/docs/ognl.html)
