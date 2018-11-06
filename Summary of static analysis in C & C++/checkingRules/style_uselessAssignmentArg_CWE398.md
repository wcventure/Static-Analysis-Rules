# <center> uselessAssignmentArg - CWE 398

useless Assignment of function argument

### Type

id = "uselessAssignmentArg"  
severity = "style"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="uselessAssignmentArg" severity="style" msg="Assignment of function parameter has no effect outside the function." verbose="Assignment of function parameter has no effect outside the function." cwe="398"/>



### Description

Assignment of function parameter has no effect outside the function



### Example cpp file

	void func(int x)
	{
		x = 1;
	}



### Massage output in cppcheck

	[test.cpp:3]: (style) Assignment of function parameter has no effect outside the function.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uselessAssignmentArg" severity="style" msg="Assignment of function parameter has no effect outside the function." verbose="Assignment of function parameter has no effect outside the function." cwe="398">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>