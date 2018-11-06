### Spring CSRF unrestricted RequestMapping 

**Bug Pattern:** SPRING_CSRF_UNRESTRICTED_REQUEST_MAPPING  
[CWE-352: Cross-Site Request Forgery (CSRF)](https://cwe.mitre.org/data/definitions/352.html)
##### 描述：
用RequestMapping注释的方法默认映射到所有HTTP请求方法。然而，Spring Security的CSRF保护在默认情况下是不支持HTTP请求方法GET、HEAD、TRACE和OPTIONS（因为这可能会导致token被泄露）。因此，带有RequestMapping注释的状态变化方法，而不是将映射缩小到HTTP请求方法POST、PUT、DELETE或PATCH，都很容易受到CSRF攻击的影响。
- [x] 安全相关
##### 代码：

```
@Controller
public class UnsafeController {

    @RequestMapping("/path")
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```
##### 解决方法
1. Spring Framework 4.3 and later：

```
@Controller
public class SafeController {

    /**
     * For methods without side-effects use @GetMapping.
     */
    @GetMapping("/path")
    public String readData() {
        // No state-changing operations performed within this method.
        return "";
    }

    /**
     * For state-changing methods use either @PostMapping, @PutMapping, @DeleteMapping, or @PatchMapping.
     */
    @PostMapping("/path")
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```

2. Before Spring Framework 4.3、

```
@Controller
public class SafeController {

    /**
     * For methods without side-effects use either
     * RequestMethod.GET, RequestMethod.HEAD, RequestMethod.TRACE, or RequestMethod.OPTIONS.
     */
    @RequestMapping(value = "/path", method = RequestMethod.GET)
    public String readData() {
        // No state-changing operations performed within this method.
        return "";
    }

    /**
     * For state-changing methods use either
     * RequestMethod.POST, RequestMethod.PUT, RequestMethod.DELETE, or RequestMethod.PATCH.
     */
    @RequestMapping(value = "/path", method = RequestMethod.POST)
    public void writeData() {
        // State-changing operations performed within this method.
    }
}
```
##### check级别：AST
##### 相关信息：
1. [Spring Security Official Documentation: Use proper HTTP verbs (CSRF protection)](https://docs.spring.io/spring-security/site/docs/current/reference/html/csrf.html#csrf-use-proper-verbs)
2. [OWASP: Cross-Site Request Forgery](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29)
3. [OWASP: CSRF Prevention Cheat Sheet](https://www.owasp.org/index.php/Cross-Site_Request_Forgery_%28CSRF%29_Prevention_Cheat_Sheet)
