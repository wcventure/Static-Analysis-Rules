### Unsafe hash equals
**Bug Pattern:** UNSAFE_HASH_EQUALS

[CWE-203: Information Exposure Through DiscrepancyKey](https://cwe.mitre.org/data/definitions/203.html)

#### 描述：
攻击者可能能够检测到由于比较时间的暴露而导致的秘密哈希值。当函数Arrays.equals()或String.equals()被调用时，如果匹配的字节数减少，它们就会提前退出。

An attacker might be able to detect the value of the secret hash due to the exposure of comparison timing. When the functions Arrays.equals() or String.equals() are called, they will exited earlier if less bytes are matched.

- [x] 安全相关

#### 漏洞代码：
```
String actualHash = ...

if(userInput.equals(actualHash)) {
    ...
}
```
#### 解决方案
```
String actualHash = ...

if(MessageDigest.isEqual(userInput.getBytes(),actualHash.getBytes())) {
    ...
}

```

##### Check级别：AST

#### 相关信息：

[CWE-203: Information Exposure Through DiscrepancyKey](https://cwe.mitre.org/data/definitions/203.html)