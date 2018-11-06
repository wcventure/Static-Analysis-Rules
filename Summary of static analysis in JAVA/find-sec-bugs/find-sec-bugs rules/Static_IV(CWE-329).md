### Static IV
**Bug Pattern:** STATIC_IV

[CWE-329: Not Using a Random IV with CBC Mode](http://cwe.mitre.org/data/definitions/329.html)

#### 描述：
必须为每个要加密的消息重新生成初始化向量。

Initialization vector must be regenerated for each message to be encrypted.

- [x] 安全相关

#### 漏洞代码：
```
private static byte[] IV = new byte[16] {(byte)0,(byte)1,(byte)2,[...]};

public void encrypt(String message) throws Exception {

    IvParameterSpec ivSpec = new IvParameterSpec(IV);
[...]
```

#### 解决方案
```
public void encrypt(String message) throws Exception {

    byte[] iv = new byte[16];
    new SecureRandom().nextBytes(iv);

    IvParameterSpec ivSpec = new IvParameterSpec(iv);
[...]
```

##### Check级别：AST

#### 相关信息：
[Wikipedia: Initialization vector](http://en.wikipedia.org/wiki/Initialization_vector)

[CWE-329: Not Using a Random IV with CBC Mode](http://cwe.mitre.org/data/definitions/329.html)

[Encryption - CBC Mode IV: Secret or Not?](https://defuse.ca/cbcmodeiv.htm)