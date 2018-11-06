# <center> ignoredReturnValue - CWE 252

函数的返回值没被使用

### Type

id = "ignoredReturnValue"  
severity = "warning"

cwe = "CWE-252"  
cwe-type = "Base"

    <error id="ignoredReturnValue" severity="warning" msg="Return value of function func_name() is not used." verbose="Return value of function func_name() is not used." cwe="252"/>



### Description

A warning that the return value of function is not used.

The software does not check the return value from a method or function, which can prevent it from detecting unexpected states and conditions.

Two common programmer assumptions are "this function call can never fail" and "it doesn't matter if this function call fails". If an attacker can force the function to fail or otherwise return a value that is not expected, then the subsequent program logic could lead to a vulnerability, because the software is not in a state that the programmer assumes. For example, if the program calls a function to drop privileges but does not check the return code to ensure that privileges were successfully dropped, then the program will continue to operate with the higher privileges.

More Detail：https://cwe.mitre.org/data/definitions/252.html  

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



