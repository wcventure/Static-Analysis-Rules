# <center> unusedVariable - CWE 563

定义的变量未被使用

### Type

id = "unusedVariable"  
severity = "style"

cwe = "CWE-563: Assignment to Variable without Use"  
cwe-type = "Variant"

    <error id="unusedVariable" severity="style" msg="Unused variable: varname" verbose="Unused variable: varname" cwe="563"/>



### Description

The variable's value is assigned but never used, making it a dead store.

After the assignment, the variable is either assigned another value or goes out of scope. It is likely that the variable is simply vestigial, but it is also possible that the unused variable points out a bug.

More Detail：https://cwe.mitre.org/data/definitions/563.html  



### Example cpp file

	void main()
	{    
		int a;
	}



### Massage output in cppcheck

	[test\a\a.cpp:3]: (style) Unused variable: a



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedVariable" severity="style" msg="Unused variable: a" verbose="Unused variable: a" cwe="563">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="3"/>
	        </error>
	    </errors>
	</results>



