# <center> unusedVariable - CWE 563

变量被赋值过，但从未被使用

### Type

id = "unreadVariable"  
severity = "style"

cwe = "CWE-563: Assignment to Variable without Use"  
cwe-type = "Variant"

    <error id="unreadVariable" severity="style" msg="Variable &apos;varname&apos; is assigned a value that is never used." verbose="Variable &apos;varname&apos; is assigned a value that is never used." cwe="563"/>



### Description

The variable's value is assigned but never used, making it a dead store.

After the assignment, the variable is either assigned another value or goes out of scope. It is likely that the variable is simply vestigial, but it is also possible that the unused variable points out a bug.

More Detail：https://cwe.mitre.org/data/definitions/563.html  



### Example cpp file

	void main(){
		int a = 10;
		int *p = &a;
	}



### Massage output in cppcheck

	[test\a\a.cpp:2]: (style) Variable 'a' is assigned a value that is never used.
	[test\a\a.cpp:3]: (style) Variable 'p' is assigned a value that is never used.


### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unreadVariable" severity="style" msg="Variable &apos;a&apos; is assigned a value that is never used." verbose="Variable &apos;a&apos; is assigned a value that is never used." cwe="563">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="2"/>
	        </error>
	        <error id="unreadVariable" severity="style" msg="Variable &apos;p&apos; is assigned a value that is never used." verbose="Variable &apos;p&apos; is assigned a value that is never used." cwe="563">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="3"/>
	        </error>
	    </errors>
	</results>




