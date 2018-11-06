### Potential SQL/HQL Injection (Hibernate)
**Bug Pattern:** SQL_INJECTION_HIBERNATE  

**CWE:**[CWE-89: Improper Neutralization of Special Elements used in an SQL Command](http://cwe.mitre.org/data/definitions/89.html) ('SQL Injection')
- [x] 安全相关：
#### 描述：
SQL查询语句中的输入需要安全检查。在输入语句中绑定变量可以很容易地降低SQL注入的风险。
#### 代码：

```
Session session = sessionFactory.openSession();
Query q = session.createQuery("select t from UserEntity t where id = " + input);
q.execute();
```
#### 解决方法：
1.
```
Session session = sessionFactory.openSession();
Query q = session.createQuery("select t from UserEntity t where id = :userId");
q.setString("userId",input);
q.execute();
```
2. 利用hibernate标准

```
Session session = sessionFactory.openSession();
Query q = session.createCriteria(UserEntity.class)
    .add( Restrictions.like("id", input) )
    .list();
q.execute();
```
#### check级别：AST
#### 相关信息：
1. [Hibernate Documentation: Query Criteria](https://docs.jboss.org/hibernate/orm/3.3/reference/en/html/querycriteria.html)
2. [Hibernate Javadoc: Query Object](https://docs.jboss.org/hibernate/orm/3.2/api/org/hibernate/Query.html)
3. [HQL for pentesters:](http://blog.h3xstream.com/2014/02/hql-for-pentesters.html)
