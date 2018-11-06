# <center> shiftNegative - CWE 758

使用二进制偏移符号<<,偏移值为负数

### Type

id = "shiftNegative"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

     <error id="shiftNegative" severity="error" msg="Shifting by a negative value is undefined behaviour" verbose="Shifting by a negative value is undefined behaviour" cwe="758"/>



### Description

The software uses an API function, data structure, or other entity in a way that relies on properties that are not always guaranteed to hold for that entity.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://cwe.mitre.org/data/definitions/758.html  



### Example cpp file

	void main()  
	{  
		int a = 1 << -1;
	}  

### Massage output in cppcheck

	[test.cpp:3]: (error) Shifting by a negative value is undefined behaviour



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="shiftNegative" severity="error" msg="Shifting by a negative value is undefined behaviour" verbose="Shifting by a negative value is undefined behaviour" cwe="758">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>