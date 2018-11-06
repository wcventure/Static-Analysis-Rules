###  Predictable pseudorandom number generator  
 
**Bug Pattern:** PREDICTABLE_RANDOM  
 
在某些安全关键环境中使用可预测的随机值会导致漏洞。  

[CWE-330: Use of Insufficiently Random Values](http://cwe.mitre.org/data/definitions/330.html)
#### 描述：
The use of a predictable random value can lead to vulnerabilities when used in certain security critical contexts. For example, when the value is used as:

- a CSRF token: a predictable token can lead to a CSRF attack as an attacker will know the value of the token  
- a password reset token (sent by email): a predictable password token can lead to an account takeover, since an attacker will guess the URL of the change password form
any other secret value  
- A quick fix could be to replace the use of java.util.Random with something stronger, such as java.security.SecureRandom.
- [x] 是否安全相关  
#### 相关代码：

```
String generateSecretToken() {
    Random r = new Random();
    return Long.toHexString(r.nextLong());
}
```
#### 解决方法：

```
import org.apache.commons.codec.binary.Hex;

String generateSecretToken() {
    SecureRandom secRandom = new SecureRandom();

    byte[] result = new byte[32];
    secRandom.nextBytes(result);
    return Hex.encodeHexString(result);
}
```
#### check级别：AST
#### 参考信息:

1. [Cracking Random Number Generators - Part 1 (http://jazzy.id.au)](http://jazzy.id.au/default/2010/09/20/cracking_random_number_generators_part_1.html)
2. [CERT: MSC02-J. Generate strong random numbers](https://www.securecoding.cert.org/confluence/display/java/MSC02-J.+Generate+strong+random+numbers)
3. [CWE-330: Use of Insufficiently Random Values](http://cwe.mitre.org/data/definitions/330.html)
4. [Predicting Struts CSRF Token (Example of real-life vulnerability and exploitation)](http://blog.h3xstream.com/2014/12/predicting-struts-csrf-token-cve-2014.html)
