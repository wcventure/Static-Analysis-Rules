# <center> redundantAssignment - CWE 563

变量被定义后，直到再次被定义都没被使用过

### Type

id = "redundantAssignment"  
severity = "style"

cwe = "CWE-563: Assignment to Variable without Use"  
cwe-type = "Variant"

    <error id="redundantAssignment" severity="style" msg="Variable &apos;var&apos; is reassigned a value before the old one has been used." verbose="Variable &apos;var&apos; is reassigned a value before the old one has been used." cwe="563"/>



### Description

The variable's value is assigned but never used, making it a dead store.

After the assignment, the variable is either assigned another value or goes out of scope. It is likely that the variable is simply vestigial, but it is also possible that the unused variable points out a bug.

More Detail：https://cwe.mitre.org/data/definitions/563.html  



### Example cpp file

	void main()
	{
		int a = 1;
		a = 2;
	}



### Massage output in cppcheck

	[test.cpp:3] -> [test.cpp:4]: (style) Variable 'a' is reassigned a value before the old one has been used.
	[test.cpp:4]: (style) Variable 'a' is assigned a value that is never used.


### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="redundantAssignment" severity="style" msg="Variable &apos;a&apos; is reassigned a value before the old one has been used." verbose="Variable &apos;a&apos; is reassigned a value before the old one has been used." cwe="563">
	            <location file="test.cpp" line="4"/>
	            <location file="test.cpp" line="3"/>
	        </error>
	        <error id="unreadVariable" severity="style" msg="Variable &apos;a&apos; is assigned a value that is never used." verbose="Variable &apos;a&apos; is assigned a value that is never used." cwe="563">
	            <location file="test.cpp" line="4"/>
	        </error>
	    </errors>
	</results>