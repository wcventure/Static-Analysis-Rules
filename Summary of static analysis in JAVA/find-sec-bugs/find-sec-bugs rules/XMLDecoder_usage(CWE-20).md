### XMLDecoder usage 
**Bug Pattern:** XML_DECODER

[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)

#### 描述：
不应该使用XMLDecoder来解析不受信任的数据。反序列化用户输入可以导致任意代码执行。这是可能的，因为XMLDecoder支持任意方法调用。此功能旨在调用setter方法，但在实践中，可以调用任何方法。

XMLDecoder should not be used to parse untrusted data. Deserializing user input can lead to arbitrary code execution. This is possible because XMLDecoder supports arbitrary method invocation. This capability is intended to call setter methods, but in practice, any method can be called.

- [x] 安全相关

**Malicious XML example:**
```
<?xml version="1.0" encoding="UTF-8" ?>
<java version="1.4.0" class="java.beans.XMLDecoder">
  <object class="java.io.PrintWriter">
    <string>/tmp/Hacked.txt</string>
    <void method="println">
      <string>Hello World!</string>
    </void>
    <void method="close"/>
  </object>
</java>
```
The XML code above will cause the creation of a file with the content "Hello World!".
#### 漏洞代码：
```
XMLDecoder d = new XMLDecoder(in);
try {
    Object result = d.readObject();
}
[...]
```

#### 解决方案
The solution is to avoid using XMLDecoder to parse content from an untrusted source.

##### Check级别：AST

#### 相关信息：
[Dinis Cruz Blog: Using XMLDecoder to execute server-side Java Code on an Restlet application](http://blog.diniscruz.com/2013/08/using-xmldecoder-to-execute-server-side.html)

[RedHat blog : Java deserialization flaws: Part 2, XML deserialization](https://securityblog.redhat.com/2014/01/23/java-deserialization-flaws-part-2-xml-deserialization/)

[CWE-20: Improper Input Validation](http://cwe.mitre.org/data/definitions/20.html)
