# <center> uninitStructMember - CWE 908

未初始化结构体成员

### Type

id = "uninitStructMember"  
severity = "error"

cwe = "CWE-908: Use of Uninitialized Resource"  
cwe-type = "Base"

	<error id="uninitStructMember" severity="error" msg="Uninitialized struct member: a.b" verbose="Uninitialized struct member: a.b" cwe="908"/>



### Description

The software uses a resource that has not been properly initialized.

This can have security implications when the associated resource is expected to have certain properties or values.

More Detail：https://cwe.mitre.org/data/definitions/908.html  



### Example cpp file

	struct N{  
	    int a;  
	};  
	
	void main(){
		N p;
		cout<<p.a;
	}


### Massage output in cppcheck

	[test.cpp:7]: (error) Uninitialized struct member: p.a


### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uninitStructMember" severity="error" msg="Uninitialized struct member: p.a" verbose="Uninitialized struct member: p.a" cwe="908">
	            <location file="test.cpp" line="7"/>
	        </error>
	    </errors>
	</results>



