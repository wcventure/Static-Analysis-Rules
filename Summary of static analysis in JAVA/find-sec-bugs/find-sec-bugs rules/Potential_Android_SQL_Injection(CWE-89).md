### Potential Android SQL Injection
**Bug Pattern:** SQL_INJECTION_ANDROID  
[CWE-89: Improper Neutralization of Special Elements used in an SQL Command ('SQL Injection')](http://cwe.mitre.org/data/definitions/89.html)
#### 描述：
安卓中的SQL注入

- [x] 安全相关
#### 漏洞代码：

```
String query = "SELECT * FROM  messages WHERE uid= '"+userInput+"'" ;
Cursor cursor = this.getReadableDatabase().rawQuery(query,null);
```
#### 解决方法：

```
String query = "SELECT * FROM  messages WHERE uid= ?" ;
Cursor cursor = this.getReadableDatabase().rawQuery(query,new String[] {userInput});
```
##### check级别：AST
#### 相关信息(Android SQLite)：
1. [InformIT.com: Practical Advice for Building Secure Android Databases in SQLite](http://www.informit.com/articles/article.aspx?p=2268753&seqNum=5)
2. [Packtpub.com: Knowing the SQL-injection attacks and securing our Android applications from them](https://www.packtpub.com/books/content/knowing-sql-injection-attacks-and-securing-our-android-applications-them)
3. [Android Database Support (Enterprise Android: Programming Android Database Applications for the Enterprise)](https://books.google.ca/books?id=SXlMAQAAQBAJ&lpg=PR1&pg=PA64#v=onepage&q&f=false)
4. [Safe example of Insert, Select, Update and Delete queryies provided by Suragch](https://stackoverflow.com/a/29797229/89769)
