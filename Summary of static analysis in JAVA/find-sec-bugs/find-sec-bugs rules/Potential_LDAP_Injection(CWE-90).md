### Potential LDAP Injection
**Bug Pattern:** LDAP_INJECTION  
[CWE-90: Improper Neutralization of Special Elements used in an LDAP Query ('LDAP Injection')](http://cwe.mitre.org/data/definitions/90.html)
#### 描述：
就像SQL一样，传递给LDAP查询的所有输入都需要安全地传递。然而，DAP没有像SQL这样的准备语句接口。因此，对LDAP注入的主要防御是在将其放入LDAP查询之前，对任何不可信的数据进行强大的输入验证。
- [x] 安全相关
#### 漏洞代码：

```
NamingEnumeration<SearchResult> answers = context.search("dc=People,dc=example,dc=com",
        "(uid=" + username + ")", ctrls);
```
##### check级别：AST
#### 相关信息：
1. [WASC-29: LDAP Injection](http://projects.webappsec.org/w/page/13246947/LDAP%20Injection)
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)
3. [LDAP Injection Guide: Learn How to Detect LDAP Injections and Improve LDAP Security](http://www.veracode.com/security/ldap-injection)
