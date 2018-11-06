### FilenameUtils not filtering null bytes
**Bug Pattern:** WEAK_FILENAMEUTILS   
[CWE-78: Improper Neutralization of Special Elements used in an OS Command ('OS Command Injection')](http://cwe.mitre.org/data/definitions/78.html)
#### 描述：
一些 FilenameUtils 方法不会过滤NULL 字节(0x00).如果mull注入到了文件名，而这些文件传给了底层的OS，那被检索的将是NULL之前的文件名，这样可以绕过文件后缀的检查。
- [x] 安全相关  
**check级别：** dataflow
#### 相关信息：
1. [OWASP: Command Injection](https://www.owasp.org/index.php/Command_Injection)  
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)
