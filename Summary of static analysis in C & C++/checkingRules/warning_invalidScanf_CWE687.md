# <center> invalidScanfFormatWidth - CWE 687

scanf等函数存在于版本较旧的CRT（C runtime library, part of the C standard library）中，具有安全性问题，比如在读取字符时，若不指定%s的宽度，可能会导致缓冲区溢出。

### Type

id = "invalidScanfFormatWidth"  
severity = "warning"

cwe = "CWE-119: Improper Restriction of Operations within the Bounds of a Memory Buffer"  
cwe-type = "Class"

    <error id="invalidscanf" severity="warning" msg="scanf() without field width limits can crash with huge input data." verbose="scanf() without field width limits can crash with huge input data. Add a field width specifier to fix this problem.\012\012Sample program that can crash:\012\012#include &lt;stdio.h&gt;\012int main()\012{\012    char c[5];\012    scanf(&quot;%s&quot;, c);\012    return 0;\012}\012\012Typing in 5 or more characters may make the program crash. The correct usage here is &apos;scanf(&quot;%4s&quot;, c);&apos;, as the maximum field width does not include the terminating null byte.\012Source: http://linux.die.net/man/3/scanf\012Source: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/libkern/stdio/scanf.c" cwe="119"/>


### Description

The software performs operations on a memory buffer, but it can read from or write to a memory location that is outside of the intended boundary of the buffer.

Certain languages allow direct addressing of memory locations and do not automatically ensure that these locations are valid for the memory buffer that is being referenced. This can cause read or write operations to be performed on memory locations that may be associated with other variables, data structures, or internal program data.

As a result, an attacker may be able to execute arbitrary code, alter the intended control flow, read sensitive information, or cause the system to crash.

More Detail：https://cwe.mitre.org/data/definitions/119.html  
https://blog.csdn.net/whchina/article/details/51259863

### Example cpp file

scanf等函数存在于版本较旧的CRT（C runtime library, part of the C standard library）中，具有安全性问题，比如在读取字符时，若不指定%s的宽度，可能会导致缓冲区溢出。

	void main()
	{
	    char str[10];
	    scanf("%s", &str);
	    printf("i is %s", str);
	}

### Massage output in cppcheck

	[test.cpp:4]: (warning) scanf() without field width limits can crash with huge input data.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="invalidscanf" severity="warning" msg="scanf() without field width limits can crash with huge input data." verbose="scanf() without field width limits can crash with huge input data. Add a field width specifier to fix this problem.\012\012Sample program that can crash:\012\012#include &lt;stdio.h&gt;\012int main()\012{\012    char c[5];\012    scanf(&quot;%s&quot;, c);\012    return 0;\012}\012\012Typing in 5 or more characters may make the program crash. The correct usage here is &apos;scanf(&quot;%4s&quot;, c);&apos;, as the maximum field width does not include the terminating null byte.\012Source: http://linux.die.net/man/3/scanf\012Source: http://www.opensource.apple.com/source/xnu/xnu-1456.1.26/libkern/stdio/scanf.c" cwe="119">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>

