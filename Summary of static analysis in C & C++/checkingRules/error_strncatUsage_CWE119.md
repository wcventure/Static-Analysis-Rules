# <center> strncatUsage - CWE 119

strncpy与strncat的第三个参数，以及警告C6059

### Type

id = "strncatUsage"  
severity = "warning"

cwe = "CWE-119: Improper Restriction of Operations within the Bounds of a Memory Buffer"  
cwe-type = "class"

    <error id="strncatUsage" severity="warning" msg="Dangerous usage of strncat - 3rd parameter is the maximum number of characters to append." verbose="At most, strncat appends the 3rd parameter&apos;s amount of characters and adds a terminating null byte.\012The safe way to use strncat is to subtract one from the remaining space in the buffer and use it as 3rd parameter.Source: http://www.cplusplus.com/reference/cstring/strncat/\012Source: http://www.opensource.apple.com/source/Libc/Libc-167/gen.subproj/i386.subproj/strncat.c" cwe="119"/>


### Description

char* strncpy(char *strDest, const char *strSource, size_t count);

The strncpy function copies the initial count characters of strSource to strDest and returns strDest. If count is less than or equal to the length of strSource, a null character is NOT appended automatically to the copied string. If count is greater than the length of strSource, the destination string is padded with null characters up to length count. The behavior of strncpy is undefined if the source and destination strings overlap.

第三个参数count其实隐含是指拷贝的最多字节数；（下面2，3是“最多”的确切含义） 2。 如果count<=strlen(strSource)的话，将会在strDest的后面粘贴strSource的前count字节字符，且不会自动补上NULL作为结尾； 3。 如果count> strlen(strSource)的话，将会在strDest的后面粘贴strSource的全部字符，并在最后自动补上count-strlen(strSource)个NULL作为结尾。

char* strncat(char *strDest, const char *strSource, size_t count );

The strncat function appends, at most, the first count characters of strSource to strDest. The initial character of strSource overwrites the terminating null character of strDest. If a null character appears in strSource before count characters are appended, strncat appends all characters from strSource, up to the null character. If count is greater than the length of strSource, the length of strSource is used in place of count. The all cases, the resulting string is terminated with a null character. If copying takes place between strings that overlap, the behavior is undefined.

与strncpy类似，strncat也是最多取strSource先头的count个字符粘贴到strDest，并在结尾自动补上NULL； 2。如果cout > strlen(strSource)的话，则第三个参数相当于传入了strlen(strSource),即粘贴strSource的所有字符至strDest。



最后，终于可以理解visual Studio报的一个报告——

http://msdn.microsoft.com/zh-cn/library/2b5wde95(v=VS.100).aspx 警告 C6059：<function> 调用中的长度参数不正确。应传递剩余字符数，而不是 <variable> 的缓冲区大小

此警告意味着在调用字符串串联函数时，所传递的表示串联字符数的值可能不正确。此缺陷可能会导致可利用的缓冲区溢出或系统崩溃。导致此缺陷的常见原因是传递给字符串操作函数的是缓冲区大小，而不是缓冲区内的剩余字符数。

即便如此，如上MSDN链接的警告订正版本源代码还是留下一个小小的遗憾。

	include <string.h>  
	define MAX 25  
	  
	void f( )  
	{  
	  char szTarget[MAX];  
	  char *szState ="Washington";  
	  char *szCity="Redmond, ";  
	  
	  strncpy(szTarget,szCity, MAX);  
	  szTarget[MAX -1] = '\0';  
	  strncat(szTarget, szState, MAX - strlen(szTarget)); // correct size   
	  // code ...                                   
	}  

直接运行上面这个例子倒不会出问题，如果在szState的后面再加6个字符（比如6个空格），这时一旦运行这段小程序，会报Run-time Check Failure #2 - stack around the variable 'szTarget' was corrupted. 为什么呢？？原来啊，strncat的第三个参数传入的应该是“缓冲区内的剩余字符数”而不是“剩余缓冲区大小”哦。故只需要修改第三个参数为（MAX - strlen(szTarget) - 1）即可。

More Detail：https://cwe.mitre.org/data/definitions/119.html  
https://blog.csdn.net/mxclxp/article/details/7991276

