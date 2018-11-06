# <center> leakReturnValNotUsed - CWE 771

分配函数函数名的返回值不被存储，这会阻止资源被回收。

### Type

id = "leakReturnValNotUsed"  
severity = "error"

cwe = "CWE-771: Missing Reference to Active Allocated Resource"  
cwe-type = "base"

	<error id="leakReturnValNotUsed" severity="error" msg="Return value of allocation function &apos;funcName&apos; is not stored." verbose="Return value of allocation function &apos;funcName&apos; is not stored." cwe="771"/>



### Description

The software does not properly maintain a reference to a resource that has been allocated, which prevents the resource from being reclaimed.

This does not necessarily apply in languages or frameworks that automatically perform garbage collection, since the removal of all references may act as a signal that the resource is ready to be reclaimed.

More Detail：https://cwe.mitre.org/data/definitions/771.html  



### Example cpp file

	int* f()
	{
		int *p = new int[10];
		return p;
	}
	
	void main()
	{
		f();
	}


### Massage output in cppcheck

	[test.cpp:9]: (error) Return value of allocation function 'f' is not stored.


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="leakReturnValNotUsed" severity="error" msg="Return value of allocation function &apos;f&apos; is not stored." verbose="Return value of allocation function &apos;f&apos; is not stored." cwe="771">
	            <location file="test.cpp" line="9"/>
	        </error>
	    </errors>
	</results>