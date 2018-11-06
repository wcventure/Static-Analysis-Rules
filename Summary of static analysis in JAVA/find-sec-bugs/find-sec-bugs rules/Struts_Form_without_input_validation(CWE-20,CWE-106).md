### Struts Form without input validation
**Bug Pattern:** STRUTS_FORM_VALIDATION

[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

[CWE-106: Struts: Plug-in Framework not in Use](http://cwe.mitre.org/data/definitions/106.html)

#### 描述：
表单输入应该有最小的输入验证。预防性验证有助于在不同的风险下提供深度防御。
通过实现验证方法可以引入验证。

Form inputs should have minimal input validation. Preventive validation helps provide defense in depth against a variety of risks.
Validation can be introduce by implementing a validate method.

- [x] 安全相关

#### 漏洞代码：
```
public class RegistrationForm extends ValidatorForm {

    private String name;
    private String email;

    [...]

    public ActionErrors validate(ActionMapping mapping, HttpServletRequest request) {
        //Validation code for name and email parameters passed in via the HttpRequest goes here
    }
}
```

##### Check级别：AST

#### 相关信息：

[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

[CWE-106: Struts: Plug-in Framework not in Use](http://cwe.mitre.org/data/definitions/106.html)