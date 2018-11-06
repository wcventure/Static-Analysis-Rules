### ECB mode is insecure
**Bug Pattern:** PADDING_ORACLE

[CWE-696: Incorrect Behavior Order](http://cwe.mitre.org/data/definitions/696.html)

#### 描述：
CBC与PKCS5Padding的这种特定模式容易受到oracle攻击。如果系统暴露了明文与无效填充或有效填充之间的区别，则对手可能会对消息进行解密。有效和无效填充之间的区别通常通过在每个条件返回的不同的错误消息来显示。

This specific mode of CBC with PKCS5Padding is susceptible to padding oracle attacks. An adversary could potentially decrypt the message if the system exposed the difference between plaintext with invalid padding or valid padding. The distinction between valid and invalid padding is usually revealed through distinct error messages being returned for each condition.

- [x] 安全相关

#### 漏洞代码：
```
Cipher c = Cipher.getInstance("AES/CBC/PKCS5Padding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```

#### 解决方案
```
Cipher c = Cipher.getInstance("AES/GCM/NoPadding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```

##### Check级别：AST

#### 相关信息：
[Padding Oracles for the masses (by Matias Soler)](http://www.infobytesec.com/down/paddingoracle_openjam.pdf)

[Wikipedia: Authenticated encryption](http://en.wikipedia.org/wiki/Authenticated_encryption)

[NIST: Authenticated Encryption Modes](http://csrc.nist.gov/groups/ST/toolkit/BCM/modes_development.html#01)

[CAPEC: Padding Oracle Crypto Attack](http://capec.mitre.org/data/definitions/463.html)

[CWE-696: Incorrect Behavior Order](http://cwe.mitre.org/data/definitions/696.html)
