# <center> duplInheritedMember - CWE 398

成员变量的名称与父类的相同	

### Type

id = "duplInheritedMember"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="duplInheritedMember" severity="warning" msg="The class &apos;class&apos; defines member variable with name &apos;variable&apos; also defined in its parent class &apos;class&apos;." verbose="The class &apos;class&apos; defines member variable with name &apos;variable&apos; also defined in its parent class &apos;class&apos;." cwe="398"/>



### Description

成员变量的名称与父类的相同	


### Example cpp file
	
	class A
	{
	int a;
	int b;    
	};
	
	class B :public A
	{
		int a;
	
	};