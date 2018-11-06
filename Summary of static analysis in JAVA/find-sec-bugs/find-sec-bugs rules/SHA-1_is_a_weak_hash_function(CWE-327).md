### SHA-1 is a weak hash function 
**Bug Pattern:** WEAK_MESSAGE_DIGEST_SHA1  
[CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)
#### 描述：
算法SHA-1并不是用于哈希密码、签名验证和其他用途的推荐算法。例如，应该使用PBKDF2来对密码进行散列。
- [x] 安全相关

#### 漏洞代码：

```
MessageDigest sha1Digest = MessageDigest.getInstance("SHA1");
    sha1Digest.update(password.getBytes());
    byte[] hashValue = sha1Digest.digest();
    
    byte[] hashValue = DigestUtils.getSha1Digest().digest(password.getBytes());

```
#### 解决方法：

1. Using bouncy castle
```
public static byte[] getEncryptedPassword(String password, byte[] salt) throws NoSuchAlgorithmException, InvalidKeySpecException {
    PKCS5S2ParametersGenerator gen = new PKCS5S2ParametersGenerator(new SHA256Digest());
    gen.init(password.getBytes("UTF-8"), salt.getBytes(), 4096);
    return ((KeyParameter) gen.generateDerivedParameters(256)).getKey();
}
```
2. Java 8 and later

```
public static byte[] getEncryptedPassword(String password, byte[] salt) throws NoSuchAlgorithmException, InvalidKeySpecException {
    KeySpec spec = new PBEKeySpec(password.toCharArray(), salt, 4096, 256 * 8);
    SecretKeyFactory f = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256");
    return f.generateSecret(spec).getEncoded();
}
```
#### check级别：AST
#### 相关信息：
1. [Qualys blog: SHA1 Deprecation: What You Need to Know](https://community.qualys.com/blogs/securitylabs/2014/09/09/sha1-deprecation-what-you-need-to-know)
2. [NIST: Transitions: Recommendation for Transitioning the Use of Cryptographic Algorithms and Key Lengths](http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-131Ar1.pdf)  
3. [NIST: Recommendation for Password-Based Key Derivation](http://csrc.nist.gov/publications/nistpubs/800-132/nist-sp800-132.pdf)
4. [Stackoverflow: Reliable implementation of PBKDF2-HMAC-SHA256 for Java](https://stackoverflow.com/q/22580853/89769)
