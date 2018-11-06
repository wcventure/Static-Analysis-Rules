### World writable file (Android) 
**Bug Pattern:** ANDROID_WORLD_WRITABLE

[CWE-312: Cleartext Storage of Sensitive Information](https://cwe.mitre.org/data/definitions/312.html)

[CWE-927: Use of Implicit Intent for Sensitive Communication](https://cwe.mitre.org/data/definitions/927.html)

#### 描述：
在此上下文中写入的文件使用的是创建模式mode_world_read。可能不是预期的行为暴露了所写的内容。

The file written in this context is using the creation mode MODE_WORLD_READABLE. It might not be the expected behavior to exposed the content being written.
- [x] 安全相关

#### 漏洞代码：

```
fos = openFileOutput(filename, MODE_WORLD_READABLE);
fos.write(userInfo.getBytes());
```

#### 解决方案
使用 MODE_PRIVATE：
```
fos = openFileOutput(filename, MODE_PRIVATE);
```
使用local SQLite Database：

Using a local SQLite database is probably the best solution to store structured data. Make sure the database file is not create on external storage. See references below for implementation guidelines.


##### Check级别：AST

#### 相关信息：

[CERT: DRD11-J. Ensure that sensitive data is kept secure](https://www.securecoding.cert.org/confluence/display/java/DRD11-J.+Ensure+that+sensitive+data+is+kept+secure)

[Android Official Doc: Security Tips](http://developer.android.com/training/articles/security-tips.html#InternalStorage)

[Android Official Doc: Context.MODE_PRIVATE](http://developer.android.com/reference/android/content/Context.html#MODE_PRIVATE)

[vogella.com: Android SQLite database and content provider - Tutorial](http://www.vogella.com/tutorials/AndroidSQLite/article.html#databasetutorial_database)

[OWASP Mobile Top 10 2014-M2: Insecure Data Storage](https://www.owasp.org/index.php/Mobile_Top_10_2014-M2)

[CWE-312: Cleartext Storage of Sensitive Information](https://cwe.mitre.org/data/definitions/312.html)