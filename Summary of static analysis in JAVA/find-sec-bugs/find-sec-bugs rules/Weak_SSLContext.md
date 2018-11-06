### Weak SSLContext 
**Bug Pattern:** SSL_CONTEXT
#### 描述：
SSL的安全性比较弱，最好换为更为安全的TLS。
#### 代码：

```
    SSLContext.getInstance("SSL");

```
#### 解决方法：
实现升级到以下内容，并配置https。协议JVM选项包括TLSv 1.2：

```
    SSLContext.getInstance("TLS");
```
#### 相关内容：
[Diagnosing TLS, SSL, and HTTPS](https://blogs.oracle.com/java-platform-group/entry/diagnosing_tls_ssl_and_https)
