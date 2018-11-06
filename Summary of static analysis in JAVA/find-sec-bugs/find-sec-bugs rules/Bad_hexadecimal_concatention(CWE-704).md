### Potential external control of configuration
**Bug Pattern:** BAD_HEXA_CONVERSION

[CWE-704: Incorrect Type Conversion or Cast](http://cwe.mitre.org/data/definitions/704.html)

#### 描述：
当将一个包含哈希签名的字节数组转换为人类可读的字符串时，如果该数据是按字节读取的，则会产生转换错误。

When converting a byte array containing a hash signature to a human readable string, a conversion mistake can be made if the array is read byte by byte. 

- [x] 安全相关

#### 漏洞代码：
下面演示了Integer.toHexString() 的用法，它将从计算的哈希值的每个字节中删除任何前导零。
```
MessageDigest md = MessageDigest.getInstance("SHA-256");
byte[] resultBytes = md.digest(password.getBytes("UTF-8"));

StringBuilder stringBuilder = new StringBuilder();
for(byte b :resultBytes) {
    stringBuilder.append( Integer.toHexString( b & 0xFF ) );
}

return stringBuilder.toString();
```
这个错误削弱了计算的哈希值，因为它引入了更多冲突。例如，哈希值"0x0679" 和 "0x6709"均将通过上述函数输出"679"。
#### 解决方案：
将上述的函数toHexString()用String.format() 进行如下替换：
```
stringBuilder.append( String.format( "%02X", b ) );
```
##### Check级别：AST

#### 相关信息：

[CWE-704: Incorrect Type Conversion or Cast](http://cwe.mitre.org/data/definitions/704.html)