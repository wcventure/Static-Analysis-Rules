### Regex DOS (ReDOS) 
**Bug Pattern:** REDOS
#### 描述：
正则表达式（regexs）经常被拒绝服务（DOS）攻击（称为ReDOS）。这是因为在分析某些字符串时，regex引擎可能会花费大量的时间，这取决于正则表达式的定义。  
例如，对于regex：（a+）+$，输入“aaaaaaaaaaaaaax”将导致正则引擎分析65536条不同的路径。因此，单个请求可能会在服务器端造成大量的计算。这个正则表达式的问题，以及其他类似的问题，是由于括号内的+（或a）和括号之外的+（或a），所以有两种不同的方式可以被正则表达式接受。这样写的方式，要么+可以使用“a”这个字符。为了解决这个问题，应该重写正则表达式来消除歧义。例如，这可以简单地重写为：a+$（任意数量的a）。假设这就是最初的正则表达式的含义，那么这个新的正则表达式可以快速地进行评估，并且不受ReDOS的影响。
- [ ] 安全相关

#### check级别：AST
#### 相关信息：
1. [Sebastian Kubeck's Weblog: Detecting and Preventing ReDoS Vulnerabilities](http://www.jroller.com/sebastianKuebeck/entry/detecting_and_preventing_redos_vulnerabilities)
2. [OWASP: Regular expression Denial of Service](https://www.owasp.org/index.php/Regular_expression_Denial_of_Service_-_ReDoS)
3. [CWE-400: Uncontrolled Resource Consumption ('Resource Exhaustion')](http://cwe.mitre.org/data/definitions/400.html)
