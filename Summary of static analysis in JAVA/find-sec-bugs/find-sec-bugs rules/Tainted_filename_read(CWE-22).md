### Tainted filename read
**Bug Pattern:** FILE_UPLOAD_FILENAME  
 [CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://cwe.mitre.org/data/definitions/22.html)
#### 描述：
FileUpload API提供的文件名可以被客户机篡改，以引用未经授权的文件。
- [x] 安全相关

#### 例子：

- "../../../config/overide_file"
- "shell.jsp\u0000expected.gif"

因此，这些值不应该直接传递给文件系统API。如果可接受，应用程序应该生成自己的文件名并使用它们。否则，应正确验证所提供的文件名，以确保其结构正确，不包含未经授权的路径字符（例如，/），并引用一个授权文件。
#### check级别：AST
#### 相关信息：
1. [Securiteam: File upload security recommendations](http://blogs.securiteam.com/index.php/archives/1268)
2. [CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)
3. [WASC-33: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)
4. [OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)
