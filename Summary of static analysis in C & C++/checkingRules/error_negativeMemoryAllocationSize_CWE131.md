# <center> negativeMemoryAllocationSize - CWE 131

分配的内存空间大小为负数

### Type

id = "negativeMemoryAllocationSize"  
severity = "error"

cwe = "CWE-131: Incorrect Calculation of Buffer Size"  
cwe-type = "Base"

    <error id="negativeMemoryAllocationSize" severity="error" msg="Memory allocation size is negative." verbose="Memory allocation size is negative.Negative allocation size has no specified behaviour." cwe="131"/>



### Description

The software does not correctly calculate the size to be used when allocating a buffer, which could lead to a buffer overflow.

More Detail：https://cwe.mitre.org/data/definitions/131.html  



### Example cpp file

	void main(){
		int *p = new int [-1];
	}


### Massage output in cppcheck

	[test.cpp:2]: (error) Memory allocation size is negative.
	[test.cpp:3]: (error) Memory leak: p


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="negativeMemoryAllocationSize" severity="error" msg="Memory allocation size is negative." verbose="Memory allocation size is negative.Negative allocation size has no specified behaviour." cwe="131">
	            <location file="test.cpp" line="2"/>
	        </error>
	        <error id="memleak" severity="error" msg="Memory leak: p" verbose="Memory leak: p" cwe="401">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>