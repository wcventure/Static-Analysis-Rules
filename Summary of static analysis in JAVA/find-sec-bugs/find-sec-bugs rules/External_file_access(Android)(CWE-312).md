### External file access (Android)
**Bug Pattern:** ANDROID_EXTERNAL_FILE_ACCESS

[CWE-312: Cleartext Storage of Sensitive Information](http://cwe.mitre.org/data/definitions/312.html)

#### 描述：
应用程序将数据写入外部存储器(可能是SD卡)。此操作有多种安全含义。首先，SD卡上的文件存储将被具有[READ_EXTERNAL_STORAGE](http://developer.android.com/reference/android/Manifest.permission.html#READ_EXTERNAL_STORAGE)权限的应用程序访问。另外，如果数据持续包含关于用户的机密信息，则需要加密。

The application write data to external storage (potentially SD card). There are multiple security implication to this action. First, file store on SD card will be accessible to the application having the [READ_EXTERNAL_STORAGE](http://developer.android.com/reference/android/Manifest.permission.html#READ_EXTERNAL_STORAGE) permission. Also, if the data persisted contains confidential information about the user, encryption would be needed.
- [x] 安全相关

#### 漏洞代码：

```
file file = new File(getExternalFilesDir(TARGET_TYPE), filename);
fos = new FileOutputStream(file);
fos.write(confidentialData.getBytes());
fos.flush();
```

#### 解决方案
```
fos = openFileOutput(filename, Context.MODE_PRIVATE);
fos.write(string.getBytes());

```
##### Check级别：PDG

#### 相关信息：

[Android Official Doc: Security Tips](http://developer.android.com/training/articles/security-tips.html#ExternalStorage)

[CERT: DRD00-J: Do not store sensitive information on external storage [...]](https://www.securecoding.cert.org/confluence/display/java/DRD00-J.+Do+not+store+sensitive+information+on+external+storage+%28SD+card%29+unless+encrypted+first)

[Android Official Doc: Using the External Storage](http://developer.android.com/guide/topics/data/data-storage.html#filesExternal)

[OWASP Mobile Top 10 2014-M2: Insecure Data Storage](https://www.owasp.org/index.php/Mobile_Top_10_2014-M2)

[CWE-312: Cleartext Storage of Sensitive Information](http://cwe.mitre.org/data/definitions/312.html)