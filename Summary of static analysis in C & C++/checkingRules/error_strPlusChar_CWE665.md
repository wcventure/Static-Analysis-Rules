# <center> strPlusChar - CWE 665

不寻常的指针算术。一个char类型的值添加到字符串文字。

### Type

id = "strPlusChar"  
severity = "error"

cwe = "CWE-665: Improper Initialization"  
cwe-type = "Class"

    <error id="strPlusChar" severity="error" msg="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." verbose="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." cwe="665"/>



### Description

The software does not initialize or incorrectly initializes a resource, which might leave the resource in an unexpected state when it is accessed or used.

This can have security implications when the associated resource is expected to have certain properties or values, such as a variable that determines whether a user has been authenticated or not.

More Detail：https://cwe.mitre.org/data/definitions/665.html  



### Example cpp file

	void main()
	{
		char *str = "awgweg"+'a';
	}

### Massage output in cppcheck

	[test.cpp:3]: (error) Unusual pointer arithmetic. A value of type 'char' is added to a string literal.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="strPlusChar" severity="error" msg="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." verbose="Unusual pointer arithmetic. A value of type &apos;char&apos; is added to a string literal." cwe="665">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>
