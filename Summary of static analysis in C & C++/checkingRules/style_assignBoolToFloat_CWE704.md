# <center> assignBoolToFloat - CWE 561

bool类型的变量赋值给float类型

### Type

id = "assignBoolToFloat"  
severity = "style"

cwe = "CWE-704: Incorrect Type Conversion or Cast"  
cwe-type = "Class"

    <error id="assignBoolToFloat" severity="style" msg="Boolean value assigned to floating point variable." verbose="Boolean value assigned to floating point variable." cwe="704"/>



### Description

The software does not correctly convert an object, resource, or structure from one type to a different type.

More Detail：https://cwe.mitre.org/data/definitions/704.html  



### Example cpp file

	void main()
	{
		bool b = false;
		float f = b;
	}



### Massage output in cppcheck

	[test.cpp:4]: (style) Boolean value assigned to floating point variable.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="assignBoolToFloat" severity="style" msg="Boolean value assigned to floating point variable." verbose="Boolean value assigned to floating point variable." cwe="704">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>
