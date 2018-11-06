# <center> uninitdata - CWE 908

结构体变量，分配了空间没有初始化

### Type

id = "uninitdata"  
severity = "error"

cwe = "CWE-908: Use of Uninitialized Resource"  
cwe-type = "Base"

	<error id="uninitdata" severity="error" msg="Memory is allocated but not initialized: varname" verbose="Memory is allocated but not initialized: varname" cwe="908"/>



### Description

The software uses a resource that has not been properly initialized.

This can have security implications when the associated resource is expected to have certain properties or values.

More Detail：https://cwe.mitre.org/data/definitions/908.html  


### Example cpp file

结构体变量，分配了空间没有初始化

	struct N{  
	    int a;  
	    double b;  
	};  
	
	void main(){
		N *p = new N;
		cout<<*p;
		delete p;
	}


### Massage output in cppcheck

	[test.cpp:8]: (error) Memory is allocated but not initialized: p


### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uninitdata" severity="error" msg="Memory is allocated but not initialized: p" verbose="Memory is allocated but not initialized: p" cwe="908">
	            <location file="test.cpp" line="8"/>
	        </error>
	    </errors>
	</results>



