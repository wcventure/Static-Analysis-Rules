### Potential Path Traversal (file write)
**Bug Pattern:** PATH_TRAVERSAL_OUT  
[CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://capec.mitre.org/data/definitions/126.html)
#### 描述：
与上个rule类似，如果直接不对输入的地址做任何过滤，那么用户将可以对任意路径的文件写入。  
- [x] 安全相关  
#### check级别：AST
#### 参考信息：
[WASC: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)  
[OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)  
[CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)  
