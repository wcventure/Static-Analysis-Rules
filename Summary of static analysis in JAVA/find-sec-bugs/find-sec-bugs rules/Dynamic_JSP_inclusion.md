### Dynamic JSP inclusion
**Bug Pattern:** JSP_INCLUDE

#### 描述：
包含JSP文件允许动态值的输入。它可能允许攻击者控制包含的JSP页面。如果是这种情况，攻击者将尝试在他控制的磁盘上包含一个文件。通过包含任意文件，攻击者可以执行任何代码。

The inclusion of JSP file allow the entry of dynamic value. It may allow an attacker to control the JSP page included. If this is the case, an attacker will try to include a file on disk that he controls. By including arbitrary files, the attacker gets the ability to execute any code.

- [x] 安全相关

#### 漏洞代码：
```
<jsp:include page="${param.secret_param}" />
```

#### 解决方案
```
<c:if test="${param.secret_param == 'page1'}">
    <jsp:include page="page1.jsp" />
</c:if>
```

##### Check级别: AST

#### 相关信息：
[InfosecInstitute: File Inclusion Attacks](http://resources.infosecinstitute.com/file-inclusion-attacks/)

[WASC-05: Remote File Inclusion](http://projects.webappsec.org/w/page/13246955/Remote%20File%20Inclusion)