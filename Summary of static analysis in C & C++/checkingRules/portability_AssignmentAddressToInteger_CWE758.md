# <center> AssignmentAddressToInteger - CWE 758

指针变量的值赋值给整数变量

### Type

id = "AssignmentAddressToInteger"  
severity = "portability"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

    <error id="AssignmentAddressToInteger" severity="portability" msg="Assigning a pointer to an integer is not portable." verbose="Assigning a pointer to an integer (int/long/etc) is not portable across different platforms and compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux they are of different width. In worst case you end up assigning 64-bit address to 32-bit integer. The safe way is to store addresses only in pointer types (or typedefs like uintptr_t)." cwe="758"/>



### Description

The software uses an API function, data structure, or other entity in a way that relies on properties that are not always guaranteed to hold for that entity.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://cwe.mitre.org/data/definitions/758.html  



### Example cpp file

	int foo(int *p)
	{
	    int a = p;
	    return a + 4;
	}
	
	int main()
	{
	    int i[10];
	    foo(i);
	}



### Massage output in cppcheck

	[samples\AssignmentAddressToInteger\bad.c:3]: (portability) Assigning a pointer to an integer is not portable.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="AssignmentAddressToInteger" severity="portability" msg="Assigning a pointer to an integer is not portable." verbose="Assigning a pointer to an integer (int/long/etc) is not portable across different platforms and compilers. For example in 32-bit Windows and linux they are same width, but in 64-bit Windows and linux they are of different width. In worst case you end up assigning 64-bit address to 32-bit integer. The safe way is to store addresses only in pointer types (or typedefs like uintptr_t)." cwe="758">
	            <location file0="test\AssignmentAddressToInteger\bad.c" file="test\AssignmentAddressToInteger\bad.c" line="3"/>
	        </error>
	    </errors>
	</results>



