### Spring CSRF protection disabled 
**Bug Pattern:** SPRING_CSRF_PROTECTION_DISABLED  
[CWE-352: Cross-Site Request Forgery (CSRF)](https://cwe.mitre.org/data/definitions/352.html)
#### 描述：
禁用Spring Security的CSRF保护对于标准web应用程序是不安全的。
- [x] 安全相关：

#### 漏洞代码：

```
@EnableWebSecurity
public class WebSecurityConfig extends WebSecurityConfigurerAdapter {

    @Override
    protected void configure(HttpSecurity http) throws Exception {
        http.csrf().disable();
    }
}
```
#### check级别：AST
#### 相关信息：
1. [Spring Security Official Documentation: When to use CSRF protection](https://docs.spring.io/spring-security/site/docs/current/reference/html/csrf.html#when-to-use-csrf-protection)
2. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29)
3. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)
