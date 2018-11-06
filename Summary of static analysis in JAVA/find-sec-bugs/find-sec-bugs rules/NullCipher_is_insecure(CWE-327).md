### NullCipher is insecure 
**Bug Pattern:** NULL_CIPHER

[CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)

#### 描述：
在产品应用中很少使用NullCipher。它通过返回与提供的明文相同的密文来实现密码接口。在一些情况下，像测试，一个NullCipher可能是合适的。

The NullCipher is rarely used intentionally in production applications. It implements the Cipher interface by returning ciphertext identical to the supplied plaintext. In a few contexts, such as testing, a NullCipher may be appropriate.

- [x] 安全相关

#### 漏洞代码：
```
Cipher doNothingCihper = new NullCipher();
[...]
//The ciphertext produced will be identical to the plaintext.
byte[] cipherText = c.doFinal(plainText);
```
#### 解决方案：
避免使用NullCipher。它的偶然使用会带来重大的保密风险。

##### Check级别：AST

#### 相关信息：

[CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)