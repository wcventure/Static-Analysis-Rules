# <center> constStatement - CWE 398



### Type

id = "constStatement"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="constStatement" severity="warning" msg="Redundant code: Found a statement that begins with type constant." verbose="Redundant code: Found a statement that begins with type constant." cwe="398"/>



### Description

Redundant code: Found a statement that begins with numeric constant.



### Example cpp file

	void main()
	{
		char *s = "";
		strlen(s);
	}



### Massage output in cppcheck

	[test.cpp:4]: (warning) Return value of function strlen() is not used.
	[test.cpp:4]: (warning) Redundant code: Found a statement that begins with numeric constant.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="ignoredReturnValue" severity="warning" msg="Return value of function strlen() is not used." verbose="Return value of function strlen() is not used." cwe="252">
	            <location file="test.cpp" line="4"/>
	        </error>
	        <error id="constStatement" severity="warning" msg="Redundant code: Found a statement that begins with numeric constant." verbose="Redundant code: Found a statement that begins with numeric constant." cwe="398">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>






