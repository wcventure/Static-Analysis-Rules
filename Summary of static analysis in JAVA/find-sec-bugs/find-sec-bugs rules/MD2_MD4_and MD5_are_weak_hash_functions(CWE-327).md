### MD2, MD4 and MD5 are weak hash functions
**Bug Pattern:** WEAK_MESSAGE_DIGEST_MD5  
[CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)
#### 描述：
MD2, MD4 and MD5是不被推荐的加密算法。例如哈希算法应当考虑使用PBKDF2.
- [x] 安全相关

#### 漏洞代码:

```
MessageDigest md5Digest = MessageDigest.getInstance("MD5");
    md5Digest.update(password.getBytes());
    byte[] hashValue = md5Digest.digest();
    byte[] hashValue = DigestUtils.getMd5Digest().digest(password.getBytes());

```
#### 解决方法:
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
**check级别：** AST
##### 相关信息：
1. [On Collisions for MD5: ](http://www.win.tue.nl/hashclash/On%20Collisions%20for%20MD5%20-%20M.M.J.%20Stevens.pdf)
2. [Chosen-prefix collisions for MD5 and applications: Paper written by Marc Stevens](http://homepages.cwi.nl/~stevens/papers/stJOC%20-%20Chosen-Prefix%20Collisions%20for%20MD5%20and%20Applications.pdf/)
3. [Wikipedia: MD5](https://en.wikipedia.org/wiki/MD5)
4. [NIST: Transitions: Recommendation for Transitioning the Use of Cryptographic Algorithms and Key Lengths](http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-131Ar1.pdf)
5. [NIST: Recommendation for Password-Based Key Derivation](http://csrc.nist.gov/publications/nistpubs/800-132/nist-sp800-132.pdf)
6. [Stackoverflow: Reliable implementation of PBKDF2-HMAC-SHA256 for Java](https://stackoverflow.com/q/22580853/89769)
