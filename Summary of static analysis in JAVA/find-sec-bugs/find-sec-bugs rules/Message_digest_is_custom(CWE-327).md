### Message digest is custom
**Bug Pattern:** CUSTOM_MESSAGE_DIGEST
[CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)
#### 描述：
实现自定义MessageDigest是容易出错的。NIST推荐使用SHA-224，SHA-256，SHA-384，SHA-512，SHA-512/224，或SHA-512/256。  
**用于数字签名生成的SHA-1：**
SHA-1只能用于特定于NIST协议特定指导的数字签名生成。对于所有其他应用程序，SHA-1不能用于数字签名生成。  
**用于数字签名验证的SHA-1：**
对于数字签名验证，SHA-1是允许使用的。  
**SHA-224，SHA-256，SHA-384，SHA-512，SHA-512/224，和SHA-512/256：**
所有哈希函数应用程序都可以使用这些哈希函数。

- [x] 安全相关
#### 漏洞代码：

```
MyProprietaryMessageDigest extends MessageDigest {
    @Override
    protected byte[] engineDigest() {
        [...]
        //Creativity is a bad idea
        return [...];
    }
}
```
#### 解决方案：

```
MessageDigest sha256Digest = MessageDigest.getInstance("SHA256");
sha256Digest.update(password.getBytes());
```
#### check级别：AST

#### 相关信息：
1. [NIST Approved Hashing Algorithms](http://csrc.nist.gov/groups/ST/toolkit/secure_hashing.html)
