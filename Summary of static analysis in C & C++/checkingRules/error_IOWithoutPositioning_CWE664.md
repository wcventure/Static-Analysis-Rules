# <center> IOWithoutPositioning - CWE 665

读和写操作没有调用Positioning函数

### Type

id = "IOWithoutPositioning"  
severity = "error"

cwe = "CWE-664: Improper Control of a Resource Through its Lifetime"  
cwe-type = "Class"

    <error id="IOWithoutPositioning" severity="error" msg="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." verbose="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." cwe="664"/>



### Description

The software does not maintain or incorrectly maintains control over a resource throughout its lifetime of creation, use, and release.

Resources often have explicit instructions on how to be created, used and destroyed. When software does not follow these instructions, it can lead to unexpected behaviors and potentially exploitable states.

Even without explicit instructions, various principles are expected to be adhered to, such as "Do not use an object until after its creation is complete," or "do not use an object after it has been slated for destruction."

More Detail：https://cwe.mitre.org/data/definitions/664.html  



### Example cpp file

Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour.

	include <stdio.h>  
	include <stdlib.h>  
	include <string.h>  
	
	int main()  
	{  
		char buf[20];
		char msg[] = "hello world!";
		FILE *fp = fopen("a.txt", "w+");
	
		if(fp == NULL) {  
			perror("a.txt:");  
			exit(EXIT_FAILURE);  
		}
		//fseek(fp, 0, SEEK_SET);  
		fwrite(msg, 1, strlen(msg) + 1, fp);  
		//fseek(fp, 0, SEEK_SET);  
		fread(buf, 1, strlen(msg) + 1, fp);  
		printf("%s\n", buf);
		return 0;  
	}  

首先看第9行，我们打开一个文本，并且设置为读写模式"w+"。这就意味着读写都用的是一个文件指针了。

注意第15行和第17行的fseek。在16行执行之后，字符指针去到了末尾，因此要用fseek让指针回到文件的开始位置，SEEK_SET就是回到文件开头，并且偏移量为0。

去掉16行，18行读的时候就从结尾开始了，这当然不是我们的本意。

当然，如果读写不共用同一个文件指针，那么久没有必要用fseek了。

### Massage output in cppcheck

	[test.cpp:17]: (error) Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour.
	[test.cpp:19]: (error) Resource leak: fp


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="IOWithoutPositioning" severity="error" msg="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." verbose="Read and write operations without a call to a positioning function (fseek, fsetpos or rewind) or fflush in between result in undefined behaviour." cwe="664">
	            <location file="test.cpp" line="17"/>
	        </error>
	        <error id="resourceLeak" severity="error" msg="Resource leak: fp" verbose="Resource leak: fp" cwe="775">
	            <location file="test.cpp" line="19"/>
	        </error>
	    </errors>
	</results>
