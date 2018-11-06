# <center> returnReference - CWE 562

函数返回局部对象的引用

### Type

id = "returnReference"  
severity = "error"

cwe = "CWE-562: Return of Stack Variable Address"  
cwe-type = "base"

    <error id="returnReference" severity="error" msg="Reference to auto variable returned." verbose="Reference to auto variable returned." cwe="562"/>



### Description

A function returns the address of a stack variable, which will cause unintended program behavior, typically in the form of a crash.  

Because local variables are allocated on the stack, when a program returns a pointer to a local variable, it is returning a stack address. A subsequent function call is likely to re-use this same stack address, thereby overwriting the value of the pointer, which no longer corresponds to the same variable since a function's stack frame is invalidated when it returns. At best this will cause the value of the pointer to change unexpectedly. In many cases it causes the program to crash the next time the pointer is dereferenced.  

More Detail：https://cwe.mitre.org/data/definitions/562.html  



### Example cpp file

	int &func(int &a){
		int temp = a;
		return temp;
	}



### Massage output in cppcheck

	[test.cpp:3]: (error) Reference to auto variable returned.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="returnReference" severity="error" msg="Reference to auto variable returned." verbose="Reference to auto variable returned." cwe="562">
	            <location file="test.cpp" line="3"/>
	        </error>
	    </errors>
	</results>


