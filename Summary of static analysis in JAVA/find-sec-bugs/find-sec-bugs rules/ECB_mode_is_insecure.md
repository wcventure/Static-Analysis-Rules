### ECB mode is insecure
**Bug Pattern:** ECB_MODE

#### 描述：
一种为加密数据提供更好的机密性的身份验证密码模式，应该被用来代替电子码本(ECB)模式，它不能提供良好的机密性。具体地说，欧洲央行模式每次都为相同的输入输出相同的输出。例如，如果用户发送了一个密码，那么每次加密的值都是相同的。这允许攻击者拦截和重放数据。
要解决这个问题，应该使用Galois/Counter模式(GCM)。

An authentication cipher mode which provides better confidentiality of the encrypted data should be used instead of Electronic Codebook (ECB) mode, which does not provide good confidentiality. Specifically, ECB mode produces the same output for the same input each time. So, for example, if a user is sending a password, the encrypted value is the same each time. This allows an attacker to intercept and replay the data.

To fix this, something like Galois/Counter Mode (GCM) should be used instead.

- [x] 安全相关

#### 漏洞代码：
```
Cipher c = Cipher.getInstance("AES/ECB/NoPadding");
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
[Wikipedia: Authenticated encryption](http://en.wikipedia.org/wiki/Authenticated_encryption)

[NIST: Authenticated Encryption Modes](http://csrc.nist.gov/groups/ST/toolkit/BCM/modes_development.html#01)

[Wikipedia: Block cipher modes of operation](http://en.wikipedia.org/wiki/Block_cipher_modes_of_operation#Electronic_codebook_.28ECB.29)

[NIST: Recommendation for Block Cipher Modes of Operation](http://csrc.nist.gov/publications/nistpubs/800-38a/sp800-38a.pdf)