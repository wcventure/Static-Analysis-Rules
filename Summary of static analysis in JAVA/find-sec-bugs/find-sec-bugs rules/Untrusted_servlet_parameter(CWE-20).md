### Untrusted servlet parameter
服务器会读取不安全的GET和POST方法。
**Bug Pattern:** SERVLET_PARAMETER  
##### 描述:
The Servlet can read GET and POST parameters from various methods. The value obtained should be considered unsafe. You may need to validate or sanitize those values before passing them to sensitive APIs such as:
- SQL query (May lead to SQL injection)
- File opening (May lead to path traversal)
- Command execution (Potential Command injection)
- HTML construction (Potential XSS)
- etc..

<font size=3>**check级别:**</font> dataflow
- [x] 安全相关
- 
#### 代码：

```
public static final double price = 20.00;
int quantity = currentUser.getAttribute("quantity");
double total = price * quantity;
chargeUser(total);
```


#### 参考信息：
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)  
