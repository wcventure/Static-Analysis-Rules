### Hard Coded Password
**Bug Pattern:** HARD_CODE_PASSWORD

[CWE-259: Use of Hard-coded Password](http://cwe.mitre.org/data/definitions/259.html)

#### 描述：
密码不应保存在源代码中。源代码可以在企业环境中广泛共享，并且可以在开源中共享。要安全管理，密码和密钥应该存储在单独的配置文件或密钥存储库中。(硬编码键分别用硬编码的键模式分别报告)

Passwords should not be kept in the source code. The source code can be widely shared in an enterprise environment, and is certainly shared in open source. To be managed safely, passwords and secret keys should be stored in separate configuration files or keystores. (Hard coded keys are reported separately by Hard Coded Key pattern)

- [x] 安全相关

#### 漏洞代码：
```
private String SECRET_PASSWORD = "letMeIn!";

Properties props = new Properties();
props.put(Context.SECURITY_CREDENTIALS, "p@ssw0rd");
```

##### Check级别：AST

#### 相关信息：

[CWE-259: Use of Hard-coded Password](http://cwe.mitre.org/data/definitions/259.html)