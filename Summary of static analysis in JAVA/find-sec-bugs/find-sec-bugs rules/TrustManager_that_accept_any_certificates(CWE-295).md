### TrustManager that accept any certificates
**Bug Pattern:** WEAK_TRUST_MANAGER
 [CWE-295: Improper Certificate Validation](http://cwe.mitre.org/data/definitions/295.html)
#### 描述：
空信任管理器的实现通常很容易被用来连接到一个没有根证书颁发机构签名的主机。这很容易受到中间人攻击，因为客户端会信任任何证书。 
应当建立一个证书的信任库。
- [x] 安全相关
#### 代码：

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
#### 解决方法:

```
KeyStore ks = //Load keystore containing the certificates trusted

SSLContext sc = SSLContext.getInstance("TLS");

TrustManagerFactory tmf = TrustManagerFactory.getInstance("SunX509");
tmf.init(ks);

sc.init(kmf.getKeyManagers(), tmf.getTrustManagers(),null);
```


#### 参考信息：
1. [WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)
