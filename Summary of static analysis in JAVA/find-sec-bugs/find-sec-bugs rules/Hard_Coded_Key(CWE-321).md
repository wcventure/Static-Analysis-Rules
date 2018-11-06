### Hard Coded Key
**Bug Pattern:** HARD_CODE_KEY

[CWE-321: Use of Hard-coded Cryptographic Key](http://cwe.mitre.org/data/definitions/321.html)

#### 描述：
密码不应保存在源代码中。源代码可以在企业环境中广泛共享，并且可以在开源中共享。要安全管理，密码和密钥应该存储在单独的配置文件或密钥存储库中。(硬编码键分别用硬编码的键模式分别报告)

Cryptographic keys should not be kept in the source code. The source code can be widely shared in an enterprise environment, and is certainly shared in open source. To be managed safely, passwords and secret keys should be stored in separate configuration files or keystores. (Hard coded passwords are reported separately by Hard Coded Password pattern)

- [x] 安全相关

#### 漏洞代码：
```
byte[] key = {1, 2, 3, 4, 5, 6, 7, 8};
SecretKeySpec spec = new SecretKeySpec(key, "AES");
Cipher aes = Cipher.getInstance("AES");
aes.init(Cipher.ENCRYPT_MODE, spec);
return aesCipher.doFinal(secretData);
```

##### Check级别：AST

#### 相关信息：

[CWE-321: Use of Hard-coded Cryptographic Key](http://cwe.mitre.org/data/definitions/321.html)