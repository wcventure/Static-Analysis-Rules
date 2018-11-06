# <center> unusedFunction - CWE 561

Dead code，有未使用过的函数

### Type

id = "unusedFunction"  
severity = "style"

cwe = "CWE-561: Dead Code"  
cwe-type = "Variant"

    <error id="unusedFunction" severity="style" msg="The function &apos;funcName&apos; is never used." verbose="The function &apos;funcName&apos; is never used." cwe="561"/>



### Description

The software contains dead code, which can never be executed.

Dead code is source code that can never be executed in a running program. The surrounding code makes it impossible for a section of code to ever be executed.

More Detail：https://cwe.mitre.org/data/definitions/561.html  



### Example cpp file

	int main()
	{
		\\...	
		return 0;
	}

	int f()
	{    
		\\...
		return 0;
	}



### Massage output in cppcheck

	[test\a\a.cpp:1]: (style) The function 'f' is never used.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedFunction" severity="style" msg="The function &apos;getName&apos; is never used." verbose="The function &apos;getName&apos; is never used." cwe="561">
	            <location file="test\a\a.cpp" line="1"/>
	        </error>
	    </errors>
	</results>



