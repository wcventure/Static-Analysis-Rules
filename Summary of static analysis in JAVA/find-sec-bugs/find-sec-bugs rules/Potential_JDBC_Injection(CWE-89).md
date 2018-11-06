### Potential JDBC Injection
**Bug Pattern:** SQL_INJECTION_JDBC  
**cwe:**[CWE-89: Improper Neutralization of Special Elements used in an SQL Command ('SQL Injection')](http://cwe.mitre.org/data/definitions/89.html)
- [x] 安全相关
#### 漏洞代码：

```
Connection conn = [...];
Statement stmt = con.createStatement();
ResultSet rs = stmt.executeQuery("update COFFEES set SALES = "+nbSales+" where COF_NAME = '"+coffeeName+"'");
```
#### 解决方案：

```
Connection conn = [...];
conn.prepareStatement("update COFFEES set SALES = ? where COF_NAME = ?");
updateSales.setInt(1, nbSales);
updateSales.setString(2, coffeeName);

```
#### check级别：AST
#### 相关信息(JDBC)：
[Oracle Documentation: The Java Tutorials > Prepared Statements](http://docs.oracle.com/javase/tutorial/jdbc/basics/prepared.html)
