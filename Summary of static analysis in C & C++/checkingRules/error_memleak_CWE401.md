# <center> memleak - CWE 401

堆内存分配，结束时要释放堆区的空间

### Type

id = "memleak"  
severity = "error"

cwe = "CWE-401: Improper Release of Memory Before Removing Last Reference ('Memory Leak')"  
cwe-type = "base"

    <error id="memleak" severity="error" msg="Memory leak: varname" verbose="Memory leak: varname" cwe="401"/>



### Description

The software does not sufficiently track and release allocated memory after it has been used, which slowly consumes remaining memory.  
This is often triggered by improper handling of malformed data or unexpectedly interrupted sessions.

More Detail：https://cwe.mitre.org/data/definitions/401.html  



### Example cpp file

    include <stdlib.h>*
	
	int main()
	{
    	int result;
	    char *a = malloc(10);
	    a[0] = 0;
	    result = a[0];
	    return result;
	}



### Massage output in cppcheck

	[test\memleak\bad.c:8]: (error) Memory leak: a



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memleak" severity="error" msg="Memory leak: a" verbose="Memory leak: a" cwe="401">
	            <location file0="test\memleak\bad.c" file="test\memleak\bad.c" line="8"/>
	        </error>
	    </errors>
	</results>

