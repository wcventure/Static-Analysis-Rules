# <center> negativeIndex - CWE 786

数组索引的地址为负数

### Type

id = "negativeIndex"  
severity = "error"

cwe = "CWE-786: Access of Memory Location Before Start of Buffer"  
cwe-type = "Base"

    <error id="negativeIndex" severity="error" msg="Array index -1 is out of bounds." verbose="Array index -1 is out of bounds." cwe="786"/>



### Description

The software reads or writes to a buffer using an index or pointer that references a memory location prior to the beginning of the buffer.

This typically occurs when a pointer or its index is decremented to a position before the buffer, when pointer arithmetic results in a position before the beginning of the valid memory location, or when a negative index is used.

More Detail：https://cwe.mitre.org/data/definitions/786.html  



### Example cpp file

	void main()
	{
		int a[2]={1,1};
		a[-1]=0;
	}



### Massage output in cppcheck

	[test.cpp:4]: (error) Array index -1 is out of bounds.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="negativeIndex" severity="error" msg="Array index -1 is out of bounds." verbose="Array index -1 is out of bounds." cwe="786">
	            <location file="test.cpp" line="4" info="Negative array index"/>
	        </error>
	    </errors>
	</results>