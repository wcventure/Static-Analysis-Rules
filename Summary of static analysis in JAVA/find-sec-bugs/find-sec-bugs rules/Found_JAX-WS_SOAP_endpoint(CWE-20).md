### Found JAX-WS SOAP endpoint
**Bug Pattern:** JAXWS_ENDPOINT  
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)
#### 描述：
该方法是SOAP的一部分。  
身份验证如果被执行，应该进行测试。
访问控制如果被执行，应该进行测试。
应该跟踪这些输入，以寻找潜在的漏洞。
最好是通过SSL来进行通信。
- [x] 安全相关  
#### check级别: AST, data-flow  
#### 相关信息：
1. [OWASP: REST Assessment Cheat Sheet](https://www.owasp.org/index.php/REST_Assessment_Cheat_Sheet)
2. [OWASP: REST Security Cheat Sheet](https://www.owasp.org/index.php/REST_Security_Cheat_Sheet)
3. [OWASP: Web Service Security Cheat Sheet](https://www.owasp.org/index.php/Web_Service_Security_Cheat_Sheet)
4. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_(CSRF))

5. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)
