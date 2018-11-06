### Unencrypted Socket 
**Bug Pattern:** UNENCRYPTED_SOCKET

[CWE-319: Cleartext Transmission of Sensitive Information](http://cwe.mitre.org/data/definitions/319.html)

#### 描述：
使用的通信通道未加密。攻击者可以通过拦截网络流量来读取流量。

The communication channel used is not encrypted. The traffic could be read by an attacker intercepting the network traffic.

- [x] 安全相关

#### 漏洞代码：
```
Plain socket (Cleartext communication):

Socket soc = new Socket("www.google.com",80);
```
#### 解决方案：
```
SSL Socket (Secure communication):

Socket soc = SSLSocketFactory.getDefault().createSocket("www.google.com", 443);
```
除了使用SSL Socket之外，你还需要确保使用SSLSocketFactory 进行所有适当的证书验证检查，以保证你不受中间人攻击。请阅读OWASP传输层保护备忘录了解详细情况，了解如何正确操作。

##### Check级别：AST

#### 相关信息：
[OWASP: Top 10 2010-A9-Insufficient Transport Layer Protection](https://www.owasp.org/index.php/Top_10_2010-A9)

[OWASP: Top 10 2013-A6-Sensitive Data Exposure](https://www.owasp.org/index.php/Top_10_2013-A6-Sensitive_Data_Exposure)

[OWASP: Transport Layer Protection Cheat Sheet](https://www.owasp.org/index.php/Transport_Layer_Protection_Cheat_Sheet)

[WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)

[CWE-319: Cleartext Transmission of Sensitive Information](http://cwe.mitre.org/data/definitions/319.html)
