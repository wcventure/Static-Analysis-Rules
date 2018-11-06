# <center> unusedStructMember - CWE 563

结构体中有没被使用的成员变量

### Type

id = "unusedStructMember"  
severity = "style"

cwe = "CWE-563: Assignment to Variable without Use"  
cwe-type = "Variant"

    <error id="unusedStructMember" severity="style" msg="struct member &apos;structname::variable&apos; is never used." verbose="struct member &apos;structname::variable&apos; is never used." cwe="563"/>



### Description

The variable's value is assigned but never used, making it a dead store.

After the assignment, the variable is either assigned another value or goes out of scope. It is likely that the variable is simply vestigial, but it is also possible that the unused variable points out a bug.

More Detail：https://cwe.mitre.org/data/definitions/563.html  



### Example cpp file

	struct PackedStruct 
	{
		short a;
		short b;
	};



### Massage output in cppcheck

	[test.cpp:4]: (style) struct member 'PackedStruct::a' is never used.
	[test.cpp:6]: (style) struct member 'PackedStruct::b' is never used.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedStructMember" severity="style" msg="struct member &apos;PackedStruct::a&apos; is never used." verbose="struct member &apos;PackedStruct::a&apos; is never used." cwe="563">
	            <location file="test.cpp" line="4"/>
	        </error>
	        <error id="unusedStructMember" severity="style" msg="struct member &apos;PackedStruct::b&apos; is never used." verbose="struct member &apos;PackedStruct::b&apos; is never used." cwe="563">
	            <location file="test.cpp" line="6"/>
	        </error>
	    </errors>
	</results>