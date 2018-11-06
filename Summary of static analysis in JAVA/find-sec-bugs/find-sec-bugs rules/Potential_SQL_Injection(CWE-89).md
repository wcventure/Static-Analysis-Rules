### Potential SQL Injection
[CWE-89: Improper Neutralization of Special Elements used in an SQL Command](http://cwe.mitre.org/data/definitions/89.html) ('SQL Injection')
#### 描述：
SQL查询中包含的输入值需要安全地传递。
- [x] 安全相关

#### 代码：

```
createQuery("select * from User where id = '"+inputId+"'");

```
#### 解决方法：

```
import org.owasp.esapi.Encoder;

createQuery("select * from User where id = '"+Encoder.encodeForSQL(inputId)+"'");
```
##### check级别：AST

#### 相关信息：
1.  [WASC-19: SQL Injection](http://projects.webappsec.org/w/page/13246963/SQL%20Injection)
2. [CAPEC-66: SQL Injection](http://capec.mitre.org/data/definitions/66.html)
3. [OWASP: Query Parameterization Cheat Sheet](https://www.owasp.org/index.php/Query_Parameterization_Cheat_Sheet)
4. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)
5. [OWASP: SQL Injection Prevention Cheat Sheet](https://www.owasp.org/index.php/SQL_Injection_Prevention_Cheat_Sheet)
