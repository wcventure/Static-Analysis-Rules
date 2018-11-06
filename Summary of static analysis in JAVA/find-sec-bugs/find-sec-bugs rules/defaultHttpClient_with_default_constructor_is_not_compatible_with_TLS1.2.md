### DefaultHttpClient with default constructor is not compatible with TLS 1.2 
**Bug Pattern:** DEFAULT_HTTP_CLIENT
#### 漏洞代码：

```
    HttpClient client = new DefaultHttpClient();
```
#### 解决方法：
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
#### check级别：AST
#### 相关信息：
[Diagnosing TLS, SSL, and HTTPS](https://blogs.oracle.com/java-platform-group/entry/diagnosing_tls_ssl_and_https)
