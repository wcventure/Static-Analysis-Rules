### Potential SQL/JDOQL Injection(JDO) 
**Bug Pattern:** SQL_INJECTION_JDO  
[CWE-89: Improper Neutralization of Special Elements used in an SQL Command](http://cwe.mitre.org/data/definitions/89.html) ('SQL Injection')
#### 描述：
SQL查询语句中的输入需要安全检查。在输入语句中绑定变量可以很容易地降低SQL注入的风险。
#### 漏洞代码：

```
PersistenceManager pm = getPM();

Query q = pm.newQuery("select * from Users where name = " + input);
q.execute();
```
#### 解决方法：

```
PersistenceManager pm = getPM();

Query q = pm.newQuery("select * from Users where name = nameParam");
q.declareParameters("String nameParam");
q.execute(input);
```
#### check级别：AST
#### References (JDO)
[JDO: Object Retrieval](https://db.apache.org/jdo/object_retrieval.html)
