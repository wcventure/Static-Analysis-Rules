# <center> returnTempReference - CWE 562

函数返回临时对象的引用

### Type

id = "returnTempReference"  
severity = "error"

cwe = "CWE-562: Return of Stack Variable Address"  
cwe-type = "base"

    <error id="returnTempReference" severity="error" msg="Reference to temporary returned." verbose="Reference to temporary returned." cwe="562"/>



### Description

A function returns the address of a stack variable, which will cause unintended program behavior, typically in the form of a crash.  

Because local variables are allocated on the stack, when a program returns a pointer to a local variable, it is returning a stack address. A subsequent function call is likely to re-use this same stack address, thereby overwriting the value of the pointer, which no longer corresponds to the same variable since a function's stack frame is invalidated when it returns. At best this will cause the value of the pointer to change unexpectedly. In many cases it causes the program to crash the next time the pointer is dereferenced.  

More Detail：https://cwe.mitre.org/data/definitions/562.html  



### Example cpp file

	class A
	{
	public:
		A(int i): a(i){ }
		~A() {}
	private:
		int a;
	};
	
	const A & f(int i) 
	{ 
		return A(i); 
	}

函数const A & f(int i)中，返回的是一个临时对象的引用。
调用f(int i)时，会先释放。由于临时变量已经释放，其返回的引用为无效地址，所以取的值也就错误了。

参考例子：[https://www.cnblogs.com/qytan36/archive/2011/03/19/1988722.html](https://www.cnblogs.com/qytan36/archive/2011/03/19/1988722.html)

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


