# <center> invalidPrintfArgType_sint - CWE 686

无效的sint输出格式，prinf中参数格式不正确

### Type

id = "invalidPrintfArgType_sint"  
severity = "portability"

cwe = "CWE-686: Function Call With Incorrect Argument Type"  
cwe-type = "Variant"

    <error id="invalidPrintfArgType_sint" severity="warning" msg="%i in format string (no. 1) requires &apos;int&apos; but the argument type is Unknown." verbose="%i in format string (no. 1) requires &apos;int&apos; but the argument type is Unknown." cwe="686"/>



### Description

The software calls a function, procedure, or routine, but the caller specifies an argument that is the wrong data type, which may lead to resultant weaknesses.

This weakness is most likely to occur in loosely typed languages, or in strongly typed languages in which the types of variable arguments cannot be enforced at compilation time, or where there is implicit casting.

More Detail：https://cwe.mitre.org/data/definitions/686.html  



### Example cpp file

	int main()  
	{  
	    int array[] = {1, 2, 3};  
	    int *pa = array;  
	    int *pb = array + 3;  
	    printf("sizeof(pa)...%d\n", sizeof(pa));  
	    printf("pb-pa...%d\n", pb - pa);  
	    printf("((unsigned)pb-(unsigned)pa)/sizeof(pa)...%d\n", ((unsigned)pb - (unsigned)pa) / sizeof(pa));  
	    return 0;  
	}  



### Massage output in cppcheck

	[test.cpp:6]: (portability) %d in format string (no. 1) requires 'int' but the argument type is 'size_t {aka unsigned long}'.
	[test.cpp:7]: (portability) %d in format string (no. 1) requires 'int' but the argument type is 'ptrdiff_t {aka signed int}'.




### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="invalidPrintfArgType_sint" severity="portability" msg="%d in format string (no. 1) requires &apos;int&apos; but the argument type is &apos;size_t {aka unsigned long}&apos;." verbose="%d in format string (no. 1) requires &apos;int&apos; but the argument type is &apos;size_t {aka unsigned long}&apos;." cwe="686">
	            <location file="test.cpp" line="6"/>
	        </error>
	        <error id="invalidPrintfArgType_sint" severity="portability" msg="%d in format string (no. 1) requires &apos;int&apos; but the argument type is &apos;ptrdiff_t {aka signed int}&apos;." verbose="%d in format string (no. 1) requires &apos;int&apos; but the argument type is &apos;ptrdiff_t {aka signed int}&apos;." cwe="686">
	            <location file="test.cpp" line="7"/>
	        </error>
	    </errors>
	</results>



