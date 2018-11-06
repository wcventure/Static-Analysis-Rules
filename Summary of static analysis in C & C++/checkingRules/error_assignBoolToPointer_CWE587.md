# <center> assignBoolToPointer - CWE 587

将bool类型的值赋值给指针变量

### Type

id = "assignBoolToPointer"  
severity = "error"

cwe = "CWE-587: Assignment of a Fixed Address to a Pointer"  
cwe-type = "base"

    <error id="assignBoolToPointer" severity="error" msg="Boolean value assigned to pointer." verbose="Boolean value assigned to pointer." cwe="587"/>



### Description

The software sets a pointer to a specific address other than NULL or 0.

Using a fixed address is not portable because that address will probably not be valid in all environments or platforms.

More Detail：https://cwe.mitre.org/data/definitions/587.html  



### Example cpp file

	void main()
	{
		bool b = false;
		int *p;
		p = b;
	}

p为指针变量，b为bool类型变量


