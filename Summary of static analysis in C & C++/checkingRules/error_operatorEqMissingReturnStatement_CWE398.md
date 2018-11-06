# <center> operatorEqMissingReturnStatement - CWE 398

函数中的无“返回”语句导致未定义行为

### Type

id = "operatorEqMissingReturnStatement"  
severity = "error"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="operatorEqMissingReturnStatement" severity="error" msg="No &apos;return&apos; statement in non-void function causes undefined behavior." verbose="No &apos;return&apos; statement in non-void function causes undefined behavior." cwe="398"/>



### Description

函数声明中有返回值，但是函数退出时没有返回语句，这容易导致不确定的行为



### Example cpp file

	int add(int a, int b)
	{
		a + b;
	}
	
	void main()
	{
		cout<<add(1,2);
	}