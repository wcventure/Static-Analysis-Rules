###Potential information leakage in Scala Play

Scala Play中潜在的信息泄漏

**Bug Pattern：**SCALA_SENSITIVE_DATA_EXPOSURE
####描述：
应用程序可能会无意中泄漏有关其配置，内部工作的信息，或通过各种应用程序问题侵犯隐私。 [1]根据数据的有效性提供不同响应的页面可能导致信息泄漏; 特别是当数据被视为保密时，由于网络应用程序的设计而显露出来。[2]



敏感数据的例子包括（但不限于）：API密钥，密码，产品版本或环境配置。

- [x] 安全相关

####风险代码：
```
def doGet(value:String) = Action {

    WS.url(value).get().map { response =>

        Ok(response.body)

    }

}

```
应用程序配置元素不应在响应内容中发送，并且不应允许用户控制代码将使用哪些配置元素。



#### Check级别：AST

####相关信息：
1. [OWASP: Top 10 2013-A6-Sensitive Data Exposure](https://www.owasp.org/index.php/Top_10_2013-A6-Sensitive_Data_Exposure )
1. [CWE-200: Information Exposure ](https://cwe.mitre.org/data/definitions/200.html )

1. [[1] OWASP: Top 10 2007-Information Leakage and Improper Error Handling](https://www.owasp.org/index.php/Top_10_2007-Information_Leakage_and_Improper_Error_Handling )
1. [[2] WASC-13: Information Leakage ](https://cwe.mitre.org/data/definitions/200.html )



