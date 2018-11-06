# <center> autovarInvalidDeallocation - CWE 590

用free()释放栈区域的数组的空间

### Type

id = "autovarInvalidDeallocation"  
severity = "error"

cwe = "CWE-590: Free of Memory not on the Heap"  
cwe-type = "Variant"

    <error id="autovarInvalidDeallocation" severity="error" msg="Deallocation of an auto-variable results in undefined behaviour." verbose="The deallocation of an auto-variable results in undefined behaviour. You should only free memory that has been allocated dynamically." cwe="590"/>



### Description

The application calls free() on a pointer to memory that was not allocated using associated heap allocation functions such as malloc(), calloc(), or realloc().

When free() is called on an invalid pointer, the program's memory management data structures may become corrupted. This corruption can cause the program to crash or, in some circumstances, an attacker may be able to cause free() to operate on controllable memory locations to modify critical program variables or execute code.

More Detail：https://cwe.mitre.org/data/definitions/590.html  



### Example cpp file

	void foo(){
	record_t bar[MAX_SIZE];
	
	/* do something interesting with bar */ 
	
	...
	free(bar);
	}



### Massage output in cppcheck

	[test\a\a.cpp:7]: (error) Deallocation of an auto-variable results in undefined behaviour.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="autovarInvalidDeallocation" severity="error" msg="Deallocation of an auto-variable results in undefined behaviour." verbose="The deallocation of an auto-variable results in undefined behaviour. You should only free memory that has been allocated dynamically." cwe="590">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="7"/>
	        </error>
	    </errors>
	</results>



