### Blowfish usage with short key 
**Bug Pattern:** BLOWFISH_KEY_SIZE

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)

#### 描述：
Blowfish密码支持密钥大小从32位到448位。一个小的密钥大小使得密文容易受到暴力攻击。如果需要使用Blowfish，在产生钥匙时至少要使用128位的熵。
如果可以改变算法，则应该使用AES块密码。

The Blowfish cipher supports keysizes from 32 bits to 448 bits. A small key size makes the ciphertext vulnerable to brute force attacks. At least 128 bits of entropy should be used when generating the key if use of Blowfish is required.

If the algorithm can be changed, the AES block cipher should be used instead.

- [x] 安全相关

#### 漏洞代码：
```
KeyGenerator keyGen = KeyGenerator.getInstance("Blowfish");
keyGen.init(64);
```

#### 解决方案
```
KeyGenerator keyGen = KeyGenerator.getInstance("Blowfish");
keyGen.init(128);
```

##### Check级别：data-flow

#### 相关信息：
[Blowfish (cipher)](http://en.wikipedia.org/wiki/Blowfish_(cipher))

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)
