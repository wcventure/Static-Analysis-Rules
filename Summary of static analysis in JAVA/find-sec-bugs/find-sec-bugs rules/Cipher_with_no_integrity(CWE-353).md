### Cipher with no integrity 
**Bug Pattern:** PADDING_ORACLE

[CWE-353: Missing Support for Integrity Check](http://cwe.mitre.org/data/definitions/353.html)

#### 描述：
产生的密文容易被对手改变。这意味着密码无法检测数据是否被篡改。如果密文可以由攻击者控制，它可以被篡改而不被发现。

解决方案是使用包含基于散列的消息身份验证码(HMAC)的密码来签署数据。将HMAC函数与现有的密码相结合容易出错([The Cryptographic Doom Principle](https://moxie.org/blog/the-cryptographic-doom-principle/))。具体地说，总是建议您能够首先验证HMAC，并且只有在数据未被修改的情况下，才会对数据执行任何加密功能。

以下模式很容易受到攻击，因为它们不提供HMAC:
- CBC
- OFB
- CTR
- ECB

The ciphertext produced is susceptible to alteration by an adversary. This mean that the cipher provides no way to detect that the data has been tampered with. If the ciphertext can be controlled by an attacker, it could be altered without detection.

The solution is to used a cipher that includes a Hash based Message Authentication Code (HMAC) to sign the data. Combining a HMAC function to the existing cipher is prone to error ([The Cryptographic Doom Principle](https://moxie.org/blog/the-cryptographic-doom-principle/)). Specifically, it is always recommended that you be able to verify the HMAC first, and only if the data is unmodified, do you then perform any cryptographic functions on the data.

The following modes are vulnerable because they don't provide a HMAC:
- CBC
- OFB
- CTR
- ECB

- [x] 安全相关

#### 漏洞代码：
AES in CBC mode
```
Cipher c = Cipher.getInstance("AES/CBC/PKCS5Padding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```
Triple DES with ECB mode

```
Cipher c = Cipher.getInstance("DESede/ECB/PKCS5Padding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```

#### 解决方案
```
Cipher c = Cipher.getInstance("AES/GCM/NoPadding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```
In the example solution above, the GCM mode introduces an HMAC into the resulting encrypted data, providing integrity of the result.
##### Check级别：AST

#### 相关信息：
[Wikipedia: Authenticated encryption](http://en.wikipedia.org/wiki/Authenticated_encryption)

[NIST: Authenticated Encryption Modes](http://csrc.nist.gov/groups/ST/toolkit/BCM/modes_development.html#01)

[Moxie Marlinspike's blog: The Cryptographic Doom Principle](http://www.thoughtcrime.org/blog/the-cryptographic-doom-principle/)

[CWE-353: Missing Support for Integrity Check](http://cwe.mitre.org/data/definitions/353.html)