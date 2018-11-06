# <center> stringLiteralWrite - CWE 758

直接或间接修改字符串文字。

### Type

id = "stringLiteralWrite"  
severity = "error"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

	<error id="stringLiteralWrite" severity="error" msg="Modifying string literal directly or indirectly is undefined behaviour." verbose="Modifying string literal directly or indirectly is undefined behaviour." cwe="758"/>



### Description

The software uses an API function, data structure, or other entity in a way that relies on properties that are not always guaranteed to hold for that entity.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://stackoverflow.com/questions/5464183/modifying-string-literal  



### Example cpp file

Pointers are usually used to point to data that already exists

	int main()
	{
	    char* t="C++";
	    t[1]='p';
	    t[2]='p';
	    printf("%s",t);
	    return 0;
	}

### Massage output in cppcheck

	[test.cpp:4] -> [test.cpp:3]: (error) Modifying string literal "C++" directly or indirectly is undefined behaviour.
	[test.cpp:5] -> [test.cpp:3]: (error) Modifying string literal "C++" directly or indirectly is undefined behaviour.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="stringLiteralWrite" severity="error" msg="Modifying string literal &quot;C++&quot; directly or indirectly is undefined behaviour." verbose="Modifying string literal &quot;C++&quot; directly or indirectly is undefined behaviour." cwe="758">
	            <location file="test.cpp" line="3"/>
	            <location file="test.cpp" line="4"/>
	        </error>
	        <error id="stringLiteralWrite" severity="error" msg="Modifying string literal &quot;C++&quot; directly or indirectly is undefined behaviour." verbose="Modifying string literal &quot;C++&quot; directly or indirectly is undefined behaviour." cwe="758">
	            <location file="test.cpp" line="3"/>
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>