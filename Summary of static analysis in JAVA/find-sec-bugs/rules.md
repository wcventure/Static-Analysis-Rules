#### 1. Predictable pseudorandom number generator  
 
**Bug Pattern:** PREDICTABLE_RANDOM  
##### 描述 
在某些安全关键环境中使用可预测的随机值会导致漏洞。例如，当值被用作：  
-CSRF令牌：可预测的令牌可以导致CSRF攻击，攻击者将知道令牌的值  
-密码重置令牌（通过电子邮件发送）：可预测的密码令牌可以导致账户接管，因为攻击者会猜测更改密码表单的URL  
-用任何其他的加密方法替代： 
快速解决方案是替代java.util的使用。比如java.security.securerand

- [x] 是否安全相关  
##### 相关代码：

```
String generateSecretToken() {
    Random r = new Random();
    return Long.toHexString(r.nextLong());
}
```
##### 解决方法：

```
import org.apache.commons.codec.binary.Hex;

String generateSecretToken() {
    SecureRandom secRandom = new SecureRandom();

    byte[] result = new byte[32];
    secRandom.nextBytes(result);
    return Hex.encodeHexString(result);
}
```
##### 参考信息:

1. [Cracking Random Number Generators - Part 1 (http://jazzy.id.au)](http://jazzy.id.au/default/2010/09/20/cracking_random_number_generators_part_1.html)
2. [CERT: MSC02-J. Generate strong random numbers](https://www.securecoding.cert.org/confluence/display/java/MSC02-J.+Generate+strong+random+numbers)
3. [CWE-330: Use of Insufficiently Random Values](http://cwe.mitre.org/data/definitions/330.html)
4. [Predicting Struts CSRF Token (Example of real-life vulnerability and exploitation)](http://blog.h3xstream.com/2014/12/predicting-struts-csrf-token-cve-2014.html)
---

#### 2. Predictable pseudorandom number generator (Scala)
**Bug patten:** PREDICTABLE_RANDOM_SCALA 
##### 描述
The use of a predictable random value can lead to vulnerabilities when used in certain security critical contexts. For example, when the value is used as:

- a CSRF token: a predictable token can lead to a CSRF attack as an attacker will know the value of the token  
- a password reset token (sent by email): a predictable password token can lead to an account takeover, since an attacker will guess the URL of the change password form
- any other secret value 

A quick fix could be to replace the use of java.util.Random with something stronger, such as java.security.SecureRandom.

- [x] 是否安全相关
##### 代码:

```
String generateSecretToken() {
    Random r = new Random();
    return Long.toHexString(r.nextLong());
}
```
##### 解决方法：

```
import org.apache.commons.codec.binary.Hex;

String generateSecretToken() {
    SecureRandom secRandom = new SecureRandom();

    byte[] result = new byte[32];
    secRandom.nextBytes(result);
    return Hex.encodeHexString(result);
}
```
##### 参考信息：
1. [Cracking Random Number Generators - Part 1 (http://jazzy.id.au)](http://jazzy.id.au/default/2010/09/20/cracking_random_number_generators_part_1.html)
2. [CERT: MSC02-J. Generate strong random numbers](https://www.securecoding.cert.org/confluence/display/java/MSC02-J.+Generate+strong+random+numbers)
3. [CWE-330: Use of Insufficiently Random Values](http://cwe.mitre.org/data/definitions/330.html)
4. [Predicting Struts CSRF Token (Example of real-life vulnerability and exploitation)](http://blog.h3xstream.com/2014/12/predicting-struts-csrf-token-cve-2014.html)

---

#### 3.Untrusted servlet parameter

**Bug patten**:PREDICTABLE_RANDOM_SCALA  
##### 描述
The Servlet can read GET and POST parameters from various methods. The value obtained should be considered unsafe. You may need to validate or sanitize those values before passing them to sensitive APIs such as:
- SQL query (May lead to SQL injection)
- File opening (May lead to path traversal)
- Command execution (Potential Command injection)
- HTML construction (Potential XSS)
- etc..

<font size=3>**check级别:**</font> dataflow
- [x] 是否安全相关
##### 代码：

```
public static final double price = 20.00;
int quantity = currentUser.getAttribute("quantity");
double total = price * quantity;
chargeUser(total);
```


##### 参考信息：
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)  


---

#### 4.Untrusted Content-Type header 
**Bug Pattern:** SERVLET_CONTENT_TYPE  
##### 描述：
The HTTP header Content-Type can be controlled by the client. As such, its value should not be used in any security critical decisions.
- [x] 是否安全相关
##### 代码:

```
Cookie[] cookies = request.getCookies();
for (int i =0; i< cookies.length; i++) {
Cookie c = cookies[i];
if (c.getName().equals("role")) {
userRole = c.getValue();
}
}
```
**check级别**: dataflow
##### 参考信息：
[CWE-807: Untrusted Inputs in a Security Decision](http://cwe.mitre.org/data/definitions/807.html)


---

#### 5.Untrusted Hostname header
**Bug Pattern:** SERVLET_SERVER_NAME
##### 描述
The hostname header can be controlled by the client. As such, its value should not be used in any security critical decisions. Both ==ServletRequest.getServerName()== and ==HttpServletRequest.getHeader("Host")== have the same behavior which is to extract the ==Host== header.
- [x]  是否安全相关
##### 代码

```
GET /testpage HTTP/1.1
Host: www.example.com
[...]
```
The web container serving your application may redirect requests to your application by default. This would allow a malicious user to place any value in the Host header. It is recommended that you do not trust this value in any security decisions you make with respect to a request.  
**check级别**：dataflow
##### 参考信息：
[CWE-807: Untrusted Inputs in a Security Decision](http://cwe.mitre.org/data/definitions/807.html)


---

#### 6.Untrusted session cookie value 
**Bug Pattern:** SERVLET_SESSION_ID
##### 描述：
The method ==HttpServletRequest.getRequestedSessionId()== typically returns the value of the cookie JSESSIONID. This value is normally only accessed by the session management logic and not normal developer code.

The value passed to the client is generally an alphanumeric value (e.g., JSESSIONID=jp6q31lq2myn). However, the value can be altered by the client. The following HTTP request illustrates the potential modification.
- [x] 是否安全相关
##### 代码
```
GET /somePage HTTP/1.1
Host: yourwebsite.com
User-Agent: Mozilla/5.0
Cookie: JSESSIONID=Any value of the user's choice!!??'''">
```
As such, the JSESSIONID should only be used to see if its value matches an existing session ID. If it does not, the user should be considered an unauthenticated user. In addition, the session ID value should never be logged. If it is, then the log file could contain valid active session IDs, allowing an insider to hijack any sessions whose IDs have been logged and are still active.  

**check级别**：dataflow

##### 参考信息：
[OWASP: Session Management Cheat Sheet](https://www.owasp.org/index.php/Session_Management_Cheat_Sheet)  
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

---

#### 7.Untrusted query string 
**Bug Pattern:** SERVLET_QUERY_STRING
##### 描述
The query string is the concatenation of the GET parameter names and values. Parameters other than those intended can be passed in.

For the URL request ==/app/servlet.htm?a=1&b=2==, the query string extract will be ==a=1&b=2==

Just as is true for individual parameter values retrieved via methods like ==HttpServletRequest.getParameter()==, the value obtained from ==HttpServletRequest.getQueryString()== should be considered unsafe. You may need to validate or sanitize anything pulled from the query string before passing it to sensitive APIs.  
- [x] 安全相关
##### 代码

```
IntentFilter filter = new IntentFilter("com.example.URLHandler.openURL");
MyReceiver receiver = new MyReceiver();
registerReceiver(receiver, filter);
... 

public class UrlHandlerReceiver extends BroadcastReceiver {
@Override
public void onReceive(Context context, Intent intent) {
if("com.example.URLHandler.openURL".equals(intent.getAction())) {
String URL = intent.getStringExtra("URLToOpen");
int length = URL.length();
```
**check级别：** dataflow
##### 参考信息：
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

---
#### 8.Potentially sensitive data in a cookie  
**Bug Pattern:** COOKIE_USAGE
##### 描述：
存放在cookie中的信息不能包含敏感信息或者和session有关的信息。在大多数例子中，敏感的信息应该存放在session中并且通过用户的session cookie调用。如果敏感信息存放在cookie中，则很容易被攻击者窃取。
- [x] 安全相关  
##### 代码

```
response.addCookie( new Cookie("userAccountID", acctID);
```

**check级别**：dataflow
##### 参考信息：

[CWE-315: Cleartext Storage of Sensitive Information in a Cookie](http://cwe.mitre.org/data/definitions/315.html)

---

#### 9.Untrusted User-Agent header
**Bug Pattern:** COOKIE_USAGE
##### 描述：
HTTP头中的"User-Agent"字段可以很容易地被用户更改，因此根据User-Agent字段进行的操作不被推荐。
##### 代码：

```
String Agent = request.getHeader("User-Agent");
```
**check级别:** AST树


---

#### 10. Potential Path Traversal (file read) 
**Bug Pattern:** PATH_TRAVERSAL_IN
##### 描述：
未过滤的输入参数直接作为File对象的文件名输入，会导致对路径下其他文件读取。
- [x] 安全相关  
##### 代码：

```
@GET
@Path("/images/{image}")
@Produces("images/*")
public Response getImage(@javax.ws.rs.PathParam("image") String image) {
    File file = new File("resources/images/", image); //Weak point

    if (!file.exists()) {
        return Response.status(Status.NOT_FOUND).build();
    }

    return Response.ok().entity(new FileInputStream(file)).build();
}
```
##### 正确代码：

```
import org.apache.commons.io.FilenameUtils;

@GET
@Path("/images/{image}")
@Produces("images/*")
public Response getImage(@javax.ws.rs.PathParam("image") String image) {
    File file = new File("resources/images/", FilenameUtils.getName(image)); //Fix

    if (!file.exists()) {
        return Response.status(Status.NOT_FOUND).build();
    }

    return Response.ok().entity(new FileInputStream(file)).build();
}
```

**check级别：** AST
##### 参考信息：
[WASC: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)  
[OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)  
[CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)  
[CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://capec.mitre.org/data/definitions/126.html)


---

#### 11. Potential Path Traversal (file write)
**Bug Pattern:** PATH_TRAVERSAL_OUT
##### 描述：
与上个rule类似，如果直接不对输入的地址做任何过滤，那么用户将可以对任意路径的文件写入。  
- [x] 安全相关  
**check级别：**AST
##### 参考信息：
[WASC: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)  
[OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)  
[CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)  
[CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://capec.mitre.org/data/definitions/126.html)


---

#### 12. Potential Command Injection 
**Bug Pattern:** COMMAND_INJECTION
##### 描述：
未经过滤的输入会让执行的API执行任意命令。
- [x] 安全相关
##### 代码：

```
import java.lang.Runtime;

Runtime r = Runtime.getRuntime();
r.exec("/bin/sh -c some_tool" + input);
```
##### check级别：AST

##### 参考信息：
1. [OWASP: Command Injection](https://www.owasp.org/index.php/Command_Injection)  
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)  
3. [CWE-78: Improper Neutralization of Special Elements used in an OS Command ('OS Command Injection')](http://cwe.mitre.org/data/definitions/78.html)

---

#### 13. FilenameUtils not filtering null bytes
**Bug Pattern:** WEAK_FILENAMEUTILS  
##### 描述：
一些 FilenameUtils 方法不会过滤NULL 字节(0x00).如果mull注入到了文件名，而这些文件传给了底层的OS，那被检索的将是NULL之前的文件名，这样可以绕过文件后缀的检查。
- [x] 安全相关  
**check级别：** dataflow
##### 相关信息：
1. [OWASP: Command Injection](https://www.owasp.org/index.php/Command_Injection)  
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)
3. [CWE-78: Improper Neutralization of Special Elements used in an OS Command ('OS Command Injection')](http://cwe.mitre.org/data/definitions/78.html)

---


#### 14. TrustManager that accept any certificates
**Bug Pattern:** WEAK_TRUST_MANAGER
##### 描述：
空信任管理器的实现通常很容易被用来连接到一个没有根证书颁发机构签名的主机。这很容易受到中间人攻击，因为客户端会信任任何证书。 
应当建立一个证书的信任库。
- [x] 安全相关
##### 代码：

```
class TrustAllManager implements X509TrustManager {

    @Override
    public void checkClientTrusted(X509Certificate[] x509Certificates, String s) throws CertificateException {
        //Trust any client connecting (no certificate validation)
    }

    @Override
    public void checkServerTrusted(X509Certificate[] x509Certificates, String s) throws CertificateException {
        //Trust any remote server (no certificate validation)
    }

    @Override
    public X509Certificate[] getAcceptedIssuers() {
        return null;
    }
}
```
##### 解决方法:

```
KeyStore ks = //Load keystore containing the certificates trusted

SSLContext sc = SSLContext.getInstance("TLS");

TrustManagerFactory tmf = TrustManagerFactory.getInstance("SunX509");
tmf.init(ks);

sc.init(kmf.getKeyManagers(), tmf.getTrustManagers(),null);
```


##### 参考信息：
1. [WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)
2. [CWE-295: Improper Certificate Validation](http://cwe.mitre.org/data/definitions/295.html)
3. 

---


#### HostnameVerifier that accept any signed certificates
**Bug Pattern:** WEAK_HOSTNAME_VERIFIER
##### 描述：

HostnameVerifier对象经常会接受任何Host，因为许多主机的证书重用。因此，这很容易受到中间人攻击，因为客户端会信任任何证书。应该建立一个信托管理器，允许特定的证书。
- [x] 安全相关
##### 代码：

```
public class AllHosts implements HostnameVerifier {
    public boolean verify(final String hostname, final SSLSession session) {
        return true;
    }
}
```
##### 解决方法：

```
KeyStore ks = //Load keystore containing the certificates trusted

SSLContext sc = SSLContext.getInstance("TLS");

TrustManagerFactory tmf = TrustManagerFactory.getInstance("SunX509");
tmf.init(ks);

sc.init(kmf.getKeyManagers(), tmf.getTrustManagers(),null);
```
**check级别：** AST
##### 相关信息
1. [WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)
2. [CWE-295: Improper Certificate Validation](http://cwe.mitre.org/data/definitions/295.html)

---

##### Found JAX-WS SOAP endpoint
**Bug Pattern:** JAXWS_ENDPOINT
##### 描述：
该方法是SOAP的一部分。  
身份验证如果被执行，应该进行测试。
访问控制如果被执行，应该进行测试。
应该跟踪这些输入，以寻找潜在的漏洞。
最好是通过SSL来进行通信。
- [x] 安全相关  
**check级别：** AST, data-flow  
##### 相关信息：
1. [OWASP: REST Assessment Cheat Sheet](https://www.owasp.org/index.php/REST_Assessment_Cheat_Sheet)
2. [OWASP: REST Security Cheat Sheet](https://www.owasp.org/index.php/REST_Security_Cheat_Sheet)
3. [OWASP: Web Service Security Cheat Sheet](https://www.owasp.org/index.php/Web_Service_Security_Cheat_Sheet)
4. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_(CSRF))

5. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)

6. [CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

---
#### Found Tapestry page
**Bug Pattern:** TAPESTRY_ENDPOINT
##### 描述：
Tapestry应用程序的结构是支持Java类和对应的Tapestry标记语言页面（一个.tml文件）。当收到一个请求时，get/post参数被映射到后备Java类中的特定输入。输入的变量应该得到验证。映射是用fieldName完成的：

```
 [...]
    protected String input;
    [...]
```
或者显示注释的定义：

```
 [...]
    @org.apache.tapestry5.annotations.Parameter
    protected String parameter1;

    @org.apache.tapestry5.annotations.Component(id = "password")
    private PasswordField passwordField;
    [...]
```
该页面被映射到视图[/resources/package/PageName].tml.在这个应用程序中，每个Tapestry页面都应该被研究，以确保所有以这种方式自动映射的输入在使用之前都得到了正确的验证。
##### 相关信息：
1. [Apache Tapestry Home Page](http://tapestry.apache.org/)
2. [CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

---
#### MD2, MD4 and MD5 are weak hash functions
**Bug Pattern:** WEAK_MESSAGE_DIGEST_MD5
##### 描述：
MD2, MD4 and MD5是不被推荐的加密算法。例如哈希算法应当考虑使用PBKDF2.
- [x] 安全相关

##### 代码

```
MessageDigest md5Digest = MessageDigest.getInstance("MD5");
    md5Digest.update(password.getBytes());
    byte[] hashValue = md5Digest.digest();
    byte[] hashValue = DigestUtils.getMd5Digest().digest(password.getBytes());

```
##### 解决方法:
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
7. [CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)


---
#### SHA-1 is a weak hash function 
**Bug Pattern:** WEAK_MESSAGE_DIGEST_SHA1
##### 描述：
算法SHA-1并不是用于哈希密码、签名验证和其他用途的推荐算法。例如，应该使用PBKDF2来对密码进行散列。
- [x] 安全相关
##### 代码：

```
MessageDigest sha1Digest = MessageDigest.getInstance("SHA1");
    sha1Digest.update(password.getBytes());
    byte[] hashValue = sha1Digest.digest();
    
    byte[] hashValue = DigestUtils.getSha1Digest().digest(password.getBytes());

```
##### 解决方法：

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
##### check级别：AST
##### 相关信息：
1. [Qualys blog: SHA1 Deprecation: What You Need to Know](https://community.qualys.com/blogs/securitylabs/2014/09/09/sha1-deprecation-what-you-need-to-know)
2. [NIST: Transitions: Recommendation for Transitioning the Use of Cryptographic Algorithms and Key Lengths](http://nvlpubs.nist.gov/nistpubs/SpecialPublications/NIST.SP.800-131Ar1.pdf)  
3. [NIST: Recommendation for Password-Based Key Derivation](http://csrc.nist.gov/publications/nistpubs/800-132/nist-sp800-132.pdf)
4. [Stackoverflow: Reliable implementation of PBKDF2-HMAC-SHA256 for Java](https://stackoverflow.com/q/22580853/89769)
5. [CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)

---


#### DefaultHttpClient with default constructor is not compatible with TLS 1.2 **Bug Pattern:** DEFAULT_HTTP_CLIENT
##### 代码：

```
    HttpClient client = new DefaultHttpClient();
```
##### 解决方法：
1. 用SystemDefaultHttpClient 代替

```
SystemDefaultHttpClient 
```
2. 
* 创建基于SSLSocketFactory的HttpClient
* 创建基于SSLConnectionSocketFactory的HttpClient
* 在调用build（）之前使用HttpClientBuilder-调用useSystemProperties（）

```
    HttpClient client = HttpClientBuilder.create().useSystemProperties().build();

```
3. httpclient——调用createSystem（）来创建一个实例

```
    HttpClient client = HttpClients.createSystem();
```
##### check级别：AST
##### 相关信息：
[Diagnosing TLS, SSL, and HTTPS](https://blogs.oracle.com/java-platform-group/entry/diagnosing_tls_ssl_and_https)

---

##### Weak SSLContext 
**Bug Pattern:** SSL_CONTEXT
##### 描述：
SSL的安全性比较弱，最好换为更为安全的TLS。
##### 代码：

```
    SSLContext.getInstance("SSL");

```
##### 解决方法：
实现升级到以下内容，并配置https。协议JVM选项包括TLSv 1.2：

```
    SSLContext.getInstance("TLS");
```
##### 相关内容：
[Diagnosing TLS, SSL, and HTTPS](https://blogs.oracle.com/java-platform-group/entry/diagnosing_tls_ssl_and_https)

---
#### Message digest is custom
**Bug Pattern:** CUSTOM_MESSAGE_DIGEST
##### 描述：
实现自定义MessageDigest是容易出错的。NIST推荐使用SHA-224，SHA-256，SHA-384，SHA-512，SHA-512/224，或SHA-512/256。  
**用于数字签名生成的SHA-1：**
SHA-1只能用于特定于NIST协议特定指导的数字签名生成。对于所有其他应用程序，SHA-1不能用于数字签名生成。  
**用于数字签名验证的SHA-1：**
对于数字签名验证，SHA-1是允许使用的。  
**SHA-224，SHA-256，SHA-384，SHA-512，SHA-512/224，和SHA-512/256：**
所有哈希函数应用程序都可以使用这些哈希函数。

- [ ] 安全相关
##### 代码：

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
##### 解决方案：

```
MessageDigest sha256Digest = MessageDigest.getInstance("SHA256");
sha256Digest.update(password.getBytes());
```
##### check级别：AST

##### 相关信息：
1. [NIST Approved Hashing Algorithms](http://csrc.nist.gov/groups/ST/toolkit/secure_hashing.html)
2. [CWE-327: Use of a Broken or Risky Cryptographic Algorithm](http://cwe.mitre.org/data/definitions/327.html)

---

#### Tainted filename read
**Bug Pattern:** FILE_UPLOAD_FILENAME
##### 描述：
FileUpload API提供的文件名可以被客户机篡改，以引用未经授权的文件。
- [x] 安全相关

##### 例子：

- "../../../config/overide_file"
- "shell.jsp\u0000expected.gif"

因此，这些值不应该直接传递给文件系统API。如果可接受，应用程序应该生成自己的文件名并使用它们。否则，应正确验证所提供的文件名，以确保其结构正确，不包含未经授权的路径字符（例如，/），并引用一个授权文件。
##### 相关信息：
1. [Securiteam: File upload security recommendations](http://blogs.securiteam.com/index.php/archives/1268)
2. [CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://cwe.mitre.org/data/definitions/22.html)
3. [WASC-33: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)
4. [OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)
5. [CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)
6. [CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://cwe.mitre.org/data/definitions/22.html)

---

#### Regex DOS (ReDOS) 
**Bug Pattern:** REDOS
##### 描述：
正则表达式（regexs）经常被拒绝服务（DOS）攻击（称为ReDOS）。这是因为在分析某些字符串时，regex引擎可能会花费大量的时间，这取决于正则表达式的定义。  
例如，对于regex：（a+）+$，输入“aaaaaaaaaaaaaax”将导致正则引擎分析65536条不同的路径。因此，单个请求可能会在服务器端造成大量的计算。这个正则表达式的问题，以及其他类似的问题，是由于括号内的+（或a）和括号之外的+（或a），所以有两种不同的方式可以被正则表达式接受。这样写的方式，要么+可以使用“a”这个字符。为了解决这个问题，应该重写正则表达式来消除歧义。例如，这可以简单地重写为：a+$（任意数量的a）。假设这就是最初的正则表达式的含义，那么这个新的正则表达式可以快速地进行评估，并且不受ReDOS的影响。
- [ ] 安全相关

##### check级别：AST
##### 相关信息：
1. [Sebastian Kubeck's Weblog: Detecting and Preventing ReDoS Vulnerabilities](http://www.jroller.com/sebastianKuebeck/entry/detecting_and_preventing_redos_vulnerabilities)
2. [OWASP: Regular expression Denial of Service](https://www.owasp.org/index.php/Regular_expression_Denial_of_Service_-_ReDoS)
3. [CWE-400: Uncontrolled Resource Consumption ('Resource Exhaustion')](http://cwe.mitre.org/data/definitions/400.html)

---

#### Spring CSRF protection disabled 
**Bug Pattern:**SPRING_CSRF_PROTECTION_DISABLED
##### 描述：
禁用Spring Security的CSRF保护对于标准web应用程序是不安全的。
- [x] 安全相关：

##### 代码：

```
@EnableWebSecurity
public class WebSecurityConfig extends WebSecurityConfigurerAdapter {

    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http.csrf().disable();
    }
}
```
##### check级别：AST
##### 相关信息：
1. [Spring Security Official Documentation: When to use CSRF protection](https://docs.spring.io/spring-security/site/docs/current/reference/html/csrf.html#when-to-use-csrf-protection)
2. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29)
3. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)
4. [CWE-352: Cross-Site Request Forgery (CSRF)](https://cwe.mitre.org/data/definitions/352.html)

---
#### Spring CSRF unrestricted RequestMapping 
**Bug Pattern:** SPRING_CSRF_UNRESTRICTED_REQUEST_MAPPING
##### 描述：
用RequestMapping注释的方法默认映射到所有HTTP请求方法。然而，Spring Security的CSRF保护在默认情况下是不支持HTTP请求方法GET、HEAD、TRACE和OPTIONS（因为这可能会导致token被泄露）。因此，带有RequestMapping注释的状态变化方法，而不是将映射缩小到HTTP请求方法POST、PUT、DELETE或PATCH，都很容易受到CSRF攻击的影响。
- [x] 安全相关
##### 代码：

```
@Controller
public class UnsafeController {

    @RequestMapping("/path")
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```
##### 解决方法
1. Spring Framework 4.3 and later：

```
@Controller
public class SafeController {

    /**
     * For methods without side-effects use @GetMapping.
     */
    @GetMapping("/path")
    public String readData() {
        // No state-changing operations performed within this method.
        return "";
    }

    /**
     * For state-changing methods use either @PostMapping, @PutMapping, @DeleteMapping, or @PatchMapping.
     */
    @PostMapping("/path")
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```

2. Before Spring Framework 4.3、

```
@Controller
public class SafeController {

    /**
     * For methods without side-effects use either
     * RequestMethod.GET, RequestMethod.HEAD, RequestMethod.TRACE, or RequestMethod.OPTIONS.
     */
    @RequestMapping(value = "/path", method = RequestMethod.GET)
    public String readData() {
        // No state-changing operations performed within this method.
        return "";
    }

    /**
     * For state-changing methods use either
     * RequestMethod.POST, RequestMethod.PUT, RequestMethod.DELETE, or RequestMethod.PATCH.
     */
    @RequestMapping(value = "/path", method = RequestMethod.POST)
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```
##### check级别：AST
##### 相关信息：
1. [Spring Security Official Documentation: Use proper HTTP verbs (CSRF protection)](https://docs.spring.io/spring-security/site/docs/current/reference/html/csrf.html#csrf-use-proper-verbs)
2. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29)
3. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)
4. [CWE-352: Cross-Site Request Forgery (CSRF)](https://cwe.mitre.org/data/definitions/352.html)


---

#### CUSTOM_INJECTION
**Bug Pattern:** SQL_INJECTION_JDO
##### 描述：
识别方法很容易被注入，输入应当被验证并正确地转义。
- [x] 安全相关

##### 代码：

```
SqlUtil.execQuery("select * from UserEntity t where id = " + parameterInput);

```
##### check级别：AST
