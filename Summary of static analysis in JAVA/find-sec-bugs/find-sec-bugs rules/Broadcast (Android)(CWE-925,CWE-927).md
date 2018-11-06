### Broadcast (Android) 
**Bug Pattern:** ANDROID_BROADCAST

[CWE-925: Improper Verification of Intent by Broadcast Receiver](https://cwe.mitre.org/data/definitions/925.html)

[CWE-927: Use of Implicit Intent for Sensitive Communication](https://cwe.mitre.org/data/definitions/927.html)

#### 描述：
广播意图可以通过任何申请获得适当的许可。建议在可能的情况下避免发送敏感信息。

Broadcast intents can be listen by any application with the appropriate permission. It is suggested to avoid transmitting sensitive information when possible.

- [x] 安全相关

#### 漏洞代码：

```
Intent i = new Intent();
i.setAction("com.insecure.action.UserConnected");
i.putExtra("username", user);
i.putExtra("email", email);
i.putExtra("session", newSessionId);

this.sendBroadcast(v1);
```

#### 解决方案（如果可能）
```
Intent i = new Intent();
i.setAction("com.secure.action.UserConnected");

sendBroadcast(v1);

```
**Configuration (receiver)([1] Source: StackOverflow):**

```
<manifest ...>

    <!-- Permission declaration -->
    <permission android:name="my.app.PERMISSION" />

    <receiver
        android:name="my.app.BroadcastReceiver"
        android:permission="my.app.PERMISSION"> <!-- Permission enforcement -->
        <intent-filter>
            <action android:name="com.secure.action.UserConnected" />
        </intent-filter>
    </receiver>

    ...
</manifest>
```
**Configuration (sender)([1] Source: StackOverflow):**

```
<manifest>
    <!-- We declare we own the permission to send broadcast to the above receiver -->
    <uses-permission android:name="my.app.PERMISSION"/>

    <!-- With the following configuration, both the sender and the receiver apps need to be signed by the same developer certificate. -->
    <permission android:name="my.app.PERMISSION" android:protectionLevel="signature"/>
</manifest>
```


##### Check级别：AST

#### 相关信息：

[CERT: DRD03-J. Do not broadcast sensitive information using an implicit intent](https://www.securecoding.cert.org/confluence/display/java/DRD03-J.+Do+not+broadcast+sensitive+information+using+an+implicit+intent)

[Android Official Doc: BroadcastReceiver (Security)](http://developer.android.com/reference/android/content/BroadcastReceiver.html#Security)

[Android Official Doc: Receiver configuration (see android:permission)](http://developer.android.com/guide/topics/manifest/receiver-element.html)

[[1]StackOverflow: How to set permissions in broadcast sender and receiver in android](http://stackoverflow.com/a/21513368/89769)

[CWE-925: Improper Verification of Intent by Broadcast Receiver](https://cwe.mitre.org/data/definitions/925.html)

[CWE-927: Use of Implicit Intent for Sensitive Communication](https://cwe.mitre.org/data/definitions/927.html)