# <center> uselessAssignmentPtrArg - CWE 398

函数存在没被使用的参数，是否是忘记了？

### Type

id = "uselessAssignmentPtrArg"  
severity = "warning"

cwe = "CWE-398"  
cwe-type = "Variant"

    <error id="uselessAssignmentPtrArg" severity="warning" msg="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" verbose="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" cwe="398"/>


### Description

Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?


### Example cpp file

参数i没被使用过

	Void f(char *p,int i)
	{
	  char *q;
	  p=malloc(10);
	  q=malloc(10);
	  
	   return ;
	}



### Massage output in cppcheck

	[test.cpp:4]: (warning) Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?
	[test.cpp:5]: (style) Variable 'q' is allocated memory that is never used.


### XML output cppcheck
	
	<?xml version="1.0" encoding="UTF-8"?>
	<results version="2">
	    <cppcheck version="1.83"/>
	    <errors>
	        <error id="uselessAssignmentPtrArg" severity="warning" msg="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" verbose="Assignment of function parameter has no effect outside the function. Did you forget dereferencing it?" cwe="398">
	            <location file="test.cpp" line="4"/>
	        </error>
	        <error id="unusedAllocatedMemory" severity="style" msg="Variable &apos;q&apos; is allocated memory that is never used." verbose="Variable &apos;q&apos; is allocated memory that is never used." cwe="563">
	            <location file="test.cpp" line="5"/>
	        </error>
	    </errors>
	</results>






