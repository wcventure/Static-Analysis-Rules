### Potential Command Injection 
**Bug Pattern:** COMMAND_INJECTION  
[CWE-78: Improper Neutralization of Special Elements used in an OS Command ('OS Command Injection')](http://cwe.mitre.org/data/definitions/78.html)
#### 描述：
未经过滤的输入会让执行的API执行任意命令。
- [x] 安全相关
#### 代码：

```
import java.lang.Runtime;

Runtime r = Runtime.getRuntime();
r.exec("/bin/sh -c some_tool" + input);
```
#### check级别：AST

#### 参考信息：
1. [OWASP: Command Injection](https://www.owasp.org/index.php/Command_Injection)  
2. [OWASP: Top 10 2013-A1-Injection](https://www.owasp.org/index.php/Top_10_2013-A1-Injection)  
