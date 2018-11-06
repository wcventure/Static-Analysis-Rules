# <center> uninitstring - CWE 676

strncpy() 不一定添加终止符

### Type

id = "uninitstring"  
severity = "error"

cwe = "CWE-676: Use of Potentially Dangerous Function"  
cwe-type = "base"

    <error id="uninitstring" severity="error" msg="Dangerous usage of &apos;varname&apos; (strncpy doesn&apos;t always null-terminate it)." verbose="Dangerous usage of &apos;varname&apos; (strncpy doesn&apos;t always null-terminate it)." cwe="676"/>


### Description

Dangerous usage of 'varname' (strncpy doesn't always null-terminate it)

要使用 strncpy 替代 strcpy 函数，因为 strncpy 函数更安全。而今天我要告诉你，strncpy 是不安全的，并且是低效的，strncpy 的存在是由于一个历史原因造成的，你不应当再使用 strncpy 函数。

下面我来解释为什么 strncpy 函数是不安全并且是低效的，以及我们应该使用那些替代函数。

**误解一**：如果 src 长度小于 n, 那么strncpy 和 strcpy 效果一样？

**错，事实上，strncpy 还会把 dest 剩下的部分全部置为 0！**

一直认为 strncpy 只是比 strcpy 多了长度校验，确不知道 strncpy 会把剩下的部分全置为 0（粗体部分）。

char *strncpy(char *dest, const char *src, size_t n);

DESCRIPTION

	The strcpy() function copies the string pointed to by src (including the terminating `\0′ character) to the array pointed to by dest. The strings may
	not overlap, and the destination string dest must be large enough to receive the copy.
	The strncpy() function is similar, except that not more than n bytes of src are copied. Thus, if there is no null byte among the first n bytes of src,
	the result will not be null-terminated.
	In the case where the length of src is less than that of n, the remainder of dest will be padded with null bytes.

这会导致什么后果呢？

首先，如果 strncpy 的长度填错了，比如比实际的长，那么就可能会把其他数据清 0 了。我就遇到过这个问题，在后来检查代码看到这个问题时，也并不以为然，因为拷贝的字符串不可能超过缓冲区的长度。

另外，假设 dest 的长度为 1024, 而待拷贝的字符串长度只有 24，strncpy 会把余下的 1000 各字节全部置为 0. 这就可能会导致性能问题，这也是我为什么说 strncpy 是低效的。

**误解二**：如果src 长度大于等于 n, 那么 strncpy 会拷贝 n – 1 各字符到 dest, 然后补 0？

**错，大错特错**，罚抄上面的 DESCRIPTION ，直到看到：

if there is no null byte among the first n bytes of src, the result will not be null-terminated.

这就可能导致了不安全的因素。

More Detail：https://cwe.mitre.org/data/definitions/676.html  
http://blog.haipo.me/?p=1065  


### Example cpp file

如果待拷贝字符串长度大于了 n, 那么 dest 是不会有结尾字符 0 的。假设这样一种情况：

	char s[] = "hello world";
	strncpy(s, "shit!", 5);
	puts(s);

输出的结果是 “shit” 还是 “shit! world” ?

这种情况只是导致了输出结果错误，严重的，如果 dest n 字节后面一直没有 0，那么就会导致程序段错误。

strncpy 最开始引入标准库是用来处理结构体中固定长度的字符串，比如路径名，而这些字符串的用法不同于 C 中带结尾字 0 的字符串。所以 strncpy 的初衷并不是一个安全的 strcpy.

[停止使用 strncpy 函数！](http://blog.haipo.me/?p=1065)
