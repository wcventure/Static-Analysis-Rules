### Hazelcast symmetric encryption
**Bug Pattern:** HAZELCAST_SYMMETRIC_ENCRYPTION

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)

#### 描述：

Hazelcast的网络通信被配置为使用对称密码（可能是DES或者blowfish）。
仅仅这些密码不提供完整或者安全身份验证。使用对称加密是首选。

The network communications for Hazelcast is configured to use a symmetric cipher (probably DES or blowfish).

Those ciphers alone do not provide integrity or secure authentication. The use of asymmetric encryption is preferred.


#### 相关信息：
[WASC-04: Insufficient Transport Layer Protection](http://projects.webappsec.org/w/page/13246945/Insufficient%20Transport%20Layer%20Protection)

[Hazelcast Documentation: Encryption](http://docs.hazelcast.org/docs/3.5/manual/html/encryption.html)

[CWE-326: Inadequate Encryption Strength](http://cwe.mitre.org/data/definitions/326.html)