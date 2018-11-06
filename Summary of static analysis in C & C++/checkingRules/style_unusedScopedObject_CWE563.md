# <center> unusedScopedObject - CWE 563

对象刚创建就被析构了

### Type

id = "unusedScopedObject"  
severity = "style"

cwe = "CWE-563: Assignment to Variable without Use"  
cwe-type = "Variant"

    <error id="unusedScopedObject" severity="style" msg="Instance of &apos;varname&apos; object is destroyed immediately." verbose="Instance of &apos;varname&apos; object is destroyed immediately." cwe="563"/>


### Description

The variable's value is assigned but never used, making it a dead store.

After the assignment, the variable is either assigned another value or goes out of scope. It is likely that the variable is simply vestigial, but it is also possible that the unused variable points out a bug.

More Detail：https://cwe.mitre.org/data/definitions/563.html  



### Example cpp file

	class foo
	{
	public:
	int i ;int j ;
	foo(int x):i(x), j(i){}; // ok, 先初始化i，后初始化j
	};
	
	
	void main()
	{
		foo(1);
		foo(2);
	}



### Massage output in cppcheck

	[test.cpp:11]: (style) Instance of 'foo' object is destroyed immediately.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="unusedScopedObject" severity="style" msg="Instance of &apos;foo&apos; object is destroyed immediately." verbose="Instance of &apos;foo&apos; object is destroyed immediately." cwe="563">
	            <location file="test.cpp" line="11"/>
	        </error>
	    </errors>
	</results>