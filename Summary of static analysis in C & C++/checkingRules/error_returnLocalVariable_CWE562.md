# <center> returnLocalVariable - CWE 562

函数返回局部变量的指针

### Type

id = "returnLocalVariable"  
severity = "error"

cwe = "CWE-562: Return of Stack Variable Address"  
cwe-type = "base"

    <error id="returnLocalVariable" severity="error" msg="Pointer to local array variable returned." verbose="Pointer to local array variable returned." cwe="562"/>



### Description

A function returns the address of a stack variable, which will cause unintended program behavior, typically in the form of a crash.  

Because local variables are allocated on the stack, when a program returns a pointer to a local variable, it is returning a stack address. A subsequent function call is likely to re-use this same stack address, thereby overwriting the value of the pointer, which no longer corresponds to the same variable since a function's stack frame is invalidated when it returns. At best this will cause the value of the pointer to change unexpectedly. In many cases it causes the program to crash the next time the pointer is dereferenced.  

More Detail：https://cwe.mitre.org/data/definitions/562.html  



### Example cpp file

	char* getName() {
		char name[STR_MAX];
		fillInName(name);
		return name;
	}



### Massage output in cppcheck

	[test\a\a.cpp:4]: (error) Pointer to local array variable returned.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="returnLocalVariable" severity="error" msg="Pointer to local array variable returned." verbose="Pointer to local array variable returned." cwe="562">
	            <location file0="test\a\a.cpp" file="test\a\a.cpp" line="4"/>
	        </error>
	    </errors>
	</results>