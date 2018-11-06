# <center> resourceLeak - CWE 775

读写文件流时打开了文件，结束时要记得释放资源

### Type

id = "resourceLeak"  
severity = "error"

cwe = "CWE-775: Missing Release of File Descriptor or Handle after Effective Lifetime"  
cwe-type = "Variant"

    <error id="resourceLeak" severity="error" msg="Resource leak: varname" verbose="Resource leak: varname" cwe="775"/>



### Description

The software does not release a file descriptor or handle after its effective lifetime has ended, i.e., after the file descriptor/handle is no longer needed.

When a file descriptor or handle is not released after use (typically by explicitly closing it), attackers can cause a denial of service by consuming all available file descriptors/handles, or otherwise preventing other system processes from obtaining their own file descriptors/handles.

More Detail：https://cwe.mitre.org/data/definitions/775.html  



### Example cpp file

	include <stdio.h>
	
	int main()
	{
    	FILE *a = fopen("good.c", "r");
    	if (!a)
    	    return 0;

    	return 0;
	}



### Massage output in cppcheck

	[test\resourceLeak\bad.c:8]: (error) Resource leak: a



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="resourceLeak" severity="error" msg="Resource leak: a" verbose="Resource leak: a" cwe="775">
	            <location file0="test\resourceLeak\bad.c" file="test\resourceLeak\bad.c" line="8"/>
	        </error>
	    </errors>
	</results>


