### HostnameVerifier that accept any signed certificates
**Bug Pattern:** WEAK_HOSTNAME_VERIFIER
[CWE-295: Improper Certificate Validation](http://cwe.mitre.org/data/definitions/295.html)
#### 描述：

HostnameVerifier对象经常会接受任何Host，因为许多主机的证书重用。因此，这很容易受到中间人攻击，因为客户端会信任任何证书。应该建立一个信托管理器，允许特定的证书。
- [x] 安全相关
#### 代码：

```
public class AllHosts implements HostnameVerifier {
    public boolean verify(final String hostname, final SSLSession session) {
        return true;
    }
}
```
#### 解决方法：

```
KeyStore ks = //Load keystore containing the certificates trusted

SSLContext sc = SSLContext.getInstance("TLS");

TrustManagerFactory tmf = TrustManagerFactory.getInstance("SunX509");
tmf.init(ks);

sc.init(kmf.getKeyManagers(), tmf.getTrustManagers(),null);
```
#### check级别： AST
#### 相关信息
1. [WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)
