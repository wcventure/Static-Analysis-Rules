### Dynamic variable in Spring expression
**Bug Pattern:** JSP_SPRING_EVAL

[CWE-94: Improper Control of Generation of Code ('Code Injection')](http://cwe.mitre.org/data/definitions/94.html)

[CWE-95: Improper Neutralization of Directives in Dynamically Evaluated Code ('Eval Injection')](http://cwe.mitre.org/data/definitions/95.html)

#### 描述：
用动态值构建一个Spring表达式。应该验证值的来源，以避免未过滤的值落入这个危险的代码评估中。

A Spring expression is built with a dynamic value. The source of the value(s) should be verified to avoid that unfiltered values fall into this risky code evaluation.

- [x] 安全相关

#### 漏洞代码：
```
<%@ taglib prefix="spring" uri="http://www.springframework.org/tags" %>

<spring:eval expression="${param.lang}" var="lang" />

<%@ taglib prefix="spring" uri="http://www.springframework.org/tags" %>

<spring:eval expression="'${param.lang}'=='fr'" var="languageIsFrench" />
```

#### 解决方案
```
<c:set var="lang" value="${param.lang}"/>

<c:set var="languageIsFrench" value="${param.lang == 'fr'}"/>
```

##### Check级别：AST

#### 相关信息：
[CWE-94: Improper Control of Generation of Code ('Code Injection')](http://cwe.mitre.org/data/definitions/94.html)

[CWE-95: Improper Neutralization of Directives in Dynamically Evaluated Code ('Eval Injection')](http://cwe.mitre.org/data/definitions/95.html)

