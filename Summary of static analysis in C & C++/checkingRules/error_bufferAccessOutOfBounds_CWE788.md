# <center> bufferAccessOutOfBounds - CWE 788

超出缓冲区的内存位置的访问

### Type

id = "bufferAccessOutOfBounds"  
severity = "error"

cwe = "CWE-788: Access of Memory Location After End of Buffer"  
cwe-type = "view"

    <error id="bufferAccessOutOfBounds" severity="error" msg="Buffer is accessed out of bounds: buffer" verbose="Buffer is accessed out of bounds: buffer" cwe="788"/>



### Description

The software reads or writes to a buffer using an index or pointer that references a memory location after the end of the buffer.  

This typically occurs when a pointer or its index is decremented to a position before the buffer; when pointer arithmetic results in a position before the buffer; or when a negative index is used, which generates a position before the buffer. 

More Detail：https://cwe.mitre.org/data/definitions/788.html  



### Example cpp file

	include <stdio.h>
	
	int main()
	{
	    char str[5];
	    strcpy(str, "0123456789abcdef");
	}



### Massage output in cppcheck

	[test\outOfBounds\bad.c:5]: (error) Buffer is accessed out of bounds: str



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="bufferAccessOutOfBounds" severity="error" msg="Buffer is accessed out of bounds: str" verbose="Buffer is accessed out of bounds: str">
	            <location file0="test\outOfBounds\bad.c" file="test\outOfBounds\bad.c" line="5"/>
	        </error>
	    </errors>
	</results>


