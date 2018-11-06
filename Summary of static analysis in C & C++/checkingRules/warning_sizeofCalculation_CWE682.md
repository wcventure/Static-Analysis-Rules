# <center> sizeofCalculation - CWE 682

sizeof()函数里面还包含计算，常导致不争取的结果

### Type

id = "sizeofCalculation"  
severity = "warning"

cwe = "CWE-682"  
cwe-type = "Class"

    <error id="sizeofCalculation" severity="warning" msg="Found calculation inside sizeof()." verbose="Found calculation inside sizeof()." cwe="682"/>



### Description

The software performs a calculation that generates incorrect or unintended results that are later used in security-critical decisions or resource management.

When software performs a security-critical calculation incorrectly, it might lead to incorrect resource allocations, incorrect privilege assignments, or failed comparisons among other things. Many of the direct results of an incorrect calculation can lead to even larger problems such as failed protection mechanisms or even arbitrary code execution.

More Detail：https://cwe.mitre.org/data/definitions/682.html



### Example cpp file

	void main(){
		int x = 1;
		sizeof(x++);
	}

Another example

	int * p = x; 
	char * second_char =（char *）（p + 1）;

In this example, second_char is intended to point to the second byte of p. But, adding 1 to p actually adds sizeof(int) to p, giving a result that is incorrect (3 bytes off on 32-bit platforms). If the resulting memory address is read, this could potentially be an information leak. If it is a write, it could be a security-critical write to unauthorized memory-- whether or not it is a buffer overflow. Note that the above code may also be wrong in other ways, particularly in a little endian environment.

### Massage output in cppcheck

	[test.cpp:3]: (warning) Found calculation inside sizeof().



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="sizeofCalculation" severity="warning" msg="Found calculation inside sizeof()." verbose="Found calculation inside sizeof()." cwe="682">
	            <location file0="test\misra-test.c" file="test\misra-test.c" line="142"/>
	        </error>
	</results>






