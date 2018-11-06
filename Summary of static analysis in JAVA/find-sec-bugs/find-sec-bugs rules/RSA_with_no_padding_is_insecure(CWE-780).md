### RSA with no padding is insecure
**Bug Pattern:** RSA_NO_PADDING

[CWE-780: Use of RSA Algorithm without OAEP](http://cwe.mitre.org/data/definitions/780.html)

#### 描述：
软件使用RSA算法，但不兼容最优的非对称加密填充(OAEP)，这可能会削弱加密。

The software uses the RSA algorithm but does not incorporate Optimal Asymmetric Encryption Padding (OAEP), which might weaken the encryption.

- [x] 安全相关

#### 漏洞代码：
```
Cipher.getInstance("RSA/NONE/NoPadding")
```
#### 解决方案：
上述代码应用下面代码替换：
```
Cipher.getInstance("RSA/ECB/OAEPWithMD5AndMGF1Padding")

```

##### Check级别：AST

#### 相关信息：

[CWE-780: Use of RSA Algorithm without OAEP](http://cwe.mitre.org/data/definitions/780.html)

[Root Labs: Why RSA encryption padding is critical](http://rdist.root.org/2009/10/06/why-rsa-encryption-padding-is-critical/)