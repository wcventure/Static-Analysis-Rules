### Potential SQL Injection with Turbine 
**Bug Pattern:** SQL_INJECTION_TURBINE  
[CWE-89: Improper Neutralization of Special Elements used in an SQL Command ('SQL Injection')](http://cwe.mitre.org/data/definitions/89.html)
#### 描述：
SQL查询语句中的输入需要安全检查。在输入语句中绑定变量可以很容易地降低SQL注入的风险。
#### 漏洞代码:

```
List<Record> BasePeer.executeQuery( "select * from Customer where id=" + inputId );

```
#### 解决方法：
1. 利用标准DSL

```
Criteria c = new Criteria();
c.add( CustomerPeer.ID, inputId );

List<Customer> customers = CustomerPeer.doSelect( c );
```
2. 特定方法

```
Customer customer = CustomerPeer.retrieveByPK( new NumberKey( inputId ) );

```
3. 使用OWASP编码器

```
import org.owasp.esapi.Encoder;

BasePeer.executeQuery("select * from Customer where id = '"+Encoder.encodeForSQL(inputId)+"'");

```
#### check级别：AST
#### 相关信息(Turbine)：
[Turbine Documentation: Criteria Howto](https://turbine.apache.org/turbine/turbine-2.1/howto/criteria-howto.html)

#### 相关信息(SQL injection):
1. [WASC-19: SQL Injection](https://note.youdao.com/)
2. [CAPEC-66: SQL Injection](http://capec.mitre.org/data/definitions/66.html)
4. [OWASP: Top 10 2013-A1-Injection](https://note.youdao.com/)
5. [OWASP: SQL Injection Prevention Cheat Sheet](https://www.owasp.org/index.php/SQL_Injection_Prevention_Cheat_Sheet)
6. [OWASP: Query Parameterization Cheat Sheet](https://www.owasp.org/index.php/Query_Parameterization_Cheat_Sheet)
