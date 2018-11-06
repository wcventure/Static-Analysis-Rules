# <center> uninitvar - CWE 908

使用了未初始化的变量

### Type

id = "uninitvar"  
severity = "error"

cwe = "CWE-908: Use of Uninitialized Resource"  
cwe-type = "base"

    <error id="uninitvar" severity="error" msg="Uninitialized variable: varname" verbose="Uninitialized variable: varname" cwe="908"/>



### Description

The software uses a resource that has not been properly initialized.

This can have security implications when the associated resource is expected to have certain properties or values.

More Detail：https://cwe.mitre.org/data/definitions/908.html  



### Example cpp file

	int main()
	{
	    int x;
	    x++;
	}



### Massage output in cppcheck

	[test\a\a.cpp:4]: (error) Uninitialized variable: str



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uninitvar" severity="error" msg="Uninitialized variable: x" verbose="Uninitialized variable: x" cwe="908">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>




