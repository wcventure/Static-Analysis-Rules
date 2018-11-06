### Potential code injection when using Script Engine 
**Bug Pattern:** SCRIPT_ENGINE_INJECTION  
[CWE-94: Improper Control of Generation of Code ('Code Injection')](http://cwe.mitre.org/data/definitions/94.html)  
[CWE-95: Improper Neutralization of Directives in Dynamically Evaluated Code ('Eval Injection')](http://cwe.mitre.org/data/definitions/95.html)
#### 描述：
使用java的script engine的时候，需要对脚本进行详细检查。没有检查脚本会发生注入或其他系统漏洞。如果对用户代码的评估是有意的，应该利用一个适当的沙箱。
- [x] 安全相关

#### 漏洞代码：

```
public void evaluateExpression(String expression) {
    FacesContext context = FacesContext.getCurrentInstance();
    ExpressionFactory expressionFactory = context.getApplication().getExpressionFactory();
    ELContext elContext = context.getELContext();
    ValueExpression vex = expressionFactory.createValueExpression(elContext, expression, String.class);
    return (String) vex.getValue(elContext);
}
```
##### check级别：AST
#### 相关信息：
1. [Minded Security: Abusing EL for executing OS commands](http://blog.mindedsecurity.com/2015/11/reliable-os-shell-with-el-expression.html)
2. [The Java EE 6 Tutorial: Expression Language](https://docs.oracle.com/javaee/6/tutorial/doc/gjddd.html)
3. [Minded Security: Expression Language Injection](https://www.mindedsecurity.com/fileshare/ExpressionLanguageInjection.pdf)
4. [Dan Amodio's blog: Remote Code with Expression Language Injection](http://danamodio.com/appsec/research/spring-remote-code-with-expression-language-injection/)
5. [Remote Code Execution .. by design: Example of malicious payload.](http://blog.h3xstream.com/2014/11/remote-code-execution-by-design.html) The samples given could be used to test sandboxing rules.
