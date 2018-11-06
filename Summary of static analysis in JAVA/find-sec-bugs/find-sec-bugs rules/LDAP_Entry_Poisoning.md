### LDAP Entry Poisoning
LDAP输入中毒

**Bug Pattern：** LDAP_ENTRY_POISONING
#### 描述：
JNDI API支持在LDAP目录中绑定serialize对象。 如果显示某些属性，则将在查询目录的应用程序中进行对象的反序列化（有关详细信息，请参阅Black Hat USA 2016白皮书）。 对象反序列化应该被视为危险的操作，可能导致远程代码执行。
如果攻击者在LDAP基本查询中具有入口点，可以通过向现有LDAP条目添加属性或将应用程序配置为使用恶意LDAP服务器来利用此漏洞。

- [x] 安全相关

#### 漏洞代码：
```
DirContext ctx = new InitialDirContext();

//[...]



ctx.search(query, filter,

        new SearchControls(scope, countLimit, timeLimit, attributes,

            true, //Enable object deserialization if bound in directory

            deref));

```
#### 解决方案：
```
DirContext ctx = new InitialDirContext();

//[...]



ctx.search(query, filter,

        new SearchControls(scope, countLimit, timeLimit, attributes,

            false, //Disable

            deref));

```
#### Check级别：AST

####相关信息：
1. [Black Hat USA 2016: A Journey From JNDI/LDAP Manipulation to Remote Code Execution Dream Land (slides & video) by Alvaro Muñoz and Oleksandr Mirosh](https://www.blackhat.com/docs/us-16/materials/us-16-Munoz-A-Journey-From-JNDI-LDAP-Manipulation-To-RCE-wp.pdf )
2. [HP Enterprise: Introducing JNDI Injection and LDAP Entry Poisoning by Alvaro Muñoz]( https://community.hpe.com/t5/Security-Research/Introducing-JNDI-Injection-and-LDAP-Entry-Poisoning/ba-p/6885118)
3. [TrendMicro: How The Pawn Storm Zero-Day Evaded Java's Click-to-Play Protection by Jack Tang](http://blog.trendmicro.com/trendlabs-security-intelligence/new-headaches-how-the-pawn-storm-zero-day-evaded-javas-click-to-play-protection/)

