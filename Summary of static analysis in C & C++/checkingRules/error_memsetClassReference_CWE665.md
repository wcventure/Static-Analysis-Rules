# <center> memsetClassReference - CWE 665

memset()函数作用的类中，不应有引用

### Type

id = "memsetClassReference"  
severity = "error"

cwe = "CWE-665: Improper Initialization"  
cwe-type = "Class"

    <error id="memsetClassReference" severity="error" msg="Using &apos;memfunc&apos; on class that contains a reference." verbose="Using &apos;memfunc&apos; on class that contains a reference." cwe="665"/>



### Description

The software does not initialize or incorrectly initializes a resource, which might leave the resource in an unexpected state when it is accessed or used.

This can have security implications when the associated resource is expected to have certain properties or values, such as a variable that determines whether a user has been authenticated or not.

More Detail：https://cwe.mitre.org/data/definitions/665.html  



### Example cpp file

	typedef struct _test  
	{  
	    int score;  
	    int &name = score;  
	}Student;  
	  
	void main()  
	{  
	    Student a;  
	    memset(&a, 0, sizeof(a));  
	}



### Massage output in cppcheck

	[test.cpp:10]: (error) Using 'memset' on struct that contains a reference.



### XML output cppcheck

	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="memsetClassReference" severity="error" msg="Using &apos;memset&apos; on struct that contains a reference." verbose="Using &apos;memset&apos; on struct that contains a reference." cwe="665">
	            <location file="test.cpp" line="10"/>
	        </error>
	    </errors>
	</results>

