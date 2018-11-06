# <center> autoVariables - CWE 562

定义域大的指针存放定义域更小的局部变量的地址

### Type

id = "autoVariables"  
severity = "error"

cwe = "CWE-562: Return of Stack Variable Address"  
cwe-type = "base"

    <error id="autoVariables" severity="error" msg="Address of local auto-variable assigned to a function parameter." verbose="Dangerous assignment - the function parameter is assigned the address of a local auto-variable. Local auto-variables are reserved from the stack which is freed when the function ends. So the pointer to a local variable is invalid after the function ends." cwe="562"/>



### Description

A function returns the address of a stack variable, which will cause unintended program behavior, typically in the form of a crash.  

Because local variables are allocated on the stack, when a program returns a pointer to a local variable, it is returning a stack address. A subsequent function call is likely to re-use this same stack address, thereby overwriting the value of the pointer, which no longer corresponds to the same variable since a function's stack frame is invalidated when it returns. At best this will cause the value of the pointer to change unexpectedly. In many cases it causes the program to crash the next time the pointer is dereferenced.  

More Detail：https://cwe.mitre.org/data/definitions/562.html  



### Example cpp file

	void foo(int **a)
	{
	    int b = 1;
	    *a = &b;
	}
	
	int main()
	{
	    int *c;
	    foo(&c);
	}



### Massage output in cppcheck

	[test\autoVariables\bad.c:4]: (error) Address of local auto-variable assigned to a function parameter.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="autoVariables" severity="error" msg="Address of local auto-variable assigned to a function parameter." verbose="Dangerous assignment - the function parameter is assigned the address of a local auto-variable. Local auto-variables are reserved from the stack which is freed when the function ends. So the pointer to a local variable is invalid after the function ends." cwe="562">
	            <location file0="test\autoVariables\bad.c" file="test\autoVariables\bad.c" line="4"/>
	        </error>
	    </errors>
	</results>


