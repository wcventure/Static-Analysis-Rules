### Found Tapestry page
**Bug Pattern:** TAPESTRY_ENDPOINT  
[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)
#### 描述：
Tapestry应用程序的结构是支持Java类和对应的Tapestry标记语言页面（一个.tml文件）。当收到一个请求时，get/post参数被映射到后备Java类中的特定输入。输入的变量应该得到验证。映射是用fieldName完成的。
- [x] 安全相关
```
 [...]
    protected String input;
    [...]
```
或者显示注释的定义：

```
 [...]
    @org.apache.tapestry5.annotations.Parameter
    protected String parameter1;

    @org.apache.tapestry5.annotations.Component(id = "password")
    private PasswordField passwordField;
    [...]
```
该页面被映射到视图[/resources/package/PageName].tml.在这个应用程序中，每个Tapestry页面都应该被研究，以确保所有以这种方式自动映射的输入在使用之前都得到了正确的验证。
#### check级别：AST
#### 相关信息：
1. [Apache Tapestry Home Page](http://tapestry.apache.org/)
 
