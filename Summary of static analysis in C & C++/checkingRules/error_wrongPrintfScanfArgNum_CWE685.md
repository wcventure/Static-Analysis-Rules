# <center> wrongPrintfScanfArgNum - CWE 685

Printf格式字符串需要3个参数，但只给出2个参数。

### Type

id = "wrongPrintfScanfArgNum"  
severity = "error"

cwe = "CWE-685: Function Call With Incorrect Number of Arguments"  
cwe-type = "Variant"

    <error id="wrongPrintfScanfArgNum" severity="error" msg="printf format string requires 3 parameters but only 2 are given." verbose="printf format string requires 3 parameters but only 2 are given." cwe="685"/>



### Description

printf format string requires 3 parameters but only 2 are given.

格式化字符串漏洞是由像printf(user_input)这样的代码引起的，其中user_input是用户输入的数据，具有Set-UID root权限的这类程序在运行的时候，printf语句将会变得非常危险，因为它可能会导致下面的结果：

1.使得程序崩溃  
2.任意一块内存读取数据  
3.修改任意一块内存里的数据  

最后一种结果是非常危险的，因为它允许用户修改set-UID root程序内部变量的值，从而改变这些程序的行为。

The software calls a function, procedure, or routine, but the caller specifies too many arguments, or too few arguments, which may lead to undefined behavior and resultant weaknesses.

More Detail：https://blog.csdn.net/colouful987/article/details/50588392  


### Example cpp file

如果只有一个不匹配会发生什么？

	printf ("a has value %d, b has value %d, c is at address: %08x\n",a, b);

在上面的例子中格式字符串需要3个参数，但程序只提供了2个。

More Detail：https://blog.csdn.net/colouful987/article/details/50588392  


### Massage output in cppcheck

	[test.cpp:2]: (error) printf format string requires 3 parameters but only 2 are given.



### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="wrongPrintfScanfArgNum" severity="error" msg="printf format string requires 3 parameters but only 2 are given." verbose="printf format string requires 3 parameters but only 2 are given." cwe="685">
	            <location file="test.cpp" line="2"/>
	        </error>
	    </errors>
	</results>


### Example cpp file2

	void main()
	{
	    float a;
	    float b;
	    scanf("%f",&a,&b);
	    printf("i is %f", a);
	}


### Massage output in cppcheck2

	[test.cpp:5]: (warning) scanf format string requires 1 parameter but 2 are given.




### XML output cppcheck2
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="wrongPrintfScanfArgNum" severity="warning" msg="scanf format string requires 1 parameter but 2 are given." verbose="scanf format string requires 1 parameter but 2 are given." cwe="685">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>
