### CUSTOM_INJECTION
**Bug Pattern:** SQL_INJECTION_JDO

[CWE-89: Improper Neutralization of Special Elements used in an SQL Command ('SQL Injection')](http://cwe.mitre.org/data/definitions/89.html)
#### 描述：
识别方法很容易被注入，输入应当被验证并正确地转义。
- [x] 安全相关

#### 漏洞代码：

```
SqlUtil.execQuery("select * from UserEntity t where id = " + parameterInput);

```
如果输入了"or'1'='1'#"就可以实施注入。
#### check级别：AST
#### 相关信息：
1. [WASC-19: SQL Injection](http://projects.webappsec.org/w/page/13246963/SQL%20Injection)
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)
3. [OWASP: SQL Injection Prevention Cheat Sheet](https://www.owasp.org/index.php/SQL_Injection_Prevention_Cheat_Sheet)
4. [OWASP: Query Parameterization Cheat Sheet](https://www.owasp.org/index.php/Query_Parameterization_Cheat_Sheet)
5. [CAPEC-66: SQL Injection](http://capec.mitre.org/data/definitions/66.html)
