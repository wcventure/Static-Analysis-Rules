# <center> CastIntegerToAddressAtReturn - CWE 758

函数返回值为指针类型，却返回整型。

### Type

id = "CastIntegerToAddressAtReturn"  
severity = "portability"

cwe = "CWE-758: Reliance on Undefined, Unspecified, or Implementation-Defined Behavior"  
cwe-type = "Class"

    <error id="CastIntegerToAddressAtReturn" severity="portability" msg="Returning an integer in a function with pointer return type is not portable." verbose="Returning an integer (int/long/etc) in a function with pointer return type is not portable across different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit integer down to 32-bit pointer. The safe way is to always return a pointer." cwe="758"/>



### Description

Returning an address value in a function with integer (int/long/etc) return type is not portable across different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit address down to 32-bit integer. The safe way is to always return an integer.

This can lead to resultant weaknesses when the required properties change, such as when the software is ported to a different platform or if an interaction error (CWE-435) occurs.

More Detail：https://cwe.mitre.org/data/definitions/758.html  



### Example cpp file

	int* func(){
		int a = 1;
		return a;
	}



### Massage output in cppcheck

	[test.cpp:3]: (portability) Returning an integer in a function with pointer return type is not portable.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="CastIntegerToAddressAtReturn" severity="portability" msg="Returning an integer in a function with pointer return type is not portable." verbose="Returning an integer (int/long/etc) in a function with pointer return type is not portable across different platforms and compilers. For example in 32-bit Windows and Linux they are same width, but in 64-bit Windows and Linux they are of different width. In worst case you end up casting 64-bit integer down to 32-bit pointer. The safe way is to always return a pointer." cwe="758">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>