# <center> unassignedVariable - CWE 665

存在没有被赋值的变量。

### Type

id = "unassignedVariable"  
severity = "style"

cwe = "CWE-665: Improper Initialization"  
cwe-type = "Class"

    <error id="unassignedVariable" severity="style" msg="Variable &apos;varname&apos; is not assigned a value." verbose="Variable &apos;varname&apos; is not assigned a value." cwe="665"/>



### Description

The software does not initialize or incorrectly initializes a resource, which might leave the resource in an unexpected state when it is accessed or used.

This can have security implications when the associated resource is expected to have certain properties or values, such as a variable that determines whether a user has been authenticated or not.

More Detail：https://cwe.mitre.org/data/definitions/665.html  



### Example cpp file

	void main(){
		int a;
		int b[2] = {a,1};
	}



### Massage output in cppcheck

	[test.cpp:2]: (style) Variable 'a' is not assigned a value.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unassignedVariable" severity="style" msg="Variable &apos;a&apos; is not assigned a value." verbose="Variable &apos;a&apos; is not assigned a value." cwe="665">
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>