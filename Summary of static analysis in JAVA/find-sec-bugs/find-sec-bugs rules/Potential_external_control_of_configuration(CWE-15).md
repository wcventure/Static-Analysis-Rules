### Potential external control of configuration
**Bug Pattern:** EXTERNAL_CONFIG_CONTROL

[CWE-15: External Control of System or Configuration Setting](http://cwe.mitre.org/data/definitions/15.html)

#### 描述：
允许外部控制的系统设置会破坏服务或者导致应用程序以意外的、潜在的恶意方式运行。攻击者可以通过提供不存在的目录名或者连接到数据库的未授权部分而造成错误。

Allowing external control of system settings can disrupt service or cause an application to behave in unexpected, and potentially malicious ways. An attacker could cause an error by providing a nonexistent catalog name or connect to an unauthorized portion of the database.

- [x] 安全相关

#### 漏洞代码：
```
conn.setCatalog(request.getParameter("catalog"));
```

##### Check级别：AST

#### 相关信息：

[CWE-15: External Control of System or Configuration Setting](http://cwe.mitre.org/data/definitions/15.html)