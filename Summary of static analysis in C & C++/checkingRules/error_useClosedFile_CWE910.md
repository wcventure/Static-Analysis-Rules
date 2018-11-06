# <center> useClosedFile - CWE 910

文件被关闭以后，依然继续使用

### Type

id = "useClosedFile"  
severity = "error"

cwe = "CWE-910: Use of Expired File Descriptor"  
cwe-type = "base"

    <error id="useClosedFile" severity="error" msg="Used file that is not opened." verbose="Used file that is not opened." cwe="910"/>



### Description

The software uses or accesses a file descriptor after it has been closed.

After a file descriptor for a particular file or device has been released, it can be reused. The code might not write to the original file, since the reused file descriptor might reference a different file or device.

More Detail：https://cwe.mitre.org/data/definitions/910.html  



### Example cpp file

	int main()
	{
	   	FILE *fp = fopen("good.c", "r");
		fclose(fp);//关闭文件
	
		while ((ch[i] = getc(fp)) != EOF)
		{
	        	putchar(ch[i]);
			i ++;
		}
	
		fp=NULL;//需要指向空，否则会指向原打开文件地址
	   	return 0;
	}



### Massage output in cppcheck

	[test\a\a.cpp:6]: (error) Used file that is not opened.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="useClosedFile" severity="error" msg="Used file that is not opened." verbose="Used file that is not opened." cwe="910">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="6"/>
	        </error>
	    </errors>
	</results>

