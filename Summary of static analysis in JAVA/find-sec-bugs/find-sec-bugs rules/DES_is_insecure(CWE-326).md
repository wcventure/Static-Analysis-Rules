### DES is insecure 
**Bug Pattern:** DES_USAGE

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)

#### 描述：
DES被认为是现代应用的强大密码。目前，NIST建议使用AES块密码，而不是DES。

DES is considered strong ciphers for modern applications. Currently, NIST recommends the usage of AES block ciphers instead of DES.

- [x] 安全相关

#### 漏洞代码：
```
Cipher c = Cipher.getInstance("DES/ECB/PKCS5Padding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```
#### 解决方案：
```
Cipher c = Cipher.getInstance("AES/GCM/NoPadding");
c.init(Cipher.ENCRYPT_MODE, k, iv);
byte[] cipherText = c.doFinal(plainText);
```

##### Check级别：AST

#### 相关信息：
[NIST Withdraws Outdated Data Encryption Standard](http://www.nist.gov/itl/fips/060205_des.cfm)

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)