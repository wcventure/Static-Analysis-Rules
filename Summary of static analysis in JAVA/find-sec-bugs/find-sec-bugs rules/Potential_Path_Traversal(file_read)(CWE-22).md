### Potential Path Traversal (file read) 
**Bug Pattern:** PATH_TRAVERSAL_IN  
[CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://capec.mitre.org/data/definitions/126.html)
#### 描述：

未过滤的输入参数直接作为File对象的文件名输入，会导致对路径下其他文件读取。
- [x] 安全相关  
#### 代码：

```
@GET
@Path("/images/{image}")
@Produces("images/*")
public Response getImage(@javax.ws.rs.PathParam("image") String image) {
    File file = new File("resources/images/", image); //Weak point

    if (!file.exists()) {
        return Response.status(Status.NOT_FOUND).build();
    }

    return Response.ok().entity(new FileInputStream(file)).build();
}
```
#### 正确代码：

```
import org.apache.commons.io.FilenameUtils;

@GET
@Path("/images/{image}")
@Produces("images/*")
public Response getImage(@javax.ws.rs.PathParam("image") String image) {
    File file = new File("resources/images/", FilenameUtils.getName(image)); //Fix

    if (!file.exists()) {
        return Response.status(Status.NOT_FOUND).build();
    }

    return Response.ok().entity(new FileInputStream(file)).build();
}
```

#### check级别： AST
#### 参考信息：
1. [WASC: Path Traversal](http://projects.webappsec.org/w/page/13246952/Path%20Traversal)  
2. [OWASP: Path Traversal](https://www.owasp.org/index.php/Path_Traversal)  
3. [CAPEC-126: Path Traversal](http://capec.mitre.org/data/definitions/126.html)  
4. [CWE-22: Improper Limitation of a Pathname to a Restricted Directory ('Path Traversal')](http://capec.mitre.org/data/definitions/126.html)
